"""Joining the adapters: names of every space resolved to units, across languages; what stays unresolved is reported, never dropped."""
import collections
import re

from .core import words_of
from .graph import BASE_CONFIDENCE, Graph
from .model import LEXICAL, NAME

ROLE_SPACE = {"method": "method", "signal": "signal", "property": "property", "group": "group", "path": "file", "file": "file", "name": None, "node": "node"}
TEMPLATE = re.compile(r"%[sdf]|\{\w*\}")
ENDS_OPEN = re.compile(r"[._/-]$")
# A text worth linking looks like a name: no spaces.
NAMELIKE = re.compile(r"^[\w./:#%{}&-]+$")


class Names:
    """Who defines what, by space, and by file suffix for paths written from a project root."""

    def __init__(self, facts):
        self.table = collections.defaultdict(set)
        self.any = collections.defaultdict(set)
        self.space_of = {}
        self.suffix = collections.defaultdict(set)
        self.prefix = collections.defaultdict(set)
        for space, name, unit in facts.defs:
            self.table[(space, name)].add(unit)
            self.any[name].add(unit)
            self.space_of[unit] = space
            if space == "file":
                parts = name.split("/")
                for i in range(len(parts)):
                    self.suffix["/".join(parts[i:])].add(unit)
            elif "." in name or "_" in name:
                for i, c in enumerate(name):
                    if c in "._":
                        self.prefix[name[:i + 1]].add(unit)

    def find(self, space, text):
        if space == "file":
            return self.suffix.get(text.replace("res://", "").lstrip("./"), set())
        if space:
            return self.table.get((space, text), set())
        return self.any.get(text, set())

    def confidence(self, targets):
        """Different spaces are different things with one name, a table key and the method it names: ambiguity is only within a space."""
        count = collections.Counter(self.space_of.get(t, "") for t in targets)
        return {t: 1.0 / count[self.space_of.get(t, "")] for t in targets}


def _resolve_refs(graph, names, facts, report):
    literals = collections.defaultdict(set)
    for space, text, unit, file, line, role in facts.refs:
        if role not in ("data", "doc", "attribute") and text and unit in graph.units:
            literals[text].add(unit)
    for space, text, unit, file, line, role in facts.refs:
        # Punctuation in quotes is no name, even when a table has it as a key.
        if not text or unit not in graph.units or not any(c.isalpha() for c in text):
            continue
        if role == "doc":
            # A name in prose: the weakest link there is, never one that keeps anything alive.
            for t in sorted(names.any.get(text, ())):
                graph.add_edge(unit, t, "lexical", LEXICAL, 1.0 / len(names.any[text]), file, line, {"text": text, "role": role})
            continue
        if role == "attribute":
            for t in sorted(names.table.get(("key", text), ())):
                graph.add_edge(unit, t, "lexical", LEXICAL, 1.0 / len(names.table[("key", text)]), file, line, {"text": text, "role": role})
            continue
        real = None if space is None or space == "name" or space.startswith("role:") else space
        if space is not None and space.startswith("role:"):
            word = space[5:]
            real = next((ROLE_SPACE[w] for w in words_of(word) if w in ROLE_SPACE), None)
        kind = "name_call" if real in ("method",) else "name_ref"
        # A word in plain text that happens to spell a name links the two, and that is all: only what is written as a name, or says what it names, can be followed.
        named = {"named": True} if real is not None or space == "name" or role == "xmacro" else {}
        if real == "group":
            # A group has no declaration: whoever writes its name meets the others there.
            graph.add_unit((f"group:{text}", "group", text, file, line, line, None, "", {}))
            names.table[("group", text)].add(f"group:{text}")
        targets = names.find(real, text)
        if real == "property":
            # The engine's `get` takes a property of an object and a key of a table alike: the word says nothing of which.
            targets = set(targets) | set(names.table.get(("key", text), ()))
        if not targets and real is None and NAMELIKE.match(text):
            targets = _from_shape(names, text)
            kind = "route" if "/" in text and targets else kind
        if not targets and real is None:
            targets = names.suffix.get(text.replace("res://", ""), set())
        if targets:
            conf = names.confidence(targets)
            for t in sorted(targets):
                graph.add_edge(unit, t, kind, NAME, conf[t], file, line, {"text": text, "role": role, **named})
            report["resolved"].add((file, line, text))
        elif role == "data" and text in literals:
            # A value in data that code compares with or looks for: the data chooses code.
            for t in sorted(literals[text]):
                graph.add_edge(unit, t, "data_choice", NAME, 1.0 / len(literals[text]), file, line, {"text": text})
            report["resolved"].add((file, line, text))
        elif NAMELIKE.match(text):
            report["open"].append((file, line, text, role, space or ""))


def _from_shape(names, text):
    """`area/method`, a key with a part still to come: the name inside the text."""
    if "/" in text and not text.startswith(("res:", ".")):
        last = text.rsplit("/", 1)[-1]
        found = names.any.get(last, set())
        if found:
            return found
    if TEMPLATE.search(text):
        head = TEMPLATE.split(text)[0]
        return names.prefix.get(head, set())
    if ENDS_OPEN.search(text):
        return names.prefix.get(text, set())
    return set()


def _resolve_edges(graph, names, facts, report):
    for src, kind, dst, file, line, source, attrs in facts.edges:
        if dst.startswith("name:"):
            _, space, text = dst.split(":", 2)
            targets = _near(graph, names.find(space if space != "computed" else None, text), file)
            if space == "computed":
                graph.add_edge(src, "ext:computed", kind, source, 1.0, file, line, attrs)
            if not targets:
                report["open"].append((file, line, text, kind, space))
                continue
            conf = names.confidence(targets)
            for t in sorted(targets):
                graph.add_edge(src, t, kind, NAME, conf[t], file, line, attrs)
            report["resolved"].add((file, line, text))
        else:
            graph.add_edge(src, dst, kind, source, BASE_CONFIDENCE.get(source, 1.0), file, line, attrs)


def _near(graph, targets, file):
    """Of many with the name, those in the same file, if any: nearer names shadow farther ones."""
    same = {t for t in targets if t in graph.units and graph.units[t]["file"] == file}
    return same or targets


def _overrides(graph):
    """A method replaces the one of the same name and signature in a base class: a call to the base may run any of them."""
    bases = collections.defaultdict(set)
    for e in graph.edges.values():
        if e.kind == "inherit":
            bases[e.src].add(e.dst)
    methods = collections.defaultdict(dict)
    for uid, u in graph.units.items():
        if u["kind"] == "method" and u["parent"]:
            methods[u["parent"]][(u["name"], u["attrs"].get("signature", ""))] = uid
    for cls, own in sorted(methods.items()):
        seen, todo = set(), sorted(bases.get(cls, ()))
        while todo:
            b = todo.pop()
            if b in seen:
                continue
            seen.add(b)
            todo.extend(sorted(bases.get(b, ())))
            for key, uid in own.items():
                other = methods.get(b, {}).get(key)
                if other:
                    graph.add_edge(other, uid, "override", "ast", 1.0, graph.units[uid]["file"], graph.units[uid]["line"])


def _signals(graph, names, facts):
    """Who emits a signal reaches who is connected to it, through the signal."""
    for e in list(graph.edges.values()):
        if e.kind == "signal_connect":
            handler = e.attrs.get("handler", "").split(".bind(")[0].rsplit(".", 1)[-1]
            for t in sorted(_near(graph, names.find("method", handler), e.sites[0][0])):
                graph.add_edge(e.dst, t, "connected", NAME, e.conf, *e.sites[0])


def _copied_by_name(graph, names):
    """A table key with the very name of a uniform: a loop over the table hands the value over by that name, with no text of it in any code."""
    for (space, name), keys in sorted(names.table.items(), key=lambda kv: (kv[0][0], kv[0][1])):
        uniforms = names.table.get(("uniform", name)) if space == "key" else None
        for key in sorted(keys) if uniforms else ():
            for t in sorted(uniforms):
                graph.add_edge(key, t, "name_ref", NAME, 1.0 / len(uniforms), graph.units[key]["file"], graph.units[key]["line"], {"text": name, "role": "uniform"})


def _groups(graph):
    by = collections.defaultdict(set)
    for e in graph.edges.values():
        if e.kind == "name_ref" and e.attrs.get("role", "").endswith(":group"):
            by[e.attrs.get("text")].add(e.src)
    return by


def build(all_facts):
    """One graph out of every adapter's facts, and the record of what could not be joined."""
    graph = Graph()
    for u in all_facts.units:
        graph.add_unit(u)
    files = {u["file"] for u in graph.units.values()}
    for f in sorted(files):
        graph.add_unit(("file:" + f, "file", f.rsplit("/", 1)[-1], f, 1, 1, None, "", {}))
    names = Names(all_facts)
    report = {"resolved": set(), "open": []}
    _resolve_edges(graph, names, all_facts, report)
    _resolve_refs(graph, names, all_facts, report)
    _overrides(graph)
    _signals(graph, names, all_facts)
    _copied_by_name(graph, names)
    graph.index_children()
    return graph, names, report
