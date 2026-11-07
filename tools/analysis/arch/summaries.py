"""Function summaries joined into one program: who calls whom and by which mechanism, which callable parameters a function runs and which it
keeps, the members each function reads and writes with everything it calls (as bit sets), and the roles the code itself shows: readers of bytes,
budget draws, deferring calls (job submission), registries of work in flight, memo caches."""
import collections

MUTATING = {"insert", "erase", "clear", "resize", "reserve", "swap"}
SIZING = {"insert", "erase", "clear", "resize"}
LINEAR = {"linear", "iterate", "sort"}


class Program:
    def __init__(self, summaries):
        self.fn, self.cls, self.glob = {}, {}, {}
        for s in summaries:
            table = self.cls if s["kind"] == "class" else self.glob if s["kind"] == "global" else self.fn
            table.setdefault(s["id"], s)
        self.field = {}
        for c in self.cls.values():
            for f in c["fields"]:
                self.field.setdefault(f[0], (c["id"], f))
        self.bit, self._names = {}, []
        self.overriders = collections.defaultdict(set)
        for f in self.fn.values():
            for o in f["over"]:
                self.overriders[o].add(f["id"])
        self.children = collections.defaultdict(list)
        for f in self.fn.values():
            if f["kind"] == "lambda":
                self.children[f["parent"]].append(f["id"])
        self.binds = sorted({(name, fid) for f in self.fn.values() for name, fid in f["binds"] if fid in self.fn})
        self._uses()
        self._closures()
        self.readers = self._readers()
        self.draws = {f["id"] for f in self.fn.values() if self._is_draw(f)}
        self.registries = self._registries()

    # -- calls ---------------------------------------------------------------------------------------------------------------------------------
    def dispatch(self, callee, virtual):
        """The bodies a call may run: the one named, and every override of it when the call is virtual."""
        out, todo = [], [callee]
        while todo:
            c = todo.pop()
            if c in self.fn and c not in out:
                out.append(c)
            if virtual:
                todo.extend(sorted(self.overriders.get(c, ())))
        return out

    def runs(self, callee, i):
        """Does the callee run the callable it gets as argument i before it returns? Its own summary says: it calls the parameter."""
        if callee.startswith("lang:"):
            return True
        use = self.use.get(callee, {}).get(i, set())
        return "called" in use

    def keeps(self, callee, i):
        if callee.startswith("lang:"):
            return False
        use = self.use.get(callee, {}).get(i, set())
        return "stored" in use and "called" not in use

    def sync_edges(self, f):
        """(event, target) for every body a function runs before it returns, context-free: calls, local lambdas, lambdas handed to a callee that runs
        them."""
        out = []
        for e in f["ev"]:
            kind = e[0]
            if kind == "call":
                for t in self.dispatch(e[3], e[7]):
                    out.append((e, t))
                for i, a in enumerate(e[4]):
                    if a[0] == "l" and self.runs(e[3], i):
                        out.append((e, a[1]))
                    elif a[0] == "m" and self.runs(e[3], i) and a[1] in self.fn:
                        out.append((e, a[1]))
            elif kind == "vcall" and e[3] in self.fn:
                out.append((e, e[3]))
        return out

    def _uses(self):
        """Fixed point: a parameter is called when the function or a lambda it runs calls it, or hands it to a callee that does; it is kept when
        written into a member, handed to a callee that keeps it, or captured by a lambda that is kept."""
        use = collections.defaultdict(lambda: collections.defaultdict(set))
        for f in self.fn.values():
            for e in f["ev"]:
                if e[0] == "pcall":
                    use[e[3]][e[4]].add("called")
                elif e[0] == "store":
                    use[f["id"]][e[3]].add("stored")
                elif e[0] == "ostore":
                    use[e[3]][e[4]].add("stored")
        self.use = use
        changed = True
        while changed:
            changed = False
            for f in self.fn.values():
                fid = f["id"]
                for e in f["ev"]:
                    if e[0] == "call":
                        for i, a in enumerate(e[4]):
                            if a[0] in ("p", "o"):
                                owner, j = (fid, a[1]) if a[0] == "p" else (a[1], a[2])
                                for t in self.dispatch(e[3], e[7]) or [e[3]]:
                                    for mark in use.get(t, {}).get(i, set()) - use[owner][j]:
                                        use[owner][j].add(mark)
                                        changed = True
                            elif a[0] == "l" and self.keeps(e[3], i):
                                changed |= self._kept_lambda(a[1])
                    elif e[0] in ("c", "thread"):
                        descs = [e[5]] if e[0] == "c" and e[5] else e[3] if e[0] == "thread" else []
                        for a in descs:
                            if a and a[0] == "l":
                                changed |= self._kept_lambda(a[1])

    def _kept_lambda(self, lid):
        """A lambda kept for later keeps what it uses of its enclosing function's parameters."""
        lam = self.fn.get(lid)
        if lam is None:
            return False
        changed = False
        for e in lam["ev"]:
            if e[0] in ("opr", "pcall") and e[3] != lid:
                if "stored" not in self.use[e[3]][e[4]]:
                    self.use[e[3]][e[4]].add("stored")
                    changed = True
        return changed

    # -- members ---------------------------------------------------------------------------------------------------------------------------------
    def field_bit(self, field):
        b = self.bit.get(field)
        if b is None:
            b = self.bit[field] = 1 << len(self.bit)
        return b

    def fields_of(self, bits):
        if len(self._names) != len(self.bit):
            self._names = [None] * len(self.bit)
            for k, v in self.bit.items():
                self._names[v.bit_length() - 1] = k
        out = []
        while bits:
            low = bits & -bits
            bits ^= low
            out.append(self._names[low.bit_length() - 1])
        return out

    def direct(self, f):
        reads = writes = sized = 0
        for e in f["ev"]:
            kind = e[0]
            if kind == "r":
                reads |= self.field_bit(e[3])
            elif kind == "w":
                writes |= self.field_bit(e[3])
            elif kind == "c":
                b = self.field_bit(e[4])
                if e[3] in MUTATING:
                    writes |= b
                    if e[3] in SIZING:
                        sized |= b
                else:
                    reads |= b
        return reads, writes, sized

    def _closures(self):
        """Reads, writes and resized members of each function with everything it runs, over the strongly connected parts of the call graph."""
        succ = {fid: sorted({t for _, t in self.sync_edges(f)}) for fid, f in self.fn.items()}
        self.succ = succ
        own = {fid: self.direct(f) for fid, f in self.fn.items()}
        self.R, self.W, self.S = {}, {}, {}
        for comp in _components(succ):
            r = w = s = 0
            for fid in comp:
                a, b, c = own[fid]
                r, w, s = r | a, w | b, s | c
                for t in succ[fid]:
                    if t not in comp:
                        r, w, s = r | self.R[t], w | self.W[t], s | self.S[t]
            for fid in comp:
                self.R[fid], self.W[fid], self.S[fid] = r, w, s

    def inputs(self, fid):
        """What a function reads and does not itself produce."""
        return self.R.get(fid, 0) & ~self.W.get(fid, 0)

    # -- roles -----------------------------------------------------------------------------------------------------------------------------------
    def _readers(self):
        """Functions that hand out bytes taken from a buffer they walk: a call of the language copies from members of the function's class at an
        offset member the function itself advances. A function returning what a reader returned is a reader too."""
        out = set()
        for f in self.fn.values():
            mine = {fid for fid, (owner, _) in self.field.items() if owner == f["cls"]} if f["cls"] else set()
            written = {e[3] for e in f["ev"] if e[0] == "w"}
            for e in f["ev"]:
                if e[0] == "call" and e[3].startswith("lang:"):
                    touched = {x for a in e[4] if a[0] == "x" for x in a[1]} & mine
                    if len(touched) >= 2 and touched & written:
                        out.add(f["id"])
        changed = True
        while changed:
            changed = False
            for f in self.fn.values():
                if f["id"] not in out and any(e[0] == "ret" and any(s[0] == "call" and s[1] in out for s in e[3]) for e in f["ev"]):
                    out.add(f["id"])
                    changed = True
        return out

    def _is_draw(self, f):
        """A budget draw: answers yes or no, says no on a test of its own members, and on yes moves a member it keeps count with."""
        if not f["cls"] or not f["rejects"] or ["lit", 1] not in f["returns"]:
            return False
        mine = {fid for fid, (owner, _) in self.field.items() if owner == f["cls"]}
        moved = {a[0] for a in f["assigns"] if a[0] in mine}
        tested = {x for t in f["rejects"] for x in _fields(t)}
        return bool(moved & tested)

    def _registries(self):
        """Members a function fills with a key in the same body that submits a job: the record of work in flight, by key."""
        out = collections.defaultdict(set)
        for f in self.fn.values():
            if not self.submits(f):
                continue
            for e in f["ev"]:
                if e[0] == "c" and e[3] in ("index", "insert") and e[5]:
                    out[e[4]].add(f["id"])
        return out

    def submits(self, f):
        """Does a function hand a lambda to a callee that keeps it (a job, run later elsewhere)?"""
        return any(e[0] == "call" and any(a[0] == "l" and self.keeps(e[3], i) for i, a in enumerate(e[4])) for e in f["ev"])

    def deferred(self):
        """Every lambda that runs later: handed to a callee that keeps it, kept in a member container, or started as a thread."""
        out = {}
        for f in self.fn.values():
            for e in f["ev"]:
                if e[0] == "call":
                    for i, a in enumerate(e[4]):
                        if a[0] in ("l", "m") and self.keeps(e[3], i) and a[1] in self.fn:
                            out.setdefault(a[1], ("job", f["id"], e))
                elif e[0] == "thread":
                    for a in e[3]:
                        if a[0] in ("l", "m") and a[1] in self.fn:
                            out.setdefault(a[1], ("thread", f["id"], e))
                elif e[0] == "c" and e[5] and e[5][0] == "l" and e[5][1] in self.fn and e[3] == "insert":
                    out.setdefault(e[5][1], ("job", f["id"], e))
        return out


def _fields(tree):
    if not isinstance(tree, list) or not tree:
        return []
    if tree[0] == "f":
        return [tree[1]]
    return [x for part in tree[1:] for x in _fields(part)]


def _components(succ):
    """Strongly connected parts, callees before callers (Tarjan, without recursion)."""
    index, low, on, stack, out, counter = {}, {}, set(), [], [], [0]
    for root in sorted(succ):
        if root in index:
            continue
        work = [(root, iter(succ[root]))]
        index[root] = low[root] = counter[0]
        counter[0] += 1
        stack.append(root)
        on.add(root)
        while work:
            node, it = work[-1]
            advanced = False
            for nxt in it:
                if nxt not in succ:
                    continue
                if nxt not in index:
                    index[nxt] = low[nxt] = counter[0]
                    counter[0] += 1
                    stack.append(nxt)
                    on.add(nxt)
                    work.append((nxt, iter(succ[nxt])))
                    advanced = True
                    break
                if nxt in on:
                    low[node] = min(low[node], index[nxt])
            if advanced:
                continue
            work.pop()
            if work:
                low[work[-1][0]] = min(low[work[-1][0]], low[node])
            if low[node] == index[node]:
                comp = set()
                while True:
                    x = stack.pop()
                    on.discard(x)
                    comp.add(x)
                    if x == node:
                        break
                out.append(comp)
    return out
