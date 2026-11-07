"""The token every gate lexer produces: kind, exact text, 1-based line, offset in the source, and whether it sits in a
preprocessor directive line."""

CODE_KINDS = frozenset({"ident", "number", "string", "char", "punct", "header", "pp"})


class Tok:
    __slots__ = ("kind", "text", "line", "start", "pp")

    def __init__(self, kind, text, line, start, pp=False):
        self.kind = kind
        self.text = text
        self.line = line
        self.start = start
        self.pp = pp

    @property
    def end(self):
        return self.start + len(self.text)

    @property
    def last_line(self):
        return self.line + self.text.count("\n")

    def is_(self, kind, text=None):
        return self.kind == kind and (text is None or self.text == text)

    def __repr__(self):
        return f"Tok({self.kind!r}, {self.text!r}, {self.line})"


def string_body(tok):
    """Contents of a string token without its prefix and quotes (raw strings and triple quotes included)."""
    t = tok.text
    i = 0
    while i < len(t) and t[i] not in "\"'`/":
        i += 1
    if t[:i].endswith("R") and t[i:].startswith('"'):  # C++ raw string R"delim( ... )delim"
        open_paren = t.index("(", i)
        delim = t[i + 1:open_paren]
        return t[open_paren + 1: len(t) - len(delim) - 2]
    q = t[i:]
    for quote in ('"""', "'''", '"', "'", "`"):
        if q.startswith(quote):
            body = q[len(quote):]
            return body[:-len(quote)] if body.endswith(quote) and len(body) >= len(quote) else body
    return q
