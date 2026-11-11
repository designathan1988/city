"""JSDoc types in a JavaScript file (log/trace-rules.md, S2'; TypeScript handbook, JSDoc Reference and Type Checking
JavaScript Files).

The `/** ... */` comment node of the file's tree (S2''': never `/**` inside a string, a template or a regular
expression) written right before a declaration (only modifiers and the declaration's own introduction between them:
`export`, `async`, `static`, `const x =`, ...) types its places as a TypeScript annotation would:
`@param {T} x` (`[x]`, `[x=v]`, `{T=}`: optional), `@returns {T}` / `@return {T}`, `@type {T}` on a variable, a class
field or `this.x = v`; `@template T` names type parameters (any value). Each type is read with TypeScript's grammar
once Closure's forms are written as TypeScript's: `?T` is T or null, `!T` T, `T=` T or undefined, `*` and `?` any,
`Array.<T>` `Array<T>`, `Object<K, V>` `Record<K, V>`, `function(A): R` a function.
"""
from __future__ import annotations

import bisect
import re

import tree_sitter
import tree_sitter_typescript

from . import js_bounds
from .source import Source, comments, field_of, named_children

JS_EXTS = (".js", ".mjs", ".cjs", ".jsx")
DOC_OPEN, DOC_CLOSE, EMPTY = b"/**", b"*/", b"/**/"
GAP = re.compile(r"(?:\s+|export\b|default\b|async\b|static\b|public\b|private\b|protected\b|readonly\b|declare\b"
                 r"|abstract\b|override\b|get\b|set\b|function\b|const\b|let\b|var\b|\*|[A-Za-z_$][\w$]*\s*=(?!=))*")
TAG = re.compile(r"@(param|arg|argument|returns|return|type|template)\b\s*(.*)")
FUNCTION_TYPE = re.compile(r"\bfunction\s*\((?:[^()]|\([^()]*\))*\)(?:\s*:\s*[\w.$]+(?:<[^<>]*>)?)?")
PARSER = tree_sitter.Parser(tree_sitter.Language(tree_sitter_typescript.language_typescript()))
_TYPES = {}                      # (TypeScript text, type parameters) -> tokens


class _Snippet:
    """A type written alone, read as `js_bounds` reads a file's annotation."""

    def __init__(self, data: bytes, scope: frozenset):
        self.src = Source(None, "<jsdoc>", data)
        self.scope = scope

    def type_scope(self) -> set:
        return set(self.scope)


def is_js(r) -> bool:
    return r.src.path is not None and r.src.path.suffix.lower() in JS_EXTS


def doc_of(r, node) -> str | None:
    """The JSDoc comment written right before `node` (or before the statement it introduces)."""
    docs = getattr(r, "jsdoc_index", None)
    if docs is None:
        data = r.src.data
        targets, texts = [], []
        for comment in comments(r.tree_root, r.grammar):
            raw = data[comment.start_byte:comment.end_byte]
            if not raw.startswith(DOC_OPEN) or raw == EMPTY or not raw.endswith(DOC_CLOSE):
                continue                                # a line comment or a plain block comment
            j = comment.end_byte
            while j < len(data) and data[j] in b" \t\r\n":
                j += 1
            targets.append(j)
            texts.append(raw[len(DOC_OPEN):-len(DOC_CLOSE)].decode("utf-8", "replace"))
        docs = r.jsdoc_index = (targets, texts)
    targets, texts = docs
    i = bisect.bisect_right(targets, node.start_byte) - 1
    if i < 0:
        return None
    gap = r.src.data[targets[i]:node.start_byte].decode("utf-8", "replace")
    return texts[i] if GAP.fullmatch(gap) else None


def tags(text: str) -> list[tuple[str, str | None, str]]:
    """Each tag of a comment: (tag, its `{type}` or None, the word after it)."""
    out = []
    for raw in text.splitlines():
        line = raw.strip().lstrip("*").strip()
        found = TAG.match(line)
        if not found:
            continue
        rest, type_text = found.group(2).strip(), None
        if rest.startswith("{"):
            type_text, rest = braced(rest)
        words = rest.split()
        out.append((found.group(1), type_text, words[0] if words else ""))
    return out


def braced(text: str) -> tuple[str | None, str]:
    """`{T} rest` as (T, rest), braces inside T kept."""
    depth = 0
    for i, c in enumerate(text):
        depth += c == "{"
        depth -= c == "}"
        if depth == 0:
            return text[1:i], text[i + 1:].strip()
    return None, text


def typescript(text: str) -> str:
    """A Closure type as TypeScript writes it."""
    t = text.strip()
    optional = t.endswith("=")
    t = t[:-1].strip() if optional else t
    t = t[3:] if t.startswith("...") else t
    nullable = t.startswith("?") and len(t) > 1
    t = t[1:] if nullable or t.startswith("!") else t
    if t in ("*", "?", ""):
        return "any"
    t = FUNCTION_TYPE.sub("Function", t.replace(".<", "<"))
    t = re.sub(r"\bObject<", "Record<", t)
    return f"({t})" + (" | null" if nullable else "") + (" | undefined" if optional else "")


def type_node(text: str, scope: frozenset):
    """The TypeScript node of a JSDoc type, with the reader that reads it."""
    snippet = _Snippet(f"type __JSDOC = {typescript(text)};".encode(), scope)
    root = PARSER.parse(snippet.src.data).root_node
    alias = next((c for c in named_children(root) if c.type == "type_alias_declaration"), None)
    return (field_of(alias, "value") if alias is not None else None), snippet


def bound_of(text: str, scope: frozenset, optional: bool = False) -> tuple:
    key = (text, scope, optional)
    if key not in _TYPES:
        node, snippet = type_node(text, scope)
        _TYPES[key] = js_bounds.of(snippet, node, optional) if node is not None else ()
    return _TYPES[key]


def templates(found: list) -> frozenset:
    names = set()
    for tag, _, word in found:
        if tag == "template":
            names |= {n.strip() for n in word.split(",") if n.strip()}
    return frozenset(names)


def function(r, node, unit) -> None:
    """A function's or method's JSDoc: its parameters' bounds and its result's."""
    text = doc_of(r, node)
    if text is None:
        return
    found = tags(text)
    scope = templates(found) | frozenset(r.type_scope())
    for tag, type_text, word in found:
        if tag in ("param", "arg", "argument") and type_text is not None:
            optional = word.startswith("[")
            name = word.strip("[]").split("=")[0]
            if name in unit.params and not unit.bounds.get(name):
                unit.bounds[name] = bound_of(type_text, scope, optional)
        elif tag in ("returns", "return") and type_text is not None and not unit.bound:
            snippet_node, snippet = type_node(type_text, scope)
            unit.bound = js_bounds.result(snippet, snippet_node, node) if snippet_node is not None else ()


def declared(r, node) -> tuple:
    """The bound a `@type {T}` written before a variable, a field or `this.x = v` gives it."""
    text = doc_of(r, node)
    if text is None:
        return ()
    found = tags(text)
    scope = templates(found) | frozenset(r.type_scope())
    return next((bound_of(t, scope) for tag, t, _ in found if tag == "type" and t is not None), ())
