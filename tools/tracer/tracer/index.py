"""The analysis kept between runs: what a trace reads of it (definitions, labels, links, state, gaps), without the
trees, saved under a key made of every file read (path, size, time written), of the tracer's own code and of the
options. A query whose key is saved reads it back in under a second instead of analysing again; any file changed,
added or removed makes a new key.
"""
from __future__ import annotations

import hashlib
import pickle
from dataclasses import dataclass, field
from pathlib import Path

from .languages import NODE_PACKAGES
from .read import discover

HOME = Path(__file__).resolve().parent
FOLDER = ".index"                            # beside the tracer's code
KEEP = 4                                     # the saved analyses kept (the oldest go)


@dataclass
class Spot:
    """Where a definition is written, as a trace reads it."""
    start_point: tuple
    end_point: tuple
    start_byte: int = 0
    end_byte: int = 0


@dataclass
class Shown:
    display: str
    module: int = -1


@dataclass
class Kept:
    """A definition as a trace reads it."""
    id: int
    kind: str
    name: str
    file: int
    line: int
    node: Spot
    owner: int = -1
    holder: int = -1
    body: bool = False
    parts: dict = field(default_factory=dict)


class Snapshot:
    """The program as a trace reads it: what `Program` answers, without the trees."""

    def __init__(self, program):
        self.files = [Shown(f.display, f.module) for f in program.files]
        self.defs = [Kept(d.id, d.kind, d.name, d.file, d.line,
                          Spot(tuple(d.node.start_point), tuple(d.node.end_point), d.node.start_byte,
                               d.node.end_byte), d.owner, d.holder, d.body) for d in program.defs]
        self.labels = list(program.labels)
        self.names = {c: {name: list(ids) for name, ids in held.items()} for c, held in program.names.items()}
        self.by_label = {label: list(ids) for label, ids in program.by_label.items()}
        self.classes = {d.id for d in program.defs if program.is_class(d.id)}

    def label(self, i: int) -> str:
        return self.labels[i]

    def where(self, i: int) -> str:
        d = self.defs[i]
        return f"{self.files[d.file].display}:{d.line}"

    def chain(self, i: int) -> list:
        out = []
        while i != -1:
            out.append(i)
            i = self.defs[i].owner
        return out

    def is_class(self, i: int) -> bool:
        return i in self.classes

    def held(self, c: int, name: str):
        return self.names.get(c, {}).get(name, ())

    def bodies(self, ids) -> list:
        out = []
        for i in ids:
            d = self.defs[i]
            full = [j for j in self.by_label.get(self.labels[i], ()) if self.defs[j].body] \
                if d.kind == "function" and not d.body else []
            out += full or [i]
        return list(dict.fromkeys(out))


def kept(result):
    """The result as it is saved: the program as a snapshot, no scopes."""
    result.program = Snapshot(result.program)
    result.scopes = None
    return result


def key(roots, base, among, compilers: bool = True) -> str:
    """What the analysis depends on: every file read (path, size, time written), the tracer's code (its script run by
    node too), the options (the compilers asked or not)."""
    h = hashlib.sha256()
    for path in discover([Path(r).resolve() for r in roots]):
        st = path.stat()
        h.update(f"{path}|{st.st_size}|{st.st_mtime_ns}\n".encode())
    for path in sorted(HOME.rglob("*")):
        if path.suffix in (".py", ".scm", ".cjs") and NODE_PACKAGES not in path.parts:
            h.update(path.read_bytes())
    h.update(f"{base}|{among}|{compilers}".encode())
    return h.hexdigest()[:24]


def folder() -> Path:
    where = HOME.parent / FOLDER
    where.mkdir(parents=True, exist_ok=True)
    return where


def load(name: str):
    path = folder() / f"{name}.pickle"
    if not path.exists():
        return None
    try:
        return pickle.loads(path.read_bytes())
    except (OSError, pickle.UnpicklingError, EOFError, AttributeError, TypeError, ValueError, ImportError):
        return None


ANSWERS = "solver-answers"                   # the solver's answers, by formula (beside the saved analyses)
MOST_ANSWERS = 200000                        # the most answers kept (the oldest go)


def load_answers() -> dict:
    """The solver's answers kept from earlier runs: a formula's text -> its answer. A formula that did not change (a
    function no file change touched) is not solved again."""
    path = folder() / ANSWERS
    try:
        return pickle.loads(path.read_bytes()) if path.exists() else {}
    except (OSError, pickle.UnpicklingError, EOFError, AttributeError, TypeError, ValueError, ImportError):
        return {}


def save_answers(answers: dict) -> None:
    kept = dict(list(answers.items())[-MOST_ANSWERS:])
    (folder() / ANSWERS).write_bytes(pickle.dumps(kept, protocol=pickle.HIGHEST_PROTOCOL))


def save(name: str, result) -> None:
    where = folder()
    (where / f"{name}.pickle").write_bytes(pickle.dumps(result, protocol=pickle.HIGHEST_PROTOCOL))
    saved = sorted(where.glob("*.pickle"), key=lambda p: p.stat().st_mtime, reverse=True)
    for old in saved[KEEP:]:
        old.unlink(missing_ok=True)
