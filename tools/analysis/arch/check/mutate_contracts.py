"""Mutations that break a contract in real code of the analysed commit, one per class: relax a budget's refusal at its boundary, drop a budget
draw before per-item jobs, erase a dirty key on the branch where its job is in flight, drop a member from a cache key, remove the comparison that
bounds a size read from bytes. Each finds its site from the program's structure and names where the new finding must appear."""
import re

from ..contracts import _in_flight, _lossy, _memos
from .mutate_text import NoSite, inside, line_span, mutation, replace_in_line

RELATION = re.compile(rb"(?<![<>=!\-])(<=|>=|<|>)(?![<>=])")
FLIP = {b"<=": b"<", b">=": b">", b"<": b"<=", b">": b">="}


def relax_admission(c):
    """Turn the comparison of a healthy budget's refusal by one step (relational operator replacement): at zero it no longer refuses."""
    for did in sorted(c.prog.draws):
        d = c.prog.fn[did]
        if not d["guards"] or inside(c.findings, "budget_boundary", d["file"], d["line"], d["end"]):
            continue
        line = d["guards"][0][1]
        text = c.text(d["file"])
        new = replace_in_line(text, line, RELATION, lambda m: FLIP[m.group(1)])
        return mutation(d["file"], text, new, (d["file"], d["line"], d["end"]), f"refusal of {d['name']} at line {line} turned by one step")
    raise NoSite("no healthy budget draw")


def drop_draw(c):
    """Remove the draw that admits per-item jobs in one consumer of a shared budget: replace the draw in its guard by the answer `yes`."""
    for eid, sl in sorted(c.slices().items()):
        for s in sl.submits:
            if not s["drawn"] or not s["per_item"]:
                continue
            for fid, line, sized in reversed(s["frames"]):
                f = c.prog.fn[fid]
                ctx = next((e for e in f["ev"] if e[1] == line and e[0] in ("call", "pcall", "vcall")), None)
                if ctx is None or not sized or inside(c.findings, "unbudgeted_submit", f["file"], f["line"], f["end"]):
                    continue
                for d in f["ctxs"][ctx[2]][1]:
                    g = f["guards"][abs(d) - 1]
                    for a in g[3]:
                        if a[0] == "call" and a[1] in c.prog.draws:
                            name = c.prog.fn[a[1]]["name"].encode()
                            pattern = re.compile(rb"!\s*[A-Za-z_][\w.\[\]]*(?:->|\.)" + re.escape(name) + rb"\s*\([^()]*\)" if a[-1] else
                                                 rb"[A-Za-z_][\w.\[\]]*(?:->|\.)" + re.escape(name) + rb"\s*\([^()]*\)")
                            text = c.text(f["file"])
                            new = replace_in_line(text, g[1], pattern, b"false" if a[-1] else b"true")
                            return mutation(f["file"], text, new, (f["file"], f["line"], f["end"]), f"draw at line {g[1]} dropped")
                break
    raise NoSite("no per-item job admitted by a shared budget")


def erase_in_flight(c):
    """On a branch that keeps a key while its job is in flight, erase it from the set being walked instead."""
    for fid, f in sorted(c.prog.fn.items()):
        for lid, line, _, bound, var in f["loops"]:
            if bound[0] != "field" or not var:
                continue
            for gid, gline, op, atoms, leaves in f["guards"]:
                if op == "and" or not leaves or gline <= line or gline > f["end"]:
                    continue
                key = ["v", var]
                held = next((h for a in atoms if not a[-1] for h in [_in_flight(c.prog, f, a[:-1], key, gline)] if h in c.prog.registries), None)
                branch = [e for e in f["ev"] if gid in f["ctxs"][e[2]][1]]
                if held is None or any(e[0] == "c" and e[3] == "erase" for e in branch) or inside(c.findings, "lost_update", f["file"], gline, gline + 8):
                    continue
                text = c.text(f["file"])
                start, _ = line_span(text, gline)
                stop = text.find(b"continue;", start)
                head_start, head_end = line_span(text, line)
                range_of = re.search(rb"([A-Za-z_][\w.\[\]>-]*?)\.begin\(\)", text[head_start:head_end]) or re.search(rb":\s*([A-Za-z_][\w.\[\]>-]*)\s*\)", text[head_start:head_end])
                if stop < 0 or range_of is None:
                    continue
                container, name = range_of.group(1), var.encode()
                step = re.compile(rb"(\+\+\s*" + re.escape(name) + rb"|" + re.escape(name) + rb"\s*\+\+)\s*;")
                found = step.search(text, start, stop)
                if found:
                    new = text[:found.start()] + name + b" = " + container + b".erase(" + name + b");" + text[found.end():]
                else:
                    new = text[:stop] + container + b".erase(" + name + b"); " + text[stop:]
                end_line = gline + text[start:stop].count(b"\n")
                return mutation(f["file"], text, new, (f["file"], gline, end_line), f"key erased on the in-flight branch at line {gline}")
    raise NoSite("no branch keeps an in-flight key")


def drop_key_field(c):
    """Take one member out of a cache key: the key no longer covers what the value is made of."""
    for f, field, var, kf in _memos(c.prog):
        if kf is None:
            continue
        k = c.prog.fn[kf]
        _, source = _lossy(c.prog, kf)
        if source is None:
            continue
        read = sorted({e[3] for e in k["ev"] if e[0] == "r" and c.prog.field.get(e[3], (None,))[0] == source})
        made = set()
        for e in f["ev"]:
            if e[0] == "call" and e[3] != kf and e[3] in c.prog.fn:
                made |= set(c.prog.fields_of(c.prog.R.get(e[3], 0)))
        text = c.text(k["file"])
        start, _ = line_span(text, k["line"])
        _, end = line_span(text, k["end"])
        for fid in read:
            if fid not in made:
                continue
            name = c.prog.field[fid][1][1].encode()
            use = re.compile(rb"\b[A-Za-z_]\w*\s*(?:\.|->)\s*" + re.escape(name) + rb"\b")
            hits = list(use.finditer(text, start, end))
            if len(hits) != 1:
                continue
            new = text[:hits[0].start()] + b"0" + text[hits[0].end():]
            return mutation(k["file"], text, new, (k["file"], k["line"], k["end"]), f"member {name.decode()} dropped from the key")
    raise NoSite("no cache key with a member to drop")


def remove_bound(c):
    """Make the comparison that bounds a size read from bytes compare a constant instead: the size reaches its allocation unbounded."""
    for fid, f in sorted(c.prog.fn.items()):
        tainted = set()
        for e in f["ev"]:
            if e[0] == "set" and any(s[0] == "call" and s[1] in c.prog.readers for s in e[4]):
                tainted.add(e[3])
            elif e[0] == "alloc" and not inside(c.findings, "taint_alloc", f["file"], e[1], e[1]):
                via = {s[1] for s in e[3] if s[0] == "v"} & tainted
                for d in f["ctxs"][e[2]][1]:
                    g = f["guards"][abs(d) - 1]
                    names = {x[1] for a in g[3] if a[0] == "cmp" for x in (a[2], a[3]) if x[0] == "v"} & via
                    if not names:
                        continue
                    name = sorted(names)[0].encode()
                    text = c.text(f["file"])
                    new = replace_in_line(text, g[1], re.compile(rb"(?<![\w.>])" + re.escape(name) + rb"\b(?!\s*\()"), b"0", count=0)
                    return mutation(f["file"], text, new, (f["file"], e[1], e[1]), f"bound on {name.decode()} at line {g[1]} removed")
    raise NoSite("no bounded allocation from bytes")
