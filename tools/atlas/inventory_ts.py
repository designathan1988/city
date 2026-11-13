"""The inventory's lexical reading of TypeScript (tools/atlas/inventory_reference.py): tokens (comments, string literals,
names, numbers, punctuation) with their offsets, the tokens of one bracketed value whole, a string literal's text, and the
three declarations the reference's features are read from: a dictionary of texts (`'key': 'text'`, a text possibly several
literals joined by +), an exported constant table (a list of strings, a list of objects with an id, a map of named objects),
and an exported union of string literals. It reads, it never runs, and it names nothing of any program."""
import ast
import bisect
import re

# A string's body is the unrolled loop [^q\\]*(?:\\.[^q\\]*)*: its class runs as one repeat, never an alternation stepped
# per character (tools/atlas/inventory_json.py measured 1.4 s against 0.03 s on a 7 MB string).
TOKEN = re.compile(r"""
 (?P<comment>//[^\n]*|/\*.*?\*/)
|(?P<string>'[^'\\\n]*(?:\\.[^'\\\n]*)*'|"[^"\\\n]*(?:\\.[^"\\\n]*)*"|`[^`\\]*(?:\\.[^`\\]*)*`)
|(?P<name>[A-Za-z_$][\w$]*)
|(?P<number>\d[\w.]*)
|(?P<punct>\S)
""", re.S | re.X)
OPEN, CLOSE = "([{", ")]}"
ID_PROPS = ("id", "key", "name")  # the property an object of a constant list is named by, in this order
TABLE_CONST = re.compile(r"^export const ([A-Z][A-Z0-9_]*)\b", re.M)
UNION = re.compile(r"^export type ([A-Z]\w*)\s*=", re.M)
DICT_ENTRY = re.compile(r"""^[ \t]*(['"])((?:\\.|(?!\1).)+?)\1[ \t]*:""", re.M)
NAMED_MAP = 2  # a map of named objects has at least this many
WRAPPERS = 4  # tokens a table's literal may sit behind (Object.freeze( )


class Lines:
    """Offsets to line numbers, by a search in the offsets lines start at."""

    def __init__(self, text):
        self.starts = [0] + [m.end() for m in re.finditer("\n", text)]

    def of(self, offset):
        return bisect.bisect_right(self.starts, offset)


def tokens(text, start=0):
    """(kind, text, offset) from `start`, comments left out."""
    for m in TOKEN.finditer(text, start):
        if m.lastgroup != "comment":
            yield m.lastgroup, m.group(), m.start()


def string_value(literal):
    """A string literal's text: escapes read as Python reads them (JavaScript's are the same for text), a template's raw."""
    if literal.startswith("`"):
        return literal[1:-1]
    try:
        return ast.literal_eval(literal)
    except (ValueError, SyntaxError):
        return literal[1:-1]


def bracketed(toks):
    """The tokens of one bracketed value whole, from its opening bracket in `toks` (an iterator)."""
    out, depth = [], 0
    for tok in toks:
        out.append(tok)
        if tok[0] == "punct" and tok[1] in OPEN:
            depth += 1
        elif tok[0] == "punct" and tok[1] in CLOSE:
            depth -= 1
            if depth == 0:
                return out
    return out


def dictionary(text):
    """Each `'key': text` entry written at a line's start whose text is one literal or several joined by +: (key, text,
    offset). Entries whose value is anything else are counted apart: (entries, skipped)."""
    out, skipped = [], 0
    for m in DICT_ENTRY.finditer(text):
        parts, toks = [], tokens(text, m.end())
        for kind, value, _at in toks:
            if kind == "string" and (not parts or parts[-1] == "+"):
                parts.append(value)
            elif value == "+" and parts and parts[-1] != "+":
                parts.append("+")
            else:
                break
        literals = [p for p in parts if p != "+"]
        if literals and parts[-1] != "+":
            out.append((string_value(m.group(1) + m.group(2) + m.group(1)), "".join(string_value(p) for p in literals),
                        m.start()))
        else:
            skipped += 1
    return out, skipped


def _value_start(text, at):
    """The opening bracket of an exported constant's literal value, past its type and `=` (and Object.freeze( ), or None."""
    depth, toks = 0, tokens(text, at)
    for kind, value, _off in toks:
        if kind == "punct" and value in OPEN:
            depth += 1
        elif kind == "punct" and value in CLOSE:
            depth -= 1
        elif kind == "punct" and value == "=" and depth == 0:
            break
        elif kind == "punct" and value == ";" and depth == 0:
            return None
    for _n, tok in zip(range(WRAPPERS + 1), toks):
        if tok[0] == "punct" and tok[1] in "[{":
            return tok
        if tok[0] == "punct" and tok[1] not in ".(":
            return None
    return None


def _entries(toks):
    """A constant literal's entries: (name, offset, how)."""
    stack, strings, ids, named = [], [], [], []
    for i, (kind, value, off) in enumerate(toks):
        if kind == "punct" and value in OPEN:
            stack.append(value)
            continue
        if kind == "punct" and value in CLOSE:
            stack.pop()
            continue
        nxt = toks[i + 1][1] if i + 1 < len(toks) else ""
        after = toks[i + 2] if i + 2 < len(toks) else ("", "", 0)
        if stack == ["["] and kind == "string":
            strings.append((string_value(value), off, "a string of the list"))
        elif stack == ["[", "{"] and kind == "name" and value in ID_PROPS and nxt == ":" and after[0] == "string":
            ids.append((string_value(after[1]), after[2], f"an object named by its {value}", value))
        elif stack == ["{"] and kind in ("name", "string") and nxt == ":" and after[1] == "{":
            named.append((string_value(value) if kind == "string" else value, off, "a named object"))
    if strings:
        return strings
    if ids:
        first = ids[0][3]
        return [(n, o, how) for n, o, how, prop in ids if prop == first]
    return named if len(named) >= NAMED_MAP else []


def tables(text):
    """Each exported upper-case constant whose value is a table: (constant, entries [(name, offset, how)], offset); and
    the count of upper-case constants whose value is none."""
    out, other = [], 0
    for m in TABLE_CONST.finditer(text):
        start = _value_start(text, m.end())
        found = _entries(bracketed(tokens(text, start[2]))) if start else []
        if found:
            out.append((m.group(1), found, m.start()))
        else:
            other += 1
    return out, other


def unions(text):
    """Each exported union of string literals: (type, [(member, offset)], offset); and the count of exported types that are
    no such union."""
    out, other = [], 0
    for m in UNION.finditer(text):
        members = []
        for kind, value, off in tokens(text, m.end()):
            if kind == "string":
                members.append((string_value(value), off))
            elif value == "|":
                continue
            elif value == ";" or (members and "\n" in text[members[-1][1]:off]):
                break
            else:
                members = []
                break
        if members:
            out.append((m.group(1), members, m.start()))
        else:
            other += 1
    return out, other
