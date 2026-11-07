"""The shape of a piece of code with its names taken out: what stays the same when code is renamed or moved."""
import re

from .core import digest

C_STYLE = re.compile(r"//[^\n]*|/\*.*?\*/|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'|[A-Za-z_]\w*|\d[\w.]*|[^\s\w]", re.S)
HASH_STYLE = re.compile(r"#[^\n]*|\"(?:\\.|[^\"\\\n])*\"|'(?:\\.|[^'\\\n])*'|[A-Za-z_]\w*|\d[\w.]*|[^\s\w]", re.S)


def tokens(text, style=C_STYLE):
    """The tokens of a text, comments gone; a string is one token."""
    return [t for t in style.findall(text) if not t.startswith(("//", "/*", "#"))]


def shape(text, style=C_STYLE):
    """A hash of the token stream with every name, string and number made the same."""
    out = []
    for t in tokens(text, style):
        head = t[0]
        out.append("a" if head.isalpha() or head == "_" else "0" if head.isdigit() else "s" if head in "\"'" else t)
    return digest("".join(out))
