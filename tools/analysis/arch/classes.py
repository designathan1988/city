"""The relation of possibilities: every class of link, with what counts as one. Each returns the items found, so a count is a list one can read."""
import collections

from .core import words_of


class Seen:
    """Indexes the class tests share."""

    def __init__(self, run, a):
        self.run, self.a, self.graph = run, a, run.graph
        self.by_kind = collections.defaultdict(list)
        for e in run.graph.edges.values():
            self.by_kind[e.kind].append(e)
        self.costs = collections.defaultdict(list)
        for c in run.facts.costs:
            self.costs[c[1]].append(c)
        self.when = a["when"]

    def edges(self, *kinds, test=None):
        return [f"{e.src}>{e.dst}" for k in kinds for e in self.by_kind.get(k, ()) if test is None or test(e)]

    def units(self, test):
        return [uid for uid, u in self.graph.units.items() if test(u)]

    def text_role(self, *words):
        """Text in a position the engine's description says means this."""
        return [f"{e.src}>{e.dst}" for k in ("name_ref", "name_call") for e in self.by_kind.get(k, ()) if set(words) & set(words_of(e.attrs.get("role", "").split("#")[0]))]

    def by_api(self, *words):
        want = set(words)
        return [f"{c[0]}>{c[6]['api']}" for c in self.costs.get("boundary", ()) if want & set(words_of(c[6]["api"]))]

    def named(self, *words):
        want = set(words)
        return self.units(lambda u: want & set(words_of(u["name"])))


def _to_kind(s, kinds):
    return lambda e: s.graph.units.get(e.dst, {}).get("kind") in kinds


def link_classes():
    """(id, title, how to find them). The id ties a class to the record of the work."""
    u = lambda f: (lambda s: s.units(f))  # noqa: E731
    return (
        ("B1", "direct call", lambda s: s.edges("call", test=lambda e: "ast" in e.sources or "types" in e.sources)),
        ("B2", "virtual call", lambda s: s.edges("virtual", "override")),
        ("B3", "function pointer", lambda s: s.edges("fnref")),
        ("B4", "member pointer", lambda s: s.edges("memberptr")),
        ("B5", "std::function call", lambda s: s.edges("indirect")),
        ("B6", "stored lambda", lambda s: s.edges("stored", "callback", "stores_fn")),
        ("B7", "instantiated template", lambda s: s.edges("template")),
        ("B8", "macro and X-macro", lambda s: s.edges("macro") + [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[5] == "xmacro"]),
        ("B9", "operator", lambda s: s.edges("operator")),
        ("B10", "constructor and destructor", lambda s: s.edges("construct", "destroy", "raii")),
        ("B11", "lock by RAII", lambda s: s.edges("lock")),
        ("B12", "implicit conversion", lambda s: s.edges("convert")),
        ("B13", "inheritance", lambda s: s.edges("inherit")),
        ("B14", "overload", _overloads),
        ("B15", "inline in header", u(lambda x: x["attrs"].get("inline"))),
        ("B16", "dispatch by text", lambda s: s.edges("name_ref", test=_to_kind(s, ("entry",)))),
        ("B17", "query by text", lambda s: s.edges("route")),
        ("B18", "tool or enum by text", lambda s: s.edges("enum_by_text")),
        ("B19", "constant table", u(lambda x: x["kind"] == "entry")),
        ("B20", "Callable", lambda s: s.edges("callable")),
        ("B21", "bind", lambda s: s.edges("bind")),
        ("B22", "call by name", lambda s: s.text_role("call")),
        ("B23", "callv", lambda s: s.text_role("callv") + s.by_api("callv")),
        ("B24", "call_deferred", lambda s: s.text_role("deferred") + s.by_api("deferred")),
        ("B25", "has_method", lambda s: s.text_role("has")),
        ("B26", "get and set by name", lambda s: s.text_role("get", "set")),
        ("B27", "Object::call from C++", lambda s: [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[5].startswith("call#")]),
        ("B28", "ClassDB::bind", lambda s: s.edges("bind", test=lambda e: "ast" in e.sources)),
        ("B29", "match and switch", lambda s: s.edges("case") + [c[0] for c in s.costs["decision"]]),
        ("B30", "data that chooses code", lambda s: s.edges("data_choice")),
        ("B31", "signal", u(lambda x: x["kind"] == "signal")),
        ("B32", "connection in code", lambda s: s.edges("signal_connect", "connected")),
        ("B33", "connection in a scene", lambda s: s.edges("connection")),
        ("B34", "timer", lambda s: s.by_api("timer") + s.by_api("timeout")),
        ("B35", "engine callback", u(lambda x: x["attrs"].get("callback") or x["attrs"].get("overrides"))),
        ("B36", "input", u(lambda x: x["attrs"].get("input"))),
        ("B37", "autoload", lambda s: s.edges("autoload")),
        ("B38", "group", lambda s: s.text_role("group")),
        ("B39", "node path", lambda s: s.edges("name_ref", test=_to_kind(s, ("node",))) + [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[0] in ("node", "role:node")]),
        ("B40", "preload", lambda s: s.edges("name_ref", test=lambda e: e.attrs.get("role") == "preload")),
        ("B41", "load with a built path", lambda s: s.edges("load") + s.edges("name_ref", test=lambda e: e.attrs.get("role") == "load")),
        ("B42", "scene", lambda s: s.edges("name_ref", test=lambda e: e.dst.endswith(".tscn") or ".tscn" in e.attrs.get("text", ""))),
        ("B43", "script", lambda s: s.edges("script")),
        ("B44", "shader", lambda s: s.edges("name_ref", test=lambda e: e.attrs.get("text", "").endswith(".gdshader"))),
        ("B45", "shader include", lambda s: s.edges("name_ref", test=lambda e: e.attrs.get("role") == "include")),
        ("B46", "material by name", lambda s: s.edges("name_ref", test=lambda e: "json:" in e.dst and "material" in e.attrs.get("role", ""))),
        ("B47", "uniform by name", lambda s: s.edges("name_ref", test=_to_kind(s, ("uniform",)))),
        ("B48", "global uniform", u(lambda x: x["kind"] == "uniform" and (x["attrs"].get("scope") == "global" or x["attrs"].get("globally")))),
        ("B49", "instance uniform", u(lambda x: x["kind"] == "uniform" and x["attrs"].get("scope") == "instance")),
        ("B50", "JSON key", lambda s: s.edges("name_ref", test=_to_kind(s, ("key",)))),
        ("B51", "Dictionary read by text", lambda s: [f"{r[2]}>{r[1]}" for r in s.run.facts.refs if r[2].startswith("cpp:") and r[5].endswith("#0")]),
        ("B52", "built tr() key", lambda s: s.edges("name_ref", test=lambda e: "%" in e.attrs.get("text", "") or e.attrs.get("text", "")[-1:] in (".", "_"))),
        ("B53", "table in parts", _pieces_of),
    )


def _overloads(s):
    key = lambda u: (u["parent"] or u["file"], u["name"])  # noqa: E731
    per = collections.Counter(key(u) for u in s.graph.units.values() if u["kind"] in ("function", "method"))
    return [uid for uid, u in s.graph.units.items() if u["kind"] in ("function", "method") and per[key(u)] > 1]


def _pieces_of(s):
    """Data files named alike that one piece of code lists together."""
    out = []
    for uid, es in s.graph.out.items():
        files = {e.dst for e in es if e.kind == "name_ref" and e.dst.startswith("file:")}
        stems = collections.Counter(f.rsplit("/", 1)[-1].split("_")[0].split(".")[0] for f in files)
        out += [f"{uid}>{f}" for f in files if stems[f.rsplit("/", 1)[-1].split("_")[0].split(".")[0]] > 1]
    return out
