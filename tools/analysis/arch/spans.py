"""The named pieces of any version of a file, cheaply: what history compares from commit to commit. Grammar only, no build needed."""
import ast
import json
import re

import tree_sitter
import tree_sitter_cpp
import tree_sitter_gdscript
import tree_sitter_typescript

from .core import digest, text_of
from .shape import C_STYLE, HASH_STYLE, shape

CPP = tree_sitter.Parser(tree_sitter.Language(tree_sitter_cpp.language()))
GD = tree_sitter.Parser(tree_sitter.Language(tree_sitter_gdscript.language()))
TS = tree_sitter.Parser(tree_sitter.Language(tree_sitter_typescript.language_tsx()))
SHADER_FN = re.compile(r"^[ \t]*(?:[\w\[\]]+[ \t]+)+(\w+)[ \t]*\([^;{}]*\)\s*\{", re.M)


def _short(text):
    """A function's own name out of a declarator: `int A::b(int x) const` gives `A::b`."""
    head = text.split("(")[0].strip()
    return head.split()[-1].lstrip("*&") if head else ""


def _cpp(src):
    out = {}
    stack = [(CPP.parse(src).root_node, "")]
    while stack:
        n, scope = stack.pop()
        k = n.type
        if k == "function_definition":
            d = n.child_by_field_name("declarator")
            name = _short(src[d.start_byte:d.end_byte].decode("utf8", "replace")) if d is not None else ""
            if name:
                full = name if "::" in name or not scope else f"{scope}::{name}"
                out[full] = (n.start_point[0] + 1, n.end_point[0] + 1, shape(src[n.start_byte:n.end_byte].decode("utf8", "replace")))
            continue
        if k in ("class_specifier", "struct_specifier"):
            name_node = n.child_by_field_name("name")
            if name_node is not None:
                scope = src[name_node.start_byte:name_node.end_byte].decode("utf8", "replace")
        stack.extend((c, scope) for c in n.children)
    return out


def _by_grammar(parser, src, pieces, scopes, style, inside):
    """Name -> place of the pieces a grammar names, each inside the class it is written in. `inside`: pieces can hold other pieces."""
    out = {}
    stack = [(parser.parse(src).root_node, "")]
    while stack:
        n, scope = stack.pop()
        if n.type in pieces:
            name_node = n.child_by_field_name("name")
            if name_node is not None:
                name = src[name_node.start_byte:name_node.end_byte].decode("utf8", "replace")
                out[scope + name] = (n.start_point[0] + 1, n.end_point[0] + 1, shape(src[n.start_byte:n.end_byte].decode("utf8", "replace"), style))
            if not inside:
                continue
        if n.type in scopes:
            name_node = n.child_by_field_name("name")
            scope = src[name_node.start_byte:name_node.end_byte].decode("utf8", "replace") + "." if name_node is not None else scope
        stack.extend((c, scope) for c in n.children)
    return out


def _gd(src):
    return _by_grammar(GD, src, ("function_definition", "variable_statement", "const_statement", "signal_statement"), ("class_definition",), HASH_STYLE, False)


def _ts(src):
    return _by_grammar(TS, src, ("function_declaration", "method_definition", "generator_function_declaration"), ("class_declaration", "abstract_class_declaration"), C_STYLE, True)


def _py(text):
    out = {}
    try:
        tree = ast.parse(text)
    except (SyntaxError, ValueError):
        return out
    lines = text.split("\n")
    for n in ast.walk(tree):
        if isinstance(n, (ast.FunctionDef, ast.AsyncFunctionDef, ast.ClassDef)):
            out[n.name] = (n.lineno, n.end_lineno, shape("\n".join(lines[n.lineno - 1:n.end_lineno]), HASH_STYLE))
    return out


def _shader(text):
    out = {}
    for m in SHADER_FN.finditer(text):
        line = text.count("\n", 0, m.start()) + 1
        end = text.find("\n}", m.end())
        out[m.group(1)] = (line, text.count("\n", 0, end) + 2, shape(text[m.start():end + 2], C_STYLE))
    return out


def _json(text):
    out = {}
    try:
        value = json.loads(text)
    except ValueError:
        return out
    stack = [("", value)]
    while stack:
        path, v = stack.pop()
        if isinstance(v, dict):
            for k, x in v.items():
                stack.append((f"{path}/{k}", x))
        else:
            out[path] = (0, 0, digest(repr(v)))
    return out


def _csv(text):
    out = {}
    for n, line in enumerate(text.split("\n")[1:], 2):
        key = line.split(",", 1)[0]
        if key:
            out[key] = (n, n, digest(line))
    return out


def spans(path, data):
    """name -> (first line, last line, shape) of what a version of a file holds: code and tables have pieces, prose has none."""
    text = text_of(data)
    if text is None:
        return {}
    ext = path.rsplit(".", 1)[-1].lower()
    src = text.encode("utf8")
    if ext in ("cpp", "cc", "cxx", "c", "h", "hpp", "hh", "inl"):
        return _cpp(src)
    if ext == "gd":
        return _gd(src)
    if ext in ("ts", "tsx", "js", "jsx", "mjs", "cjs", "mts", "cts"):
        return _ts(src)
    if ext == "py":
        return _py(text)
    if ext in ("gdshader", "gdshaderinc"):
        return _shader(text)
    if ext == "json":
        return _json(text)
    if ext == "csv":
        return _csv(text)
    return {}
