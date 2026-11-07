"""The API of whatever the repository builds on, read at run time from the description it ships. Nothing here names a method of it:
each role comes from a method's shape (virtual, the type and name of an argument)."""
import json
import re

from .core import words_of


class Api:
    """The engine's methods by name. Empty when the repository ships no description: then by-name positions are not known."""

    def __init__(self):
        self.methods = {}
        self.virtual = set()
        self.repeating = set()
        self.input = set()
        self.classes = {}
        self.singletons = set()
        self.words = set()
        self.props = set()

    def add_class(self, name, base, methods, members=()):
        self.classes[name] = base
        self.props.update(m["name"] for m in members)
        for m in methods:
            args = [(a["name"], a["type"]) for a in m.get("arguments", [])]
            self.methods.setdefault(m["name"], []).append((name, args, m.get("return_value", {}).get("type", m.get("return_type", "")), bool(m.get("is_vararg"))))
            self.words.add(m["name"])
            if m.get("is_virtual"):
                self.virtual.add(m["name"])
                if len(args) == 1 and args[0][1] == "float" and args[0][0] == "delta":
                    self.repeating.add(m["name"])
                if any("input" in words_of(t) or n == "event" for n, t in args):
                    self.input.add(m["name"])

    def roles(self, method):
        """What each argument of a method means when it is written as text or given as a function: the argument's own type and name."""
        out = {}
        for _, args, _, vararg in self.methods.get(method, ()):
            for i, (name, type_) in enumerate(args):
                kinds = set(words_of(type_))
                if "callable" in kinds:
                    out[i] = "callable"
                elif "node" in kinds and "path" in kinds:
                    out[i] = "node"
                elif kinds & {"string", "path", "signal"}:
                    out.setdefault(i, f"{type_}:{name}")
        return out

    def returns(self, cls, method):
        for owner, _, ret, _ in self.methods.get(method, ()):
            if owner == cls:
                return ret
        return ""


def _version_wanted(texts):
    for text in texts:
        found = re.search(r"features=PackedStringArray\(\"(\d+\.\d+)\"", text)
        if found:
            return found.group(1)
    return None


_PARSED = {}


def load(repo, tracked):
    """The description among the tracked files that matches what the project asks for, by the shape of the file."""
    api = Api()
    wanted = _version_wanted(repo.text(b) or "" for p, b in tracked.items() if p.endswith(".godot"))
    found = []
    for path in sorted(tracked):
        if path.endswith(".json") and "api" in path.rsplit("/", 1)[-1]:
            blob = tracked[path]
            if blob not in _PARSED:
                data = repo.text(blob)
                _PARSED[blob] = json.loads(data) if data and '"builtin_classes"' in data and '"singletons"' in data else None
            if _PARSED[blob] is not None:
                found.append(_PARSED[blob])
    pick = None
    for table in found:
        head = table.get("header", {})
        if wanted == f"{head.get('version_major')}.{head.get('version_minor')}":
            pick = table
    pick = pick or (found[-1] if found else None)
    if pick is None:
        return api
    for c in pick["classes"]:
        api.add_class(c["name"], c.get("inherits", ""), c.get("methods", []), [*c.get("properties", []), *c.get("signals", []), *c.get("constants", [])])
    for c in pick["builtin_classes"]:
        api.add_class(c["name"], "", c.get("methods", []), [*c.get("members", []), *c.get("constants", [])])
    api.singletons = {s["name"] for s in pick["singletons"]}
    for f in pick["utility_functions"]:
        api.words.add(f["name"])
    return api
