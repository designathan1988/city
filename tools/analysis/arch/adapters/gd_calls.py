"""GDScript calls and attribute chains: what a call runs when the type is known, the name left for the linker when it is not, and the
meaning of each argument read from the engine's description."""
from ..core import words_of
from ..model import GRAMMAR, NAME
from .gd_body import READ, TEXT, WRITE, literal, node_text


SIZING = {"push", "append", "insert", "resize", "pop", "erase", "remove", "clear"}


def _args(n):
    a = next((c for c in n.named_children if c.type == "arguments"), None)
    return list(a.named_children) if a is not None else []


def call(b, n, st):
    head = n.named_children[0] if n.named_children else None
    if head is None or head.type != "identifier":
        for c in n.named_children:
            b.expr(c, st)
        return
    name = node_text(head, b.src)
    line = b.line(n)
    args = _args(n)
    if name in ("preload", "load") and args:
        kind = "preload" if name == "preload" else "load"
        if args[0].type in TEXT:
            b.f.ref("file", literal(args[0], b.src), st.unit, b.file, line, kind)
            b.f.site(b.file, line, args[0].start_point[1] + 1, "string")
        else:
            b.f.edge(st.unit, "load", "name:computed:" + node_text(args[0], b.src), b.file, line, GRAMMAR)
            b.expr(args[0], st)
        return
    if words_of(name) == ["callable"] and len(args) > 1:
        # The type that holds a function by object and name.
        word = literal(args[1], b.src)
        found, _ = b.t.lookup(b.cls, word, "funcs")
        b.f.edge(st.unit, "callable", found or "name:method:" + word, b.file, line, GRAMMAR, via=name)
        b.f.site(b.file, line, args[1].start_point[1] + 1, "string")
        return
    uid, _ = b.t.lookup(b.cls, name, "funcs")
    b.f.site(b.file, line, head.start_point[1] + 1, "call")
    if uid:
        b.f.edge(st.unit, "call", uid, b.file, line, GRAMMAR, depth=st.depth, locks=())
    else:
        api_call(b, st, name, line, None)
    arguments(b, args, st, name)


def api_call(b, st, name, line, receiver):
    """A call nothing in the project declares for this type: the engine's, when its description has the name, else a name to link. A function of no class
    is the language's; of a receiver no one can tell, it crosses only if no value type answers to that name, and it is told by no class it cannot be sure of."""
    if name in b.t.api.words:
        owners = {cls for cls, _, _, _ in b.t.api.methods.get(name, ())}
        owner = receiver or (next(iter(owners)) if len(owners) == 1 else "")
        b.f.edge(st.unit, "call", f"ext:{owner}.{name}", b.file, line, GRAMMAR, depth=st.depth, locks=())
        if receiver not in b.t.api.values if receiver else owners and not owners & b.t.api.values:
            b.f.cost(st.unit, "boundary", b.file, line, st.depth, api=f"{owner}.{name}")
        if name == "new" and st.depth > 0:
            b.f.cost(st.unit, "alloc", b.file, line, st.depth, what="new")
    else:
        b.f.edge(st.unit, "call", "name:method:" + name, b.file, line, NAME, depth=st.depth, locks=())


def arguments(b, args, st, method):
    """Each argument by what the engine says it means: a function given, a name of a method, signal, property, group or path."""
    roles = b.t.api.roles(method)
    of_text = _of_text(b, method)
    for i, a in enumerate(args):
        role = roles.get(i, "")
        if a.type in TEXT and of_text:
            b.f.site(b.file, b.line(a), a.start_point[1] + 1, "string")
        elif a.type in TEXT:
            b.text(a, st, f"{method}#{i}:{role.split(':')[-1]}" if role else f"{method}#{i}")
        elif role == "callable":
            given(b, a, st, method)
        else:
            b.expr(a, st)


def _of_text(b, method):
    """A method only text answers to: what it is handed is text to compare with, never a name of anything."""
    owners = b.t.api.methods.get(method, ())
    return bool(owners) and all("string" in words_of(cls) for cls, _, _, _ in owners)


def given(b, a, st, method):
    """A function handed to the engine: by name, as a bound method, or written there."""
    text = node_text(a, b.src)
    line = b.line(a)
    if a.type == "lambda":
        uid = b.lambda_(a, st)
        b.f.edge(st.unit, "callable", uid, b.file, line, GRAMMAR, via=method)
        return
    name = text.split(".bind(")[0].split(".call")[0].rsplit(".", 1)[-1]
    uid, _ = b.t.lookup(b.cls, name, "funcs")
    b.f.edge(st.unit, "callable", uid or "name:method:" + name, b.file, line, GRAMMAR, via=method)
    if ".bind(" in text:
        b.f.edge(st.unit, "bind", uid or "name:method:" + name, b.file, line, GRAMMAR)
    b.expr(a, st)


def base_class(b, first, st):
    """The class of the first name of a chain: a local, a field, an autoload, a class by name, or this class."""
    name = node_text(first, b.src)
    if name == "self":
        return b.cls
    if name in st.types:
        return b.t.script_of(st.types[name])
    uid, owner = b.t.lookup(b.cls, name, "vars")
    if uid:
        return b.t.script_of(owner.types.get(name, ""))
    return b.t.script_of(name)


def chain(b, n, st, mode, emit):
    """`a.b.c(x).d`: each step resolved while the type is known; the rest becomes a name. Returns the class it ends in."""
    parts = n.named_children
    if not parts:
        return None, ""
    first = parts[0]
    cls = None
    if first.type == "identifier":
        cls = base_class(b, first, st)
        name = node_text(first, b.src)
        if emit and name != "self" and name not in st.types:
            b.identifier(first, st, READ)
    elif emit:
        b.expr(first, st)
    last = ""
    for i, p in enumerate(parts[1:], 1):
        final = i == len(parts) - 1
        line = b.line(p)
        if p.type == "attribute_call":
            head = p.named_children[0]
            last = node_text(head, b.src)
            cls = member_call(b, cls, last, p, st, emit, line, head)
        elif p.type == "identifier":
            last = node_text(p, b.src)
            cls = member_field(b, cls, last, st, mode if final else READ, emit, line, p)
        elif p.type == "attribute_subscript":
            # `a.b[i]`: the field or key `b`, read like any other, then one of what it holds.
            head = p.named_children[0]
            last = node_text(head, b.src)
            member_field(b, cls, last, st, mode if final else READ, emit, line, head)
            cls = None
            if emit:
                for index in p.named_children[1:]:
                    b.expr(index, st)
        elif emit:
            b.expr(p, st)
    return cls, last


def member_field(b, cls, name, st, mode, emit, line, node):
    uid = owner = None
    if cls is not None:
        uid, owner = b.t.lookup(cls, name, "vars")
        if uid is None:
            uid, owner = b.t.lookup(cls, name, "signals")
    if uid is None:
        if emit:
            b.f.ref("key", name, st.unit, b.file, line, "attribute")  # `table.key`: a value of data read by its name, as far as text can tell
            target = "ext:." + name if name in b.t.api.props else "name:property:" + name
            b.f.edge(st.unit, "write" if mode else "read", target, b.file, line, GRAMMAR if target[0] == "e" else NAME, depth=st.depth, locks=())
        return None
    if emit:
        b.member(cls, name, st, mode, line, node.start_point[1] + 1)
    return b.t.script_of(owner.types.get(name, ""))


def member_call(b, cls, name, p, st, emit, line, head):
    args = _args(p)
    uid = None
    owner = None
    if cls is not None:
        uid, owner = b.t.lookup(cls, name, "funcs")
    if emit:
        b.f.site(b.file, line, head.start_point[1] + 1, "call")
        signal_use(b, p, name, st, line, args)
        if uid:
            b.f.edge(st.unit, "call", uid, b.file, line, GRAMMAR, depth=st.depth, locks=())
        elif cls is None or name in b.t.api.words:
            api_call(b, st, name, line, None)
        else:
            b.f.edge(st.unit, "call", "name:method:" + name, b.file, line, NAME, depth=st.depth, locks=())
        arguments(b, args, st, name)
    return b.t.script_of(owner.returns.get(name, "")) if owner is not None else None


def constant(b, x):
    """A value written in the code: a number, a list spelled out, a constant of a class."""
    if x.type in ("integer", "array"):
        return True
    if x.type not in ("identifier", "attribute"):
        return False
    words = node_text(x, b.src).split(".")
    cls = b.t.script_of(words[0]) if len(words) == 2 else b.cls
    uid, owner = b.t.lookup(cls, words[-1], "vars") if cls is not None and len(words) < 3 else (None, None)
    return uid is not None and words[-1] in owner.constants


def fixed(b, n, over):
    """A loop whose count is written down: the same few steps whatever the state holds, straight-line work and not one more level of growth."""
    if n.type != "for_statement":
        return False
    if over.type == "call" and node_text(over.named_children[0], b.src) == "range":
        return all(constant(b, a) for a in _args(over))
    return constant(b, over)


def resizes(b, body, field):
    """A `while` that grows or trims what its condition measures brings it to a size: it does not walk what it holds."""
    name = field.rsplit(".", 1)[-1]
    for x in b.walk(body):
        parts = x.named_children if x.type == "attribute" else ()
        if len(parts) > 1 and node_text(parts[0], b.src) == name and parts[-1].type == "attribute_call" and SIZING & set(words_of(node_text(parts[-1].named_children[0], b.src))):
            return True
    return False


def signal_use(b, p, name, st, line, args):
    """`x.connect(f)` and `x.emit()`: the signal is whatever the chain named just before; by the shape of the engine's Signal type."""
    parent = p.parent
    if parent is None or parent.type != "attribute":
        return
    index = next((i for i, c in enumerate(parent.named_children) if c == p), 0)
    if index == 0:
        return
    word = node_text(parent.named_children[index - 1], b.src)
    methods = b.t.api.methods.get(name, ())
    if any(words_of(cls) == ["signal"] for cls, _, _, _ in methods):
        b.f.edge(st.unit, "signal_" + name, "name:signal:" + word, b.file, line, GRAMMAR, handler=node_text(args[0], b.src) if args else "")
    if any(words_of(cls) == ["callable"] for cls, _, _, _ in methods):
        uid, _ = b.t.lookup(b.cls, word, "funcs")
        if uid:
            b.f.edge(st.unit, name, uid, b.file, line, GRAMMAR)


__all__ = ["call", "chain", "fixed", "resizes", "WRITE"]
