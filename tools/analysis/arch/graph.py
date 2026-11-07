"""The program as one graph: units and typed edges, each edge with where it was seen, what vouches for it and how much."""
import collections

from .core import noisy_or
from .model import AST, GRAMMAR, HISTORY, LEXICAL, NAME, RECORDED, TYPES

BASE_CONFIDENCE = {AST: 1.0, TYPES: 1.0, GRAMMAR: 1.0, RECORDED: 1.0}
# Execution flows along these; data along the next; the rest only tells what is near what.
FLOW = {"call", "virtual", "operator", "construct", "destroy", "convert", "template", "override", "branch", "lambda", "callback", "stored", "thread",
        "callable", "setter", "await", "signal_emit", "connected", "route", "name_call", "bind", "script", "main", "autoload", "indirect", "raii", "connection", "recorded_call", "data_choice"}
DATA = {"read", "write", "enumref", "case"}
STRUCTURE = {"type", "inherit", "include", "import", "macro", "preload", "load", "fnref", "memberptr", "name_ref", "group", "uid", "lexical", "cochange"}
# An edge whose only source is a shared word never keeps a target alive.
GENERIC_SOURCES = {LEXICAL, HISTORY}


class Edge:
    __slots__ = ("src", "dst", "kind", "sources", "conf", "sites", "attrs", "generic")

    def __init__(self, src, dst, kind):
        self.src, self.dst, self.kind = src, dst, kind
        self.sources, self.conf, self.sites, self.attrs = {}, 0.0, [], {}

    def seen(self, source, conf, file, line, attrs):
        self.sources[source] = max(self.sources.get(source, 0.0), conf)
        self.conf = noisy_or(self.sources.values())
        self.generic = all(s in GENERIC_SOURCES for s in self.sources)
        if (file, line) not in self.sites:
            self.sites.append((file, line))
        for k, v in attrs.items():
            if k == "depth":
                self.attrs[k] = max(self.attrs.get(k, 0), v)
            elif v or k not in self.attrs:
                self.attrs[k] = v


class Graph:
    def __init__(self):
        self.units = {}
        self.edges = {}
        self.out = collections.defaultdict(list)
        self.into = collections.defaultdict(list)
        self.children = collections.defaultdict(list)

    def add_unit(self, record):
        uid, kind, name, file, line, end, parent, shape, attrs = record
        known = self.units.get(uid)
        if known is not None and not known["attrs"].get("declared") and not attrs.get("declared"):
            configs = {*known["attrs"].get("configs", ()), attrs.get("config")} - {None}
            known["attrs"].update(attrs, configs=sorted(configs))
            return
        if known is not None and attrs.get("declared"):
            return
        self.units[uid] = {"id": uid, "kind": kind, "name": name, "file": file, "line": line, "end": end, "parent": parent, "shape": shape, "attrs": dict(attrs)}
        if "config" in attrs:
            self.units[uid]["attrs"]["configs"] = [attrs["config"]]

    def add_edge(self, src, dst, kind, source, conf, file, line, attrs=None):
        if src == dst:
            return None
        key = (src, dst, kind)
        e = self.edges.get(key)
        if e is None:
            e = Edge(src, dst, kind)
            self.edges[key] = e
            self.out[src].append(e)
            self.into[dst].append(e)
        e.seen(source, conf, file, line, attrs or {})
        return e

    def index_children(self):
        self.children.clear()
        for uid, u in self.units.items():
            if u["parent"] is not None:
                self.children[u["parent"]].append(uid)

    def ancestors(self, uid):
        u = self.units.get(uid)
        while u is not None and u["parent"] is not None:
            yield u["parent"]
            u = self.units.get(u["parent"])

    def callable_of(self, uid):
        """The function a branch, lambda or loop body belongs to."""
        u = self.units.get(uid)
        while u is not None and u["kind"] in ("branch",):
            u = self.units.get(u["parent"])
        return u["id"] if u is not None else uid


__all__ = ["Graph", "Edge", "FLOW", "DATA", "STRUCTURE", "BASE_CONFIDENCE", "NAME"]
