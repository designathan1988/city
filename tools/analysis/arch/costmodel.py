"""What a trigger costs, composed through calls: a loop over state that runs a callee whose own walk over a member grows with the loop (or is the
loop's own range) costs the product, and a recomputation whose inputs the slice never changed is work done again. Each slice also gets its cost
vector: sized loops, the deepest composition, foreign calls inside loops, jobs submitted, members marked."""
from .contracts import FREQ_RANK, field_name, finding
from .entries import display
from .summaries import LINEAR, SIZING, _components


def _linear_sites(prog):
    """Every walk over a member (an algorithm over its range, a loop over it, a sort), each a bit; and for each function the walks it runs with all
    its callees."""
    sites, bit = [], {}
    own = {}
    for fid, f in prog.fn.items():
        mine = 0
        for e in f["ev"]:
            if e[0] == "c" and e[3] in LINEAR:
                key = (fid, e[1], e[4])
                if key not in bit:
                    bit[key] = 1 << len(sites)
                    sites.append(key)
                mine |= bit[key]
        own[fid] = mine
    closure = {}
    for comp in _components(prog.succ):
        acc = 0
        for fid in comp:
            acc |= own[fid]
            for t in prog.succ[fid]:
                if t not in comp:
                    acc |= closure[t]
        for fid in comp:
            closure[fid] = acc
    return sites, bit, closure


def _path(prog, start, goal):
    """The calls from one function down to another, by the first way found."""
    back, todo = {start: None}, [start]
    while todo:
        x = todo.pop(0)
        if x == goal:
            break
        for t in prog.succ.get(x, ()):
            if t not in back:
                back[t] = x
                todo.append(t)
    if goal not in back:
        return []
    out, x = [], goal
    while x is not None:
        out.append(x)
        x = back[x]
    return out[::-1]


def composed_quadratic(prog, slices=None):
    """A loop over state, and inside it (directly or in what it calls) a walk over a member that the loop's own work resizes or that is the loop's
    own range: the walk grows with the loop, so the trigger costs their product."""
    sites, bit, closure = _linear_sites(prog)
    found = {}
    for fid, f in sorted(prog.fn.items()):
        for lid, line, _, bound, _ in f["loops"]:
            if bound[0] in ("const", "open"):
                continue
            inside = [e for e in f["ev"] if lid in f["ctxs"][e[2]][0]]
            sized, walks, calls = 0, 0, []
            for e in inside:
                if e[0] == "c" and e[3] in SIZING:
                    sized |= prog.field_bit(e[4])
                elif e[0] == "c" and e[3] in LINEAR and not (e[3] == "iterate" and e[1] == line):
                    walks |= bit[(fid, e[1], e[4])]
                elif e[0] == "call":
                    for t in prog.dispatch(e[3], e[7]):
                        sized |= prog.S.get(t, 0)
                        walks |= closure.get(t, 0)
                        calls.append((t, e[1], closure.get(t, 0)))
            b = walks
            while b:
                low = b & -b
                b ^= low
                op_fn, op_line, field = sites[low.bit_length() - 1]
                if not (prog.field_bit(field) & sized or bound == ["field", field]):
                    continue
                key = (prog.fn[op_fn]["file"], op_line, field)
                if key in found:
                    found[key]["places"].append({"file": f["file"], "line": line, "what": "outer loop"})
                    continue
                callee, call_line = next(((c, at) for c, at, bits in calls if bits & low), (op_fn, op_line))
                chain = [op_fn] if callee == op_fn and op_fn == fid else _path(prog, callee, op_fn)
                proof = [(f["file"], line, f"loop over {_bound(prog, bound)} in {display(prog, fid)}"), (f["file"], call_line, "calls")]
                proof += [(prog.fn[x]["file"], prog.fn[x]["line"], display(prog, x)) for x in chain]
                owner = prog.cls.get(prog.field.get(field, (None,))[0], {}).get("name", "")
                found[key] = finding("composed_quadratic", key[0], op_line, f"{field_name(prog, field)} walked inside a loop that grows it or walks it",
                                     places=[(key[0], op_line, "inner walk"), (f["file"], line, "outer loop")], proof=proof, owner=owner,
                                     cost={"order": 2, "outer": _bound(prog, bound), "inner": field_name(prog, field)})
    return [found[k] for k in sorted(found)]


def _bound(prog, bound):
    if bound[0] == "field":
        return field_name(prog, bound[1])
    if bound[0] in ("param", "local"):
        return f"{bound[0]} {bound[1]}"
    return "a value read from state"


def channels(prog, slices):
    """Members many slices mark and a repeating slice drains: change channels. Writing one changes no input, reading one is no input."""
    marked_by = {}
    for eid, sl in slices.items():
        for m in sl.marked:
            marked_by.setdefault(m, set()).add(eid)
    drained = set().union(*(sl.drained for sl in slices.values() if sl.freq in ("frame", "tick")))
    bits = 0
    for m, users in marked_by.items():
        if len(users) >= 2 and m in drained:
            bits |= prog.field_bit(m)
    return bits


def verdicts(prog, slices):
    """Each candidate recomputation with the members it needs and the members written before it, channels left out of both."""
    chan = channels(prog, slices)
    for eid, sl in sorted(slices.items()):
        for c in sl.candidates:
            if prog.direct(prog.fn[c["callee"]])[2] & ~chan:
                continue
            yield eid, sl, c, c["inputs"] & ~chan, c["written"] & ~chan


def redone_work(prog, slices):
    """Recomputations a slice runs per item on inputs it never changed before them."""
    out, seen = [], set()
    for eid, sl, r, needs, written in verdicts(prog, slices):
        if not needs or needs & written or not written:
            continue
        key = (eid, r["site"], r["callee"])
        if key in seen:
            continue
        seen.add(key)
        proof = [(prog.fn[x]["file"], line, display(prog, x)) for x, line, _ in r["frames"]]
        inputs = [field_name(prog, x) for x in prog.fields_of(needs)][:6]
        owner = display(prog, r["frames"][-1][0]) if r["frames"] else ""
        out.append(finding("redone_work", r["site"][0], r["site"][1], f"{display(prog, r['callee'])} rebuilt from members {display(prog, eid)} did not change",
                           places=[(*r["site"], "recomputation")], proof=proof, owner=owner, entry=eid, cost={"inputs": inputs, "loops": r["loops"]}))
    return out


def cost_vector(prog, entry, sl):
    return {"freq": entry["freq"], "rank": FREQ_RANK.get(entry["freq"], 0), "functions": len(sl.functions), "sized_loops": sl.loops,
            "deepest": sl.deepest, "foreign_in_loops": len(sl.foreign_in_loops), "jobs": len(sl.submits),
            "jobs_unbudgeted": sum(1 for s in sl.submits if s["per_item"] and not s["drawn"]), "marks": sorted(field_name(prog, m) for m in sl.marked)[:12],
            "writes": bin(sl.written).count("1")}
