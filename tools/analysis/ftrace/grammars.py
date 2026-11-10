"""Finds the installed tree-sitter grammars and chooses, per file extension, the one that recognises the tree's own files.

Accepting text is not recognising it: a permissive grammar (a shell, a build language) parses prose and other
languages with few ERROR nodes. Recognition is read from the grammar's reserved words, which the grammar itself
lists as its alphabetic anonymous node kinds: in text written in the grammar's language, words spelled like
reserved words are parsed as reserved words; in foreign text they are parsed as plain words. A grammar is accepted
for an extension when, over the sampled files, most bytes parse without ERROR and most reserved-word spellings are
realised as reserved words. Among accepted grammars the one realising the most reserved words wins."""
import hashlib
import importlib
import os
import pkgutil
import re
from collections import namedtuple

import tree_sitter

MODULE_PREFIX = 'tree_sitter_'
# A grammar is used for an extension only when most of that extension's bytes parse without ERROR nodes...
ACCEPT_ERROR_SHARE = 0.5
# ...and most of the words spelled like its reserved words are parsed as reserved words.
RECOGNITION_MAJORITY = 0.5
# Files per extension whose parse decides the grammar, spread evenly through the sorted list.
SELECTION_SAMPLE = 24
RESERVED_SPELLING = re.compile(r'[A-Za-z_][A-Za-z0-9_]*\Z')

Grammar = namedtuple('Grammar', 'name language ident')


def load_grammars():
    """({name: Grammar}, [(name, reason)]) for every importable tree_sitter_* module; unusable ones carry the reason.

    A grammar's identity is the hash of its module's files, so cached results follow the grammar actually installed."""
    found, skipped = {}, []
    for info in sorted(pkgutil.iter_modules(), key=lambda m: m.name):
        if not info.name.startswith(MODULE_PREFIX):
            continue
        try:
            module = importlib.import_module(info.name)
        except Exception as exc:
            skipped.append((info.name, repr(exc)))
            continue
        version = _module_digest(module)
        base = info.name[len(MODULE_PREFIX):]
        for attr in sorted(dir(module)):
            entry = getattr(module, attr)
            if not callable(entry) or (attr != 'language' and not attr.startswith('language_')):
                continue
            name = base if attr == 'language' else base + '/' + attr[len('language_'):]
            try:
                language = tree_sitter.Language(entry())
            except Exception as exc:
                skipped.append((name, repr(exc)))
                continue
            found[name] = Grammar(name, language, f'{name}@{version}@abi{language.abi_version}')
    return found, skipped


def _module_digest(module):
    folder = os.path.dirname(module.__file__)
    digest = hashlib.sha1()
    for current, dirs, files in os.walk(folder):
        dirs[:] = sorted(d for d in dirs if d != '__pycache__')
        for name in sorted(files):
            with open(os.path.join(current, name), 'rb') as handle:
                digest.update(name.encode() + b'\0' + handle.read())
    return digest.hexdigest()[:16]


def error_share(tree, size):
    """Fraction of the file's bytes inside ERROR nodes (each MISSING node counts one byte)."""
    root = tree.root_node
    if not root.has_error:
        return 0.0
    bad, stack = 0, [root]
    while stack:
        node = stack.pop()
        if node.is_error:
            bad += node.end_byte - node.start_byte
        elif node.is_missing:
            bad += 1
        elif node.has_error:
            stack.extend(node.children)
    return min(1.0, bad / max(size, 1))


def extension_key(rel):
    base = rel.rsplit('/', 1)[-1]
    ext = os.path.splitext(base)[1]
    return ext.lower() if ext else base


def sample(rels):
    rels = sorted(rels)
    if len(rels) <= SELECTION_SAMPLE:
        return rels
    return [rels[(i * len(rels)) // SELECTION_SAMPLE] for i in range(SELECTION_SAMPLE)]


def keywords(language):
    """The grammar's reserved words: visible anonymous node kinds spelled like identifiers."""
    found = set()
    for kind_id in range(language.node_kind_count):
        kind = language.node_kind_for_id(kind_id)
        if (kind and not language.node_kind_is_named(kind_id) and language.node_kind_is_visible(kind_id)
                and RESERVED_SPELLING.match(kind)):
            found.add(kind)
    return frozenset(found)


def recognition(tree, src, reserved):
    """(realised, spelled): leaves outside ERROR and comments spelled like a reserved word, and how many of those
    the parse made reserved words (anonymous leaves) rather than names."""
    spellings = {word.encode() for word in reserved}
    realised = spelled = 0
    cursor = tree.walk()
    while True:
        node = cursor.node
        enter = not (node.is_error or node.is_extra)
        if enter and node.child_count == 0:
            if src[node.start_byte:node.end_byte] in spellings:
                spelled += 1
                realised += not node.is_named
        elif enter and cursor.goto_first_child():
            continue
        while not cursor.goto_next_sibling():
            if not cursor.goto_parent():
                return realised, spelled


def choose_grammars(stats):
    """{ext: grammar name or None} from {ext: {grammar: [(error share, realised, spelled) per sampled file]}}."""
    choice = {}
    for ext, per_grammar in stats.items():
        accepted = []
        for name, rows in per_grammar.items():
            if not rows:
                continue
            share = sum(r[0] for r in rows) / len(rows)
            realised = sum(r[1] for r in rows)
            spelled = sum(r[2] for r in rows)
            if share < ACCEPT_ERROR_SHARE and spelled and realised / spelled > RECOGNITION_MAJORITY:
                accepted.append((-realised, share, name))
        choice[ext] = min(accepted)[2] if accepted else None
    return choice
