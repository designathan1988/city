"""Costs that are not one line on a hot path: configuration, engine behaviour, data with no reader, what tests leave in the delivered binary."""
import collections
import itertools
import math
import re

from .adapters.images import pixels
from .core import natural_break, words_of
from .costs import _boundary_with
from .costs_decode import serial_decode


BUILT_BOX = re.compile(r"\baabb\s+(\w+)\s*\(.*vector3\(", re.I)
TEMPLATE_FILES = (".h", ".hpp", ".inl", ".cpp")
GENERIC = re.compile(r"\btemplate\s*<|\(const auto &|\(auto &")
MEMBER_READ = re.compile(r"(?:\.|->)(\w+)\b(?!\s*(?:=[^=]|\())")


def no_reader(k):
    """A key of a table that is read, which no code reads and nothing takes over by its name: a value that does nothing."""
    graph = k.graph
    named = {r[1].rsplit("/", 1)[-1].split(".")[0] for r in k.run.facts.refs if r[1] and r[5] != "doc"}
    # A table no code of this repository names is read by something else: a tool of its own. What is not read here says nothing.
    read_files = {f for f in (graph.units[e.dst]["file"] for e in graph.edges.values() if e.dst in graph.units and graph.units[e.dst]["kind"] == "key") if f.rsplit("/", 1)[-1].split(".")[0] in named}
    out = []
    for uid, u in graph.units.items():
        if u["kind"] == "key" and u["file"] in read_files and uid not in k.when.left_out and not graph.into.get(uid) and not graph.out.get(uid) and not graph.children.get(uid) and not u["name"].startswith("_"):
            out.append(k.finding("no_reader", uid, u["file"], u["line"], key=u["name"]))
    return out


def _read_in_templates(k):
    """Members named through a type a template or a generic lambda has not fixed yet: the compiler gives them no referent, so only the text says they are read."""
    out = set()
    for file, text in k.run.texts.items():
        if file.endswith(TEMPLATE_FILES) and GENERIC.search(text or ""):
            out.update(MEMBER_READ.findall(text))
    return out


def no_effect(k):
    """A field only ever assigned: nothing reads it, locks it, hands it over or calls anything on it. The option is taken in and dropped."""
    graph = k.graph
    unresolved = _read_in_templates(k)
    out = []
    for uid, u in graph.units.items():
        if u["kind"] in ("field", "global") and k.writers.get(uid) and uid not in k.when.tests and u["name"] not in unresolved:
            if all(e.kind == "write" and e.attrs.get("via") in (None, "assign") for e in graph.into.get(uid, ())):
                out.append(k.finding("no_effect", uid, u["file"], u["line"], field=u["name"]))
    return out


def costly_option(k):
    """A branch tested against an option in front of work that costs: a value set once, read every frame, that decides the price."""
    expensive = {c[0] for c in k.by_kind["loop"]} | {c[0] for c in k.by_kind["alloc"]}
    out = []
    for branch, fields in sorted(k.guards.items()):
        fields = [f for f in fields if k.writers.get(f) and not k.writers[f] & k.when.repeats]
        if branch in expensive and fields and branch in k.when.repeats and branch not in k.when.tests:
            u = k.graph.units[branch] if branch in k.graph.units else None
            if u is not None:
                out.append(k.finding("costly_option", branch, u["file"], u["line"], on=sorted(set(fields))[0]))
    return out


def tiny_triangles(k):
    """Many instances of a small mesh drawn at any distance: shading runs on 2x2 quads, and a small triangle wastes most of each."""
    return [k.finding("tiny_triangles", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"multimesh"}) if c[4] > 0 or k.repeated(c)]


def _source_line(k, c):
    lines = (k.run.texts.get(c[2]) or "").split("\n")
    return " ".join(lines[c[3] - 1:c[3] + 2]) if 0 < c[3] <= len(lines) else ""


def _bounds_handed_over(k, setter, api):
    """A box a caller built by hand a few lines above the call that gives it to whatever sets the bounds."""
    graph = k.graph
    callers = [e for e in graph.into.get(setter, ()) if e.kind == "call"]
    callers += [x for e in callers for x in graph.into.get(graph.callable_of(e.src), ()) if x.kind == "call"]
    out = []
    for e in callers:
        home = graph.units.get(graph.callable_of(e.src))
        file, line = e.sites[0][0], e.sites[0][1]
        lines = (k.run.texts.get(file) or "").split(chr(10))
        for i in range(min(line, len(lines)) - 1, (home["line"] if home else line) - 1, -1):
            built = BUILT_BOX.search(lines[i - 1])
            if built and re.search(rf"\b{built[1]}\b", lines[line - 1]):
                out.append(k.finding("wide_bounds", e.src, file, i, api=api))
                break
    return out


def wide_bounds(k):
    """Bounds written by hand: a box built in the call out of constants, not measured from what it holds. One such box keeps everything in it drawn."""
    out = []
    for c in _boundary_with(k, {"aabb"}):
        method = c[6]["api"].rsplit(".", 1)[-1]
        text = _source_line(k, c)
        if "custom" in words_of(method) and re.search(rf"{re.escape(method)}\(.*\baabb\(\s*vector3\(", text, re.I):
            out.append(k.finding("wide_bounds", c[0], c[2], c[3], api=c[6]["api"]))
        elif "custom" in words_of(method):
            out += _bounds_handed_over(k, k.graph.callable_of(c[0]), c[6]["api"])
    return out


def pipeline_compile(k):
    """A shader or material made on the repeating path: the driver compiles while the player waits."""
    made = [c for c in _boundary_with(k, {"create"}) if {"shader", "material", "pipeline"} & set(words_of(c[6]["api"]))]
    return [k.finding("pipeline_compile", c[0], c[2], c[3], api=c[6]["api"]) for c in made if k.repeated(c)]


def measure_feedback(k):
    """A measuring path that does heavy work itself: the long frame it reports makes the next one longer."""
    time_readers = {c[0] for c in k.by_kind["boundary"] if {"ticks", "usec", "msec", "time"} & set(words_of(c[6]["api"]))}
    heavy = {c[0]: c for c in k.by_kind["loop"] if c[6]["over"]}
    out = []
    for uid in sorted(time_readers & k.when.repeats):
        for e in k.when.edges.get(uid, ()):
            target = e.dst
            hop = [x.dst for x in k.graph.out.get(target, ()) if x.kind in ("route", "name_ref")]
            for t in [target, *hop]:
                if t in heavy and t != uid:
                    c = heavy[t]
                    out.append(k.finding("measure_feedback", t, c[2], c[3], measured_by=uid))
    return out


def rebuilt_interface(k):
    """Children thrown away and made again on a timer or a frame: the interface rebuilt with nothing changed."""
    out = []
    frees = [c for c in _boundary_with(k, {"free"}) if c[4] > 0]
    for c in frees:
        if c[0] in k.when.repeats or any(e.kind == "await" for e in k.graph.out.get(c[0], ())):
            out.append(k.finding("rebuilt_interface", c[0], c[2], c[3], api=c[6]["api"]))
    return out


def boundary_conversion(k):
    """Values converted by code outside the program, on the repeating path."""
    out = []
    for e in k.graph.edges.values():
        if e.kind == "convert" and e.attrs.get("crosses") and e.src in k.when.repeats and e.src not in k.when.tests and not k.gated(e.src):
            out.append(k.finding("boundary_conversion", e.src, *e.sites[0], to=e.dst[4:]))
    return out


def engine_copies(k):
    """One engine object per item in a loop: the engine keeps its own copies of each."""
    makes = {"create", "instantiate", "new"}
    return [k.finding("engine_copies", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, makes) if c[4] > 0 and c[0] not in k.when.tests and words_of(c[6]["api"].rsplit(".", 1)[-1])[0] in makes]


def _arguments(k, c, method):
    """The text of a call's arguments, split at its own commas."""
    lines = (k.run.texts.get(c[2]) or "").split("\n")
    text = " ".join(lines[c[3] - 1:c[3] + 3])
    start = text.find(method + "(")
    if start < 0:
        return []
    out, depth, word = [], 0, ""
    for ch in text[start + len(method):]:
        depth += (ch in "([{") - (ch in ")]}")
        if depth == 0:
            return out + [word]
        if ch == "," and depth == 1:
            out.append(word)
            word = ""
        elif depth > 1 or ch != "(":
            word += ch
    return out + [word]


def level_flip(k):
    """A distance range set with nothing at its margins, as the engine's own description names them: at the edge a thing flips every frame the camera moves."""
    out = []
    for c in _boundary_with(k, {"visibility"}):
        method = c[6]["api"].rsplit(".", 1)[-1]
        names = [a for owner, args, _, _ in k.run.api.methods.get(method, ()) for a in args][:16]
        spots = [i for i, a in enumerate(names) if "margin" in words_of(str(a))]
        given = _arguments(k, c, method)
        if spots and all(i < len(given) and given[i].strip() in ("0", "0.0", "0.0f") for i in spots):
            out.append(k.finding("level_flip", c[0], c[2], c[3], api=c[6]["api"]))
    return out


def long_save(k):
    """Encoding the state while holding its lock: a loop or a call outside under a lock, in code that saves. Taking the lock to swap a pointer costs nothing."""
    out = []
    for kind in ("loop", "boundary"):
        for c in k.by_kind[kind]:
            unit = k.graph.units.get(k.graph.callable_of(c[0]))
            if c[5] and unit is not None and {"save", "encode", "serialize", "persist"} & set(words_of(unit["name"]) + words_of(unit["file"].rsplit("/", 1)[-1])):
                out.append(k.finding("long_save", c[0], c[2], c[3], under=c[5][-1], what=kind))
    return out


def thread_churn(k):
    """A thread or job started per item: what starts it costs more than what it does."""
    out = []
    for e in k.graph.edges.values():
        if e.kind in ("thread", "stored"):
            parent = k.graph.units.get(e.src, {}).get("parent")
            depth = next((x.attrs.get("depth", 0) for x in k.graph.out.get(parent, ()) if x.dst == e.src), 0)
            if depth > 0:
                out.append(k.finding("thread_churn", parent, *e.sites[0], job=e.src))
    return out


def false_sharing(k):
    """Atomics side by side in one object, each written from another context: every write invalidates the other's cache line."""
    by_parent = collections.defaultdict(list)
    for uid, u in k.graph.units.items():
        if u["kind"] == "field" and "atomic" in u["attrs"].get("words", ()):
            by_parent[u["parent"]].append(uid)
    out = []
    for parent, fields in sorted(by_parent.items(), key=lambda kv: str(kv[0])):
        writes = {f: {c for w in k.writers.get(f, ()) for c in k.when.labels(w)} - {"cold"} for f in fields}
        apart = [a for a, b in itertools.combinations(sorted(fields), 2) if writes[a] and writes[b] and not writes[a] & writes[b]]
        if apart:
            u = k.graph.units[apart[0]]
            out.append(k.finding("false_sharing", apart[0], u["file"], u["line"], fields=sorted(fields)))
    return out


def texture_bandwidth(k):
    """Textures stored uncompressed that are large among the project's own: every sample reads several times the bytes. An icon of a few pixels costs nothing."""
    tracked = k.run.tracked
    by_tail = collections.defaultdict(list)
    for path in tracked:
        by_tail[path.rsplit("/", 1)[-1]].append(path)
    sized = []
    for c in k.by_kind["importer"]:
        if c[6].get("uncompressed"):
            source = c[6].get("source", "")
            home = [p for p in by_tail.get(source.rsplit("/", 1)[-1], ()) if p.endswith(source)]
            size = pixels(k.run.repo.blob(tracked[home[0]])) if home else None
            if size:
                sized.append((math.log(size), c, size))
    cut = natural_break([s[0] for s in sized])
    return [k.finding("texture_bandwidth", "file:" + c[2][:-len(".import")], c[2][:-len(".import")], 1, pixels=size) for weight, c, size in sized if weight >= cut]


def test_code_shipped(k):
    """Code only tests reach that lives in what is delivered."""
    delivered = set(k.when.tests) - k.when.left_out
    out = []
    for uid in sorted(delivered):
        u = k.graph.units[uid]
        if u["kind"] in ("method", "function") and u["attrs"].get("mangled"):
            out.append(k.finding("test_code_shipped", uid, u["file"], u["line"]))
    return out


def shader_cost(k):
    """Shaders whose compiled size stands out from the others'."""
    sized = {c[0]: c for c in k.by_kind["shader_size"]}
    if len(sized) < 2:
        return []
    cut = natural_break([c[6]["operations"] for c in sized.values()])
    return [k.finding("shader_cost", u, c[2], c[3], operations=c[6]["operations"], samples=c[6]["samples"]) for u, c in sorted(sized.items()) if c[6]["operations"] >= cut]


CATALOGUE = (
    ("no_reader", "data with no reader", no_reader), ("no_effect", "option taken in and dropped", no_effect), ("costly_option", "option that decides the price", costly_option),
    ("tiny_triangles", "many small instances at any distance", tiny_triangles), ("wide_bounds", "bounds set by hand", wide_bounds),
    ("pipeline_compile", "pipeline compiled while playing", pipeline_compile), ("measure_feedback", "measure that causes what it measures", measure_feedback),
    ("rebuilt_interface", "interface rebuilt on a timer", rebuilt_interface), ("boundary_conversion", "conversion at the boundary every frame", boundary_conversion),
    ("engine_copies", "one engine object per item", engine_copies), ("level_flip", "level change with no margin", level_flip),
    ("serial_decode", "input decoded twice while opening", serial_decode), ("long_save", "state encoded under its lock", long_save),
    ("thread_churn", "thread or job per item", thread_churn), ("false_sharing", "atomics sharing a cache line", false_sharing),
    ("texture_bandwidth", "uncompressed textures", texture_bandwidth), 
    ("test_code_shipped", "test code in the delivered binary", test_code_shipped), ("shader_cost", "shader cost from its compiled form", shader_cost),
)
