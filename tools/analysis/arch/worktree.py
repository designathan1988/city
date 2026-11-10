"""The repository as it stands on disk, for the program reading it: the commit with every file the working tree changed, added or
removed laid over it. The same questions as Repo; what another session left uncommitted is analysed, because that is the code being worked on."""
import os

from .check.edited import Edited


class Worktree(Edited):
    def __init__(self, base):
        edits, gone = {}, set()
        fields = [f for f in base.git("status", "--porcelain", "-z", "--untracked-files=all").decode("utf8", "surrogateescape").split("\0") if f]
        skip = False
        for field in fields:
            if skip:  # the old name of a rename follows its new one
                gone.add(field)
                skip = False
                continue
            state, path = field[:2], field[3:]
            skip = "R" in state or "C" in state
            full = os.path.join(base.root, path)
            if os.path.isfile(full):
                with open(full, "rb") as f:
                    edits[path] = f.read()
            elif "D" in state:
                gone.add(path)
        super().__init__(base, edits)
        self.gone = gone - set(edits)
        self.changed = sorted(edits)

    def files(self, rev=None):
        if self._files is None:
            self._files = {p: b for p, b in super().files().items() if p not in self.gone}
        return self._files
