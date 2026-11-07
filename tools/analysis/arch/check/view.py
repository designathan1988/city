"""A copy of a repository that exists only for the program reading it: same questions as Repo, with paths renamed, files dropped or added,
commits left out. Nothing here touches the real history."""
import atexit
import os
import pickle
import tempfile

from ..core import digest, text_of

CODE_ON_DISK = (".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".inl", ".ts", ".tsx", ".js", ".mjs", ".cjs")


class View:
    def __init__(self, base, rename=None, drop=(), add=None, edit=None, last_commits_out=0, tag="view"):
        self.base, self.rename, self.drop = base, rename, set(drop)
        self.add, self.edit = dict(add or {}), dict(edit or {})
        self.root = base.root
        self.cut = last_commits_out
        self.head = digest(f"{base.head}{tag}{sorted(self.add)}{sorted(self.edit)}{len(self.drop)}{last_commits_out}")
        self._history = None
        self._files = None
        self._texts = {}
        self._spec = None

    def translate(self, rel):
        return self.rename.path(rel) if self.rename else rel

    def flag(self, word):
        return self.rename.text(word) if self.rename else word

    def profile(self, text):
        return self.rename.profile(text) if self.rename else text

    def on_disk(self, pattern):
        return self.base.on_disk(pattern)

    def content_id(self, blob):
        if blob.startswith("v:"):
            return digest(self.blob(blob))
        return "r" + blob if self.rename else blob

    def spec(self):
        """Another process needs the map itself to read renamed text; everything else it reads from the repository."""
        if not self.rename:
            return self.base.spec()
        if self._spec is None:
            handle, path = tempfile.mkstemp(suffix=".renamer")
            with os.fdopen(handle, "wb") as f:
                pickle.dump(self.rename, f, pickle.HIGHEST_PROTOCOL)
            atexit.register(os.remove, path)
            self._spec = ("renamed", self.base.root, path)
        return self._spec

    def export_key(self):
        """A copy of the same commit that only adds shares the commit's files on disk; its own are laid over them."""
        code = any(p.endswith(CODE_ON_DISK) for p in (*self.drop, *self.edit))
        return self.head if self.rename or self.cut or code else self.base.export_key()

    def overlay(self, target):
        for path, data in {**self.add, **self.edit}.items():
            place = os.path.join(target, path)
            os.makedirs(os.path.dirname(place), exist_ok=True)
            with open(place, "wb") as f:
                f.write(data)

    def prefetch(self, blobs):
        self.base.prefetch(b for b in blobs if not b.startswith("v:"))

    def text(self, blob):
        if blob.startswith("v:"):
            return text_of(self.blob(blob))
        plain = self.base.text(blob)
        if not self.rename or plain is None:
            return plain
        if blob not in self._texts:
            self._texts[blob] = self.rename.text(plain)
        return self._texts[blob]

    def _commits(self):
        if self._history is None:
            full = self.base.history()
            self._history = full[:len(full) - self.cut] if self.cut else full
        return self._history

    def files(self, rev=None):
        if self._files is None:
            source = self.base.files(self._commits()[-1][0] if self.cut else None)
            out = {}
            for path, blob in source.items():
                if path not in self.drop:
                    out[self.translate(path)] = blob
            for path in self.edit:
                out[path] = "v:" + path
            for path in self.add:
                out[path] = "v:" + path
            self._files = out
        return self._files

    def blob(self, blob):
        if blob.startswith("v:"):
            path = blob[2:]
            return self.add[path] if path in self.add else self.edit[path]
        if self.rename and self.text(blob) is not None:
            return self.text(blob).encode("utf8")
        return self.base.blob(blob)

    def history(self):
        out = []
        for sha, changes in self._commits():
            kept = [(s, self.translate(p), n, o) for s, p, n, o in changes if p not in self.drop]
            out.append((sha, kept))
        new = [("A", p, "v:" + p, "0" * 40) for p in sorted(self.add)]
        new += [("M", p, "v:" + p, self.files().get(p, "0" * 40) if False else self._old(p)) for p in sorted(self.edit)]
        if new:
            out.append(("view", new))
        return out

    def _old(self, path):
        return self.base.files().get(path, "0" * 40)

    def export(self, dest, paths=None, rev=None):
        if not self.rename:
            rev = self._commits()[-1][0] if self.cut else None
            known = self.base.files(rev)
            there = [p for p in paths or () if p in known or any(k.startswith(p.rstrip("/") + "/") for k in known)]
            refused = self.base.export(dest, there or None, rev)
            for path in self.drop:
                if os.path.exists(os.path.join(dest, path)):
                    os.remove(os.path.join(dest, path))
            return refused
        keep = tuple(p.rstrip("/") + "/" for p in paths) if paths else None
        exact = set(paths or ())
        for path, blob in self.files().items():
            if keep is None or path.startswith(keep) or path in exact:
                target = os.path.join(dest, path)
                os.makedirs(os.path.dirname(target), exist_ok=True)
                with open(target, "wb") as f:
                    f.write(self.blob(blob))
        return []

    def git(self, *args, check=True):
        return self.base.git(*args, check=check)

    def close(self):
        pass
