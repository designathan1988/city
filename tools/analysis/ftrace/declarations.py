"""Declarations and their definitions: a callable declared without a body joins the one definition of its scope and
name (its place becomes an extra location of it), a reference to a declaration binds to the definitions as a linker
binds a call, and a declaration left with several definitions (overloads) refers to each of them."""
from collections import defaultdict



class Declarations:
    def __init__(self, graph):
        self.g = graph

    def merge(self):
        """A callable declared without a body joins the definition of the same scope and name whose file imports
        the declaration's file; failing that, the only such definition. Its place becomes an extra location."""
        groups = defaultdict(list)
        for node in self.g.nodes:
            if node.kind == 'callable':
                groups[(self.g.language_of(node.path), node.scope, node.name)].append(node)
        owner_of = {}
        for members in groups.values():
            bodies = [n for n in members if n.body]
            for declaration in members:
                if declaration.body or not bodies:
                    continue
                owners = [d for d in bodies if declaration.path in self.g.imports.get(d.path, ())]
                if len(owners) != 1:
                    owners = bodies if len(bodies) == 1 else []
                if owners:
                    owner_of[declaration.id] = owners[0].id
        for declaration, owner in owner_of.items():
            self.g.nodes[owner].locations.append(self.g.nodes[declaration].locations[0])
        self.g.nodes = [n for n in self.g.nodes if n.id not in owner_of]
        for new_id, node in enumerate(self.g.nodes):
            node.id = new_id

    def defined(self, candidates):
        """Candidates with each body-less declaration replaced by its definitions (scope and name), as a linker binds a call."""
        if not hasattr(self, '_bodies'):
            self._bodies = defaultdict(list)
            for node in self.g.nodes:
                if node.kind == 'callable' and node.body:
                    self._bodies[(self.g.language_of(node.path), node.scope, node.name)].append(node.id)
        out = []
        for c in candidates:
            node = self.g.nodes[c]
            key = (self.g.language_of(node.path), node.scope, node.name)
            targets = self._bodies.get(key) if node.kind == 'callable' and not node.body else None
            out.extend(targets or [c])
        return list(dict.fromkeys(out))

    def link(self):
        """A declaration that joined no single definition (overloads) refers to every definition of its scope and name."""
        bodies = defaultdict(list)
        for node in self.g.nodes:
            if node.kind == 'callable' and node.body:
                bodies[(self.g.language_of(node.path), node.scope, node.name)].append(node.id)
        for node in self.g.nodes:
            if node.kind == 'callable' and not node.body:
                targets = bodies.get((self.g.language_of(node.path), node.scope, node.name), [])
                for target in targets:
                    self.g.link(node.id, target, 1.0 / len(targets), node.name, node.name_line, len(targets) == 1, ('call',))
