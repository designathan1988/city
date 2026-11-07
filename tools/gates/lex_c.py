"""C-family lexer (C++, GLSL/gdshader, JavaScript): comments, string/char/raw-string/template literals, numbers,
identifiers, punctuation and preprocessor directives, each token with its line and offset."""
import re

from tokens import Tok

_SPLICE = re.compile(r"\\\r?\n")
_SPACE = re.compile(r"[^\S\n]+")
_LINE_COMMENT = re.compile(r"//(?:[^\n\\]|\\\r?\n|\\.)*")
_BLOCK_COMMENT = re.compile(r"/\*.*?(?:\*/|\Z)", re.S)
_RAW_OPEN = re.compile(r'(?:u8|u|U|L)?R"([^()\\\s]{0,16})\(')
_STRING = re.compile(r'(?:u8|u|U|L)?"(?:\\.|\\\n|[^"\\\n])*"?')
_CHAR = re.compile(r"(?:u8|u|U|L)?'(?:\\.|[^'\\\n])*'?")
_SQ_STRING = re.compile(r"'(?:\\.|\\\n|[^'\\\n])*'?")
_TEMPLATE = re.compile(r"`(?:\\.|[^`\\])*`?", re.S)
_NUMBER = re.compile(r"\.?\d(?:[eEpP][+-]|'(?=[0-9A-Za-z_])|[\w.])*")
_IDENT = re.compile(r"[^\W\d]\w*")
_IDENT_JS = re.compile(r"(?:[^\W\d]|\$)[\w$]*")
_PUNCT = re.compile(r"->\*|->|::|\.\.\.|<<=|>>=|<=>|===|!==|\?\?=|\?\?|\?\.|=>|\*\*=|\*\*|[-+*/%&|^!=<>]=|&&|\|\||<<|>>|\+\+|--|##|\S", re.S)
_HEADER = re.compile(r"<[^>\n]*>")
_REGEX = re.compile(r"/(?![*/])(?:\\.|\[(?:\\.|[^\]\\\n])*\]|[^/\\\n\[])+/[A-Za-z]*")
_REGEX_AFTER = set("(,=:[!&|?{};+-*%<>~^") | {"return", "typeof", "case", "do", "else", "in", "of", "new", "delete", "void", "throw"}
_INCLUDES = {"include", "include_next", "import"}

LANG_CPP, LANG_GLSL, LANG_JS = "cpp", "glsl", "js"


def lex(text, lang=LANG_CPP):
    """Tokens of `text`. Kinds: comment, string, char, number, ident, punct, header, pp (the `#` of a directive).
    Every token inside a directive line carries pp=True; the directive name is the ident right after the pp token."""
    out = []
    i, n, line = 0, len(text), 1
    at_line_start, in_pp, include_next = True, False, False
    ident_rx = _IDENT_JS if lang == LANG_JS else _IDENT

    def emit(kind, start, end):
        out.append(Tok(kind, text[start:end], line, start, in_pp))

    while i < n:
        c = text[i]
        if c == "\n":
            line += 1
            i += 1
            at_line_start, in_pp, include_next = True, False, False
            continue
        m = _SPACE.match(text, i)
        if m:
            i = m.end()
            continue
        if c == "\\":
            m = _SPLICE.match(text, i)
            if m:
                line += 1
                i = m.end()
                continue
        start = i
        if c == "/" and text.startswith("//", i):
            m = _LINE_COMMENT.match(text, i)
            emit("comment", i, m.end())
            line += m.group(0).count("\n")
            i = m.end()
            continue
        if c == "/" and text.startswith("/*", i):
            m = _BLOCK_COMMENT.match(text, i)
            emit("comment", i, m.end())
            line += m.group(0).count("\n")
            i = m.end()
            continue
        if c == "#" and at_line_start and lang != LANG_JS:
            in_pp = True
            emit("pp", i, i + 1)
            i += 1
            m = _SPACE.match(text, i)
            if m:
                i = m.end()
            m = _IDENT.match(text, i)
            if m:
                emit("ident", i, m.end())
                include_next = m.group(0) in _INCLUDES
                i = m.end()
            at_line_start = False
            continue
        at_line_start = False
        if include_next and c == "<":
            m = _HEADER.match(text, i)
            if m:
                emit("header", i, m.end())
                i = m.end()
                include_next = False
                continue
        if lang == LANG_CPP:
            m = _RAW_OPEN.match(text, i)
            if m:
                close = text.find(")" + m.group(1) + '"', m.end())
                end = n if close < 0 else close + len(m.group(1)) + 2
                emit("string", i, end)
                line += text.count("\n", i, end)
                i = end
                continue
        if lang == LANG_JS and c == "`":
            m = _TEMPLATE.match(text, i)
            emit("string", i, m.end())
            line += m.group(0).count("\n")
            i = m.end()
            continue
        if lang == LANG_JS and c == "/" and _regex_allowed(out):
            m = _REGEX.match(text, i)
            if m:
                emit("string", i, m.end())
                i = m.end()
                continue
        m = _STRING.match(text, i)
        if m and (c == '"' or text[m.start():m.end()].lstrip("u8UL").startswith('"')):
            emit("string", i, m.end())
            line += m.group(0).count("\n")
            i = m.end()
            continue
        if lang == LANG_JS and c == "'":
            m = _SQ_STRING.match(text, i)
            emit("string", i, m.end())
            i = m.end()
            continue
        if lang == LANG_CPP:
            m = _CHAR.match(text, i)
            if m and (c == "'" or m.group(0).lstrip("u8UL").startswith("'")):
                emit("char", i, m.end())
                i = m.end()
                continue
        m = _NUMBER.match(text, i)
        if m:
            emit("number", i, m.end())
            i = m.end()
            continue
        m = ident_rx.match(text, i)
        if m:
            emit("ident", i, m.end())
            i = m.end()
            continue
        m = _PUNCT.match(text, i)
        emit("punct", start, m.end())
        i = m.end()
    return out


def _regex_allowed(out):
    """A `/` starts a regular expression literal where an operand is expected (after an operator, keyword or start)."""
    for t in reversed(out):
        if t.kind == "comment":
            continue
        return t.text in _REGEX_AFTER
    return True


_SUFFIX = re.compile(r"(?:[uU](?:ll|LL|l|L|z|Z)?|(?:ll|LL|l|L|z|Z)[uU]?|[fFhH]|f16|f32|f64|bf16)$")


def number_value(text):
    """Numeric value of a C/C++/GLSL/JS number token, or None when it is not a plain number."""
    t = text.replace("'", "").replace("_", "")
    low = t.lower()
    try:
        if low.startswith("0x"):
            body = re.sub(r"[uUlLzZ]+$", "", t[2:])
            if "p" in body.lower() or "." in body:
                return float.fromhex("0x" + body.rstrip("fFlL"))
            return float(int(body, 16))
        if low.startswith("0b"):
            return float(int(re.sub(r"[uUlLzZ]+$", "", t[2:]), 2))
        core = _SUFFIX.sub("", t) if not re.fullmatch(r"\d+[eE]\d+", t) else t
        if re.fullmatch(r"0[0-7]+", core):
            return float(int(core, 8))
        if core.endswith("n"):  # JavaScript BigInt
            core = core[:-1]
        return float(core)
    except ValueError:
        return None
