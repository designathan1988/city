"""What was computed, by content and by the code that computed it: whole tables, read once and written once. Off for runs that must prove
themselves from nothing. A selfcheck starts its own empty one, so nothing older than the run can hide a failure."""
import glob
import os
import pickle

from .core import digest

_TABLES = {}


def _makers():
    """The text of everything that decides what is stored: a change there and nothing old is read."""
    here = os.path.dirname(os.path.abspath(__file__))
    paths = sorted(glob.glob(os.path.join(here, "adapters", "*"))) + [os.path.join(here, n) for n in ("core.py", "model.py", "shape.py", "spans.py", "roles.py")]
    out = []
    for path in paths:
        if os.path.isfile(path):
            with open(path, encoding="utf8") as f:
                out.append(digest(f.read()))
    return digest("".join(out))


class Cache:
    def __init__(self, folder, on=True):
        self.folder, self.on = folder, on
        self.salt = _makers() if on else ""
        self.mine = {}

    def table(self, kind):
        """key -> value of one kind of result. Empty and private when the cache is off."""
        if not self.on:
            return self.mine.setdefault(kind, {})
        path = os.path.join(self.folder, f"{kind}_{self.salt}.pkl")
        if path not in _TABLES:
            _TABLES[path] = {}
            if os.path.exists(path):
                with open(path, "rb") as f:
                    _TABLES[path] = pickle.load(f)
        self.mine[kind] = path
        return _TABLES[path]

    def save(self):
        if not self.on:
            return
        os.makedirs(self.folder, exist_ok=True)
        for path in self.mine.values():
            with open(path + ".tmp", "wb") as f:
                pickle.dump(_TABLES[path], f, pickle.HIGHEST_PROTOCOL)
            os.replace(path + ".tmp", path)
