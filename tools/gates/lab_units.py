"""Where a program's functions and names are, in the languages LAB_COPY reads: each function body not inside another (C++ and
GLSL by cpp_syntax, GDScript by gd_syntax, TypeScript and JavaScript by their braces here), the spans a source is judged and
indexed by (those bodies, the code outside them, each program held in a multi-line string, as a shader is), and the names a
source defines for others: outside every function body and every parenthesis (functions, classes, members, constants, module
variables, enumerators; never a local nor a parameter), each function's own name, a data table's keys."""
import cpp_syntax
import gd_syntax
import lab_tokens
import lex_c
from tokens import string_body

CONTROL = frozenset({"if", "for", "while", "switch", "catch", "with"})  # words whose `(...) {` opens a block, not a function
_SCRIPT_DECLARE = frozenset({"function", "class", "interface", "type", "enum", "const", "let", "var", "func", "class_name",
                             "signal", "namespace"})
_MEMBER_AFTER = frozenset({"{", ";", "}", ",", "readonly", "static", "private", "public", "protected", "async", "get", "set",
                           "abstract", "override", "declare"})
_CPP_HEAD = frozenset({"struct", "class", "enum", "namespace", "using", "union"})


class Span:
    """A piece of a source: the line it is known by, its function's name ('' outside functions), its neutral tokens and
    its non-trivial numbers in order."""
    __slots__ = ("line", "name", "tokens", "values")

    def __init__(self, line, name, tokens, values):
        self.line, self.name, self.tokens, self.values = line, name, tokens, values


def units(src):
    """[(name, first line, last line)] of a source's function bodies not inside another ('?' for an unnamed one)."""
    if src.binary or src.lang is None:
        return []
    if src.lang == "gd":
        return [(n, first, first + length - 1) for n, first, length in gd_syntax.function_spans(gd_syntax.logical_lines(src))]
    if src.lang in ("cpp", "glsl", "js"):
        return code_units(src.code(), src.lang)
    return []


def code_units(code, lang):
    code = [t for t in code if not t.pp]
    if lang == "js":
        return _js_units(code)
    return [(n, line, line + length - 1) for n, line, length in cpp_syntax.function_spans(code)]


def _js_units(code):
    out, stack = [], []
    for i, t in enumerate(code):
        if t.kind != "punct":
            continue
        if t.text == "{":
            if any(kind == "func" for kind, _n, _l in stack):
                stack.append(("block", None, t.line))
            else:
                kind, name = _js_opens(code, i)
                stack.append((kind, name, t.line))
        elif t.text == "}" and stack:
            kind, name, line = stack.pop()
            if kind == "func":
                out.append((name, line, t.line))
    return out


def _js_opens(code, i):
    """('func', name) when the `{` at i opens a function body: after `=>`, or after a parameter list (with or without a
    return type) not of a control word; else ('block', None)."""
    j = i - 1
    if j < 0:
        return "block", None
    if code[j].text == "=>":
        return "func", _arrow_name(code, j)
    close = _params_close(code, j)
    opened = _matching_open(code, close) if close is not None else None
    if not opened:
        return "block", None
    before = code[opened - 1]
    if before.kind == "ident" and before.text in CONTROL:
        return "block", None
    if before.kind == "ident" and before.text != "function":
        return "func", before.text
    return "func", _assigned_name(code, opened - 2 if before.text == "function" else opened - 1)


def _params_close(code, j):
    """The `)` closing a parameter list that ends at j or before a return type `): Type` ending at j, else None."""
    if code[j].text == ")":
        return j
    depth = 0
    for k in range(j, -1, -1):
        x = code[k].text
        if x in (">", "]", ")"):
            depth += 1
        elif x in ("<", "[", "("):
            if depth == 0:
                return None
            depth -= 1
        elif depth == 0 and x == ":":
            return k - 1 if k and code[k - 1].text == ")" else None
        elif depth == 0 and x in ("{", "}", ";", "=", ","):
            return None
    return None


def _matching_open(code, close):
    depth = 0
    for k in range(close, -1, -1):
        if code[k].text == ")":
            depth += 1
        elif code[k].text == "(":
            depth -= 1
            if depth == 0:
                return k
    return None


def _arrow_name(code, arrow):
    """The name an arrow function is given: its parameters (in parentheses, with or without a return type, or one bare
    name) are passed over to the `=` or `:` before them."""
    k = arrow - 1
    close = _params_close(code, k) if k >= 0 else None
    start = _matching_open(code, close) if close is not None else k
    return _assigned_name(code, (start if start is not None else k) - 1)


def _assigned_name(code, k):
    """The name a function expression is given (`name = ...`, `name: ...`), else '?'."""
    if k >= 0 and code[k].text == "async":
        k -= 1
    if k >= 1 and code[k].text in ("=", ":") and code[k - 1].kind == "ident":
        return code[k - 1].text
    return "?"


def api_names(src):
    """[(name, line)] of what a source defines for others."""
    if src.binary or src.lang is None:
        return []
    if src.lang == "data":
        return data_keys(src.text)
    return code_names(src.code(), src.lang, units(src))


def code_names(code, lang, spans):
    body = {line for _n, first, last in spans for line in range(first, last + 1)}
    code = [t for t in code if not t.pp]
    found = _declared_cpp(code) if lang in ("cpp", "glsl") else _declared_script(code)
    return [(n, line) for n, line in found if line not in body] + [(n, first) for n, first, _l in spans if n and n != "?"]


def _declared_cpp(code):
    out, depth, braces, head = [], 0, [], []
    for i, tok in enumerate(code):
        t = tok.text
        if t in ("(", ")"):
            depth = depth + 1 if t == "(" else max(0, depth - 1)
            continue
        if t == "{":
            braces.append("enum" if "enum" in head else "")
        elif t == "}" and braces:
            braces.pop()
        if t in ("{", "}", ";"):
            head = []
            continue
        head.append(t)
        if depth or tok.kind != "ident" or not i or i + 1 >= len(code) or t in lab_tokens.DECLARE or t in lab_tokens.KEPT_WORDS:
            continue
        prev, nxt = code[i - 1], code[i + 1].text
        typed = prev.text in lab_tokens.DECLARE or prev.text in (">", "*", "&") or (
            prev.kind == "ident" and prev.text not in lab_tokens.KEPT_WORDS and prev.text not in ("return", "else"))
        enumerator = braces and braces[-1] == "enum" and prev.text in ("{", ",") and nxt in (",", "}", "=")
        if prev.text in _CPP_HEAD or enumerator or (typed and nxt in ("(", "=", ";", "{", "[", ",")):
            out.append((t, tok.line))
    return out


def _declared_script(code):
    out, stack, depth = [], [], 0
    for i, tok in enumerate(code):
        t = tok.text
        if t in ("(", ")"):
            depth = depth + 1 if t == "(" else max(0, depth - 1)
            continue
        if t == "{":
            stack.append(code[i - 1].text if i else "")
            continue
        if t == "}":
            if stack:
                stack.pop()
            continue
        if depth or tok.kind != "ident" or i + 1 >= len(code):
            continue
        prev = code[i - 1].text if i else ""
        nxt = code[i + 1].text
        member = nxt in ("(", "=", ":", "?", ";", ",", "}") and prev in _MEMBER_AFTER and stack and stack[-1] not in (")", "=>")
        if prev in _SCRIPT_DECLARE or member:
            out.append((t, tok.line))
    return out


def data_keys(text):
    """[(key, line)] of a JSON table: every string followed by ':'."""
    toks = [t for t in lex_c.lex(text, lex_c.LANG_JS) if t.kind != "comment"]
    return [(string_body(t), t.line) for i, t in enumerate(toks[:-1]) if t.kind == "string" and toks[i + 1].text == ":"]


def spans(src, trivial, lines=None):
    """[Span]: each function body not inside another, the code outside them (known by its first line, or 0 when whole), and
    each program held in a multi-line string; with `lines` (a set), only the bodies holding one of them, the code outside on
    them and the programs crossing them."""
    if src.binary or src.lang not in ("cpp", "glsl", "js", "gd"):
        return []
    found = units(src)
    owner = {line: k for k, (_n, first, last) in enumerate(found) for line in range(first, last + 1)}
    tokens = lab_tokens.neutral_tokens(src)
    values = lab_tokens.numbers_in_order(src.code(), src.lang, trivial)
    out = []
    for k, (name, first, last) in enumerate(found):
        if lines is None or any(line in lines for line in range(first, last + 1)):
            body = _body([t for t in tokens if owner.get(t[1]) == k], src.lang)
            out.append(Span(first, name, lab_tokens.flatten(body), [v for v, line in values if owner.get(line) == k]))
    outside = [t for t in tokens if t[1] not in owner and (lines is None or t[1] in lines)]
    if outside:
        out.append(Span(0 if lines is None else outside[0][1], "", lab_tokens.flatten(outside),
                        [v for v, line in values if line not in owner and (lines is None or line in lines)]))
    return out + _programs(src, trivial, lines)


def _body(tokens, lang):
    """A function's tokens past its head, which every language writes its own way: through the `{` in the C family, through
    the parameters' `)` in GDScript."""
    if lang != "gd":
        start = next((i for i, (t, _l) in enumerate(tokens) if t == "{"), -1)
        return tokens[start + 1:]
    depth = 0
    for i, (t, _l) in enumerate(tokens):
        depth += 1 if t == "(" else -1 if t == ")" else 0
        if t == ")" and depth == 0:
            return tokens[i + 1:]
    return tokens


def _programs(src, trivial, lines):
    out = []
    for t in src.code():
        if t.kind != "string" or "\n" not in t.text or (lines is not None and not any(n in lines for n in range(t.line, t.last_line + 1))):
            continue
        inner = lex_c.lex(string_body(t), lex_c.LANG_GLSL)
        out.append(Span(t.line, "", lab_tokens.flatten(lab_tokens.neutral_of(inner, "glsl", t.line - 1)),
                        [v for v, _line in lab_tokens.numbers_in_order(inner, "glsl", trivial)]))
    return out


def program_names(src):
    """[(name, line)] defined by the programs a source holds in multi-line strings (a shader's uniforms and functions)."""
    out = []
    for t in src.code():
        if t.kind == "string" and "\n" in t.text:
            inner = lex_c.lex(string_body(t), lex_c.LANG_GLSL)
            out += [(n, line + t.line - 1) for n, line in code_names(inner, "glsl", code_units(inner, "glsl"))]
    return out
