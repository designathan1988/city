"""The code of three languages read as one, what LAB_COPY's fingerprints are made of: declarations and the languages' own types
become D, a function's head F, identifiers I, numbers their value, strings S, control words and operators the same word across
TypeScript, C++ and GDScript (`===` is `==`, `elif` is `else if`, `::` is `.`), where a translation keeps them alike; the winnowed
k-gram fingerprints of such a stream, the names a source defines and their normal form, and its runs of non-trivial numbers."""
import re

import facts
import lex_c
import lex_gd

# The languages' own declaration words and built-in types (ECMA-262 and TypeScript, ISO C++ and <cstdint>, GDScript's
# Variant types): what a translation changes between languages and keeps nothing of the program.
DECLARE = frozenset({
    "let", "const", "var", "auto", "static", "constexpr", "inline", "export", "readonly", "public", "private", "protected",
    "async", "unsigned", "signed", "long", "short", "volatile", "mutable", "extern", "typename",
    "number", "string", "boolean", "void", "any", "unknown", "never", "bigint", "symbol", "object",
    "int", "float", "double", "bool", "char", "size_t", "ptrdiff_t", "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t",
    "String", "StringName", "Array", "Dictionary", "Vector2", "Vector2i", "Vector3", "Vector3i", "Color", "Variant",
})
FUNCTION_WORDS = frozenset({"function", "func", "def"})
SAME = {"===": "==", "!==": "!=", "::": ".", "and": "&&", "or": "||", "not": "!", "nullptr": "null", "undefined": "null",
        "None": "null", "self": "this", "True": "true", "False": "false"}
KEPT_WORDS = frozenset({"if", "else", "for", "while", "do", "return", "break", "continue", "switch", "case", "default",
                        "new", "delete", "this", "true", "false", "null", "try", "catch", "throw", "in", "of", "class",
                        "struct", "enum", "match", "await", "yield"})
_PART = re.compile(r"[A-Z]+(?![a-z])|[A-Z]?[a-z0-9]+")
_TYPE_END = frozenset({"=", ",", ")", ";", "{", "=>"})


def normal_name(name, min_len):
    """A defined name's normal form (case and separators gone: openStretch = open_stretch = OpenStretch), or None for a name
    of one part or shorter than `min_len` letters: one word is the domain's, not a program's."""
    parts = _PART.findall(name)
    joined = "".join(p.lower() for p in parts)
    return joined if len(parts) >= 2 and len(joined) >= min_len else None


def _number(src, tok):
    return lex_gd.number_value(tok.text) if src.lang == "gd" else lex_c.number_value(tok.text)


def _skip_type(code, i):
    """The index past a type annotation starting at `i` (TypeScript and GDScript), at bracket depth 0."""
    depth = 0
    while i < len(code):
        t = code[i].text
        if depth == 0 and t in _TYPE_END:
            return i
        if t in ("<", "(", "["):
            depth += 1
        elif t in (">", ")", "]"):
            if depth == 0:
                return i
            depth -= 1
        i += 1
    return i


def _annotation(code, i, stack):
    """Whether the ':' or '->' at `i` opens a type annotation: a GDScript `->`, a declared name's or a parameter's type, or
    a TypeScript return type after `)`."""
    t = code[i].text
    if t == "->":
        return True
    prev = code[i - 1].text if i else ""
    if prev == ")":
        return True
    j = i - 1 if prev != "?" else i - 2  # an optional parameter's `?:`
    if j < 0 or code[j].kind != "ident":
        return False
    before = code[j - 1].text if j else ""
    return before in DECLARE or (stack and stack[-1] == "(" and before in ("(", ",", "...")) or before in ("var", "const")


def _block_colon(code, i):
    """A GDScript block's opening ':' (the last token of its line)."""
    return i + 1 >= len(code) or code[i + 1].line != code[i].line


def neutral_tokens(src):
    """[(neutral text, line)] of a source's code, preprocessor lines left out."""
    code = [t for t in src.code() if not t.pp]
    out, stack = [], []
    i = 0
    while i < len(code):
        tok = code[i]
        text = tok.text
        if text in ("(", "[", "{"):
            stack.append(text)
        elif text in (")", "]", "}") and stack:
            stack.pop()
        if text in (":", "->") and _annotation(code, i, stack):
            i = _skip_type(code, i + 1)
            continue
        if src.lang == "gd" and text == ":" and _block_colon(code, i):
            i += 1
            continue
        if tok.kind == "number":
            value = _number(src, tok)
            out.append((f"N:{value:.10g}" if value is not None else "N", tok.line))
        elif tok.kind in ("string", "char"):
            out.append(("S", tok.line))
        elif tok.kind == "ident":
            if text == "elif":
                out += [("else", tok.line), ("if", tok.line)]
            elif text in FUNCTION_WORDS:
                out.append(("F", tok.line))
            elif text in DECLARE:
                if not out or out[-1][0] != "D":
                    out.append(("D", tok.line))
            else:
                word = SAME.get(text, text)
                out.append((word if word in KEPT_WORDS else "I", tok.line))
        else:
            out.append((SAME.get(text, text), tok.line))
        i += 1
    return _function_heads(out)


def _function_heads(out):
    """A C++ definition's head `D I (` reads as the other languages' `F I (`."""
    for k in range(len(out) - 2):
        if out[k][0] == "D" and out[k + 1][0] == "I" and out[k + 2][0] == "(" and (k == 0 or out[k - 1][0] in (";", "{", "}")):
            out[k] = ("F", out[k][1])
    return out


def fingerprints(tokens, k, w):
    """[(hash, line)] winnowed from the k-grams of `tokens` (Schleimer, Wilkerson and Aiken 2003): each k-gram hashed
    Karp-Rabin over facts.token_id, the minimum of every window of `w` hashes kept (the rightmost on ties), once."""
    if len(tokens) < k:
        return []
    ids = [facts.token_id(t) for t, _line in tokens]
    power = pow(facts.BASE, k - 1, facts.MOD)
    grams, h = [], 0
    for i, v in enumerate(ids):
        if i >= k:
            h = (h - ids[i - k] * power) % facts.MOD
        h = (h * facts.BASE + v) % facts.MOD
        if i >= k - 1:
            grams.append((h, tokens[i - k + 1][1]))
    out, last = [], -1
    for s in range(max(1, len(grams) - w + 1)):
        window = grams[s:s + w]
        best = min(range(len(window)), key=lambda j: (window[j][0], -j))
        at = s + best
        if at != last:
            out.append(grams[at])
            last = at
    return out


def _defined_ts(code):
    found = {}
    stack = []
    for i, tok in enumerate(code):
        t = tok.text
        if t == "{":
            stack.append(code[i - 1].text if i else "")
            continue
        if t == "}" and stack:
            stack.pop()
            continue
        if tok.kind != "ident" or i + 1 >= len(code):
            continue
        prev = code[i - 1].text if i else ""
        nxt = code[i + 1].text
        if prev in ("function", "class", "interface", "type", "enum", "const", "let", "var", "func", "class_name", "signal"):
            found.setdefault(t, tok.line)
        elif nxt in ("(", "=", ":", "?") and prev in ("{", ";", "}", "readonly", "static", "private", "public", "protected",
                                                       "async", "get", "set") and _in_class(stack):
            found.setdefault(t, tok.line)
    return found


def _in_class(stack):
    return bool(stack) and stack[-1] not in (")", "=>")


def _defined_cpp(code):
    found = {}
    for i, tok in enumerate(code[:-1]):
        if tok.kind != "ident" or tok.text in DECLARE or tok.text in KEPT_WORDS:
            continue
        prev = code[i - 1] if i else None
        nxt = code[i + 1].text
        if prev is None:
            continue
        typed = prev.text in DECLARE or prev.text in (">", "*", "&") or (prev.kind == "ident" and prev.text not in KEPT_WORDS
                                                                       and prev.text not in ("return", "else"))
        if prev.text in ("struct", "class", "enum", "namespace", "using") or (typed and nxt in ("(", "=", ";", "{", "[", ",")):
            found.setdefault(tok.text, tok.line)
    return found


def defined_at(src):
    """{name: line} of what a source defines: functions, classes, constants, variables, fields and members."""
    code = [t for t in src.code() if not t.pp]
    return _defined_cpp(code) if src.lang == "cpp" else _defined_ts(code)


def defined_names(src):
    return set(defined_at(src))


def number_runs(src, trivial, run):
    """[(values, line)]: every run of `run` non-trivial numbers in the order the source writes them."""
    values = []
    for tok in src.code():
        if tok.kind == "number":
            v = _number(src, tok)
            if v is not None and v not in trivial:
                values.append((v, tok.line))
    return [(tuple(v for v, _ in values[s:s + run]), values[s][1]) for s in range(len(values) - run + 1)]
