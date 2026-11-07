"""Costs that are not one line on a hot path: configuration, engine behaviour, data with no reader, what tests leave in the delivered binary."""
import collections

from .core import natural_break, words_of
from .costs import _boundary_with


def no_reader(k):
    """A key of a table that is read, which no code reads: a value that does nothing."""
    graph = k.graph
    read_files = {graph.units[e.dst]["file"] for e in graph.edges.values() if e.dst in graph.units and graph.units[e.dst]["kind"] == "key"}
    out = []
    for uid, u in graph.units.items():
        if u["kind"] == "key" and u["file"] in read_files and not graph.into.get(uid) and not graph.children.get(uid) and not u["name"].startswith("_"):
            out.append(k.finding("no_reader", uid, u["file"], u["line"], key=u["name"]))
    return out


def no_effect(k):
    """A field a loader fills and nothing ever reads: the option is taken in and dropped."""
    graph = k.graph
    out = []
    for uid, u in graph.units.items():
        if u["kind"] in ("field", "global") and k.writers.get(uid):
            if not any(e.kind == "read" or e.kind.startswith("const") for e in graph.into.get(uid, ())):
                out.append(k.finding("no_effect", uid, u["file"], u["line"], field=u["name"]))
    return out


def costly_option(k):
    """A branch tested against an option or tier in front of work that costs: the choice decides the price."""
    expensive = {c[0] for c in k.by_kind["loop"]} | {c[0] for c in k.by_kind["alloc"]}
    out = []
    for branch, fields in sorted(k.guards.items()):
        if branch in expensive and fields and branch in k.when.repeats:
            u = k.graph.units[branch] if branch in k.graph.units else None
            if u is not None:
                out.append(k.finding("costly_option", branch, u["file"], u["line"], on=sorted(set(fields))[0]))
    return out


def tiny_triangles(k):
    """Many instances of a small mesh drawn at any distance: shading runs on 2x2 quads, and a small triangle wastes most of each."""
    return [k.finding("tiny_triangles", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"multimesh"}) if c[4] > 0 or k.repeated(c)]


def wide_bounds(k):
    """Bounds set by hand: one box for many things keeps all of them drawn."""
    return [k.finding("wide_bounds", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"aabb"})]


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
    """Values converted to the engine's variant types on the repeating path."""
    out = []
    for e in k.graph.edges.values():
        if e.kind == "convert" and e.src in k.when.repeats and e.dst.startswith("ext:") and e.src not in k.when.tests and not k.gated(e.src):
            out.append(k.finding("boundary_conversion", e.src, *e.sites[0], to=e.dst[4:]))
    return out


def engine_copies(k):
    """One engine object per item in a loop: the engine keeps its own copies of each."""
    return [k.finding("engine_copies", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"create", "instantiate", "new"}) if c[4] > 0 and c[0] not in k.when.tests]


def level_flip(k):
    """A distance range set with no margin: at the edge a thing flips between levels every frame the camera moves."""
    return [k.finding("level_flip", c[0], c[2], c[3], api=c[6]["api"]) for c in _boundary_with(k, {"visibility"}) if not c[6].get("margin")]


def _decoded_twice(k):
    """One caller, two steps, one big reader under both: a check that decodes everything and then the read that decodes it again."""
    graph = k.graph
    calls = collections.defaultdict(set)
    for e in graph.edges.values():
        if e.kind == "call" and e.dst in graph.units and e.src not in k.when.tests:
            calls[e.dst].add(e.src)
    readers = [u for u, x in graph.units.items() if x["kind"] in ("function", "method") and {"read", "decode", "parse", "load"} & set(words_of(x["name"])) and len(calls.get(u, ())) > 1]
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
        if e.kind == "call" and e.src in boot and e.dst in k.graph.units and len(e.sites) > 1:
            per[e.dst] = max(per[e.dst], len(e.sites))
    for uid, n in sorted(per.items()):
        if {"read", "decode", "parse", "load"} & set(words_of(k.graph.units[uid]["name"])) and uid not in k.when.tests:
            out.append(k.finding("serial_decode", uid, k.graph.units[uid]["file"], k.graph.units[uid]["line"], times=n))
    return out


def long_save(k):
    """Encoding the whole state while holding the state's lock: everyone waits for it."""
    out = []
    for c in k.by_kind["lock"]:
        unit = k.graph.units.get(c[0])
        if unit is not None and {"save", "encode", "write", "serialize", "persist"} & set(words_of(unit["name"]) + words_of(unit["file"].rsplit("/", 1)[-1])):
            out.append(k.finding("long_save", c[0], c[2], c[3], mutex=c[6]["mutex"]))
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
    """Atomics side by side in one object, written from different contexts: each write invalidates the other's cache line."""
    by_parent = collections.defaultdict(list)
    for uid, u in k.graph.units.items():
        if u["kind"] == "field" and "atomic" in u["attrs"].get("words", ()):
            by_parent[u["parent"]].append(uid)
    out = []
    for parent, fields in sorted(by_parent.items(), key=lambda kv: str(kv[0])):
        contexts = {c for f in fields for w in k.writers.get(f, ()) for c in k.when.labels(w)}
        if len(fields) > 1 and len(contexts) > 1:
            u = k.graph.units[fields[0]]
            out.append(k.finding("false_sharing", fields[0], u["file"], u["line"], fields=sorted(fields)))
    return out


def texture_bandwidth(k):
    """Textures imported without compression: every sample reads several times the bytes."""
    return [k.finding("texture_bandwidth", c[0], c[2], c[3], importer=c[6]["value"]) for c in k.by_kind["importer"] if c[6].get("uncompressed")]


def default_tier(k):
    """A default chosen from the machine that turns on what costs most: its price is paid by everyone with that machine."""
    out = []
    for uid, u in k.graph.units.items():
        if u["kind"] == "key" and {"default", "auto", "preset", "tier"} & set(words_of(u["name"])) and k.graph.into.get(uid):
            out.append(k.finding("default_tier", uid, u["file"], u["line"], key=u["name"]))
    return out


def _as_number(text):
    if text in ("true", "false"):
        return float(text == "true")
    try:
        return float(text)
    except ValueError:
        return None


def _levels(graph):
    """Sibling tables with the same keys are the levels of one choice: each key's value tells that level's setting."""
    tables = collections.defaultdict(list)
    for uid, u in graph.units.items():
        keys = [c for c in graph.children.get(uid, ()) if graph.units[c]["kind"] == "key"]
        if u["kind"] == "key" and keys and u["parent"]:
            tables[(u["parent"], frozenset(graph.units[c]["name"] for c in keys))].append(uid)
    return [sorted(v) for _, v in sorted(tables.items(), key=lambda kv: sorted(kv[0][1])) if len(v) > 1]


def tier_price(k):
    """What each level of a choice turns on, counted over what repeats: an option read on the repeating path adds to the levels that set it higher."""
    graph = k.graph
    value = {c[0]: _as_number(c[6]["value"]) for c in k.by_kind["data_value"]}
    out = []
    for levels in _levels(graph):
        options = collections.defaultdict(dict)
        for level in levels:
            for c in graph.children[level]:
                if value.get(c) is not None:
                    options[graph.units[c]["name"]][level] = c
        price = collections.Counter()
        paying = collections.Counter()
        for name, cells in sorted(options.items()):
            read = any(e.src in k.when.repeats for c in cells.values() for e in graph.into.get(c, ()))
            order = sorted({value[c] for c in cells.values()})
            if read and len(order) > 1:
                for level, c in cells.items():
                    price[level] += order.index(value[c]) / (len(order) - 1)
                    paying[level] += value[c] != order[0]
        for level in levels:
            if price[level]:
                u = graph.units[level]
                out.append(k.finding("tier_price", level, u["file"], u["line"], price=round(price[level], 6), options=paying[level]))
    return out


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
    ("texture_bandwidth", "uncompressed textures", texture_bandwidth), ("default_tier", "default that turns on the most expensive", default_tier),
    ("tier_price", "what each graphics level turns on", tier_price),
    ("test_code_shipped", "test code in the delivered binary", test_code_shipped), ("shader_cost", "shader cost from its compiled form", shader_cost),
)
