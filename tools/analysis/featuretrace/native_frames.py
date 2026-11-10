"""Names of the frames of a native stack: an offset in a module -> the C++ function it runs, as the tracer names it.

A function's name comes from the linker's map of the module (lld's COFF map: `Address Size Align Out In Symbol`) or,
for a module that keeps its symbol table or debug information, from llvm-symbolizer. A module with neither has no names
(only its exports, which would mislead): its frames stay unnamed. A demangled name (Itanium ABI, as clang and gcc print
it) becomes the tracer's qualified name: return type, parameters, template arguments and ABI tags dropped, anonymous
namespaces left out (the tracer does not write them either), a closure (`f(...)::$_0`, `f(...)::'lambda'(...)`) named
as a lambda of its enclosing function.
"""
from __future__ import annotations

import bisect
import re
import struct
import subprocess
from pathlib import Path

MAP_HEADER = ("Address", "Size", "Align", "Out", "In", "Symbol")   # lld's map: one column each, as its header
ABI_TAG = re.compile(r"\[abi:[^\]]*\]")
OPERATOR = re.compile(r"operator(?:\s*(?:\(\)|\[\]|<=>|<<=|>>=|<<|>>|<=|>=|->\*|->|&&|\|\||\+\+|--|"
                      r"new\[\]|delete\[\]|[-+*/%^&|~!=<>,]=?)|\s+(?:new|delete|\"\"\s*\w+|[\w:]+))")
TRAILING = re.compile(r"\s*(?:const|volatile|&&|&|noexcept)\s*$")
CLOSURE = re.compile(r"^(?:\$_\d+|'lambda\d*'(?:\(.*\))?|\{lambda\(.*\)#\d+\})$")
ANONYMOUS = ("(anonymous namespace)::", "{anonymous}::")
SYMBOLIZE_TIMEOUT_S = 600


class LinkMap:
    """Function start -> names, from a linker map: the function holding an offset is the last one starting at or
    before it, inside the code section."""

    def __init__(self, path: Path):
        starts, names, objects, code, columns, current = [], [], [], None, None, ""
        with open(path, encoding="utf-8", errors="replace") as f:
            for line in f:
                line = line.rstrip("\n")
                if columns is None:                     # where each column starts, read from the header line
                    words = line.split()
                    if tuple(words) == MAP_HEADER:
                        columns = {w: line.index(w) for w in MAP_HEADER}
                    continue
                fields = line[:columns["Out"]].split()
                if len(fields) != 3:
                    continue
                address, size = int(fields[0], 16), int(fields[1], 16)
                text = line[columns["Out"]:]
                start = columns["Out"] + len(text) - len(text.lstrip())   # the column a line's text starts in tells
                text = text.strip()                                       # what it is; the text runs to the line's end
                if start < columns["In"]:               # an output section: the code's range
                    if text == ".text":
                        code = (address, address + size)
                elif start < columns["Symbol"]:         # an input section: the object file the next symbols are in
                    current = text.split(":(")[0]
                else:                                   # a symbol
                    starts.append(address)
                    names.append(text)
                    objects.append(current)
        if columns is None:
            raise ValueError(f"{path}: not a linker map with the header {' '.join(MAP_HEADER)}")
        order = sorted(range(len(starts)), key=starts.__getitem__)
        self.starts = [starts[i] for i in order]
        self.names = [names[i] for i in order]
        self.objects = [objects[i] for i in order]
        self.code = code

    def name(self, offset: int) -> tuple[str, str] | None:
        """(the function holding the offset, the object file it was compiled into)."""
        if self.code and not self.code[0] <= offset < self.code[1]:
            return None
        i = bisect.bisect_right(self.starts, offset) - 1
        return (self.names[i], self.objects[i]) if i >= 0 else None


def has_symbols(path: Path) -> bool:
    """A PE module that keeps a symbol table or DWARF sections (not stripped): llvm-symbolizer can name its code."""
    with open(path, "rb") as f:
        head = f.read(4096)
    pe = struct.unpack_from("<I", head, 0x3C)[0]
    symbols, count = struct.unpack_from("<II", head, pe + 4 + 8)
    if symbols and count:
        return True
    sections, optional = struct.unpack_from("<H", head, pe + 6)[0], struct.unpack_from("<H", head, pe + 20)[0]
    table = pe + 24 + optional
    return any(head[table + 40 * i:table + 40 * i + 8].startswith(b"/") for i in range(sections))  # long names: .debug_*


def symbolize(path: Path, offsets: list[int], symbolizer: str) -> dict[int, tuple[str, str] | None]:
    """Offsets of one unstripped module -> function names, by llvm-symbolizer over its symbols."""
    with open(path, "rb") as f:
        head = f.read(4096)
    pe = struct.unpack_from("<I", head, 0x3C)[0]
    image_base = struct.unpack_from("<Q", head, pe + 24 + 24)[0]
    ask = sorted(set(offsets))
    r = subprocess.run([symbolizer, f"--obj={path}", "--demangle", "--no-inlines"], timeout=SYMBOLIZE_TIMEOUT_S,
                       input="\n".join(f"0x{image_base + o:x}" for o in ask), capture_output=True, text=True)
    if r.returncode != 0:
        raise RuntimeError(f"{symbolizer} failed on {path}: {r.stderr.strip()[:300]}")
    blocks = [b.splitlines() for b in r.stdout.strip().split("\n\n")]
    if len(blocks) != len(ask):
        raise RuntimeError(f"{symbolizer} answered {len(blocks)} of {len(ask)} addresses of {path}")
    return {o: ((b[0], "") if b and b[0] not in ("??", "") else None) for o, b in zip(ask, blocks)}


def _closing(s: str, end: int, open_c: str, close_c: str) -> int:
    """The index of the bracket that opens the group closed at s[end]."""
    depth = 0
    for i in range(end, -1, -1):
        if s[i] == close_c:
            depth += 1
        elif s[i] == open_c:
            depth -= 1
            if depth == 0:
                return i
    return -1


def _top_split(s: str, sep: str) -> list[str]:
    """`s` split at `sep` outside every (), <> and [] group."""
    parts, depth, start, i = [], 0, 0, 0
    while i < len(s):
        c = s[i]
        if c in "(<[":
            depth += 1
        elif c in ")>]":
            depth -= 1
        elif depth == 0 and s.startswith(sep, i):
            parts.append(s[start:i])
            start = i + len(sep)
            i += len(sep)
            continue
        i += 1
    parts.append(s[start:])
    return parts


def _drop_groups(s: str, open_c: str, close_c: str) -> str:
    out, depth = [], 0
    for c in s:
        if c == open_c:
            depth += 1
        elif c == close_c:
            depth -= 1
        elif depth == 0:
            out.append(c)
    return "".join(out)


def qualified(demangled: str) -> tuple[str, int]:
    """A demangled function name as (qualified name, closures): `ns::A::f` and 0, or the enclosing function and the
    number of closures nested in it for a lambda (`ns::A::f` and 1 for `ns::A::f(int)::$_0::operator()(int) const`)."""
    s = ABI_TAG.sub("", demangled)
    for anonymous in ANONYMOUS:
        s = s.replace(anonymous, "")
    operators = []

    def keep(match):
        rest = match.group(0)[len("operator"):].strip()     # `operator new`, `operator bool`: a word; else a symbol
        operators.append("operator " + rest if rest[:1].isalpha() or rest[:1] in "_\"" else "operator" + rest)
        return f"operator@{len(operators) - 1}@"

    s = OPERATOR.sub(keep, s).strip()
    while True:                                         # trailing qualifiers of a member function
        trimmed = TRAILING.sub("", s)
        if trimmed == s:
            break
        s = trimmed
    if s.endswith(")"):                                 # the parameter list
        s = s[:_closing(s, len(s) - 1, "(", ")")]
    head = _top_split(s, " ")                           # a return type written before the name
    s = head[-1] if head else s
    parts = [p for p in _top_split(s, "::") if p]
    closures, cut = 0, len(parts)
    for i, part in enumerate(parts):
        if CLOSURE.match(_drop_groups(part, "<", ">")) or CLOSURE.match(part):
            closures += 1
            cut = min(cut, i)
    names = []
    for part in parts[:cut]:
        part = _drop_groups(part, "<", ">")
        part = _drop_groups(part, "(", ")") if "(" in part else part
        names.append(part)
    name = "::".join(names)
    name = re.sub(r"operator@(\d+)@", lambda m: operators[int(m.group(1))], name)
    return name, closures
