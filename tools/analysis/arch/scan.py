"""One scan: the commit read, files classified, adapters run, names joined. Later stages work on what this returns."""
import concurrent.futures
import os
import shutil
import sys
import tempfile
import time
import uuid

from . import external, link, roles
from .adapters import cpp, data, gdscript, godot_text
from .cache import Cache
from .core import own_prefixes, program_root
from .model import Facts
from .repo import Repo

ADAPTERS = ("cpp", "gdscript", "data", "godot")


class Run:
    """Everything known about one repository at one commit."""

    def __init__(self, path, rev="HEAD", cache=True, off=(), out=None, quiet=False, repo=None):
        self.repo = repo or Repo(path, rev)
        self.off = set(off)
        self.quiet = quiet
        self.base = out or os.environ.get("ARCH_OUT") or os.path.join(program_root(), "dist", "arch")
        self.cache = Cache(os.path.join(self.base, "cache"), cache)
        self.started = time.time()
        self._exports = {}

    def __copy__(self):
        """A copy in memory keeps everything: what `__getstate__` leaves out is left out of the disk."""
        other = object.__new__(type(self))
        other.__dict__.update(self.__dict__)
        return other

    def __getstate__(self):
        """A run handed to another process: what it knows, without its pipes."""
        return {k: v for k, v in self.__dict__.items() if k not in ("repo", "cache", "graph", "names", "report", "_exports")}

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
        """A private folder for this run's copy of the commit: no other process ever reads or writes it."""
        if part not in self._exports:
            parent = os.path.join(self.base, "work")
            os.makedirs(parent, exist_ok=True)
            self._exports[part] = [os.path.join(parent, f"{part}-{os.getpid()}-{uuid.uuid4().hex[:12]}").replace("\\", "/"), None]
        return self._exports[part][0]

    def export(self, paths, part):
        """The files on disk, written into a staging folder and published under the private name in one step; any failure raises."""
        target = self.export_path(part)
        if self._exports[part][1] is None:
            staging = tempfile.mkdtemp(prefix=f"{part}-staging-", dir=os.path.dirname(target))
            refused = self.repo.export(staging, paths)
            self.repo.overlay(staging)
            os.replace(staging, target)
            self._exports[part][1] = refused
        return target, self._exports[part][1]

    def close(self):
        """The private copies go with the run that made them."""
        from . import workers
        workers.close()
        for target, done in self._exports.values():
            if done is not None:
                shutil.rmtree(target)
        self._exports.clear()

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
            if "data" not in off:
                take("data", data.scan(repo, tracked, scope, self.mentioned()))
            for name, result in compiled.result():
                if name == "refused":
                    info[name] = result
                else:
                    take(name, result)
        return facts, info

    def _on_disk(self):
        """What needs the files themselves, and only when something really must be parsed: the compiler."""
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
        return out


def cpp_exts():
    return tuple("." + e for e in cpp.LANG)


def _folders(paths):
    """The top folders holding the files: one archive call, not one per file."""
    return sorted({p.split("/", 2)[0] + "/" + p.split("/", 2)[1] if p.count("/") > 1 else p for p in paths})
