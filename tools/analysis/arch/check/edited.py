"""A commit of the repository with some files' text replaced, for the program reading it: same questions as Repo, the edited files answered from
memory and laid over every export. Nothing here touches the real history or the working tree."""
from ..core import digest, text_of

CODE_ON_DISK = (".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx", ".inl")


class Edited:
    def __init__(self, base, edits):
        self.base, self.edit = base, dict(edits)
        self.root = base.root
        self.head = digest(f"{base.head}{sorted((p, digest(d)) for p, d in self.edit.items())}")
        self._files = None

    def translate(self, rel):
        return rel

    def flag(self, word):
        return word

    def profile(self, text):
        return text

    def on_disk(self, pattern):
        return self.base.on_disk(pattern)

    def content_id(self, blob):
        return digest(self.blob(blob)) if blob.startswith("v:") else blob

    def spec(self):
        return self.base.spec()

    def export_key(self):
        return self.head

    def overlay(self, target):
        import os
        for path, data in self.edit.items():
            place = os.path.join(target, path)
            os.makedirs(os.path.dirname(place), exist_ok=True)
            with open(place, "wb") as f:
                f.write(data)

    def prefetch(self, blobs):
        self.base.prefetch(b for b in blobs if not b.startswith("v:"))

    def text(self, blob):
        return text_of(self.blob(blob)) if blob.startswith("v:") else self.base.text(blob)

    def files(self, rev=None):
        if self._files is None:
            out = dict(self.base.files())
            for path in self.edit:
                out[path] = "v:" + path
            self._files = out
        return self._files

    def blob(self, blob):
        return self.edit[blob[2:]] if blob.startswith("v:") else self.base.blob(blob)

    def history(self):
        return self.base.history()

    def export(self, dest, paths=None, rev=None):
        return self.base.export(dest, paths, rev)

    def git(self, *args, check=True):
        return self.base.git(*args, check=check)

    def close(self):
        self.base.close()
