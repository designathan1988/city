"""The one lookup of a plain name from a unit. Resolution is search-based (by name, as code navigation without a
compiler does), kept to what the unit can see (its file, the files it includes or imports, the scopes it sits in and
its bases) and narrowed the way scoped lookup narrows it; each step applies only when it keeps a candidate:
1. candidates in the referencing unit's own scope;
2. candidates located in the unit's own file or in a file that file imports;
3. candidates whose container's name appears in the unit's context: its own names plus the names written in the
   data units it references (the declared type of a receiver, for example).
Names a unit defines locally (parameters, locals) shadow outer definitions."""
import posixpath

from .roles import NAME_ROLES
from .words import convention_key



class Lookup:
    def __init__(self, graph, scopes, declarations):
        self.g, self.scopes, self.declarations = graph, scopes, declarations

    def lookup(self, node, text, context, string=False):
        """The one lookup of a plain name from a unit: by name (else by naming convention), declarations replaced by
        their definitions, kept to what the unit can see (a name carried in a string reaches anywhere), narrowed by
        scope. Returns (candidates, certain)."""
        candidates = self.g.by_name.get(text)
        if not candidates:
            key = convention_key(text)
            candidates = self.g.by_convention.get(key) if key else None
        if not candidates:
            return [], False
        certain = len(candidates) == 1
        candidates = self.declarations.defined(c for c in candidates if c != node.id)
        if not string:
            candidates = self.visible(node, candidates)
        if len(candidates) > 1:
            candidates = self.narrow(node, candidates, context())
        return candidates, certain

    def visible(self, node, candidates):
        """What a plain name can reach from a unit, as the language's own lookup does: its file, the files that file
        includes or imports (transitively), the files of its class's bases; and anywhere, a type: a class or namespace,
        or a script file's class (a file-level name its file is named after)."""
        files = self.visible_files(node)
        language = self.g.language_of(node.path)
        out = []
        for c in candidates:
            other = self.g.nodes[c]
            if other.scope and other.kind not in ('container', 'file') and other.scope not in self.open_scopes(node):
                continue  # a member of a class or namespace the unit is not inside: reached through obj.x or A::x
            if any(p in files for p, _, _ in other.locations):
                out.append(c)
            elif self.g.language_of(other.path) != language:
                continue
            elif other.kind in ('container', 'file'):
                out.append(c)
            elif not other.scope and convention_key(other.name) == self.file_key(other.path):
                out.append(c)
        return out

    def open_scopes(self, node):
        """The scopes whose members a unit names plainly: the scopes it sits in (its class, the namespaces around it),
        the scopes of its class's bases, and the namespaces its file opens with a dependency construct (using ...)."""
        if not hasattr(self, '_open'):
            self._open = {}
        key = (node.path, node.scope)
        if key in self._open:
            return self._open[key]
        scopes = {node.scope[:i] for i in range(len(node.scope) + 1)}
        todo, seen = list(self.scopes.own(node)), set()
        while todo:
            holder = todo.pop()
            if holder in seen:
                continue
            seen.add(holder)
            if holder[0] == 'scope':
                scopes.add(holder[1])
            todo.extend(self.scopes.bases(holder))
        file_id = self.g.file_by_path.get(node.path)
        if file_id is not None:
            for role, text, _ in self.g.nodes[file_id].tokens:
                if role == 'dep':
                    scopes.update(h[1] for h in self.scopes.of_type(text, node.path) if h[0] == 'scope')
        self._open[key] = scopes
        return scopes

    def file_key(self, path):
        stem = posixpath.splitext(posixpath.basename(path))[0]
        return convention_key(stem) or stem.lower()

    def visible_files(self, node):
        if not hasattr(self, '_closure'):
            self._closure = {}
        key = (node.path, node.scope)
        if key in self._closure:
            return self._closure[key]
        files, todo = set(), [p for p, _, _ in node.locations]
        holders = list(self.scopes.own(node))
        seen_holders = set()
        while holders:
            holder = holders.pop()
            if holder in seen_holders:
                continue
            seen_holders.add(holder)
            if holder[0] == 'file':
                todo.append(holder[1])
            else:
                todo.extend(self.g.nodes[c].path for c in self.g.by_name.get(holder[1][-1], ()) if holder[1] and
                            self.g.nodes[c].kind == 'container' and self.g.nodes[c].scope + (self.g.nodes[c].name,) == holder[1])
            holders.extend(self.scopes.bases(holder))
        language = self.g.language_of(node.path)
        while todo:
            path = todo.pop()
            if path in files:
                continue
            files.add(path)
            todo.extend(p for p in self.g.imports.get(path, ()) if self.g.language_of(p) == language)
        self._closure[key] = files
        return files

    def context(self, node):
        own = {t for r, t, _ in node.tokens if r in NAME_ROLES}
        names = set(own)
        for text in own - node.locals:
            for target in self.g.by_name.get(text, ()):
                if self.g.nodes[target].kind == 'data':
                    names.update(t for r, t, _ in self.g.nodes[target].tokens if r in NAME_ROLES)
        visible = set()
        for path, _, _ in node.locations:
            visible.add(path)
            visible.update(self.g.imports.get(path, ()))
        return names, visible

    def narrow(self, node, candidates, context):
        names, visible = context
        steps = (
            lambda c: self.g.nodes[c].scope == node.scope,
            lambda c: any(p in visible for p, _, _ in self.g.nodes[c].locations),
            lambda c: bool(self.g.nodes[c].scope) and self.g.nodes[c].scope[-1] in names,
        )
        for keep in steps:
            kept = [c for c in candidates if keep(c)]
            if kept:
                candidates = kept
            if len(candidates) == 1:
                break
        return candidates
