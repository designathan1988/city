"""Reading the same input twice: a check that decodes everything before the read that decodes it again, and one reader called twice over the same words."""
import collections
import re

from .core import words_of

DECODED = re.compile(r"char|byte|uint8|string|path|buffer|reader", re.I)


def _decodes(u):
    """Named for reading, and handed what is read: bytes or text. Loading a lorry is not reading a file."""
    return {"read", "decode", "parse", "load"} & set(words_of(u["name"])) and DECODED.search(u["attrs"].get("signature") or "") is not None


def _same_input(k, e, name):
    """The arguments as written at each place of a call: the same words twice are the same input."""
    written = collections.Counter()
    for file, line, *_ in e.sites:
        lines = (k.run.texts.get(file) or "").split(chr(10))
        for found in re.findall(re.escape(name) + r"\(([^()]*)\)", lines[line - 1] if 0 < line <= len(lines) else ""):
            written[found] += 1
    return max(written.values(), default=0) > 1


def _decoded_twice(k):
    """One caller, two steps, one big reader under both: a check that decodes everything and then the read that decodes it again."""
    graph = k.graph
    calls = collections.defaultdict(set)
    for e in graph.edges.values():
        if e.kind == "call" and e.conf == 1.0 and e.dst in graph.units and e.src not in k.when.tests:
            calls[e.dst].add(e.src)
    readers = [u for u, x in graph.units.items() if x["kind"] in ("function", "method") and _decodes(x) and len(calls.get(u, ())) > 1]
    sizes = sorted(graph.units[u]["attrs"].get("statements", 0) for u in readers)
    out = []
    for r in sorted(readers):
        if graph.units[r]["attrs"].get("statements", 0) < sizes[len(sizes) // 2]:
            continue
        over = collections.defaultdict(set)
        for step in calls[r]:
            for top in calls.get(step, ()):
                over[top].add(step)
        for top, steps in sorted(over.items()):
            if len(steps) > 1:
                u = graph.units[r]
                f = k.finding("serial_decode", r, u["file"], u["line"], through=sorted(steps), caller=top)
                f["places"] = [(x, graph.units[x]["file"], graph.units[x]["line"]) for x in (r, top, *sorted(steps))]
                out.append(f)
    return out


def serial_decode(k):
    """The same reader run twice over the same input on the opening path."""
    out = _decoded_twice(k)
    boot = k.when.reach["boot"]
    per = collections.Counter()
    for e in k.graph.edges.values():
        if e.kind == "call" and e.src in boot and e.dst in k.graph.units and len(e.sites) > 1 and _same_input(k, e, k.graph.units[e.dst]["name"]):
            per[e.dst] = max(per[e.dst], len(e.sites))
    for uid, n in sorted(per.items()):
        if {"read", "decode", "parse", "load"} & set(words_of(k.graph.units[uid]["name"])) and uid not in k.when.tests:
            out.append(k.finding("serial_decode", uid, k.graph.units[uid]["file"], k.graph.units[uid]["line"], times=n))
    return out
