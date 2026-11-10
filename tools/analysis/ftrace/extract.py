"""Turns one source file into definition units and the tokens inside each, for any tree-sitter grammar or plain text.

The walk uses only conventions that tree-sitter grammars share, never a list of node kinds:
- comments are `extra` nodes; keywords and punctuation are anonymous nodes; names are named leaves;
- a definition reaches its name through the field `name` (with `scope` for qualified names) or a `declarator` chain;
- a definition is callable when a `parameters`/`parameter` field hangs off it or off its `declarator`/`value` chain;
- a definition with a `body` that is not callable is a container: its body holds member definitions;
- definitions sit in statement position: children of the root or of a `body`, reached through unfielded children or
  the fields `declaration`/`definition`; a node whose unfielded children are definitions (an include guard, a
  conditional block) is a wrapper, not a definition;
- inside a callable, the same rule finds locals; with the parameter names they shadow outer definitions;
- inside a parameter, the `type`, `value` and `default_value` fields hold types and defaults, not the name.
Files without an accepted grammar fall back to a universal lexer, one unit per file: values quoted with " or ' are
strings (role 's'); bare words, which may be prose, are text (role 'w') and never resolve to definitions.
"""
import re

import tree_sitter

from .definitions import bind_statement_names, callable_part, declarator_names, definition_of, keyword_led, member_name, opens_block, parameter_names, type_of, wraps_definitions
from .nodes import line_of, line_starts, string_content, text_of
from .roles import READ, call_arguments, classify, imports_only, mark_header, record_chain, record_registration
from .words import IDENT

CONTAINER, STATEMENT, LOCAL_CONTAINER, LOCAL_STATEMENT, CODE = range(5)
STATEMENT_MODES = (STATEMENT, LOCAL_STATEMENT)
LOCAL_MODES = (LOCAL_CONTAINER, LOCAL_STATEMENT)
STATEMENT_FIELDS = frozenset([None, 'declaration', 'definition'])
LEXICAL_TOKEN = re.compile(r'"(?:\\.|[^"\\\n])*"|\'(?:\\.|[^\'\\\n])*\'|[^\W\d]\w*')
EMPTY = frozenset()


def extract_source(rel, data, language, parser_name=None):
    """Facts of one file: {'path', 'parser', 'lines', 'units'}; units[0] is the file itself."""
    stem = rel.rsplit('/', 1)[-1]
    file_unit = _unit(stem, (), 'file', True, 1, data.count(b'\n') + 1, None)
    if language is None:
        _lexical(data, file_unit)
        return {'path': rel, 'parser': 'lexical', 'lines': file_unit['end'], 'units': [file_unit]}
    tree = tree_sitter.Parser(language).parse(data)
    units = [file_unit]
    _walk(tree, data, units, line_starts(data), {})
    return {'path': rel, 'parser': parser_name or 'tree-sitter', 'lines': file_unit['end'], 'units': units}


def _unit(name, scope, kind, body, start, end, parent):
    return {'name': name, 'scope': scope, 'kind': kind, 'body': body, 'start': start, 'end': end,
            'parent': parent, 'tokens': [], 'locals': set(), 'regs': [], 'chains': [], 'local_types': {},
            'type': None, 'name_line': start, 'params': set()}


def _walk(tree, src, units, starts, roles):
    cursor = tree.walk()
    contexts = [(0, CONTAINER, CODE, EMPTY)]
    if not cursor.goto_first_child():
        return
    while True:
        child_context = _visit(cursor.node, cursor.field_name, contexts[-1], src, units, starts, roles)
        if child_context is not None and cursor.goto_first_child():
            contexts.append(child_context)
            continue
        while not cursor.goto_next_sibling():
            if not cursor.goto_parent():
                return
            contexts.pop()
            if not contexts:
                return


def _visit(node, field, context, src, units, starts, roles):
    """Records what the node contributes and returns the context for its children (None: do not descend)."""
    owner, mode, body_mode, skip = context
    if node.is_extra:
        if node.is_named and node.end_byte > node.start_byte:
            units[owner]['tokens'].append(('c', text_of(src, node), line_of(starts, node.start_byte)))
        return None
    if not node.is_named:
        return None
    literal = string_content(src, node.start_byte, node.end_byte, node)
    if literal is not None:
        if literal:
            units[owner]['tokens'].append(('s', literal, line_of(starts, node.start_byte)))
        return None
    if node.child_count == 0:
        if node.start_byte not in skip:
            text = text_of(src, node)
            if IDENT.fullmatch(text):
                units[owner]['tokens'].append((roles.get(node.start_byte, READ), text, line_of(starts, node.start_byte)))
        return None
    classify(node, src, roles)
    record_chain(node, src, roles, skip, units[owner], starts)
    if call_arguments(node, src) is not None:
        record_registration(node, src, units[owner], starts)
    if mode == CODE:
        holder = callable_part(node, 0) if node.child_by_field_name('body') is not None else None
        if holder is not None:  # a closure inside the code: its parameters and its body's locals are local names
            names, types = parameter_names(holder, src)
            units[owner]['locals'].update(names)
            units[owner]['local_types'].update(types)
            return (owner, CODE, LOCAL_CONTAINER, skip)
        return (owner, body_mode if field == 'body' and body_mode != CODE else CODE, CODE, skip)
    declares = mode == LOCAL_STATEMENT and node.child_by_field_name('declarator') is not None  # for (int i = 0; ...)
    if mode in STATEMENT_MODES and field not in STATEMENT_FIELDS and not declares:
        if field == 'body':
            return (owner, LOCAL_CONTAINER if mode == LOCAL_STATEMENT else CONTAINER, CODE, skip)
        if mode == LOCAL_STATEMENT and opens_block(node, src):  # { ... } under `consequence` and the like
            return (owner, LOCAL_CONTAINER, CODE, skip)
        if mode == LOCAL_STATEMENT and keyword_led(node, src):  # else / elif / catch clauses
            return (owner, LOCAL_STATEMENT, CODE, skip)
        return (owner, CODE, CODE, skip)
    if mode in LOCAL_MODES:
        bind_statement_names(node, src, units[owner])
    if imports_only(node, src):  # what an import names is referenced, never declared
        return (owner, CODE, CODE, skip)
    found = definition_of(node, src)
    if found is not None:
        name, qualifier, chain = found
        in_statement = mode in STATEMENT_MODES
        if (not in_statement or node.end_byte - node.start_byte > len(name.encode())) and not wraps_definitions(node, src):
            others = [n for n in declarator_names(node, src) if n != name]  # V2 r = a, s = b;
            if mode in LOCAL_MODES:
                units[owner]['locals'].add(name)
                units[owner]['locals'].update(others)
                kind_of = type_of(src, node)
                if kind_of:
                    units[owner]['local_types'][name] = kind_of
                    units[owner]['local_types'].update({n: kind_of for n in others})
                return (owner, CODE, LOCAL_CONTAINER, skip)
            context = _new_unit(node, src, units, owner, name, qualifier, chain, starts, roles)
            first = units[context[0]]
            if first['kind'] == 'data':
                for other in others:
                    sibling = _unit(other, first['scope'], 'data', False, first['start'], first['end'], owner)
                    sibling['type'] = first['type']
                    units.append(sibling)
            return context
    member = member_name(node, src) if mode == CONTAINER and units[owner]['kind'] == 'container' else None
    if member is not None:
        unit = _unit(member[0], units[owner]['scope'] + (units[owner]['name'],), 'data', False,
                     line_of(starts, node.start_byte), line_of(starts, max(node.start_byte, node.end_byte - 1)), owner)
        units.append(unit)
        return (len(units) - 1, CODE, CODE, frozenset([member[1]]))
    return (owner, LOCAL_STATEMENT if mode in LOCAL_MODES else STATEMENT, CODE, skip)


def _new_unit(node, src, units, owner, name, qualifier, chain, starts, roles):
    parent = units[owner]
    scope = parent['scope'] + ((parent['name'],) if parent['kind'] != 'file' else ()) + qualifier
    holder = callable_part(node, 0)
    has_body = node.child_by_field_name('body') is not None or (
        holder is not None and holder.child_by_field_name('body') is not None)
    kind = 'callable' if holder is not None else ('container' if has_body else 'data')
    unit = _unit(name, scope, kind, has_body, line_of(starts, node.start_byte),
                 line_of(starts, max(node.start_byte, node.end_byte - 1)), owner)
    unit['name_line'] = line_of(starts, max(chain))
    unit['type'] = type_of(src, node) or (type_of(src, holder) if holder is not None else None)
    if holder is not None:
        unit['locals'], unit['local_types'] = parameter_names(holder, src)
        unit['params'] = set(unit['locals'])
    if kind == 'container':
        mark_header(node, src, chain, roles)
    units.append(unit)
    body_mode = LOCAL_CONTAINER if kind == 'callable' else (CONTAINER if kind == 'container' else CODE)
    return (len(units) - 1, CODE, body_mode, chain)


def _lexical(data, unit):
    text = data.decode('utf-8', 'replace')
    line, position = 1, 0
    for match in LEXICAL_TOKEN.finditer(text):
        line += text.count('\n', position, match.start())
        position = match.start()
        token = match.group()
        if token[0] in '"\'':
            if len(token) > 2:
                unit['tokens'].append(('s', token[1:-1], line))
        else:
            unit['tokens'].append(('w', token, line))
