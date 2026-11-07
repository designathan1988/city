"""One scan: the commit read, files classified, adapters run, names joined. Later stages work on what this returns."""
import concurrent.futures
import os
import shutil
import sys
import time

from . import external, link, roles
from .adapters import comments, cpp, data, gdscript, godot_text, python_src, script_tools, shader, shader_spirv, typescript
from .cache import Cache
from .core import digest, own_prefixes, program_root, read_json, write_json
from .model import Facts
from .repo import Repo

ADAPTERS = ("cpp", "gdscript", "shader", "data", "godot", "python", "scripts", "typescript", "comments")


class Run:
    """Everything known about one repository at one commit."""

    def __init__(self, path, rev="HEAD", cache=True, off=(), out=None, quiet=False, repo=None):
        self.repo = repo or Repo(path, rev)
        self.off = set(off)
        self.quiet = quiet
        self.base = out or os.environ.get("ARCH_OUT") or os.path.join(program_root(), "dist", "arch")
        self.cache = Cache(os.path.join(self.base, "cache"), cache)
        self.started = time.time()

    def __copy__(self):
        """A copy in memory keeps everything: what `__getstate__` leaves out is left out of the disk."""
        other = object.__new__(type(self))
        other.__dict__.update(self.__dict__)
        return other

    def __getstate__(self):
        """A run handed to another process: what it knows, without its pipes."""
        return {k: v for k, v in self.__dict__.items() if k not in ("repo", "cache", "graph", "names", "report")}

    def log(self, text):
        if not self.quiet:
            print(f"[{time.time() - self.started:6.1f}s] {text}", file=sys.stderr, flush=True)

    def read(self):
        repo = self.repo
        own = tuple(repo.translate(p) for p in own_prefixes(repo.root))
        self.tracked = {p: b for p, b in repo.files().items() if not (own and p.startswith(own))}
        self.history = [(sha, [c for c in changes if not (own and c[1].startswith(own))]) for sha, changes in repo.history()]
        self.log(f"{len(self.tracked)} files, {len(self.history)} commits")
        self.roles, self.texts = roles.classify(repo, self.tracked, self.history, self.cache)
        self.scope = self.roles.own(self.tracked)
        self.log(f"roles {self.roles.foreign_dirs} own {len(self.scope)} index {sorted(self.roles.index)}")
        self.api = external.load(repo, self.tracked)
        self.facts, self.info = self.adapters()
        self.graph, self.names, self.report = link.build(self.facts)
        self.log(f"graph {len(self.graph.units)} units, {len(self.graph.edges)} edges, open {len(self.report['open'])}")
        return self

    def mentioned(self):
        """File names that code writes: what a data table needs in order to be read at all."""
        out = set()
        for path in sorted(self.scope):
            if path in self.roles.mentions and not path.endswith((".json", ".md", ".csv", ".txt")):
                out |= self.roles.mentions[path]
        return out

    def export_path(self, part):
        return os.path.join(self.base, "work", digest(self.repo.root + self.repo.export_key() + part), "export").replace("\\", "/")

    def export(self, paths, part):
        target = self.export_path(part)
        ready = target + ".json"
        if not os.path.exists(ready):
            fresh = f"{target}.{os.getpid()}"
            os.makedirs(fresh, exist_ok=True)
            refused = self.repo.export(fresh, paths)
            try:
                os.rename(fresh, target)
                write_json(ready, refused)
            except OSError:  # another task got there first with the same files
                shutil.rmtree(fresh, ignore_errors=True)
        while not os.path.exists(ready):
            time.sleep(0.1)
        refused = read_json(ready)
        self.repo.overlay(target)
        return target, refused

    def adapters(self):
        """The compiler's side starts first and runs while the others read; results join in a fixed order."""
        repo, tracked, scope, off = self.repo, self.tracked, self.scope, self.off
        facts, info = Facts(), {}
        with concurrent.futures.ThreadPoolExecutor(1) as side:
            compiled = side.submit(self._on_disk)

            def take(name, result):
                facts.merge(result[0])
                info[name] = result[1]
                self.log(f"{name}: {len(result[0].units)} units, {len(result[0].edges)} edges")

            if "godot" not in off:
                take("godot", godot_text.scan(repo, tracked, scope | {p for p in self.roles.sidecar if p not in self.roles.foreign}))
            godot = info.get("godot", {})
            if "gdscript" not in off:
                take("gdscript", gdscript.scan(repo, tracked, scope, self.api, godot.get("autoloads", {})))
            if "shader" not in off:
                take("shader", shader.scan(repo, tracked, scope))
                info["spirv"] = shader_spirv.scan(repo, tracked, scope, facts)
                self.log(f"spir-v: {info['spirv']['compiled']} compiled, {len(info['spirv']['failed'])} refused")
            if "data" not in off:
                take("data", data.scan(repo, tracked, scope, self.mentioned()))
            if "comments" not in off:
                take("comments", comments.scan(scope, self.texts))
            if "python" not in off:
                take("python", python_src.scan(repo, tracked, scope))
            if "scripts" not in off:
                take("scripts", script_tools.scan(repo, tracked, scope))
            for name, result in compiled.result():
                if name == "refused":
                    info[name] = result
                else:
                    take(name, result)
        return facts, info

    def _on_disk(self):
        """What needs the files themselves, and only when something really must be parsed: the compiler and the type checker."""
        repo, tracked, scope, off = self.repo, self.tracked, self.scope, self.off
        out = []
        if "cpp" not in off and any(p.endswith(cpp_exts()) for p in tracked):
            folders = _folders(sorted(p for p in tracked if p.endswith(cpp_exts())))
            ready = []

            def ensure():
                if not ready:
                    ready.append(self.export(folders, "cpp"))
                    out.append(("refused", ready[0][1]))

            out.append(("cpp", cpp.scan(repo, tracked, scope, self.export_path("cpp"), ensure, self.cache, self.log)))
        own = sorted(p for p in tracked if (p in scope and p.endswith(typescript.EXT)) or p.rsplit("/", 1)[-1] == "tsconfig.json")
        if "typescript" not in off and any(p.endswith(typescript.EXT) for p in own):
            out.append(("typescript", typescript.scan(repo, tracked, scope, self.export(_folders(own), "ts")[0])))
        return out


def cpp_exts():
    return tuple("." + e for e in cpp.LANG)


def _folders(paths):
    """The top folders holding the files: one archive call, not one per file."""
    return sorted({p.split("/", 2)[0] + "/" + p.split("/", 2)[1] if p.count("/") > 1 else p for p in paths})
