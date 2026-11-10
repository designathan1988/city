"""Turns what each unit references into the graph's edges: plain names (through the one lookup), member-access chains
(each name after the first a member of the types reached so far), functions handed to a receiver (registrations) and
paths; then decides which method calls mutate the datum they are called on. An edge's weight is the share of the name
among the candidates that remain; one-word strings are prose, compound-identifier strings are names, path-shaped
strings are file references."""
from collections import defaultdict

from .roles import NAME_ROLES, SELF_WORDS
from .words import is_compound_identifier, path_parts

ASSIGN = 'assign'  # beside 'write': the name is the target of an assignment (its value replaced, not acted on)


class Resolver:
    def __init__(self, graph, lookup, scopes):
        self.g, self.lookup, self.scopes = graph, lookup, scopes
        self.pending = []  # method calls on data, settled once every unit is resolved

    def resolve(self, node):
        names, paths = {}, {}
        for role, text, line in node.tokens:
            if role in NAME_ROLES:
                if text not in node.locals:
                    names.setdefault(text, [line, set()])[1].add(role)
            elif role == 's':
                if is_compound_identifier(text):
                    names.setdefault(text, [line, set()])[1].add('string')
                elif path_parts(text):
                    paths.setdefault(text, line)
        held = []

        def context():
            if not held:
                held.append(self.lookup.context(node))
            return held[0]
        for text, (line, roles) in names.items():
            candidates, certain = self.lookup.lookup(node, text, context, 'string' in roles)
            if not candidates:
                self.g.unresolved[node.id].add(text)
                continue
            kinds = roles | {ASSIGN} if 'write' in roles else roles
            for target in candidates:
                self.g.link(node.id, target, 1.0 / len(candidates), text, line, certain, kinds)
        if node.chains:
            self.chains(node, context)
        if node.regs:
            self.registrations(node)
        for text, line in paths.items():
            files = [f for f in self.g.resolve_path(text, node.path) if f != node.id]
            for target in files:
                self.g.link(node.id, target, 1.0 / len(files), text, line, len(files) == 1, ('dep',))

    def chains(self, node, context):
        """Member-access chains: the first name resolves as any name; each next one is a member of the types reached so
        far (with their bases, and the overrides of a called method), never a same-named unit elsewhere. A method
        called on a datum may mutate it: unless it is found here and writes nothing."""
        for names, role, line, discarded in node.chains:
            first = names[0]
            local_root = False
            if first in SELF_WORDS:
                holders, previous = self.scopes.own(node), []
            elif first in node.local_types:
                holders, previous = self.scopes.of_type(node.local_types[first], node.path), []
                if first not in node.params:  # an object made in the body: writing its members changes no shared state
                    role = 'read' if role == 'write' else role
                    local_root = True
            elif first in node.locals:
                continue
            else:
                previous = self.lookup.lookup(node, first, context)[0]
                holders = [h for c in previous for h in self.scopes.of(c)] or self.scopes.of_type(first, node.path)
            for at, member in enumerate(names[1:], 1):
                last = at == len(names) - 1
                found = self.scopes.members(holders, member, last and role == 'call') if holders else []
                receivers = [p for p in previous if self.g.is_object(p)]
                if not local_root and last and role == 'call' and receivers and (found or discarded):
                    self.pending.append((node.id, tuple(receivers), tuple(found), names[at - 1], line))
                if not found:
                    break
                for target in found:
                    kind = role if last else 'read'
                    self.g.link(node.id, target, 1.0 / len(found), member, line, len(found) == 1,
                               (kind, ASSIGN) if kind == 'write' else (kind,))
                    if last and role == 'call' and receivers and len(found) > 1:  # dispatch: the receiver picks
                        self.g.via.setdefault((node.id, target), set()).update(receivers)
                previous = found
                holders = [h for f in found for h in self.scopes.of(f)]

    def registrations(self, node):
        """A function passed in a call is registered on the receiver of that call: the called method when the code
        has it (running it reaches the function), else the object the method is called on (a signal, a button: firing
        it reaches the function). receiver -> function. Every name here is what the unit's own references already
        resolved to (one lookup); a passed name that resolves to anything but functions is a value, not a function."""
        resolved = defaultdict(list)
        for target, edge in self.g.out[node.id].items():
            if edge.kinds - {'string', 'dep'}:  # a name written as code (a string or a path names something else)
                resolved[edge.name].append(target)
        for callee, passed, line in node.regs:
            sources = [t for t in resolved.get(callee[-1], ()) if self.g.nodes[t].kind == 'callable']
            if not sources and len(callee) > 1:
                sources = [t for t in resolved.get(callee[-2], ()) if self.g.is_object(t)]
            for name in passed:
                functions = resolved.get(name, []) if name != callee[-1] else []
                if not functions or any(self.g.nodes[f].kind != 'callable' for f in functions):
                    continue
                for source in sources:
                    for function in functions:
                        if source != function:
                            self.g.link(source, function, 1.0 / (len(sources) * len(functions)), name, line, False,
                                       ('register',), where=node.path)

    def settle_mutations(self):
        """A method called on a datum mutates it when the method is not found here (it cannot be shown read-only) or
        when it mutates something itself: writes a name or a member, or calls a mutating method on its own receiver.
        Decided to a fixed point, since mutation passes up through calls."""
        mutating = {n.id for n in self.g.nodes if n.kind == 'callable' and (
            any(r == 'write' for r, _, _ in n.tokens) or any(c[1] == 'write' for c in n.chains))}
        changed = True
        while changed:
            changed = False
            for unit, receivers, found, name, line in self.pending:
                if (not found or any(f in mutating for f in found)) and unit not in mutating:
                    mutating.add(unit)
                    changed = True
        for unit, receivers, found, name, line in self.pending:
            if not found or any(f in mutating for f in found):
                for datum in receivers:
                    self.g.link(unit, datum, 1.0 / len(receivers), name, line, len(receivers) == 1, ('write',))
        self.g.mutating = mutating
