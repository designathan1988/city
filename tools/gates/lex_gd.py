"""GDScript lexer: comments, strings (plain, triple-quoted, raw, StringName and NodePath), numbers, identifiers,
punctuation and line continuations, each token with its line and offset."""
import re

from tokens import Tok

_SPACE = re.compile(r"[^\S\n]+")
_COMMENT = re.compile(r"#[^\n]*")
_STRING = re.compile(
    r'(?:r|&|\^)?(?:"""(?:\\.|[^\\])*?(?:"""|\Z)|\'\'\'(?:\\.|[^\\])*?(?:\'\'\'|\Z)'
    r'|"(?:\\.|[^"\\\n])*"?|\'(?:\\.|[^\'\\\n])*\'?)', re.S)
_NUMBER = re.compile(
    r"0[xX][0-9a-fA-F_]+|0[bB][01_]+|(?:\d[\d_]*(?:\.(?![A-Za-z_.])[\d_]*)?|\.\d[\d_]*)(?:[eE][-+]?\d[\d_]*)?")
_IDENT = re.compile(r"[^\W\d]\w*")
_PUNCT = re.compile(r"\*\*=|<<=|>>=|\*\*|->|:=|==|!=|<=|>=|&&|\|\||<<|>>|[-+*/%&|^]=|\.\.|\S")


def lex(text):
    """Tokens of a GDScript source. Kinds: comment, string, number, ident, punct; a backslash line continuation is
    the punct token '\\'."""
    out = []
    i, n, line = 0, len(text), 1
    while i < n:
        c = text[i]
        if c == "\n":
            line += 1
            i += 1
            continue
        m = _SPACE.match(text, i)
        if m:
            i = m.end()
            continue
        if c == "#":
            m = _COMMENT.match(text, i)
            out.append(Tok("comment", m.group(0), line, i))
            i = m.end()
            continue
        if c in "\"'" or (c in "r&^" and i + 1 < n and text[i + 1] in "\"'"):
            m = _STRING.match(text, i)
            out.append(Tok("string", m.group(0), line, i))
            line += m.group(0).count("\n")
            i = m.end()
            continue
        if "0" <= c <= "9" or (c == "." and i + 1 < n and "0" <= text[i + 1] <= "9"):
            m = _NUMBER.match(text, i)
            out.append(Tok("number", m.group(0), line, i))
            i = m.end()
            continue
        m = _IDENT.match(text, i)
        if m:
            out.append(Tok("ident", m.group(0), line, i))
            i = m.end()
            continue
        m = _PUNCT.match(text, i)
        out.append(Tok("punct", m.group(0), line, i))
        i = m.end()
    return out


def number_value(text):
    """Numeric value of a GDScript number token."""
    t = text.replace("_", "")
    try:
        if t[:2] in ("0x", "0X"):
            return float(int(t[2:], 16))
        if t[:2] in ("0b", "0B"):
            return float(int(t[2:], 2))
        return float(t)
    except ValueError:
        return None
