"""Mutations that add a cost to real code of the analysed commit, one per class: move a loop into the caller of a function that walks and resizes
a member, and remove the one write that fed a recomputation so that it runs on inputs nothing changed."""
from ..costmodel import verdicts
from ..summaries import LINEAR, SIZING
from .mutate_text import NoSite, flagged, mutation

LOOP = b"for (size_t arch_mutation_i = 0, arch_mutation_n = %s.size(); arch_mutation_i < arch_mutation_n; ++arch_mutation_i) "


def wrap_in_loop(c):
    """A call, outside any loop, of a method that walks a member its own work resizes: the caller now runs it once per element of that member."""
    prog = c.prog
    done = flagged(c.findings, "composed_quadratic")
    walks = {}
    for gid, g in sorted(prog.fn.items()):
        if not g["cls"]:
            continue
        mine = {fid for fid, (owner, _) in prog.field.items() if owner == g["cls"]}
        for e in g["ev"]:
            if e[0] == "c" and e[3] in LINEAR and e[4] in mine and prog.field_bit(e[4]) & prog.S.get(gid, 0) and (g["file"], e[1]) not in done:
                walks.setdefault(gid, (e[1], e[4]))
    for fid, f in sorted(prog.fn.items()):
        for e in f["ev"]:
            if e[0] != "call" or not e[6] or f["ctxs"][e[2]][0] or e[3] not in walks or e[3] == fid:
                continue
            g = prog.fn[e[3]]
            line, field = walks[e[3]]
            if f["cls"] != g["cls"] or prog.field[field][1][5] is False:
                continue
            text = c.text(f["file"])
            start = e[6][0]
            new = text[:start] + LOOP % prog.field[field][1][1].encode() + text[start:]
            return mutation(f["file"], text, new, (g["file"], line, line), f"call at line {e[1]} run once per element of {prog.field[field][1][1]}")
    raise NoSite("no call of a method that walks and resizes a member of its class")


def remove_feeding_write(c):
    """In a slice where one call alone wrote what a recomputation needs, remove that call: the recomputation now runs on unchanged inputs."""
    prog = c.prog
    slices = c.slices(tracing=True)
    done = {(f["file"], f["line"], f["entry"]) for f in c.findings if f["class"] == "redone_work"}
    for eid, sl, r, needs, written in verdicts(prog, slices):
        if not needs or not needs & written or (r["site"][0], r["site"][1], eid) in done:
            continue
        before = sl.trace[:r["trace"]]
        chain = {(fid, line) for fid, line, _ in r["frames"]}  # a call the recomputation runs inside is no feeder: removing it removes both
        feeding = [i for i, (site, bits, _) in enumerate(before) if bits & needs and site not in chain]
        top = [i for i in feeding if not any(j < i and before[j][2] < before[i][2] and all(before[k][2] > before[j][2] for k in range(j + 1, i + 1)) for j in feeding)]
        made_by_calls = 0
        for _, bits, _ in before:
            made_by_calls |= bits
        if len(top) != 1 or written & ~made_by_calls & needs or any(before[i][0] in chain for i in range(len(before)) if before[i][1] & needs and i not in feeding):
            continue
        (fid, line), _, _ = before[top[0]]
        f = prog.fn[fid]
        e = next((x for x in f["ev"] if x[0] == "call" and x[1] == line and x[6]), None)
        if e is None or f["file"] == r["site"][0] and r["site"][1] == line:
            continue
        text = c.text(f["file"])
        new = text[:e[6][0]] + b"(void)0" + text[e[6][1]:]
        return mutation(f["file"], text, new, (r["site"][0], r["site"][1], r["site"][1]), f"call at {f['file']}:{line} that fed the recomputation removed")
    raise NoSite("no recomputation fed by one call alone")
