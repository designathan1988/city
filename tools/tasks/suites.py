# The suites file (tools/tasks/suites.json) read, checked against its schema and turned into Task objects: names, groups
# made from templates, variants (--ready, --bless, --all), sets of inputs, vars, and the dependency closure.
from __future__ import annotations

import copy
import hashlib
import json
import os
import re
import sys
import tempfile
from dataclasses import dataclass, field
from pathlib import Path

from . import inputs, proc, schema
from .task import HERE, ROOT, Task

SUITES = HERE / "suites.json"
SCHEMA = HERE.parent / "gates" / "schemas" / "suites.schema.json"
VAR = re.compile(r"\{([a-z_]+)\}")
EXPANDS = re.compile(r"^\{changed_list:(.+)\}$")  # a whole argv element: that flag and a file listing the changed files
VERSION_TIMEOUT_S = 60
NOT_COMPUTED = "<computed when the task runs>"


@dataclass
class Selection:
    tasks: list[Task] = field(default_factory=list)
    stamps: list[str] = field(default_factory=list)
    dropped: list[str] = field(default_factory=list)  # tasks with nothing to do (no changed file)
    no_cache: bool = False


class UsageError(ValueError):
    """A name, flag or variant the suites do not know: exit 2."""


class Suites:
    def __init__(self, path: Path | str | None = None, root: Path | str | None = None, listing: bool = False):
        self.path = Path(path or SUITES)
        self.root = Path(root or ROOT).resolve()
        self.listing = listing
        self.doc = json.loads(self.path.read_text(encoding="utf-8"))
        problems = schema.errors(self.doc, json.loads(SCHEMA.read_text(encoding="utf-8")))
        if problems:
            raise ValueError(f"{self.path} breaks its schema:\n  " + "\n  ".join(problems))
        self.items = {}  # task name -> (template fields, item)
        for group in self.doc["groups"].values():
            template = self.doc["templates"].get(group["template"])
            if template is None:
                raise ValueError(f"unknown template {group['template']}")
            for item in group["items"]:
                self.items[item["name"]] = (template, item)
        tasks = set(self.items) | set(self.doc["tasks"])
        clash = ((set(self.items) & set(self.doc["tasks"])) | (set(self.doc["groups"]) & set(self.doc["suites"]))
                 | (tasks & (set(self.doc["suites"]) | set(self.doc["groups"]))))
        if clash:
            raise ValueError(f"names used twice in {self.path}: {', '.join(sorted(clash))}")
        self._vars: dict[str, str] = {}
        self._changed: list[str] | None = None

    # vars -----------------------------------------------------------------------------------------------------------
    def var(self, name: str) -> str:
        if name in self._vars:
            return self._vars[name]
        if name == "root":
            value = self.root.as_posix()
        elif name == "python":
            value = Path(sys.executable).as_posix()
        elif name == "godot_version":
            value = NOT_COMPUTED if self.listing else self._godot_version()
        elif name in self.doc["vars"]:
            spec = self.doc["vars"][name]
            value = os.environ.get(spec.get("env", ""), "") or spec["windows" if os.name == "nt" else "posix"]
            value = self.expand(value)
        else:
            raise ValueError(f"unknown var {{{name}}}")
        self._vars[name] = value
        return value

    def _godot_version(self) -> str:
        with tempfile.TemporaryDirectory() as tmp:
            log = Path(tmp) / "version.txt"
            code, timed_out = proc.run([self.var("godot"), "--headless", "--version"], VERSION_TIMEOUT_S, log=log)
            text = log.read_text(encoding="utf-8", errors="replace").strip()
        if timed_out or code != 0 or not text:
            raise RuntimeError(f"{self.var('godot')} --version gave exit {code}{' (timed out)' if timed_out else ''}: {text}")
        return text.splitlines()[-1].strip()

    def expand(self, text: str, item: dict | None = None) -> str:
        def one(m: re.Match) -> str:
            key = m.group(1)
            if item is not None and key in item:
                return str(item[key])
            return self.var(key)
        return VAR.sub(one, text)

    def changed(self) -> list[str]:
        if self._changed is None:
            c = self.doc["changed"]
            self._changed = inputs.changed(c["include"], c.get("exclude", []), root=self.root)
        return self._changed

    # tasks ----------------------------------------------------------------------------------------------------------
    def task_names(self) -> list[str]:
        return list(self.doc["tasks"]) + list(self.items)

    def build(self, name: str) -> Task | None:
        """The task of that name, or None when it has nothing to do (an argv over the changed files, with none)."""
        if name in self.doc["tasks"]:
            fields, item = self.doc["tasks"][name], None
        elif name in self.items:
            fields, item = self.items[name]
        else:
            raise UsageError(f"unknown task {name}")
        f = copy.deepcopy(fields)
        timeout = item.get("timeout_s", f.get("timeout_s")) if item else f.get("timeout_s")
        if timeout is None:
            raise ValueError(f"{name}: no timeout_s (every task has its own deadline)")
        ex = (lambda s: self.expand(s, item))
        argv = []
        for a in f.get("argv", []):
            m = EXPANDS.match(a)
            if m:
                if not self.changed():
                    return None
                argv += [m.group(1), self._changed_list()]
            else:
                argv.append(ex(a))
        kw = dict(inputs=self._inputs(f.get("inputs", []), ex), extra=[ex(x) for x in f.get("extra", [])],
                  outputs=[ex(x) for x in f.get("outputs", [])], after=list(f.get("after", [])),
                  cwd=ex(f["cwd"]) if "cwd" in f else None, env={k: ex(v) for k, v in f.get("env", {}).items()} or None,
                  hidden=f.get("hidden", False), verdict=f.get("verdict"), resources=f.get("resources", {}),
                  params={k: ex(v) for k, v in f.get("params", {}).items()}, fresh=f.get("fresh", False),
                  hint=f.get("hint", ""))
        if "call" in f:
            spec = self._deep(f.get("spec", {}), ex)
            spec.setdefault("root", self.root.as_posix())
            return Task.call(name, f["call"], spec, timeout, root=self.root, **kw)
        if not argv:
            raise ValueError(f"{name}: a task needs argv or call")
        return Task(name=name, argv=argv, timeout_s=timeout, **kw)

    def _changed_list(self) -> str:
        """The changed files written one per line to a file named by its content (any number of paths fits; no command line
        limit), as a repo-relative path."""
        text = "".join(f + "\n" for f in self.changed())
        rel = f"dist/tasks/lists/changed-{hashlib.sha256(text.encode('utf-8')).hexdigest()[:16]}.txt"
        path = self.root / rel
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8", newline="\n")
        return rel

    def _inputs(self, entries: list[str], ex) -> list[str]:
        out = []
        for x in entries:
            if x.startswith("@"):
                if x[1:] not in self.doc["sets"]:
                    raise ValueError(f"unknown set {x}")
                out += [ex(p) for p in self.doc["sets"][x[1:]]]
            elif x == "{changed}":
                out += self.changed()
            else:
                out.append(ex(x))
        return out

    def _deep(self, v, ex):
        if isinstance(v, str):
            return ex(v)
        if isinstance(v, list):
            return [self._deep(x, ex) for x in v]
        if isinstance(v, dict):
            return {k: self._deep(x, ex) for k, x in v.items()}
        return v

    # selection ------------------------------------------------------------------------------------------------------
    def select(self, names: list[str], variants: list[str], no_cache: bool = False) -> Selection:
        """The tasks the names ask for (suites, groups, tasks; a name also selects its '<name>@renderer' siblings), with
        the variants applied and every task they run after; in the order named."""
        order: list[str] = []
        reached: list[str] = []
        active = list(variants)

        def add(n: str, path: tuple):
            if n in path:
                raise ValueError(f"suite cycle: {' -> '.join(path + (n,))}")
            ref = n[1:] if n.startswith("@") else n
            if ref in self.doc["suites"]:
                reached.append(ref)
                suite = self.doc["suites"][ref]
                active.extend(suite.get("apply", []))
                for e in suite["tasks"]:
                    add(e, path + (n,))
            elif ref in self.doc["groups"]:
                order.extend(i["name"] for i in self.doc["groups"][ref]["items"])
            elif ref in self.doc["tasks"] or ref in self.items:
                order.append(ref)
            else:
                siblings = [t for t in self.items if t.startswith(ref + "@")]
                if not siblings and not n.startswith("@"):
                    raise UsageError(f"unknown suite, group or task: {n}")
                if not siblings:
                    raise UsageError(f"unknown group or suite: {n}")
                order.extend(siblings)
        for n in names:
            add(n, ())
        defined = {}
        for s in reached:
            for vname, v in self.doc["suites"][s].get("variants", {}).items():
                defined.setdefault(vname, []).append(v)
        sel = Selection(no_cache=no_cache)
        for vname in dict.fromkeys(active):
            if vname not in defined:
                if vname == "all":
                    continue  # the full run is the CLI's own: no cache; a suite may add to it
                raise UsageError(f"--{vname}: no selected suite has that variant")
            for v in defined[vname]:
                order.extend(v.get("add", []))
                swap = v.get("swap", {})
                order = [swap.get(t, t) for t in order]
                if "stamp" in v:
                    sel.stamps.append(v["stamp"])
        names_in = list(dict.fromkeys(order))
        built: dict[str, Task] = {}
        todo = list(names_in)
        while todo:
            n = todo.pop(0)
            if n in built or n in sel.dropped:
                continue
            t = self.build(n)
            if t is None:
                sel.dropped.append(n)
                continue
            built[n] = t
            todo.extend(d for d in t.after if d not in built)
        needed_dropped = [n for n, t in built.items() for d in t.after if d in sel.dropped]
        if needed_dropped:
            raise ValueError(f"{', '.join(needed_dropped)} run after a task with nothing to do")
        sel.tasks = list(built.values())
        return sel
