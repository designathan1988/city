"""GDScript scripts across a set of files, for rules that follow a value from one script into another: each class (inner
classes too) with its base, members and their assignments, functions (parameters and their types, return type, locals
and their assignments, lambda parameters, valued returns), calls with their receivers, signal connections and writes to
other objects' members; the class of an access path, told by declarations and return types; the functions a call runs."""
import gd_syntax
from gd_syntax import access_path, receiver, split_args, statements, unbound, value_of
from tokens import string_body

NOT_CALLS = {"func", "if", "elif", "while", "for", "match", "return", "not", "and", "or", "in", "await", "assert", "is", "as"}
ASSIGN = {"=", "+="}
_DEPTH = 4  # how many declarations deep a class is followed (`var a := b.c()`, b's own initializer...)
VALUE_TYPES = {"String", "StringName", "int", "float", "bool", "Array", "Dictionary", "Callable", "Vector2", "Vector3", "Color"}


class Fn:
    """A named function. `assigns`: {local or parameter: [(line, value or None when not a plain value, function)]}."""

    def __init__(self, script, name, args, ret):
        self.script, self.name, self.ret = script, name, ret
        self.params = [a[0].text for a in args if a and a[0].kind == "ident"]
        self.types = {a[0].text: a[2].text for a in args if len(a) > 2 and a[1].text == ":" and a[2].kind == "ident"}
        self.required = sum(1 for a in args if a and not any(t.text in ("=", ":=") for t in a))
        self.locals, self.assigns, self.returns, self.lambda_params = {}, {}, [], set()

    def param(self, name):
        """Index of `name` as this function's own parameter (not a local's or a lambda's of the same name), or None."""
        ok = name in self.params and name not in self.lambda_params and name not in self.locals
        return self.params.index(name) if ok else None


class Script:
    """One class of a file, its own or an inner one. `lines`: (logical line, function or None); `assigns`: {member:
    [(line, value or None, function or None)]}; `writes`: (receiver, member, line, value or None, function) to another object's member."""

    def __init__(self, path, cls, outer=None):
        self.path, self.cls, self.base, self.outer = path, cls, None, outer
        self.members, self.assigns, self.funcs, self.inner = {}, {}, {}, {}
        self.lines, self.calls, self.conns, self.writes = [], [], [], []


def _func_header(toks, script):
    anns = gd_syntax.annotations(toks)
    rest = toks[anns[-1][2]:] if anns else toks
    rest = rest[1:] if rest and rest[0].text == "static" else rest
    if len(rest) < 3 or rest[0].text != "func" or rest[1].kind != "ident" or rest[2].text != "(":
        return None
    args, close = split_args(rest, 2)
    return Fn(script, rest[1].text, args, rest[close + 2].text if close + 2 < len(rest) and rest[close + 1].text == "->" else None)


def read_file(path, src):
    """The scripts of one file: its own class, then its inner classes."""
    lines = gd_syntax.logical_lines(src)
    top, stack, at_line = Script(path, None), [], {}
    scripts = [top]
    for ln in lines:
        while stack and stack[-1][0] >= ln.indent:
            stack.pop()
        owner = next((s for _, s in reversed(stack) if isinstance(s, Script)), top)
        fn = next((s for _, s in reversed(stack) if isinstance(s, Fn)), None)
        toks, head = ln.toks, _func_header(ln.toks, owner)
        at_line[ln.first] = owner
        if (not stack or isinstance(stack[-1][1], Script)) and len(toks) > 1 and toks[0].text in ("class_name", "extends"):
            setattr(owner, "cls" if toks[0].text == "class_name" else "base", toks[1].text)
        elif toks[0].text == "class" and len(toks) > 1 and toks[-1].text == ":":
            inner = Script(path, f"{top.cls or path}.{toks[1].text}", top)
            inner.base = toks[3].text if len(toks) > 4 and toks[2].text == "extends" else None
            owner.inner[toks[1].text] = inner
            scripts.append(inner)
            stack.append((ln.indent, inner))
        elif head is not None:
            owner.funcs.setdefault(head.name, head)
            stack.append((ln.indent, head))
        else:
            in_lambda = any(s == "lambda" for _, s in stack)
            for n, stmt in enumerate(statements(toks)):
                _read_line(owner, fn, stmt, in_lambda or n > 0)
                owner.lines.append((stmt, fn))
            if toks[-1].text == ":" and any(gd_syntax.lambda_at(toks, i) for i in range(len(toks))):
                stack.append((ln.indent, "lambda"))
    for d in gd_syntax.declarations(lines):
        s = at_line.get(d.line, top)
        if d.member and d.name not in s.members:
            s.members[d.name] = d
            s.assigns.setdefault(d.name, []).extend([(d.line, d.init_toks, None)] if d.init_toks else [])
    return scripts


def _read_line(owner, fn, toks, in_lambda):
    """Locals, assignments and returns of a function's line; calls, connections and writes of any line."""
    head = toks[0].text
    if fn is not None and head in ("var", "for") and len(toks) > 1:
        fn.locals.setdefault(toks[1].text, toks)
        eq = gd_syntax.top_index(toks, ("=", ":=")) if head == "var" else None
        if eq is not None or head == "for":
            fn.assigns.setdefault(toks[1].text, []).append((toks[0].line, value_of(toks[eq + 1:]) if eq else None, fn))
    elif fn is not None and len(toks) > 2 and toks[0].kind == "ident" and toks[1].text in ASSIGN:
        value = value_of(toks[2:]) if toks[1].text == "=" else None
        where = fn.assigns if head in fn.locals or head in fn.params else owner.assigns
        where.setdefault(head, []).append((toks[0].line, value, fn))
    for i, t in enumerate(toks):
        if t.kind != "ident":
            continue
        prev = toks[i - 1].text if i else ""
        nxt = toks[i + 1].text if i + 1 < len(toks) else ""
        if t.text == "return" and fn is not None and not in_lambda and prev in ("", ":") and len(toks) > i + 1 \
                and not any(gd_syntax.lambda_at(toks, k) for k in range(i)):
            fn.returns.append((t.line, value_of(toks[i + 1:])))
        elif gd_syntax.lambda_at(toks, i) and fn is not None:
            fn.lambda_params.update(a[0].text for a in split_args(toks, i + 1)[0] if a)
        elif prev == "." and nxt in ASSIGN and i > 1 and toks[0].kind == "ident":
            recv = receiver(toks, i - 1)
            value = value_of(toks[i + 2:]) if nxt == "=" else None
            if [r.text for r in recv] == ["self"]:
                owner.assigns.setdefault(t.text, []).append((t.line, value, fn))
            else:
                owner.writes.append((recv, t.text, t.line, value, fn))
        elif nxt == "(" and t.text not in NOT_CALLS:
            _read_call(owner, fn, toks, i)


def _read_call(owner, fn, toks, i):
    t, prev = toks[i], toks[i - 1].text if i else ""
    args = split_args(toks, i + 1)[0]
    sig = toks[i - 2].text if prev == "." and i > 1 and toks[i - 2].kind == "ident" else None
    named = args and args[0] and args[0][0].kind == "string"
    if t.text == "connect" and prev == "." and args:
        owner.conns.append((string_body(args[0][0]), args[1], fn) if named and len(args) > 1 else (sig, args[0], fn))
    elif t.text == "emit" and prev == "." and sig:
        owner.calls.append(("signal", sig, t.line, args, None, fn))
    elif t.text == "emit_signal" and named:
        owner.calls.append(("signal", string_body(args[0][0]), t.line, args[1:], None, fn))
    else:
        owner.calls.append(("func", t.text, t.line, args, receiver(toks, i - 1) if prev == "." else None, fn))


class Index:
    """The scripts of the given (path, source) pairs, their classes by name and their functions by name."""

    def __init__(self, sources):
        self.scripts = [s for p, src in sources for s in read_file(p, src)]
        self.classes = {s.cls: s for s in self.scripts if s.cls}
        self.by_name, self.writes = {}, {}
        for s in self.scripts:
            for name, fn in s.funcs.items():
                self.by_name.setdefault(name, []).append(fn)
            for w in s.writes:
                self.writes.setdefault(w[1], []).append((s, *w))
        self.connected = self._connections()

    def chain_of(self, s):
        """The script and its bases, nearest first."""
        seen = []
        while s is not None and s not in seen:
            seen.append(s)
            s = self.classes.get(s.base) or (s.outer.inner.get(s.base) if s.outer else None)
        return seen

    def _lookup(self, s, name, called, depth):
        """The class of member `name`, of an inner class `name`, or the return class of function `name`, of s or a base."""
        for x in self.chain_of(s):
            if called and name in x.funcs:
                return x.funcs[name].ret
            if not called and name in x.members:
                d = x.members[name]
                return self._decl_type(d.type_toks, d.init_toks, x, None, depth)
            if not called and name in x.inner:
                return x.inner[name].cls
        return None

    def _decl_type(self, type_toks, init, s, fn, depth):
        if type_toks:
            return type_toks[0].text
        if len(init) > 2 and init[-2].text == "as":
            return init[-1].text
        return self.type_of(init, s, fn, depth + 1) if depth < _DEPTH else None

    def type_of(self, toks, s, fn, depth=0):
        """The class of an access path (an object's or a class's own); None when it cannot be told."""
        chain = access_path(toks)
        if not chain or depth > _DEPTH:
            return None
        (name, called), cur = chain[0], None
        local = fn.locals.get(name) if fn is not None and not called else None
        if fn is not None and not called and name in fn.types and local is None:
            cur = fn.types[name]
        elif local is not None and len(local) > 3 and local[2].text == ":":
            cur = local[3].text
        elif local is not None and local[0].text == "var" and len(local) > 3:
            cur = self._decl_type([], local[3:], s, fn, depth)
        elif local is None and name == "self":
            cur = s.cls
        elif local is None:
            cur = self._lookup(s, name, called, depth)
            cur = name if cur is None and not called and (name in self.classes or name[:1].isupper()) else cur
        for seg, seg_called in chain[1:]:  # `X.new()` is an X
            cur = cur if (seg, seg_called) == ("new", True) else self._lookup(self.classes.get(cur), seg, seg_called, depth) if cur else None
        return cur

    def is_a(self, cls, s):
        return s in self.chain_of(self.classes.get(cls))

    def targets(self, kind, name, recv, s, fn, args=None):
        """The functions a call may run: its script's when bare, its receiver's class's when that is known (`X.new` runs
        X's `_init`), every one of that name that takes these arguments when not; for a signal, the functions connected."""
        if kind == "signal":
            return [t for sig, t in self.connected if sig == name]
        name = "_init" if name == "new" and recv else name
        if name not in self.by_name:
            return []
        cls = self.type_of(recv, s, fn) if recv else None
        if recv and cls not in self.classes:  # a class not indexed, or none told
            return [t for t in self.by_name[name] if args is None or self._takes(t, args, s, fn)]
        return [x.funcs[name] for x in self.chain_of(self.classes.get(cls) if recv else s) if name in x.funcs][:1]

    def _takes(self, t, args, s, fn):
        """False when the function cannot take these arguments: their number, or an object of an indexed class given
        where a value type or an unrelated indexed class is declared."""
        if not t.required <= len(args) <= len(t.params):
            return False
        for a, p in zip(args, t.params):
            want, got = t.types.get(p), self.type_of(a, s, fn)
            if got in self.classes and (want in VALUE_TYPES or want in self.classes and not self.is_a(got, self.classes[want])):
                return False
        return True

    def _connections(self):
        """(signal, function) of each `sig.connect(f)`, `sig.connect(obj.f)` or `sig.connect(f.bind(...))`."""
        out = []
        for s in self.scripts:
            for sig, target, fn in s.conns:
                target = unbound(target)
                chain = access_path(target)
                if chain and not chain[-1][1]:
                    out += [(sig, t) for t in self.targets("func", chain[-1][0], target[:-2] or None, s, fn)]
        return out
