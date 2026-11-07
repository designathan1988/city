"""C++ structure from tokens: include directives, named-constant declarations (UPPER_CASE const/constexpr, enumerators,
#define), function bodies (free, out-of-class and in-class inline) and the names declared with a given type."""
import re

UPPER = re.compile(r"^[A-Z][A-Z0-9_]*$")
OPEN, CLOSE = frozenset("([{"), frozenset(")]}")
_DECL_PREFIX = {"static", "inline", "extern", "thread_local", "constexpr", "const", "constinit", "volatile"}
_STATEMENT_START = {";", "{", "}", ":"}
_ACCESS = {"public", "private", "protected"}
_BODY_AFTER = {")", "const", "override", "final", "noexcept", "mutable", "volatile", "&", "&&", "try"}


def includes(source):
    """(included path, angle-bracketed, line) of every #include."""
    out, toks = [], source.tokens
    for i, t in enumerate(toks):
        if t.kind == "pp" and i + 2 < len(toks) and toks[i + 1].text in ("include", "include_next"):
            h = toks[i + 2]
            if h.kind == "header":
                out.append((h.text[1:-1], True, h.line))
            elif h.kind == "string":
                out.append((h.text.strip('"'), False, h.line))
    return out


def _skip_group(code, i):
    """Index of the token closing the bracket opened at code[i]."""
    depth = 0
    for k in range(i, len(code)):
        x = code[k].text if code[k].kind == "punct" else None
        if x in OPEN:
            depth += 1
        elif x in CLOSE:
            depth -= 1
            if depth == 0:
                return k
    return len(code) - 1


def named_constant_ranges(code):
    """Inclusive (first, last) token-index ranges of declarations that name a constant: `const`/`constexpr`
    declarations of UPPER_CASE names up to their `;`, enumerator lists, and `#define UPPER ...` lines."""
    ranges, i, n = [], 0, len(code)
    while i < n:
        t = code[i]
        if t.kind == "pp":
            j = i + 1
            while j < n and code[j].pp and code[j].kind != "pp":
                j += 1
            if j - i >= 3 and code[i + 1].text == "define" and UPPER.match(code[i + 2].text):
                ranges.append((i, j - 1))
            i = j
            continue
        if t.kind == "ident" and t.text in ("const", "constexpr", "constinit") and _at_statement_start(code, i):
            end = _constant_declaration_end(code, i)
            if end is not None:
                ranges.append((i, end))
                i = end + 1
                continue
        if t.kind == "ident" and t.text == "enum":
            j = i + 1
            while j < n and code[j].text not in ("{", ";"):
                j += 1
            if j < n and code[j].text == "{":
                close = _skip_group(code, j)
                ranges.append((j, close))
                i = close + 1
                continue
        i += 1
    return ranges


def _at_statement_start(code, i):
    k = i - 1
    while k >= 0 and code[k].kind == "ident" and code[k].text in _DECL_PREFIX:
        k -= 1
    if k >= 0 and code[k].text == "]" and k >= 1 and code[k - 1].text == "]":  # [[attribute]]
        while k >= 0 and not (code[k].text == "[" and k >= 1 and code[k - 1].text == "["):
            k -= 1
        k -= 2
    return k < 0 or code[k].pp or code[k].text in _STATEMENT_START or code[k].kind == "pp"


def _constant_declaration_end(code, i):
    """Index of the `;` ending the declaration at code[i] when it declares UPPER_CASE names, else None."""
    n, j, angle = len(code), i, 0
    while j < n:
        x = code[j].text
        if x == "<":
            angle += 1
        elif x in (">", ">>") and angle:
            angle -= len(x)
        elif angle == 0 and x in ("=", "{", "[", ";", ",", "(", ")"):
            break
        j += 1
    if j >= n or code[j].text not in ("=", "{", "[") or code[j - 1].kind != "ident" or not UPPER.match(code[j - 1].text):
        return None
    k = j
    while k < n and code[k].text != ";":
        if code[k].text in OPEN:
            k = _skip_group(code, k)
        k += 1
    return min(k, n - 1)


def function_spans(code):
    """(name, line of `{`, lines from `{` to `}`) of every function body that is not inside another function:
    namespace-scope, out-of-class and in-class inline definitions (lambdas count toward their enclosing function)."""
    spans, stack, head_start = [], [], 0
    code = [t for t in code if not t.pp]
    for k, t in enumerate(code):
        if t.kind != "punct":
            continue
        if t.text == "{":
            if "func" in (s[0] for s in stack):
                stack.append(("block", None))
            else:
                kind, name = _classify(code, head_start, k)
                stack.append((kind, (name, t.line) if kind == "func" else None))
            head_start = k + 1
        elif t.text == "}":
            kind, info = stack.pop() if stack else ("block", None)
            if kind == "func":
                spans.append((info[0], info[1], t.line - info[1] + 1))
            if kind != "init":  # an initializer's braces belong to the declaration still being read
                head_start = k + 1
        elif t.text == ";" or (t.text == ":" and k and code[k - 1].text in _ACCESS):
            head_start = k + 1
    return spans


def _classify(code, start, k):
    head = code[start:k]
    words = {t.text for t in head if t.kind == "ident"}
    if "namespace" in words or ("extern" in words and any(t.kind == "string" for t in head)):
        return "ns", None
    paren = next((i for i, t in enumerate(head) if t.text == "("), None)
    before = head[:paren] if paren is not None else head
    top_eq = any(t.text == "=" and not (i and before[i - 1].text == "operator") for i, t in enumerate(before))
    if paren is not None and not top_eq and _opens_body(head, paren):
        return "func", _name_before(head, paren)
    if words & {"class", "struct", "union", "enum"}:
        return "class", None
    return "init", None


def _opens_body(head, paren):
    """The `{` after `head` opens a function body: it follows the parameter list, a qualifier, a trailing return type
    or a member-initializer list."""
    prev = head[-1].text
    if prev in _BODY_AFTER:
        return True
    params_close = _skip_group(head, paren)
    tail = [t.text for t in head[params_close + 1:]]
    return "->" in tail or (prev == "}" and ":" in tail) or (bool(tail) and tail[0] == ":")


def _name_before(head, paren):
    for i in range(paren - 1, -1, -1):
        if head[i].kind == "ident":
            if head[i].text == "operator":
                return "operator" + "".join(t.text for t in head[i + 1:paren])
            return head[i].text
        if head[i].text not in ("~", "::", ">", "<") and head[i].kind != "punct":
            break
    return "?"


def names_of_type(code, type_name):
    """Names declared with `type_name` as their type (by value, pointer, reference or smart pointer)."""
    names = set()
    for i, t in enumerate(code):
        if t.kind != "ident" or t.text != type_name or (i and code[i - 1].text in ("class", "struct")):
            continue
        j = i + 1
        while j < len(code) and code[j].text in (">", ">>", "*", "&", "&&", "const"):
            j += 1
        if j < len(code) and code[j].kind == "ident":
            names.add(code[j].text)
    return names
