"""The units of a source tree as nodes and their references as weighted edges, with the tables by name, by naming
convention, by file and by scope that everything else looks things up in, the language of each file, and the queries
a trace starts from (a path, `file:line`, a name qualified by scope).

Built in this order: tables, each file's imports and language, declarations merged into their definitions
(declarations.py), then every unit's references resolved into edges (resolve.py, through lookup.py and scopes.py).
"""
import posixpath
import re
from collections import defaultdict

from .declarations import Declarations
from .lookup import Lookup
from .resolve import Resolver
from .scopes import Scopes
from .words import convention_key, is_absolute, path_parts

LINE_SPEC = re.compile(r'(.+):(\d+)')
SCOPE_SPLIT = re.compile(r'::|\.|#')


def _extension(path):
    return posixpath.splitext(path)[1].lower()


class Node:
    __slots__ = ('id', 'path', 'name', 'scope', 'kind', 'start', 'end', 'locations', 'tokens', 'locals', 'body', 'regs',
                 'chains', 'type', 'local_types', 'name_line', 'params')

    def __init__(self, node_id, path, unit):
        self.id = node_id
        self.path = path
        self.name = unit['name']
        self.scope = unit['scope']
        self.kind = unit['kind']
        self.start = unit['start']
        self.end = unit['end']
        self.locations = [(path, unit['start'], unit['end'])]
        self.tokens = unit['tokens']
        self.locals = unit['locals']
        self.body = unit['body']
        self.regs = unit.get('regs', [])
        self.chains = unit.get('chains', [])
        self.type = unit.get('type')
        self.local_types = unit.get('local_types', {})
        self.name_line = unit.get('name_line', unit['start'])
        self.params = unit.get('params', set())


class Edge:
    """A reference: its weight is the name's share among the remaining candidates; it is certain when the name has
    a single definition in the whole tree (or the path names a single file), before any narrowing heuristic."""
    __slots__ = ('weight', 'name', 'line', 'certain', 'kinds', 'where')

    def __init__(self, weight, name, line, certain, kinds=(), where=None):
        self.weight = weight
        self.name = name
        self.line = line
        self.certain = certain
        self.kinds = set(kinds)
        self.where = where  # the file the evidence is written in (its line is `line`)


class Graph:
    def __init__(self, index):
        self.root = index.root
        self.nodes = [Node(i, facts['path'], unit) for i, (facts, unit) in
                      enumerate((facts, unit) for facts in index.facts for unit in facts['units'])]
        self._build_tables()
        self.imports = {n.path: {self.nodes[f].path for f in self._path_targets(n)} for n in self.nodes
                        if n.kind == 'file'}
        self._languages({facts['path']: facts['parser'] for facts in index.facts})
        declarations = Declarations(self)
        declarations.merge()
        self._build_tables()
        self.out = [dict() for _ in self.nodes]
        self.inn = [dict() for _ in self.nodes]
        self.unresolved = [set() for _ in self.nodes]
        self._spans = None
        self.via = {}  # (caller, override) -> the receivers of a polymorphic call: their value picks the override
        self.scopes = Scopes(self)
        resolver = Resolver(self, Lookup(self, self.scopes, declarations), self.scopes)
        for node in self.nodes:
            resolver.resolve(node)
        declarations.link()
        resolver.settle_mutations()

    def _build_tables(self):
        self.by_name = defaultdict(list)
        self.by_convention = defaultdict(list)
        self.file_by_path = {}
        self.files_by_noext = defaultdict(list)
        self.files_by_base = defaultdict(list)
        self.files_by_stem = defaultdict(list)
        self.members = defaultdict(list)
        self.files_by_convention = defaultdict(list)
        for node in self.nodes:
            if node.kind == 'file':
                stem = posixpath.splitext(posixpath.basename(node.path))[0]
                self.files_by_convention[convention_key(stem) or stem.lower()].append(node.id)
            elif node.scope:
                self.members[(('scope', node.scope), node.name)].append(node.id)
            else:
                self.members[(('file', node.path), node.name)].append(node.id)
        for node in self.nodes:
            if node.kind == 'file':
                base = posixpath.basename(node.path)
                stem = posixpath.splitext(base)[0]
                self.file_by_path[node.path] = node.id
                self.files_by_noext[posixpath.splitext(node.path)[0]].append(node.id)
                self.files_by_base[base].append(node.id)
                if stem != base:
                    self.files_by_stem[stem].append(node.id)
            else:
                self.by_name[node.name].append(node.id)
                key = convention_key(node.name)
                if key:
                    self.by_convention[key].append(node.id)

    def _path_targets(self, node):
        targets = set()
        for role, text, _ in node.tokens:
            if role == 's' and path_parts(text):
                targets.update(self.resolve_path(text, node.path))
        targets.discard(node.id)
        return targets

    def _languages(self, parsers):
        """A file's language, from structure: extensions whose files include or import each other with the same
        grammar are one language (a header and its source); a file loaded from another language is a resource there."""
        parent = {}

        def root(ext):
            parent.setdefault(ext, ext)
            while parent[ext] != ext:
                parent[ext] = parent[parent[ext]]
                ext = parent[ext]
            return ext
        for path, targets in self.imports.items():
            for target in targets:
                if parsers.get(path) == parsers.get(target):
                    parent[root(_extension(path))] = root(_extension(target))
        self._parsers = parsers
        self._language = {path: (parsers[path], root(_extension(path))) for path in parsers}

    def language_of(self, path):
        return self._language.get(path)

    def is_object(self, unit):
        """Something calls go through rather than into: data, or a callable the code declares with no body (a signal,
        a function implemented elsewhere); a method called on it acts on it."""
        node = self.nodes[unit]
        return node.kind == 'data' or (node.kind == 'callable' and not node.body)

    def link(self, source, target, weight, name, line, certain, kinds, where=None):
        """Edge kinds: call, write, read, dep (import/include/load/path), string (a name carried as a value),
        register (a function handed to a receiver)."""
        existing = self.out[source].get(target)
        if existing is None:
            edge = Edge(weight, name, line, certain, kinds, where or self.nodes[source].path)
            self.out[source][target] = edge
            self.inn[target][source] = edge
            return
        existing.kinds.update(kinds)
        if weight > existing.weight:
            existing.weight, existing.name, existing.line = weight, name, line
        existing.certain = existing.certain or certain

    # queries

    def resolve_path(self, text, from_path):
        """File node ids a path string names: relative to the referencing file first, else by full suffix."""
        parts = path_parts(text)
        if not parts:
            return []
        if not is_absolute(text):
            joined = posixpath.normpath(posixpath.join(posixpath.dirname(from_path), *parts))
            if not joined.startswith('..'):
                if joined in self.file_by_path:
                    return [self.file_by_path[joined]]
                if joined in self.files_by_noext:
                    found = sorted(self.files_by_noext[joined])
                    # A path without extension names a module; among same-named files, one written in the
                    # referencing file's own extension is the module it imports.
                    own = posixpath.splitext(from_path)[1]
                    same = [f for f in found if posixpath.splitext(self.nodes[f].path)[1] == own]
                    return same or found
        clean = [p for p in parts if p not in ('.', '..')]
        if not clean:
            return []
        found = []
        for file_id in set(self.files_by_base.get(clean[-1], ())) | set(self.files_by_stem.get(clean[-1], ())):
            folders = self.nodes[file_id].path.split('/')[:-1]
            wanted = clean[:-1]
            if len(wanted) <= len(folders) and folders[len(folders) - len(wanted):] == wanted:
                found.append(file_id)
        return sorted(found)

    def find(self, spec):
        """Node ids for 'path:line', a file path, or a name optionally qualified by scope ('A::b', 'A.b')."""
        match = LINE_SPEC.fullmatch(spec)
        if match:
            return self.innermost(match.group(1), int(match.group(2)))
        if '/' in spec or '\\' in spec:
            return self._files(spec)
        parts = [p for p in SCOPE_SPLIT.split(spec) if p]
        if parts:
            name, tail = parts[-1], tuple(parts[:-1])
            hits = [n.id for n in self.nodes if n.kind != 'file' and n.name == name
                    and (not tail or n.scope[len(n.scope) - len(tail):] == tail)]
            if hits:
                return hits
        return self._files(spec)

    def innermost(self, path_spec, line, last_line=None):
        """The smallest unit spanning line..last_line in the file the spec names (the file itself when none does)."""
        last_line = line if last_line is None else last_line
        if self._spans is None:
            self._spans = defaultdict(list)
            for node in self.nodes:
                for path, start, end in node.locations:
                    self._spans[path].append((start, end, node.kind == 'file', node.id))
        paths = [path_spec] if path_spec in self.file_by_path else [self.nodes[f].path for f in self._files(path_spec)]
        best = [(is_file, end - start, node_id) for path in paths for start, end, is_file, node_id in self._spans[path]
                if start <= line and last_line <= end]
        return [min(best)[2]] if best else []

    def _files(self, spec):
        clean = [p for p in re.split(r'[\\/]', spec) if p and p not in ('.', '..')]
        if not clean:
            return []
        return [self.file_by_path[p] for p in self.file_by_path
                if p.split('/')[len(p.split('/')) - len(clean):] == clean]

    def label(self, node_id):
        node = self.nodes[node_id]
        if node.kind == 'file':
            return node.path
        return f"{'::'.join(node.scope + (node.name,))} ({node.path}:{node.start})"
