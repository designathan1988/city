"""One gate run's shared state: root, scope, configuration, sources, per-file facts and the result cache (tools/tasks
memo keyed by content through key_of; the self-test runs uncached on its fixture folders)."""
import glob
import hashlib
import json
import os
import sys

import facts as facts_mod
import scope
from source import Sources

GATES_DIR = os.path.dirname(os.path.abspath(__file__))
REPO = os.path.abspath(os.path.join(GATES_DIR, "..", ".."))
TOOL = "gates"


def digest(*parts):
    return hashlib.sha256(json.dumps(parts, sort_keys=True, default=sorted).encode("utf-8")).hexdigest()


class Cache:
    """memo/key_of from tools/tasks, the one orchestration owner. There is no fallback: a missing tools/tasks fails the run."""

    def __init__(self, root, enabled):
        self.root, self.enabled = root, enabled
        self._file_keys = {}
        self.version = ""
        if enabled:
            sys.path.insert(0, os.path.join(REPO, "tools"))
            from tasks import key_of, memo  # noqa: E402
            self._key_of, self._memo = key_of, memo
            sources = sorted(os.path.relpath(p, root).replace(os.sep, "/") for p in glob.glob(os.path.join(GATES_DIR, "*.py")))
            self.version = key_of(sources + ["tools/gates/rules.json", "tools/gates/owners.json"], root=root)

    def warm(self, paths):
        """One bulk content listing before the per-file keys (git hash-object once for every changed file)."""
        if self.enabled and paths:
            self._key_of(list(paths), root=self.root)

    def file_key(self, path):
        if not self.enabled:
            return ""
        k = self._file_keys.get(path)
        if k is None:
            k = self._file_keys[path] = self._key_of([path], extra=(self.version,), root=self.root)
        return k

    def key_of(self, paths, extra=(), versioned=True):
        """The content key of `paths` and `extra`; `versioned` adds the gates' own version (every gate source and the
        configuration), which a result made by a few gate sources alone leaves out, naming them in `paths` instead
        (lab_prints.STORE_SOURCES)."""
        if not self.enabled:
            return ""
        return self._key_of(list(paths), extra=((self.version,) if versioned else ()) + tuple(extra), root=self.root)

    def get(self, key, fn):
        return self._memo(TOOL, key, fn, root=self.root) if self.enabled else fn()


class Context:
    """`files`: the scope (repo-relative, posix); `listed`: every file git lists, excluded folders included;
    `changed`: {path: changed line numbers or None for a new file} against HEAD; `before_root`: a folder holding the
    files as they were before the change (a self-test case's), else HEAD is asked through git."""

    def __init__(self, root, files, listed, rules, owners, changed, cache, before_root=None):
        self.root, self.files, self.listed = root, files, listed
        self.file_set, self.listed_set = set(files), set(listed)
        self.rules, self.owners, self.changed, self.cache = rules, owners, changed, cache
        self.before_root = before_root
        self.sources = Sources(root)
        self._lazy = {}

    def before(self, path):
        """The text of `path` before the change, or None when it did not exist."""
        if self.before_root is None:
            return scope.before_text(self.root, path)
        full = os.path.join(self.before_root, path)
        if not os.path.isfile(full):
            return None
        with open(full, encoding="utf-8") as fh:
            return fh.read()

    def before_bytes(self, path):
        """The bytes of `path` before the change (line endings kept), or None when it did not exist."""
        if self.before_root is None:
            return scope.before_bytes(self.root, path)
        full = os.path.join(self.before_root, path)
        if not os.path.isfile(full):
            return None
        with open(full, "rb") as fh:
            return fh.read()

    def source(self, path):
        return self.sources.get(path)

    def facts(self, path):
        """Per-file facts the whole-repo rules index (identifiers, strings, classes, clone windows), cached by content."""
        key = digest("facts", self.cache.file_key(path)) if self.cache.enabled else None
        if key is None:
            return facts_mod.file_facts(self.source(path), self.rules)
        memo_key = ("facts", path)
        if memo_key not in self._lazy:
            self._lazy[memo_key] = self.cache.get(key, lambda: facts_mod.file_facts(self.source(path), self.rules))
        return self._lazy[memo_key]

    def lazy(self, name, build):
        if name not in self._lazy:
            self._lazy[name] = build()
        return self._lazy[name]
