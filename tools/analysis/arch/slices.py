"""The feature slice of one entry: everything its call runs before returning, in program order, with the callables it hands over bound to what
they are. Along the way it keeps what was written so far, which budget draws dominate each step, how many state-sized loops enclose it, the jobs it
submits, the members it marks and the recomputations whose inputs nothing before them changed."""
import sys

MUTATING = {"insert", "erase", "clear", "resize", "reserve", "swap"}


class Slice:
    def __init__(self, prog, entry):
        self.prog, self.entry = prog, entry
        self.visited, self.order, self.functions, self.first = set(), [], set(), {}
        self.written = 0
        self.read = 0
        self.marked = set()
        self.submits, self.candidates, self.draws, self.foreign_in_loops = [], [], [], []
        self.drained = set()
        self.loops, self.deepest = 0, 0
        self.trace = []  # (site, members its calls write, depth) in the order calls start: what fed a later step
        self.tracing = False  # when set, `trace` is kept: what fed each candidate

    def run(self):
        sys.setrecursionlimit(max(sys.getrecursionlimit(), 50000))
        self.walk(self.entry, {}, False, (), ())
        return self

    def site(self, fid, line):
        return (self.prog.fn[fid]["file"], line)

    def dominated_by_draw(self, f, doms):
        for d in doms:
            g = f["guards"][abs(d) - 1]
            atoms_op, atoms = g[2], g[3]
            for a in atoms:
                if a[0] == "call" and a[1] in self.prog.draws:
                    neg = a[-1]
                    if d > 0 and atoms_op in ("one", "and") and not neg or d < 0 and atoms_op in ("one", "or") and neg:
                        return True
        return False

    def sized_loops(self, f, loops):
        return [lid for lid in loops if f["loops"][lid][3][0] != "const"]

    def walk(self, fid, bind, drawn, frames, outer):
        """`frames`: (function, line, sized loops there) from the entry down; `outer`: the sized loops around this call, from the callers."""
        prog = self.prog
        key = (fid, tuple(sorted((k, v) for k, v in bind.items() if k[0] == fid or k[0] in self._owners(fid))), drawn)
        if key in self.visited:
            self.written |= prog.W.get(fid, 0)
            return
        self.visited.add(key)
        if fid not in self.functions:
            self.functions.add(fid)
            self.order.append(fid)
            self.first[fid] = frames
        f = prog.fn[fid]
        for e in f["ev"]:
            kind = e[0]
            loops, doms = f["ctxs"][e[2]]
            sized = self.sized_loops(f, loops)
            if kind == "w" or kind == "c" and e[3] in MUTATING:
                field = e[3] if kind == "w" else e[4]
                self.written |= prog.field_bit(field)
                if kind == "c" and e[3] == "insert":
                    self.marked.add(field)
            elif kind == "r" or kind == "c":
                self.read |= prog.field_bit(e[3] if kind == "r" else e[4])
            if kind == "c" and e[3] in ("clear", "swap"):
                self.drained.add(e[4])
            if kind == "call":
                self.call(f, e, bind, drawn, frames, outer, sized, doms)
            elif kind == "pcall":
                target = bind.get((e[3], e[4]))
                if target is not None and target in prog.fn:
                    here = (*frames, (fid, e[1], tuple(sized)))
                    self.walk(target, bind, drawn or self.dominated_by_draw(f, doms), here, outer + tuple(sized))
            elif kind == "vcall" and e[3] in prog.fn:
                self.walk(e[3], bind, drawn or self.dominated_by_draw(f, doms), (*frames, (fid, e[1], tuple(sized))), outer + tuple(sized))
        if f["loops"]:
            mine = [lp for lp in f["loops"] if lp[3][0] != "const"]
            self.loops += len(mine)
            self.deepest = max(self.deepest, len(outer) + max((self._depth(f, lp[0]) for lp in mine), default=0))

    def _depth(self, f, lid):
        depth = 0
        while lid != -1:
            lp = f["loops"][lid]
            depth += lp[3][0] != "const"
            lid = lp[2]
        return depth

    def _owners(self, fid):
        out, f = [], self.prog.fn.get(fid)
        while f is not None and f["kind"] == "lambda":
            out.append(f["parent"])
            f = self.prog.fn.get(f["parent"])
        return out

    def call(self, f, e, bind, drawn, frames, outer, sized, doms):
        prog = self.prog
        callee, args = e[3], e[4]
        here_drawn = drawn or self.dominated_by_draw(f, doms)
        here = (*frames, (f["id"], e[1], tuple(sized)))
        depth = outer + tuple(sized)
        if callee in prog.draws:
            self.draws.append((f["id"], e[1], callee))
        if callee.startswith("ext:") and depth:
            self.foreign_in_loops.append((self.site(f["id"], e[1]), callee))
        for i, a in enumerate(args):
            if a[0] in ("l", "m") and prog.keeps(callee, i):
                self.submits.append({"site": self.site(f["id"], e[1]), "job": a[1], "drawn": here_drawn, "per_item": bool(depth), "frames": here})
        targets = prog.dispatch(callee, e[7])
        if targets:
            self.recompute(f, e, targets[0], here, depth)
        made = 0
        for t in targets:
            made |= prog.W.get(t, 0)
        if made and self.tracing:
            self.trace.append(((f["id"], e[1]), made, len(here)))
        for t in targets:
            nb = dict(bind)
            for i, a in enumerate(args):
                bound = None
                if a[0] in ("l", "m"):
                    bound = a[1]
                elif a[0] == "p":
                    bound = bind.get((f["id"], a[1]))
                elif a[0] == "o":
                    bound = bind.get((a[1], a[2]))
                if bound is not None:
                    nb[(t, i)] = bound
            self.walk(t, nb, here_drawn, here, depth)
        if callee.startswith("lang:"):
            for a in args:
                if a[0] == "l" and a[1] in prog.fn:
                    self.walk(a[1], bind, here_drawn, here, depth)

    def recompute(self, f, e, target, frames, depth):
        """A call per item that rebuilds what it produces from members it does not produce, selected by numbers or handed the state, keeping no count
        of its own: a candidate. Which members are channels (marks for later consumers) is known only once every slice is walked, so the verdict
        waits; what was written so far is kept with it."""
        prog = self.prog
        t = prog.fn[target]
        if not depth or not t["params"] or t["kind"] == "lambda" or t.get("const") or t["assigns"] or any(p[2] == "data" for p in t["params"] if len(p) > 2):
            return
        inputs, produced = prog.inputs(target), prog.W.get(target, 0)
        if inputs and produced:
            self.candidates.append({"site": self.site(f["id"], e[1]), "callee": target, "frames": frames, "inputs": inputs, "written": self.written,
                                    "loops": len(depth), "trace": len(self.trace)})


def build_all(prog, entries, tracing=False):
    out = {}
    for e in entries:
        sl = Slice(prog, e["id"])
        sl.freq, sl.tracing = e["freq"], tracing
        out[e["id"]] = sl.run()
    return out
