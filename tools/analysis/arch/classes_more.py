"""The rest of the relation: state and mutation, time, boundary, role, evolution, and the classes found while working."""
import collections

from .core import words_of


def _reached(s, name):
    return sorted(s.when.reach[name])


def _writes(s, via):
    return s.edges("write", test=lambda e: e.attrs.get("via") == via)


def _channels(s):
    """A field one unit writes and another reads with no call between them: a value handed over later."""
    out = []
    for e in s.by_kind["write"]:
        readers = [r.src for r in s.graph.into.get(e.dst, ()) if r.kind == "read" and r.src != e.src]
        out += [f"{e.src}>{r}" for r in readers if r not in {x.dst for x in s.graph.out.get(e.src, ())}]
    return out


def _guarded(s, *words):
    """Branches whose test reads something with these words."""
    want = set(words)
    out = []
    for c in s.costs["guard"]:
        for field in c[6].get("on", ()):
            if field in s.graph.units and want & set(words_of(s.graph.units[field]["name"])):
                out.append(c[0])
    return out


def _configs(s):
    seen = {c for u in s.graph.units.values() for c in u["attrs"].get("configs", ())}
    if len(seen) < 2:
        return []
    return s.units(lambda u: 0 < len(u["attrs"].get("configs", ())) < len(seen))


def _roles(s, name):
    return s.a.get("roles_of", {}).get(name, [])


def _literal_shared(s):
    """The same text printed by code and searched for by a tool: a log line as a contract."""
    code, tools = collections.defaultdict(set), collections.defaultdict(set)
    for r in s.run.facts.refs:
        (tools if r[2].startswith(("py:", "sh-script:")) else code)[r[1]].add(r[2])
    return [f"{a}>{b}" for t in tools if t in code for a in tools[t] for b in code[t]]


def _shared_constant(s):
    """A name that is a constant in two languages."""
    per = collections.defaultdict(set)
    for uid, u in s.graph.units.items():
        if u["kind"] in ("field", "global", "uniform") and u["name"].isupper():
            per[u["name"]].add(uid.split(":", 1)[0])
    return s.units(lambda u: len(per.get(u["name"], ())) > 1)


def _dead(s):
    """No static edge reaches it, no name resolves to it, it never ran: a candidate, never a verdict."""
    return s.units(lambda u: u["kind"] in ("function", "method") and not u["attrs"].get("callback") and not u["attrs"].get("overrides") and not u["attrs"].get("ran")
                   and not any(not e.generic for e in s.graph.into.get(u["id"], ())))


def more_classes():
    u = lambda f: (lambda s: s.units(f))  # noqa: E731
    return (
        ("B54", "save key", lambda s: s.text_role("value", "section", "key") + s.by_api("value")),
        ("B55", "tagged block", lambda s: s.named("tag")),
        ("B56", "who reads and writes a field", lambda s: s.edges("read", "write")),
        ("B57", "mutation by reference", lambda s: _writes(s, "mutate")),
        ("B58", "mutation by pointer", lambda s: _writes(s, "assign")),
        ("B59", "alias", lambda s: _writes(s, "alias")),
        ("B60", "const_cast", lambda s: s.edges("const_cast")),
        ("B61", "state shared with atomics", u(lambda x: "atomic" in x["attrs"].get("words", ()))),
        ("B62", "locks", u(lambda x: "mutex" in x["attrs"].get("words", ()))),
        ("B63", "triple buffer", u(lambda x: x["kind"] == "field" and "[3]" in x["attrs"].get("type", ""))),
        ("B64", "change channel", _channels), ("B65", "journal", _channels), ("B66", "undo and its inverse", _channels), ("B67", "save and load", _channels),
        ("B68", "function as a value", lambda s: s.edges("stores_fn", "callable")),
        ("B69", "boot", lambda s: _reached(s, "boot")), ("B70", "per frame", lambda s: _reached(s, "frame")), ("B71", "per tick", lambda s: _reached(s, "tick")),
        ("B72", "per event", lambda s: _reached(s, "event")), ("B73", "deferred", lambda s: s.edges("await") + s.text_role("deferred") + s.by_api("deferred")),
        ("B74", "in a thread", lambda s: s.edges("thread") + _reached(s, "thread")), ("B75", "in a job", lambda s: s.edges("stored", "callback")),
        ("B76", "old save", lambda s: _guarded(s, "version")), ("B77", "only in tests", lambda s: sorted(s.when.tests)),
        ("B78", "only in debug", _configs), ("B79", "only in export", lambda s: [c[0] for c in s.costs["importer"]]),
        ("B80", "only in a renderer", lambda s: [c[0] for c in s.costs["variant"]]), ("B81", "only in a graphics tier", lambda s: _guarded(s, "tier", "preset", "quality")),
        ("B82", "by data or option", lambda s: s.edges("data_choice")), ("B83", "by a key", u(lambda x: x["attrs"].get("input"))),
        ("B84", "by a command line argument", lambda s: s.by_api("cmdline", "args")),
        ("B85", "engine", lambda s: s.by_api("server", "rendering")), ("B86", "Terrain3D and other by-name boundaries", lambda s: [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[5].startswith("call#")]),
        ("B87", "libraries", lambda s: s.edges("include", test=lambda e: e.dst.startswith("ext:"))), ("B88", "outside process", lambda s: s.edges("process")),
        ("B89", "tool that writes a file", lambda s: [c[0] for c in s.costs["writes_file"]]), ("B90", "generated code", lambda s: sorted(s.run.roles.generated)),
        ("B91", "build", lambda s: s.edges("build")), ("B92", "export presets", lambda s: sorted(s.run.info.get("godot", {}).get("filters", {}))),
        ("B93", "recorded profile", lambda s: s.units(lambda x: x["attrs"].get("ran"))),
        ("B94", "own", lambda s: _roles(s, "own")), ("B95", "others use it", lambda s: _roles(s, "used")), ("B96", "it uses others", lambda s: _roles(s, "uses")),
        ("B97", "shared", lambda s: _roles(s, "shared")), ("B98", "infrastructure", lambda s: _roles(s, "infrastructure")), ("B99", "test code", lambda s: sorted(s.when.tests)),
        ("B100", "dead code", _dead), ("B101", "data with no reader", lambda s: [f["unit"] for f in s.a["findings"] if f["category"] == "no_reader"]),
        ("B102", "option with no effect", lambda s: [f["unit"] for f in s.a["findings"] if f["category"] == "no_effect"]),
        ("B103", "rename", lambda s: _lineage(s)), ("B104", "move", lambda s: _lineage(s)), ("B105", "split", lambda s: _lineage(s)),
        ("D1", "route area/method", lambda s: s.edges("route")), ("D2", "enum by text", lambda s: s.edges("enum_by_text")),
        ("D3", "stringified and pasted names", lambda s: [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[5] == "xmacro"]),
        ("D4", "setter as a call", lambda s: s.edges("setter")), ("D5", "await as a cut in time", lambda s: s.edges("await")),
        ("D6", "closure across threads", lambda s: s.edges("lambda", test=lambda e: e.attrs.get("capture"))), ("D7", "notify and wait", lambda s: s.edges("notify", "wait") + s.edges("enumref", test=lambda e: "memory_order" in e.dst)),
        ("D8", "binary layout", lambda s: s.named("custom") + s.by_api("custom", "buffer")), ("D9", "varying", u(lambda x: x["kind"] == "varying")),
        ("D10", "masks and bits", lambda s: s.by_api("mask", "layers")), ("D11", "uid", lambda s: s.edges("uid")),
        ("D12", "import and export filters", lambda s: [c[0] for c in s.costs["importer"]]), ("D13", "order of initialisation", lambda s: s.edges("autoload") + s.units(lambda x: x["kind"] == "global" and not x["attrs"].get("constant"))),
        ("D14", "files across sessions", lambda s: [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[1].startswith("user://")]),
        ("D15", "environment variable", lambda s: s.edges("environment")), ("D16", "log text as a contract", _literal_shared),
        ("D17", "tools reading data", lambda s: s.edges("name_ref", test=lambda e: e.src.startswith(("py:", "sh-script:")))), ("D18", "same constant in two languages", _shared_constant),
        ("D19", "comment and _doc as a weak edge", lambda s: [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[5] == "doc"]),
        ("D20", "by camera distance", lambda s: s.by_api("visibility", "lod")), ("D21", "by hour and weather", lambda s: _guarded(s, "hour", "night", "weather", "day")),
        ("D22", "by map size", lambda s: _guarded(s, "size", "side", "map")), ("D23", "first pipeline compile", lambda s: [f["unit"] for f in s.a["findings"] if f["category"] == "pipeline_compile"]),
        ("D24", "on focus and on close", lambda s: s.named("focus", "close", "quit", "notification")),
    )


def _lineage(s):
    return [str(lin.names[-1]) for lin in s.a["history"].lineages if len(lin.names) > 1]
