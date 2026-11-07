"""GDScript structure from tokens: logical lines (brackets, backslashes and `;`), indentation blocks (class, function,
other), declarations (var/const with annotations, type and initializer, member or local) and function spans."""

OPEN, CLOSE = frozenset("([{"), frozenset(")]}")
STANDALONE_ANNOTATIONS = {"tool", "icon", "static_unload", "export_group", "export_subgroup", "export_category",
                          "abstract", "warning_ignore_start", "warning_ignore_restore"}


class Line:
    __slots__ = ("toks", "first", "last", "indent", "kind", "member", "parents")

    def __init__(self, toks, first, last, indent):
        self.toks, self.first, self.last, self.indent = toks, first, last, indent
        self.kind, self.member, self.parents = None, True, ()


class Decl:
    __slots__ = ("keyword", "name", "annotations", "static", "type_toks", "init_toks", "line", "member", "toks")

    def __init__(self, **kw):
        for k, v in kw.items():
            setattr(self, k, v)


def _indent_of(text, start):
    head = text[text.rfind("\n", 0, start) + 1:start]
    return len(head.expandtabs(4))


def logical_lines(source):
    """Logical lines of code tokens (comments dropped), each with its indentation, block kind and nesting."""
    lines, cur, depth, cont, prev_last = [], [], 0, False, 0
    text = source.text

    def close():
        if cur:
            lines.append(Line(list(cur), cur[0].line, cur[-1].last_line, _indent_of(text, cur[0].start)))
            cur.clear()

    for t in source.code():
        if t.kind == "punct" and t.text == "\\":
            cont = True
            continue
        if cur and t.line > prev_last and depth == 0 and not cont:
            close()
        cont = False
        if t.kind == "punct" and t.text == ";" and depth == 0:
            close()
            prev_last = t.last_line
            continue
        cur.append(t)
        prev_last = t.last_line
        if t.kind == "punct":
            if t.text in OPEN:
                depth += 1
            elif t.text in CLOSE and depth:
                depth -= 1
    close()
    _nest(lines)
    return lines


def _block_kind(line):
    toks = [t for t in _strip_annotations(line.toks)]
    if not toks:
        return "other"
    if toks[0].text == "class":
        return "class"
    if any(t.kind == "ident" and t.text == "func" for t in toks) or toks[0].text in ("var", "static"):
        return "func"
    return "other"


def _nest(lines):
    stack = []
    for ln in lines:
        while stack and stack[-1][0] >= ln.indent:
            stack.pop()
        ln.parents = tuple(k for _, k in stack)
        ln.member = "func" not in ln.parents and "other" not in ln.parents
        if ln.toks[-1].kind == "punct" and ln.toks[-1].text == ":":
            ln.kind = _block_kind(ln)
            stack.append((ln.indent, ln.kind))


def _strip_annotations(toks):
    """Tokens after the leading annotations (`@name` and `@name(...)`)."""
    i = 0
    while i + 1 < len(toks) and toks[i].text == "@" and toks[i + 1].kind == "ident":
        i += 2
        if i < len(toks) and toks[i].text == "(":
            i = _matching(toks, i) + 1
    return toks[i:]


def annotations(toks):
    """(name, start index, end index) of each leading annotation; end is exclusive and covers its arguments."""
    out, i = [], 0
    while i + 1 < len(toks) and toks[i].text == "@" and toks[i + 1].kind == "ident":
        start, name = i, toks[i + 1].text
        i += 2
        if i < len(toks) and toks[i].text == "(":
            i = _matching(toks, i) + 1
        out.append((name, start, i))
    return out


def _matching(toks, i):
    depth = 0
    for k in range(i, len(toks)):
        if toks[k].kind == "punct":
            if toks[k].text in OPEN:
                depth += 1
            elif toks[k].text in CLOSE:
                depth -= 1
                if depth == 0:
                    return k
    return len(toks) - 1


def declarations(lines):
    """var/const declarations; annotations written on their own line before a declaration belong to it."""
    out, pending = [], []
    for ln in lines:
        anns = annotations(ln.toks)
        rest = ln.toks[anns[-1][2]:] if anns else ln.toks
        names = [a[0] for a in anns]
        if not rest:
            pending += [n for n in names if n not in STANDALONE_ANNOTATIONS]
            continue
        static = bool(rest) and rest[0].text == "static"
        body = rest[1:] if static else rest
        if not body or body[0].text not in ("var", "const") or len(body) < 2:
            pending = []
            continue
        type_toks, init_toks, k = [], [], 2
        if k < len(body) and body[k].text == ":":
            k += 1
            while k < len(body) and body[k].text not in ("=",) and not _ends_decl(body, k):
                type_toks.append(body[k])
                k += 1
        if k < len(body) and body[k].text in ("=", ":="):
            init_toks = body[k + 1:]
            if init_toks and init_toks[-1].text == ":":
                init_toks = init_toks[:-1]
        out.append(Decl(keyword=body[0].text, name=body[1].text, annotations=pending + names, static=static,
                        type_toks=type_toks, init_toks=init_toks, line=ln.first, member=ln.member, toks=ln.toks))
        pending = []
    return out


def _ends_decl(body, k):
    return body[k].text == ":" and k == len(body) - 1


def function_spans(lines):
    """(name, first line, length in lines) of every `func`, from its header to the last line of its body."""
    spans = []
    for idx, ln in enumerate(lines):
        rest = _strip_annotations(ln.toks)
        if rest and rest[0].text == "static":
            rest = rest[1:]
        if len(rest) < 2 or rest[0].text != "func" or rest[1].kind != "ident":
            continue
        last = ln.last
        for nxt in lines[idx + 1:]:
            if nxt.indent <= ln.indent:
                break
            last = nxt.last
        spans.append((rest[1].text, ln.first, last - ln.first + 1))
    return spans
