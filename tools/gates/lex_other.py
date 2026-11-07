"""Lexers for the remaining text formats: Python (stdlib tokenize), POSIX shell, PowerShell, Godot resource files
(.tscn/.tres/.import/.cfg/.godot), XML/SVG, prose (the whole text is commentary) and comment-free data files."""
import io
import re
import tokenize

from tokens import Tok


class LexError(Exception):
    """The source cannot be split into tokens (a gate that needs them reports the file instead of passing it)."""


def _line_starts(text):
    starts = [0]
    for m in re.finditer("\n", text):
        starts.append(m.end())
    return starts


_PY_KIND = {tokenize.COMMENT: "comment", tokenize.STRING: "string", tokenize.NUMBER: "number",
            tokenize.NAME: "ident", tokenize.OP: "punct"}
for _name in ("FSTRING_START", "FSTRING_MIDDLE", "FSTRING_END"):
    if hasattr(tokenize, _name):
        _PY_KIND[getattr(tokenize, _name)] = "string"


def lex_python(text):
    starts = _line_starts(text)
    out = []
    try:
        for t in tokenize.generate_tokens(io.StringIO(text).readline):
            kind = _PY_KIND.get(t.type)
            if kind:
                out.append(Tok(kind, t.string, t.start[0], starts[t.start[0] - 1] + t.start[1]))
    except (tokenize.TokenError, IndentationError, SyntaxError) as e:
        raise LexError(f"Python tokenizer: {e}") from e
    return out


_SH_WORD_BREAK = set(" \t\n;|&()<>")
_HEREDOC = re.compile(r"<<-?\s*(['\"]?)([A-Za-z_]\w*)\1")


def lex_shell(text, powershell=False):
    """Comments (`#` starting a word, `<# #>` in PowerShell), quoted strings and here-documents; the rest is words."""
    out, i, n, line = [], 0, len(text), 1
    pending_heredocs = []
    while i < n:
        c = text[i]
        if c == "\n":
            line += 1
            i += 1
            for delim in pending_heredocs:
                end = re.compile(r"^[ \t]*%s[ \t]*$" % re.escape(delim), re.M).search(text, i)
                stop = end.end() if end else n
                out.append(Tok("string", text[i:stop], line, i))
                line += text.count("\n", i, stop)
                i = stop
            pending_heredocs = []
            continue
        if c in " \t\r":
            i += 1
            continue
        if powershell and text.startswith("<#", i):
            end = text.find("#>", i + 2)
            stop = n if end < 0 else end + 2
            out.append(Tok("comment", text[i:stop], line, i))
            line += text.count("\n", i, stop)
            i = stop
            continue
        if c == "#" and (i == 0 or text[i - 1] in _SH_WORD_BREAK):
            stop = text.find("\n", i)
            stop = n if stop < 0 else stop
            out.append(Tok("comment", text[i:stop], line, i))
            i = stop
            continue
        if powershell and c == "@" and text[i + 1:i + 2] in ("'", '"') and text[i + 2:i + 3] == "\n":
            close = text.find("\n" + text[i + 1] + "@", i + 2)
            stop = n if close < 0 else close + 3
            out.append(Tok("string", text[i:stop], line, i))
            line += text.count("\n", i, stop)
            i = stop
            continue
        if c in "'\"":
            j, escape = i + 1, ("`" if powershell else "\\")
            while j < n and text[j] != c:
                j += 2 if (text[j] == escape and c == '"') else 1
            stop = min(j + 1, n)
            out.append(Tok("string", text[i:stop], line, i))
            line += text.count("\n", i, stop)
            i = stop
            continue
        m = _HEREDOC.match(text, i) if not powershell else None
        if m:
            pending_heredocs.append(m.group(2))
            out.append(Tok("punct", m.group(0), line, i))
            i = m.end()
            continue
        j = i
        while j < n and text[j] not in _SH_WORD_BREAK and text[j] not in "'\"":
            j += 2 if text[j] == "\\" else 1
        j = max(j, i + 1)
        out.append(Tok("ident", text[i:j], line, i))
        i = j
    return out


_RES = re.compile(r'(?P<comment>;[^\n]*)|(?P<string>"(?:\\.|[^"\\])*"?)|(?P<word>[^\s";]+)|(?P<nl>\n)', re.S)


def lex_godot_resource(text):
    """Godot's ConfigFile/resource syntax: `;` comments, double-quoted strings (may span lines), the rest is words."""
    out, line = [], 1
    for m in _RES.finditer(text):
        kind = m.lastgroup
        if kind == "nl":
            line += 1
            continue
        out.append(Tok("ident" if kind == "word" else kind, m.group(0), line, m.start()))
        line += m.group(0).count("\n")
    return out


_XML = re.compile(r"(?P<comment><!--.*?(?:-->|\Z))|(?P<text>[^<]+|<)", re.S)


def lex_xml(text):
    out, line = [], 1
    for m in _XML.finditer(text):
        out.append(Tok("comment" if m.lastgroup == "comment" else "ident", m.group(0), line, m.start()))
        line += m.group(0).count("\n")
    return out


def lex_prose(text):
    return [Tok("comment", text, 1, 0)] if text else []


def lex_data(text):
    """JSON, CSV, Godot uid files: no comments; the whole text is one data token."""
    return [Tok("string", text, 1, 0)] if text else []
