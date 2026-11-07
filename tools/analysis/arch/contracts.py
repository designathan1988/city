"""Contracts the code must keep, each checked by its own function: a budget refuses a draw when it has nothing; every job a slice submits per item
is admitted by the budget the other consumers share; a key leaves the dirty set only with a newer job asked for it; a cache key covers what its
value is made of and its storage lives no longer than that state; a size read from bytes is bounded before it allocates."""
from .entries import display

FREQ_RANK = {"frame": 4, "tick": 3, "input": 2, "event": 1, "job": 1, "thread": 1}
CLAMPS = {"lang:.min", "lang:.clamp"}  # the language's own clamps: a value through one is bounded


def finding(cls, file, line, detail, places=(), proof=(), owner="", entry=None, cost=None):
    return {"class": cls, "file": file, "line": line, "detail": detail, "places": [{"file": p[0], "line": p[1], "what": p[2]} for p in places],
            "proof": [{"file": p[0], "line": p[1], "what": p[2]} for p in proof], "owner": owner, "entry": entry, "cost": cost or {}}


def field_name(prog, fid):
    owner, rec = prog.field.get(fid, (None, None))
    if rec is None:
        return fid.rsplit("@", 1)[-1]
    return f"{prog.cls[owner]['name']}::{rec[1]}"


def _value(t, env, params):
    kind = t[0]
    if kind == "lit":
        return t[1]
    if kind == "f":
        return env.get(t[1])
    if kind == "p":
        return params.get(t[1])
    if kind == "u":
        x = _value(t[2], env, params)
        return None if x is None else int(not x) if t[1] == "!" else -x if t[1] == "-" else ~x if t[1] == "~" else None
    if kind == "?:":
        c = _value(t[1], env, params)
        return None if c is None else _value(t[2] if c else t[3], env, params)
    if kind != "b":
        return None
    a, b = _value(t[2], env, params), _value(t[3], env, params)
    op = t[1]
    if op in ("LAnd", "&&"):
        return 0 if a == 0 or b == 0 else None if a is None or b is None else int(bool(a and b))
    if op in ("LOr", "||"):
        return 1 if a or b else None if a is None or b is None else 0
    if a is None or b is None:
        return None
    table = {"Add": lambda: a + b, "Sub": lambda: a - b, "Mul": lambda: a * b, "GT": lambda: int(a > b), "LT": lambda: int(a < b), "GE": lambda: int(a >= b),
             "LE": lambda: int(a <= b), "EQ": lambda: int(a == b), "NE": lambda: int(a != b), "And": lambda: a & b, "Or": lambda: a | b, "Xor": lambda: a ^ b}
    return table[op]() if op in table else None


def budget_boundary(prog, slices=None):
    """Built with nothing to give (every constructor argument 0), a budget must refuse a draw of 1."""
    out = []
    for did in sorted(prog.draws):
        d = prog.fn[did]
        cls = prog.cls.get(d["cls"])
        if cls is None:
            continue
        defaults = {f[0]: _value(f[6], {}, {}) if f[6] else None for f in cls["fields"]}
        for ctor in sorted((f for f in prog.fn.values() if f["kind"] == "ctor" and f["cls"] == d["cls"] and f["params"]), key=lambda f: f["id"]):
            zero = {i: 0 for i in range(len(ctor["params"]))}
            env = dict(defaults)
            for fid, tree in ctor["inits"]:
                env[fid] = _value(tree, env, zero)
            one = {i: 1 for i in range(len(d["params"]))}
            verdicts = [_value(t, env, one) for t in d["rejects"]]
            if verdicts and all(v == 0 for v in verdicts):
                line = d["guards"][0][1] if d["guards"] else d["line"]
                sig = ", ".join(p[0] for p in ctor["params"])
                out.append(finding("budget_boundary", d["file"], line, f"{display(prog, did)} admits a draw of 1 when built with 0 ({sig})",
                                   places=[(d["file"], line, "admission test"), (ctor["file"], ctor["line"], "constructor at zero")], owner=cls["name"]))
    return out


def unbudgeted_submit(prog, slices):
    """Where consumers of one slice share a budget, a job submitted per item with no draw of it dominating the way there is outside the quota."""
    out = []
    for eid, sl in sorted(slices.items()):
        consumers = {}
        for fid, _, callee in sl.draws:
            consumers.setdefault(prog.fn[callee]["cls"], set()).add(fid)
        shared = sorted(c for c, users in consumers.items() if len(users) >= 2)
        if not shared:
            continue
        names = "/".join(prog.cls[c]["name"] for c in shared if c in prog.cls)
        seen = set()
        for s in sl.submits:
            if s["drawn"] or not s["per_item"]:
                continue
            frame = next((fr for fr in reversed(s["frames"]) if fr[2]), s["frames"][-1])
            fn = prog.fn[frame[0]]
            if (fn["file"], frame[1]) in seen:
                continue
            seen.add((fn["file"], frame[1]))
            proof = [(prog.fn[x]["file"], line, display(prog, x)) for x, line, _ in s["frames"]]
            out.append(finding("unbudgeted_submit", fn["file"], frame[1], f"jobs per item outside the shared {names} quota in {display(prog, eid)}",
                               places=[(fn["file"], frame[1], "per-item loop"), (*s["site"], "job submitted")], proof=proof, owner=names, entry=eid))
    return out


def _in_flight(prog, f, atom, key, before):
    """The member a condition tests the key's membership of, through lambdas, locals and comparisons."""
    kind = atom[0]
    if kind == "c" and atom[1] in ("count", "find", "contains") and atom[3] == key:
        return atom[2]
    if kind == "cmp":
        return _in_flight(prog, f, atom[2], key, before) or _in_flight(prog, f, atom[3], key, before)
    if kind == "lcall" and atom[1] in prog.fn and atom[2] and atom[2][0] == key:
        lam = prog.fn[atom[1]]
        for e in lam["ev"]:
            if e[0] == "ret" and len(e) > 4:
                found = _in_flight(prog, lam, e[4], ["p", 0], e[1])
                if found:
                    return found
    if kind == "v":
        sets = [e for e in f["ev"] if e[0] == "set" and e[3] == atom[1] and e[1] <= before and len(e) > 5]
        if sets:
            return _in_flight(prog, f, sets[-1][5], key, before)
    return None


def _submitting(prog):
    out = {fid for fid, f in prog.fn.items() if prog.submits(f)}
    callers = {}
    for fid, succ in prog.succ.items():
        for t in succ:
            callers.setdefault(t, set()).add(fid)
    todo = list(out)
    while todo:
        x = todo.pop()
        for c in callers.get(x, ()):
            if c not in out:
                out.add(c)
                todo.append(c)
    return out


def lost_update(prog, slices=None):
    """A key erased from the set being walked, on a branch where the record of work in flight holds it, with no newer job asked: the change that
    made it dirty is never built."""
    out, submitting = [], _submitting(prog)
    for fid, f in sorted(prog.fn.items()):
        for e in f["ev"]:
            if e[0] != "c" or e[3] != "erase" or not e[5] or e[5][0] != "v":
                continue
            loops, doms = f["ctxs"][e[2]]
            if not any(f["loops"][lid][3] == ["field", e[4]] for lid in loops):
                continue
            for d in doms:
                g = f["guards"][abs(d) - 1]
                if d < 0 or g[2] == "and":
                    continue
                for atom in g[3]:
                    if atom[-1]:
                        continue
                    held = _in_flight(prog, f, atom[:-1], e[5], e[1])
                    if not held or held not in prog.registries:
                        continue
                    branch = [x for x in f["ev"] if d in f["ctxs"][x[2]][1]]
                    renewed = any(x[0] == "call" and any(t in submitting for t in prog.dispatch(x[3], x[7])) for x in branch)
                    renewed |= any(x[0] == "c" and x[3] == "insert" and x[4] == e[4] for x in branch)
                    if not renewed:
                        owner = prog.field.get(e[4], (None,))[0]
                        out.append(finding("lost_update", f["file"], e[1], f"{field_name(prog, e[4])} erased while {field_name(prog, held)} holds the key",
                                           places=[(f["file"], e[1], "erase"), (f["file"], g[1], "in-flight test")], owner=prog.cls.get(owner, {}).get("name", "")))
    return out


def _memos(prog):
    """Methods that look a key up in a member and, missing, make the value and file it under the same key."""
    for fid, f in sorted(prog.fn.items()):
        if not f["cls"]:
            continue
        finds = {(e[4], e[5][1]) for e in f["ev"] if e[0] == "c" and e[3] == "find" and e[5] and e[5][0] == "v"}
        written = {e[3] for e in f["ev"] if e[0] == "w"}
        for e in f["ev"]:
            if e[0] == "c" and e[3] in ("index", "insert") and e[5] and e[5][0] == "v" and (e[4], e[5][1]) in finds and e[4] in written:
                sets = [x for x in f["ev"] if x[0] == "set" and x[3] == e[5][1]]
                kf = next((s[1] for x in sets for s in x[4] if s[0] == "call" and s[1] in prog.fn), None)
                yield f, e[4], e[5][1], kf
                break


def _lossy(prog, kf):
    f = prog.fn[kf]
    ops = set(f.get("ops", ()))
    mixes = bool(ops & {"Xor", "XorAssign"}) or bool(ops & {"Mul", "MulAssign"}) and bool(ops & {"Shl", "ShlAssign", "Add", "AddAssign", "Or", "OrAssign"})
    owners = {}
    for e in f["ev"]:
        if e[0] == "r" and e[3] in prog.field and prog.field[e[3]][0] != f["cls"]:
            owners.setdefault(prog.field[e[3]][0], set()).add(e[3])
    source = max(sorted(owners), key=lambda c: len(owners[c]), default=None)
    return mixes and f["returns"] and source is not None and len(owners[source]) >= 2, source


def cache_identity(prog, slices=None):
    """A memo's key must name its value: a hash that mixes members into fewer bits can name two values, a key that leaves out a member the value
    is made of returns a stale one, and a cache in thread or static storage outlives every reset of the state it was filled from."""
    out, memo_classes = [], set()
    for f, field, var, kf in _memos(prog):
        memo_classes.add(f["cls"])
        if kf is None:
            continue
        k = prog.fn[kf]
        lossy, source = _lossy(prog, kf)
        cname = prog.cls[f["cls"]]["name"]
        if lossy:
            out.append(finding("cache_identity", k["file"], k["line"], f"key of {field_name(prog, field)} hashes {display(prog, kf)}: two values can share it",
                               places=[(k["file"], k["line"], "key"), (f["file"], f["line"], "memo")], owner=cname))
        if source is None:
            continue
        mine = {fid for fid, (owner, _) in prog.field.items() if owner == source}
        keyed = set(prog.fields_of(prog.R.get(kf, 0))) & mine
        made = set()
        for e in f["ev"]:
            if e[0] == "call" and e[3] != kf and e[3] in prog.fn and any(a[0] in ("p", "v") for a in e[4]):
                made |= set(prog.fields_of(prog.R.get(e[3], 0))) & mine
        for miss in sorted(made - keyed):
            out.append(finding("cache_identity", k["file"], k["line"], f"key of {field_name(prog, field)} misses {field_name(prog, miss)}",
                               places=[(k["file"], k["line"], "key"), (f["file"], f["line"], "memo")], owner=cname))
    for fid, f in sorted(prog.fn.items()):
        for e in f["ev"]:
            if e[0] == "tl" and e[4] in memo_classes:
                out.append(finding("cache_identity", f["file"], e[1], f"{prog.cls[e[4]]['name']} cache in {e[5]} storage outlives the state it caches",
                                   places=[(f["file"], e[1], f"{e[5]} storage")], owner=prog.cls[e[4]]["name"]))
    return out


def taint_alloc(prog, slices=None):
    """A size or count read from bytes reaches an allocation with no comparison that leaves on the way."""
    out = []
    for fid, f in sorted(prog.fn.items()):
        tainted = {}
        for e in f["ev"]:
            if e[0] == "set":
                src = [s for s in e[4] if s[0] == "call" and s[1] in prog.readers or s[0] == "v" and s[1] in tainted]
                clamp = any(s[0] == "call" and s[1] in CLAMPS for s in e[4])
                if src and not clamp:
                    tainted[e[3]] = src[0][1] if src[0][0] == "call" else tainted[src[0][1]]
                else:
                    tainted.pop(e[3], None)
            elif e[0] == "alloc":
                direct = [s[1] for s in e[3] if s[0] == "call" and s[1] in prog.readers]
                via = [s[1] for s in e[3] if s[0] == "v" and s[1] in tainted]
                if not direct and not via or any(s[0] == "call" and s[1] in CLAMPS for s in e[3]):
                    continue
                if via and _bounded(f, e, set(via)):
                    continue
                reader = direct[0] if direct else tainted[via[0]]
                owner = prog.cls.get(prog.fn[reader]["cls"], {}).get("name", "")
                out.append(finding("taint_alloc", f["file"], e[1], f"allocation sized by {display(prog, reader)} with no bound",
                                   places=[(f["file"], e[1], "allocation"), (prog.fn[reader]["file"], prog.fn[reader]["line"], "bytes read")], owner=owner))
    return out


def _bounded(f, e, names):
    for d in f["ctxs"][e[2]][1]:
        g = f["guards"][abs(d) - 1]
        compared = any(a[0] == "cmp" and ({x[1] for x in (a[2], a[3]) if x[0] == "v"} & names) for a in g[3])
        if compared and (d < 0 and g[4] and g[2] in ("or", "one") or d > 0 and g[2] in ("and", "one")):
            return True
    return False
