"""The new core's analysis run by the native engine (log/trace-engine-plan.md, Task E5): the index written as the
engine's input (`engine_input`), the engine run under a deadline with its lines handed on as they come (`engine`), and
its result read back as what the walk and the report read — the same `out`, `inn`, `edges`, `returned` and `points` as
`heap.analysis.Result`, the objects named by the same labels."""
from __future__ import annotations

import pickle
import tempfile
from collections import defaultdict
from pathlib import Path

from . import engine, engine_input
from .heap.objects import MADE
from .resolve import EXACT, Edge
from .vocab import NAME

DEADLINE = 600                   # the whole of a large program (the lab solves in seconds; a run past it is a bug)


class EngineResult:
    def __init__(self, ix, data: dict):
        self.ix = ix
        self.edges, self.out, self.inn = [], defaultdict(list), defaultdict(list)
        self.external, self.unreached = defaultdict(set), defaultdict(set)
        for src, dst, kind, line in data["links"]:
            edge = Edge(src, dst, kind, line, (), EXACT, ix.units[src].file)
            self.edges.append(edge)
            self.out[src].append(edge)
            self.inn[dst].append(edge)
        self.objects = data["objects"]
        self.returned_by = data["returned"]
        self.names = data["names"]
        self.missing = data["missing"]
        self.runs_of = data["runs"]

    def label(self, obj: int) -> str:
        """A readable name, as `heap.objects.Objects.label` makes it: its kind, then what identifies its site."""
        key = self.objects[obj]
        parts = key[1:-1] if key[0] in MADE else key[1:]
        return "{}:{}".format(key[0], ":".join(str(part) for part in parts))

    def runs(self, gid: int) -> int:
        """How many runs (contexts) a function had."""
        return self.runs_of.get(gid, 0)

    def returned(self, gid: int) -> set[str]:
        return {self.label(o) for o in self.returned_by.get(gid, ())}

    def points(self, gid: int, tree) -> set[str]:
        """What a name read in a function holds, in every context it ran in (the engine kept them: `names`)."""
        if not (isinstance(tree, tuple) and len(tree) == 2 and tree[0] == NAME):
            raise ValueError(f"the engine keeps the values of names only, not of {tree!r}")
        return {self.label(o) for o in self.names.get((gid, tree[1]), ())}


def analyse(ix, progress=None, contexts: bool = True, names: bool = False, deadline: float = DEADLINE,
            collapse_every: int | None = None) -> EngineResult:
    """The analysis of the index by the engine, every line it prints handed to `progress`; with `names`, what each
    local and name read holds (`points`); `collapse_every`: the items between two collapses of cycles (the engine's
    own choice when not given)."""
    progress = progress or (lambda line: None)
    with tempfile.TemporaryDirectory() as folder:
        source, out = Path(folder) / "input.pickle", Path(folder) / "output.pickle"
        engine_input.write(ix, source)
        flags = ([] if contexts else ["--no-contexts"]) + (["--names"] if names else []) + (
            ["--collapse-every", str(collapse_every)] if collapse_every else [])
        engine.run(["analyse", str(source), str(out)] + flags, deadline, progress)
        result = EngineResult(ix, pickle.loads(out.read_bytes()))
    if result.missing:
        raise engine.EngineError(f"the engine asked for {result.missing} resolutions the input does not have")
    return result
