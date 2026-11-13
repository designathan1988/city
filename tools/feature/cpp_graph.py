"""The C++ program's graph as its compiler resolves it (libclang, with the build's own compile commands and the
toolchain's own headers): every function and lambda; every call to the exact function the compiler binds it to
(templates, overloads, methods by their class); every function handed to another as a value (a lambda or a function
named), the parameter it is handed through, the functions that run what their parameter holds, and those that pass
it on; every field read and written; the conditions each call and write sits under; the calls that leave the program
(the engine, the standard library); the loops. Nothing is guessed from a name.

  python cpp_graph.py BUILD_DIR SOURCE_ROOT TOOLCHAIN OUT.json [--jobs N] [--fresh]

Each translation unit is read once and kept by the time and size of the unit and of the headers it read; a run
reads again only what changed.
"""
from __future__ import annotations

import hashlib
import json
import os
import sys
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from pathlib import Path

import clang.cindex as c

LIBCLANG = r"C:\Program Files\LLVM\bin\libclang.dll"
K = c.CursorKind
FUNCS = {K.FUNCTION_DECL, K.CXX_METHOD, K.CONSTRUCTOR, K.DESTRUCTOR, K.FUNCTION_TEMPLATE, K.CONVERSION_FUNCTION}
LOOPS = {K.FOR_STMT, K.WHILE_STMT, K.DO_STMT, K.CXX_FOR_RANGE_STMT}
ASSIGN = {"=", "+=", "-=", "*=", "/=", "%=", "&=", "|=", "^=", "<<=", ">>="}


def norm(p: str) -> str:
    return os.path.normcase(os.path.abspath(p)).replace("\\", "/")


def flags(entry: dict, toolchain: str) -> list:
    args = entry.get("arguments") or split(entry["command"])
    out, skip = [], False
    for a in args[1:]:
        if skip:
            skip = False
            continue
        if a == "-o":
            skip = True
            continue
        if a == "-c" or norm(a) == norm(entry["file"]):
            continue
        out.append(a)
    tc = toolchain
    # The toolchain's own target and include search, as its driver adds them (`clang++ -E -v`).
    return ["--target=x86_64-w64-mingw32", "-nostdinc", "-nostdinc++",
            "-isystem", f"{tc}/include/c++/v1", "-isystem", f"{tc}/lib/clang/23/include",
            "-isystem", f"{tc}/include"] + out


def split(command: str) -> list:
    out, cur, quote = [], "", None
    for ch in command:
        if quote:
            if ch == quote:
                quote = None
            else:
                cur += ch
        elif ch in "\"'":
            quote = ch
        elif ch.isspace():
            if cur:
                out.append(cur)
                cur = ""
        else:
            cur += ch
    if cur:
        out.append(cur)
    return out


class Unit:
    """What one translation unit says, limited to the code under the source root."""

    def __init__(self, root: str):
        self.root = root
        self.sources = {}
        self.functions = {}       # id -> {name, file, line, end, kind, owner}
        self.calls = []           # {from, to, line, guards, out}
        self.hands = []           # {from, fn, to, index, line, guards}: `fn` handed to `to` as its argument `index`
        self.invokes = []         # {in, param, line, guards}: a function runs what its parameter `param` holds
        self.forwards = []        # {in, param, to, index}: a function passes its parameter on to `to` as `index`
        self.stores = []          # {in, fn, field, line}: a function value kept in a field
        self.field_calls = []     # {in, field, line, guards}: what a field holds, run
        self.fields = []          # {in, field, write, line, guards}
        self.loops = []           # {in, line, end, depth, cond}
        self.texts = []           # {in, text, line}
        self.numbers = []         # {in, value, line}: a number written in place
        self.consts = []          # {in, name, value, line}: a named constant read, with what it is written as
        self.registers = []       # {in, name, fn, line}: a call handing a function together with a name (a registration)
        self.overrides = []       # {fn, base}: a method that overrides another (it runs where the other is called)
        self.last_line = 0

    def mine(self, cursor) -> bool:
        f = cursor.location.file
        return f is not None and norm(f.name).startswith(self.root)

    def src(self, file: str) -> bytes:
        if file not in self.sources:
            self.sources[file] = Path(file).read_bytes()
        return self.sources[file]

    def text(self, cursor) -> str:
        e = cursor.extent
        if e.start.file is None:
            return ""
        data = self.src(e.start.file.name)
        return " ".join(data[e.start.offset:e.end.offset].decode("utf-8", "replace").split())[:300]

    def where(self, cursor) -> tuple:
        loc = cursor.location
        return norm(loc.file.name), loc.line


def qualified(cursor) -> str:
    parts = []
    while cursor is not None and cursor.kind != K.TRANSLATION_UNIT:
        if cursor.spelling:
            parts.append(cursor.spelling)
        cursor = cursor.semantic_parent
    return "::".join(reversed(parts))


def fid(cursor) -> str:
    if cursor.kind == K.LAMBDA_EXPR:
        loc = cursor.location
        return f"lambda@{norm(loc.file.name)}:{loc.line}:{loc.column}"
    return cursor.get_usr() or qualified(cursor)


def target(cursor):
    """The function a cursor names, through the wrappers the compiler adds; a lambda itself."""
    seen = 0
    while cursor is not None and seen < 8:
        if cursor.kind == K.LAMBDA_EXPR:
            return cursor
        if cursor.kind == K.DECL_REF_EXPR and cursor.referenced is not None and cursor.referenced.kind in FUNCS:
            return cursor.referenced
        if cursor.kind == K.DECL_REF_EXPR and cursor.referenced is not None and cursor.referenced.kind == K.VAR_DECL:
            return held_lambda(cursor.referenced)    # a variable holding a lambda: that lambda
        kids = [k for k in cursor.get_children()]
        if cursor.kind not in (K.UNEXPOSED_EXPR, K.CALL_EXPR, K.PAREN_EXPR, K.UNARY_OPERATOR) or len(kids) != 1:
            if cursor.kind == K.CALL_EXPR and len(kids) == 1:     # a lambda copied into a parameter
                cursor = kids[0]
                seen += 1
                continue
            return None
        cursor = kids[0]
        seen += 1
    return None


def held_lambda(var):
    """The lambda a variable is initialised with, if it is one (`auto f = [..] {..};`)."""
    for ch in var.get_children():
        seen, cur = 0, ch
        while cur is not None and seen < 6:
            if cur.kind == K.LAMBDA_EXPR:
                return cur
            kids = list(cur.get_children())
            if len(kids) != 1:
                break
            cur, seen = kids[0], seen + 1
    return None


def param_of(cursor):
    """The parameter a cursor reads, through wrappers."""
    seen = 0
    while cursor is not None and seen < 8:
        if cursor.kind == K.DECL_REF_EXPR and cursor.referenced is not None and cursor.referenced.kind == K.PARM_DECL:
            return cursor.referenced
        kids = list(cursor.get_children())
        if len(kids) != 1:
            return None
        cursor = kids[0]
        seen += 1
    return None


def param_index(fn, parm) -> int:
    if fn is None or parm is None:
        return -1
    params = [a for a in fn.get_children() if a.kind == K.PARM_DECL]
    for i, a in enumerate(params):
        if a.get_usr() == parm.get_usr() or (a.spelling == parm.spelling and a.location.line == parm.location.line):
            return i
    return -1


def operator(unit: Unit, cursor) -> str:
    """The operator of a binary or compound assignment, from the text between its two sides."""
    kids = list(cursor.get_children())
    if len(kids) != 2:
        return ""
    a, b = kids[0].extent, kids[1].extent
    if a.end.file is None:
        return ""
    data = unit.src(a.end.file.name)
    return data[a.end.offset:b.start.offset].decode("utf-8", "replace").strip()


def walk(unit: Unit, cursor, fn, fn_cursor, guards: tuple, depth: int, lvalue: bool):
    kind = cursor.kind
    if kind in FUNCS or kind == K.LAMBDA_EXPR:
        if not unit.mine(cursor) or (kind != K.LAMBDA_EXPR and not cursor.is_definition()):
            if kind != K.LAMBDA_EXPR:
                return
        elif cursor.location.file is not None:
            i = fid(cursor)
            file, line = unit.where(cursor)
            if kind == K.CXX_METHOD:            # a virtual method's overrides run where it is called
                for o in overridden(cursor):
                    unit.overrides.append({"fn": i, "base": fid(o)})
            unit.functions[i] = {"name": qualified(cursor) if kind != K.LAMBDA_EXPR else f"{unit.functions.get(fn, {}).get('name', '?')}.<lambda:{line}>",
                                 "file": file, "line": line, "end": cursor.extent.end.line,
                                 "kind": "lambda" if kind == K.LAMBDA_EXPR else "function", "owner": fn}
            kids = list(cursor.get_children())
            for n, ch in enumerate(kids):
                # a constructor's `: field(parameter)` keeps what is handed through that parameter in the field
                if ch.kind == K.MEMBER_REF and n + 1 < len(kids) and ch.referenced is not None:
                    p = param_of(kids[n + 1])
                    if p is not None:
                        unit.stores.append({"in": i, "param": param_index(cursor, p), "field": qualified(ch.referenced),
                                            "line": ch.location.line})
                walk(unit, ch, i, cursor, (), 0, False)
            return
    if fn is None:
        for ch in cursor.get_children():
            walk(unit, ch, fn, fn_cursor, guards, depth, False)
        return
    # Inside a function of the program every node is the program's, also those the compiler adds with no place of their
    # own (an implicit conversion, a temporary): what lies under them is the code written there.
    if cursor.location.file is not None:
        unit.last_line = cursor.location.line
    line = unit.last_line                     # a node with no place of its own stands where the code before it is
    if kind == K.IF_STMT:
        kids = list(cursor.get_children())
        cond = next((k for k in kids if k.kind not in (K.COMPOUND_STMT,) and k.kind.is_expression()), kids[0] if kids else None)
        text = unit.text(cond) if cond is not None else "?"
        branches = [k for k in kids if k is not cond]
        walk(unit, cond, fn, fn_cursor, guards, depth, False) if cond is not None else None
        if branches:
            walk(unit, branches[0], fn, fn_cursor, guards + (f"{text} @{line}",), depth, False)
        for other in branches[1:]:
            walk(unit, other, fn, fn_cursor, guards + (f"!({text}) @{line}",), depth, False)
        return
    if kind == K.CONDITIONAL_OPERATOR:
        kids = list(cursor.get_children())
        if len(kids) == 3:
            text = unit.text(kids[0])
            walk(unit, kids[0], fn, fn_cursor, guards, depth, False)
            walk(unit, kids[1], fn, fn_cursor, guards + (f"{text} @{line}",), depth, False)
            walk(unit, kids[2], fn, fn_cursor, guards + (f"!({text}) @{line}",), depth, False)
            return
    if kind in LOOPS:
        text = unit.text(cursor).split("{")[0][:160]
        unit.loops.append({"in": fn, "line": line, "end": cursor.extent.end.line, "depth": depth + 1, "cond": text})
        for ch in cursor.get_children():
            walk(unit, ch, fn, fn_cursor, guards + (f"loop {text} @{line}",), depth + 1, False)
        return
    if kind == K.CASE_STMT:
        kids = list(cursor.get_children())
        label = unit.text(kids[0]) if kids else "?"
        for ch in kids[1:]:
            walk(unit, ch, fn, fn_cursor, guards + (f"case {label} @{line}",), depth, False)
        return
    if kind == K.DEFAULT_STMT:
        for ch in cursor.get_children():
            walk(unit, ch, fn, fn_cursor, guards + (f"default @{line}",), depth, False)
        return
    if kind == K.CALL_EXPR:
        ref = cursor.referenced
        args = list(cursor.get_arguments())
        if ref is not None and ref.kind == K.PARM_DECL:
            unit.invokes.append({"in": fn, "param": param_index(fn_cursor, ref), "line": line, "guards": list(guards)})
        elif ref is not None and ref.spelling == "operator()" and cursor.get_children():
            base = next(iter(cursor.get_children()), None)
            p = param_of(base) if base is not None else None
            fld = member_field(base)
            held = target(base) if base is not None else None
            if p is not None:
                unit.invokes.append({"in": fn, "param": param_index(fn_cursor, p), "line": line, "guards": list(guards)})
            elif fld is not None:
                unit.field_calls.append({"in": fn, "field": fld, "line": line, "guards": list(guards)})
            elif held is not None:                   # a lambda held in a variable, run: the lambda
                unit.calls.append({"from": fn, "to": fid(held), "name": None, "line": line, "guards": list(guards),
                                   "out": False})
        # What a call changes: the object a non-const method runs on, and what is given to a non-const reference.
        if ref is not None and ref.kind in (K.CXX_METHOD, K.FUNCTION_TEMPLATE) and not ref.is_const_method() and not ref.is_static_method():
            head = next(iter(cursor.get_children()), None)
            while head is not None and head.kind == K.UNEXPOSED_EXPR and len(list(head.get_children())) == 1:
                head = next(iter(head.get_children()))
            if head is not None and head.kind == K.MEMBER_REF_EXPR:
                base = next(iter(head.get_children()), None)
                fld = member_field(base) if base is not None else None
                if fld is not None:
                    unit.fields.append({"in": fn, "field": fld, "write": True, "line": line, "guards": list(guards),
                                        "by": ref.spelling})
        if ref is not None and ref.kind in FUNCS:
            try:
                types = list(ref.type.get_argument_types())
            except Exception:
                types = []
            for k, a in enumerate(args):
                if k < len(types) and types[k].kind == c.TypeKind.LVALUEREFERENCE and not types[k].get_pointee().is_const_qualified():
                    fld = member_field(a)
                    if fld is not None:
                        unit.fields.append({"in": fn, "field": fld, "write": True, "line": line, "guards": list(guards),
                                            "by": f"{ref.spelling} (reference)"})
        if ref is not None and ref.kind in FUNCS | {K.CONSTRUCTOR}:
            out = not (ref.location.file and norm(ref.location.file.name).startswith(unit.root))
            unit.calls.append({"from": fn, "to": fid(ref), "name": qualified(ref), "line": line,
                               "guards": list(guards), "out": out})
            handed_here = []
            for k, a in enumerate(args):
                t = target(a)
                if t is not None:
                    handed_here.append(t)
                    unit.hands.append({"from": fn, "fn": fid(t), "to": fid(ref), "index": k, "line": line,
                                       "guards": list(guards)})
                    continue
                p = param_of(a)                  # a parameter passed on: what it holds goes on to the callee
                if p is not None:
                    unit.forwards.append({"in": fn, "param": param_index(fn_cursor, p), "to": fid(ref), "index": k})
            if handed_here:
                named = [s for a in args for s in strings_in(a, 3)]
                for t in handed_here:
                    for s in named[:1]:
                        unit.registers.append({"in": fn, "name": s, "fn": fid(t), "line": line})
    if kind in (K.BINARY_OPERATOR, K.COMPOUND_ASSIGNMENT_OPERATOR):
        kids = list(cursor.get_children())
        if len(kids) == 2 and operator(unit, cursor) in ASSIGN:
            walk(unit, kids[0], fn, fn_cursor, guards, depth, True)
            t = target(kids[1])
            fld = member_field(kids[0])
            if t is not None and fld is not None:
                unit.stores.append({"in": fn, "fn": fid(t), "field": fld, "line": line})
            p = param_of(kids[1]) if t is None else None
            if p is not None and fld is not None:      # a parameter kept in a field: run where the field is
                unit.stores.append({"in": fn, "param": param_index(fn_cursor, p), "field": fld, "line": line})
            walk(unit, kids[1], fn, fn_cursor, guards, depth, False)
            return
    if kind == K.UNARY_OPERATOR:
        if any(t.spelling in ("++", "--") for t in cursor.get_tokens()):
            for ch in cursor.get_children():
                walk(unit, ch, fn, fn_cursor, guards, depth, True)
            return
    if kind == K.MEMBER_REF_EXPR and cursor.referenced is not None and cursor.referenced.kind == K.FIELD_DECL:
        # On the left of a write the whole chain is changed: `a.b.c = x` changes c, and b and a that hold it.
        unit.fields.append({"in": fn, "field": qualified(cursor.referenced), "write": lvalue, "line": line,
                            "guards": list(guards)})
    if kind == K.MEMBER_REF and cursor.referenced is not None and cursor.referenced.kind == K.FIELD_DECL:
        # A constructor's initializer list: `: field(value)` writes the field.
        unit.fields.append({"in": fn, "field": qualified(cursor.referenced), "write": True, "line": line,
                            "guards": list(guards)})
    if kind == K.STRING_LITERAL:
        unit.texts.append({"in": fn, "text": cursor.spelling.strip('"'), "line": line})
    if kind in (K.INTEGER_LITERAL, K.FLOATING_LITERAL):
        tokens = list(cursor.get_tokens())
        if tokens:
            unit.numbers.append({"in": fn, "value": tokens[0].spelling, "line": line})
    if kind == K.DECL_REF_EXPR and cursor.referenced is not None and cursor.referenced.kind == K.VAR_DECL:
        d = cursor.referenced
        scope = d.semantic_parent
        if d.type.is_const_qualified() and scope is not None and scope.kind not in FUNCS and scope.kind != K.LAMBDA_EXPR:
            unit.consts.append({"in": fn, "name": qualified(d), "value": unit.text(d)[:160], "line": line})
    for ch in cursor.get_children():
        walk(unit, ch, fn, fn_cursor, guards, depth, lvalue and kind in (K.MEMBER_REF_EXPR, K.UNEXPOSED_EXPR, K.PAREN_EXPR, K.ARRAY_SUBSCRIPT_EXPR))


def strings_in(cursor, depth: int) -> list:
    """The texts written in place inside an argument, a few levels down (a name wrapped by a call that makes it)."""
    if cursor is None or depth < 0:
        return []
    if cursor.kind == K.STRING_LITERAL:
        return [cursor.spelling.strip('"')]
    out = []
    for ch in cursor.get_children():
        out += strings_in(ch, depth - 1)
    return out


def member_field(cursor):
    seen = 0
    while cursor is not None and seen < 6:
        if cursor.kind == K.MEMBER_REF_EXPR and cursor.referenced is not None and cursor.referenced.kind == K.FIELD_DECL:
            return qualified(cursor.referenced)
        kids = list(cursor.get_children())
        if len(kids) != 1:
            return None
        cursor = kids[0]
        seen += 1
    return None


def read_unit(entry: dict, root: str, toolchain: str) -> dict:
    c.Config.set_library_file(LIBCLANG) if not c.Config.loaded else None
    t0 = time.monotonic()
    tu = c.Index.create().parse(entry["file"], args=flags(entry, toolchain),
                                options=c.TranslationUnit.PARSE_SKIP_FUNCTION_BODIES * 0)
    errors = [str(d) for d in tu.diagnostics if d.severity >= 3]
    unit = Unit(root)
    walk(unit, tu.cursor, None, None, (), 0, False)
    included = sorted({norm(i.include.name) for i in tu.get_includes() if i.include is not None})
    return {"file": norm(entry["file"]), "seconds": round(time.monotonic() - t0, 2), "errors": errors[:20],
            "included": included, "functions": unit.functions, "calls": unit.calls, "hands": unit.hands,
            "invokes": unit.invokes, "forwards": unit.forwards, "stores": unit.stores, "field_calls": unit.field_calls,
            "fields": unit.fields, "loops": unit.loops, "texts": unit.texts, "numbers": unit.numbers,
            "consts": unit.consts, "registers": unit.registers, "overrides": unit.overrides}


def stamp(paths) -> str:
    h = hashlib.sha256()
    for p in paths:
        try:
            st = os.stat(p)
            h.update(f"{p}|{st.st_size}|{st.st_mtime_ns}".encode())
        except OSError:
            h.update(f"{p}|gone".encode())
    return h.hexdigest()[:20]


def main() -> int:
    build, root, toolchain, out = sys.argv[1:5]
    jobs = int(sys.argv[sys.argv.index("--jobs") + 1]) if "--jobs" in sys.argv else 6
    fresh = "--fresh" in sys.argv
    root = norm(root)
    me = stamp([__file__])
    entries = [e for e in json.load(open(Path(build) / "compile_commands.json", encoding="utf-8"))
               if norm(e["file"]).startswith(root)]
    cache_dir = Path(out).with_suffix(".units")
    cache_dir.mkdir(parents=True, exist_ok=True)
    units, todo = [], []
    for e in entries:
        key = cache_dir / (hashlib.sha256(norm(e["file"]).encode()).hexdigest()[:16] + ".json")
        if key.exists() and not fresh:
            kept = json.loads(key.read_text(encoding="utf-8"))
            if kept.get("stamp") == stamp([e["file"]] + kept["included"]) + me:
                units.append(kept)
                continue
        todo.append((e, key))
    t0 = time.monotonic()
    print(f"{len(entries)} units, {len(units)} kept, {len(todo)} to read", file=sys.stderr, flush=True)
    with ProcessPoolExecutor(max_workers=jobs) as pool:
        futures = {pool.submit(read_unit, e, root, toolchain): key for e, key in todo}
        for n, f in enumerate(as_completed(futures), 1):
            u = f.result()
            u["stamp"] = stamp([u["file"]] + u["included"]) + me
            futures[f].write_text(json.dumps(u), encoding="utf-8")
            units.append(u)
            if n % 20 == 0 or n == len(todo):
                print(f"  {n}/{len(todo)} read, {time.monotonic() - t0:.0f} s", file=sys.stderr, flush=True)
    rows = ("calls", "hands", "invokes", "forwards", "stores", "field_calls", "fields", "loops", "texts", "numbers",
            "consts", "registers", "overrides")
    graph = {"language": "c++", "root": root, "functions": {}, "errors": {}, **{k: [] for k in rows}}
    seen = set()
    for u in units:
        graph["functions"].update(u["functions"])
        if u["errors"]:
            graph["errors"][u["file"]] = u["errors"]
        for k in rows:
            for row in u.get(k, ()):
                sig = (k, json.dumps(row, sort_keys=True))
                if sig not in seen:
                    seen.add(sig)
                    graph[k].append(row)
    Path(out).write_text(json.dumps(graph), encoding="utf-8")
    print(f"{len(graph['functions'])} functions, {len(graph['calls'])} calls, {len(graph['hands'])} handed, "
          f"{len(graph['fields'])} field uses, {len(graph['errors'])} units with errors; {time.monotonic() - t0:.0f} s",
          file=sys.stderr)
    return 0


if __name__ == "__main__":
    sys.exit(main())
