"""Reads a tree-sitter node as bytes: its text, the line of an offset, the content of a quoted literal and the names
of a chain. Lines come from byte offsets, never from tree-sitter Points (py-tree-sitter 0.26.0 corrupts memory
when a Point is read: tests/test_runtime_safety.py)."""
import bisect

from .words import IDENT

QUOTES = b'"\'`'
# Literal prefixes such as u8, R, f, b, L before the opening quote.
MAX_STRING_PREFIX = 3
# Fields and child types that are not links of a name (arguments, subscripts): the name of `a.f(x)[i]` is a.f.
NOT_A_LINK_FIELDS = frozenset(['arguments', 'index', 'indices'])


def text_of(src, node):
    return src[node.start_byte:node.end_byte].decode('utf-8', 'replace')


def line_starts(src):
    """Byte offset where each line begins; lines come from bytes, never from tree-sitter Points."""
    starts, at = [0], src.find(b'\n')
    while at >= 0:
        starts.append(at + 1)
        at = src.find(b'\n', at + 1)
    return starts


def line_of(starts, offset):
    return bisect.bisect_right(starts, offset)


def string_content(src, start, end, node=None):
    """Content of a quoted literal spanning exactly [start, end), else None (also None for literals joined by code).

    A short prefix before the quote (u8, R, f, &, ^) is part of the literal only when no named child stands before the
    quote: `a+"b"` is an expression, `&"b"` a literal."""
    if end - start < 2:
        return None
    i = 0
    while i < MAX_STRING_PREFIX and start + i < end and src[start + i] not in QUOTES and not chr(src[start + i]).isspace():
        i += 1
    if start + i >= end - 1 or src[start + i] not in QUOTES:
        return None
    if i and node is not None and any(c.is_named and c.start_byte < start + i for c in node.children):
        return None
    quote = src[start + i]
    if src[end - 1] != quote:
        return None
    body = src[start + i:end]
    k = 0
    while k < 3 and k < len(body) // 2 and body[k] == quote and body[len(body) - 1 - k] == quote:
        k += 1
    inner = body[k:len(body) - k]
    at = inner.find(quote)
    while at >= 0:
        slashes = 0
        while at - slashes - 1 >= 0 and inner[at - slashes - 1] == 0x5C:
            slashes += 1
        if slashes % 2 == 0:
            return None
        at = inner.find(quote, at + 1)
    return inner.decode('utf-8', 'replace')


def chain_leaves(src, root):
    """Names of a chain in source order, leaving out call arguments and subscripts (they are not links of the chain)."""
    found, stack = [], [root]
    while stack:
        current = stack.pop()
        if current.child_count == 0:
            if current.is_named and IDENT.fullmatch(text_of(src, current)):
                found.append(current)
            continue
        cursor = current.walk()
        cursor.goto_first_child()
        while True:
            child = cursor.node
            if cursor.field_name not in NOT_A_LINK_FIELDS and not (child.is_named and 'argument' in child.type):
                stack.append(child)
            if not cursor.goto_next_sibling():
                break
    return sorted(found, key=lambda leaf: leaf.start_byte)


def leaf_starts(node):
    starts, stack = [], [node]
    while stack:
        current = stack.pop()
        if current.child_count == 0:
            starts.append(current.start_byte)
        else:
            stack.extend(current.children)
    return starts
