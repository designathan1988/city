"""The code of three languages read as one, what LAB_COPY's fingerprints are made of: declarations and the languages' own types
become D (a C++ declaration's type too), a function's head F, identifiers I, numbers their value, strings S, control words and
operators the same word across TypeScript, C++ and GDScript (`===` is `==`, `elif` is `else if`, `::` and `->` are `.`), and
TypeScript's type-only writing (annotations, `as`, the non-null `!`) is dropped; `flatten` then takes out what the languages
write differently round the same program (statement ends, braces, a condition's parentheses, an empty argument list, the
receiver written out, a range loop's word). Also the winnowed k-gram fingerprints of such a stream, a defined name's normal
form, and the non-trivial numbers in their order."""
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
SAME = {"===": "==", "!==": "!=", "::": ".", "->": ".", ":=": "=", "and": "&&", "or": "||", "not": "!", "nullptr": "null",
        "undefined": "null", "None": "null", "self": "this", "True": "true", "False": "false"}
_AFTER_ASSERTION = frozenset({")", ".", ";", ",", "]", "}", "=", "?."})  # TypeScript's postfix `!` asserts a type, no more
_TYPE_PARTS = frozenset({"D", "I", ".", "<", ">", ">>", "*", "&", "&&", ","})
_DECLARED_BEFORE = frozenset({"=", ";", "(", ",", ")", "{", "[", ":"})  # what may follow a declared name
_STATEMENT_START = frozenset({";", "{", "}", "(", ",", ":"})
_FLAT_DROP = frozenset({";", "{", "}", "pass"})  # statement ends, block braces and GDScript's empty statement
_CONTROL_WORDS = frozenset({"if", "while", "for", "switch"})
KEPT_WORDS = frozenset({"if", "else", "for", "while", "do", "return", "break", "continue", "switch", "case", "default",
                        "new", "delete", "this", "true", "false", "null", "try", "catch", "throw", "in", "of", "class",
                        "struct", "enum", "match", "await", "yield"})
ANNOTATED = frozenset({"js", "gd"})  # the languages that write a type after a name: `name: Type`, `-> Type`
_PART = re.compile(r"[A-Z]+(?![a-z])|[A-Z]?[a-z0-9]+")
_TYPE_END = frozenset({"=", ",", ")", ";", "{", "=>"})


def normal_name(name, min_len):
    """A defined name's normal form (case and separators gone: openStretch = open_stretch = OpenStretch), or None for a name
    of one part or shorter than `min_len` letters: one word is the domain's, not a program's."""
    parts = _PART.findall(name)
    joined = "".join(p.lower() for p in parts)
    return joined if len(parts) >= 2 and len(joined) >= min_len else None


def number_of(text, lang):
    return lex_gd.number_value(text) if lang == "gd" else lex_c.number_value(text)


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


def _annotation(code, i, stack, lang):
    """Whether the ':' or '->' at `i` opens a type annotation: a GDScript `->`, a declared name's or a parameter's type, or
    a TypeScript return type after `)` (in GDScript that `:` opens the function's block)."""
    t = code[i].text
    if t == "->":
        return True
    prev = code[i - 1].text if i else ""
    if prev == ")":
        return lang == "js"
    j = i - 1 if prev != "?" else i - 2  # an optional parameter's `?:`
    if j < 0 or code[j].kind != "ident":
        return False
    before = code[j - 1].text if j else ""
    return before in DECLARE or (stack and stack[-1] == "(" and before in ("(", ",", "...")) or before in ("var", "const")


def _block_colon(code, i):
    """A GDScript block's opening ':' (the last token of its line)."""
    return i + 1 >= len(code) or code[i + 1].line != code[i].line


def neutral_of(code, lang, line_offset=0):
    """[(neutral text, line)] of code tokens (preprocessor lines left out) written in `lang`, braces and statement ends kept
    (a function's body is found by them before `flatten` takes them out)."""
    code = [t for t in code if not t.pp]
    out, stack = [], []
    i = 0
    while i < len(code):
        tok = code[i]
        text, line = tok.text, tok.line + line_offset
        if text in ("(", "[", "{"):
            stack.append(text)
        elif text in (")", "]", "}") and stack:
            stack.pop()
        if lang in ANNOTATED and text in (":", "->") and _annotation(code, i, stack, lang):
            i = _skip_type(code, i + 1)
            continue
        if lang == "js" and tok.kind == "ident" and text == "as" and i:
            i = _skip_type(code, i + 1)
            continue
        if lang == "js" and text == "!" and (i + 1 >= len(code) or code[i + 1].text in _AFTER_ASSERTION):
            i += 1
            continue
        if lang == "gd" and text == ":" and _block_colon(code, i):
            i += 1
            continue
        if tok.kind == "number":
            value = number_of(text, lang)
            out.append((f"N:{value:.10g}" if value is not None else "N", line))
        elif tok.kind in ("string", "char"):
            out.append(("S", line))
        elif tok.kind == "ident":
            if text == "elif":
                out += [("else", line), ("if", line)]
            elif text in FUNCTION_WORDS:
                out.append(("F", line))
            elif text in DECLARE:
                if not out or out[-1][0] != "D":
                    out.append(("D", line))
            else:
                word = SAME.get(text, text)
                out.append((word if word in KEPT_WORDS else "I", line))
        else:
            out.append((SAME.get(text, text), line))
        i += 1
    return _function_heads(_declarations(out) if lang in ("cpp", "glsl") else out)


def neutral_tokens(src):
    """[(neutral text, line)] of a source's code."""
    return neutral_of(src.code(), src.lang)


def _type_end(out, i):
    """The index past a C++ type starting at i (declaration words, a qualified name, its template arguments, then
    qualifiers, pointers and references), or None."""
    n, j = len(out), i
    while j < n and out[j][0] == "D":
        j += 1
    if j >= n or out[j][0] != "I":
        return None
    j += 1
    while j + 1 < n and out[j][0] == "." and out[j + 1][0] == "I":
        j += 2
    if j < n and out[j][0] == "<":
        depth = 0
        while j < n:
            t = out[j][0]
            if t not in _TYPE_PARTS and not t.startswith("N"):
                return None
            depth += 1 if t == "<" else -2 if t == ">>" else -1 if t == ">" else 0
            j += 1
            if depth <= 0:
                break
        if depth > 0:
            return None
    while j < n and out[j][0] in ("D", "*", "&", "&&"):
        j += 1
    return j


def _declarations(out):
    """A C++ declaration's type, `std::vector<Loaf> batch` or `Loaf loaf`, reads as the other languages' declaration word:
    `D I`."""
    res, i, n = [], 0, len(out)
    while i < n:
        if not res or res[-1][0] in _STATEMENT_START:
            j = _type_end(out, i)
            if j is not None and j + 1 < n and out[j][0] == "I" and out[j + 1][0] in _DECLARED_BEFORE and any(
                    t == "I" for t, _l in out[i:j]):
                res.append(("D", out[i][1]))
                i = j
                continue
        res.append(out[i])
        i += 1
    return res


def _function_heads(out):
    """A C++ definition's head `D I (` reads as the other languages' `F I (`."""
    for k in range(len(out) - 2):
        if out[k][0] == "D" and out[k + 1][0] == "I" and out[k + 2][0] == "(" and (k == 0 or out[k - 1][0] in (";", "{", "}")):
            out[k] = ("F", out[k][1])
    return out


def flatten(tokens):
    """The stream with what the languages write differently round the same program taken out: statement ends and block
    braces (GDScript writes neither), the parentheses round a control word's condition (GDScript writes none), an empty
    argument list (a property in one language is an accessor in another), the receiver written out (`this.`, `self.`,
    `this->`) and a declaration word right after `for`; a range loop's `of` or `:` reads as GDScript's `in`."""
    out, parens, n, i = [], [], len(tokens), 0
    while i < n:
        t, line = tokens[i]
        nxt = tokens[i + 1][0] if i + 1 < n else None
        if t in _FLAT_DROP or (t == "D" and out and out[-1][0] == "for"):
            i += 1
            continue
        if (t == "this" and nxt == ".") or (t == "(" and nxt == ")"):
            i += 2
            continue
        if t == "(":
            control = bool(out) and out[-1][0] in _CONTROL_WORDS
            parens.append((control, bool(out) and out[-1][0] == "for"))
            if control:
                i += 1
                continue
        elif t == ")" and parens and parens.pop()[0]:
            i += 1
            continue
        elif t == "of" or (t == ":" and parens and parens[-1][1]):
            t = "in"
        out.append((t, line))
        i += 1
    return out


def kgrams(tokens, k, informative=False):
    """[(hash, index of its first token)] of every k-gram of `tokens`, hashed Karp-Rabin over facts.token_id; with
    `informative`, only those that are no short pattern repeated (a list of like statements has a list's shape, not a
    program's logic)."""
    if len(tokens) < k:
        return []
    ids = [facts.token_id(t) for t, _line in tokens]
    power = pow(facts.BASE, k - 1, facts.MOD)
    grams, h = [], 0
    for i, v in enumerate(ids):
        if i >= k:
            h = (h - ids[i - k] * power) % facts.MOD
        h = (h * facts.BASE + v) % facts.MOD
        if i >= k - 1 and not (informative and periodic(ids[i - k + 1:i + 1])):
            grams.append((h, i - k + 1))
    return grams


def periodic(window):
    """Whether a window repeats a pattern no longer than half of it."""
    n = len(window)
    return any(all(window[j] == window[j + p] for j in range(n - p)) for p in range(1, n // 2 + 1))


def fingerprints(tokens, k, w):
    """[(hash, index)] winnowed from the k-grams of `tokens` (Schleimer, Wilkerson and Aiken 2003): the minimum of every
    window of `w` hashes kept (the rightmost on ties), once."""
    grams = kgrams(tokens, k)
    if not grams:
        return []
    out, last = [], -1
    for s in range(max(1, len(grams) - w + 1)):
        window = grams[s:s + w]
        best = min(range(len(window)), key=lambda j: (window[j][0], -j))
        at = s + best
        if at != last:
            out.append(grams[at])
            last = at
    return out


def guaranteed_length(k, w):
    """The shortest match winnowing is sure to find (w + k - 1 tokens): a piece shorter than this has too few fingerprints
    for its share of them to say anything."""
    return w + k - 1


def numbers_in_order(code, lang, trivial, line_offset=0):
    """[(value, line)] of the non-trivial numbers of code tokens, in the order they are written."""
    out = []
    for tok in code:
        if tok.kind == "number" and not tok.pp:
            v = number_of(tok.text, lang)
            if v is not None and v not in trivial:
                out.append((v, tok.line + line_offset))
    return out
