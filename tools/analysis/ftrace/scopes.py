"""Where the members of a thing are: a class or namespace's own scope, a file (a script is its class) or the holders of
a declared type; with the bases a container's header names and the subclasses that follow from them, so a member is
found through its type and never by a same-named unit elsewhere."""
from collections import defaultdict

from .words import convention_key



class Scopes:
    def __init__(self, graph):
        self.g = graph

    def of(self, node_id):
        """Where the members of what a unit names are: a class or namespace's own scope, a file (a script is its class),
        or the holders of the unit's declared type."""
        node = self.g.nodes[node_id]
        if node.kind == 'container':
            return [('scope', node.scope + (node.name,))]
        if node.kind == 'file':
            return [('file', node.path)]
        return self.of_type(node.type, node.path) if node.type else []

    def of_type(self, type_name, from_path):
        """Where the members of a type named from a file are: classes and namespaces of that file's language with
        the name, else a script file of that language named after the type (a script is its class)."""
        if not type_name:
            return []
        language = self.g.language_of(from_path)
        found = [('scope', self.g.nodes[c].scope + (self.g.nodes[c].name,)) for c in self.g.by_name.get(type_name, ())
                 if self.g.nodes[c].kind == 'container' and self.g.language_of(self.g.nodes[c].path) == language]
        if not found:
            key = convention_key(type_name) or type_name.lower()
            found = [('file', self.g.nodes[f].path) for f in self.g.files_by_convention.get(key, ())
                     if self.g.language_of(self.g.nodes[f].path) == language]
        return list(dict.fromkeys(found))

    def own(self, node):
        found = [('scope', node.scope)] if node.scope else []
        return found + [('file', node.path)]

    def members(self, holders, name, overrides):
        found, seen, todo = [], set(), list(holders)
        while todo:
            holder = todo.pop()
            if holder in seen:
                continue
            seen.add(holder)
            direct = self.g.members.get((holder, name), [])
            found.extend(direct)
            if not direct:
                todo.extend(self.bases(holder))
        if overrides:
            for holder in list(seen):
                found.extend(m for sub in self.subclasses(holder) for m in self.g.members.get((sub, name), []))
        return list(dict.fromkeys(found))

    def bases(self, holder):
        if not hasattr(self, '_base_map'):
            self._base_map = defaultdict(list)
            for node in self.g.nodes:
                if node.kind not in ('container', 'file'):
                    continue
                own = self.of(node.id)[0]
                for role, text, _ in node.tokens:
                    if role == 'dep' and text != node.name:
                        for base in self.of_type(text, node.path):
                            if base != own:
                                self._base_map[own].append(base)
        return self._base_map.get(holder, [])

    def subclasses(self, holder):
        if not hasattr(self, '_sub_map'):
            self.bases(holder)
            self._sub_map = defaultdict(set)
            for own, bases in self._base_map.items():
                for base in bases:
                    self._sub_map[base].add(own)
        found, todo = set(), [holder]
        while todo:
            for sub in self._sub_map.get(todo.pop(), ()):
                if sub not in found:
                    found.add(sub)
                    todo.append(sub)
        return found
