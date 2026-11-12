"""STATE_CONTROLLER: every state with its controller. An enum whose name ends in one of rules.json states.suffixes (a
State, a Mode, a Phase, a Speed, a Tool), or that states declares, is declared there as one of two things: a machine (the
file of its controller, which must be there, the type that holds it, the member that holds it, the one setter that writes
it, and the transition table the setter reads, or why any state may follow any) or a kind (a value chosen, never stepped,
with why). A machine's member is written only inside its setter: an assignment to it (`member =`, `member.store(`,
`member.exchange(`) anywhere else is refused, in the controller's own file whatever the value, elsewhere when it is the
machine's: the statement names the machine's enum, or the object written is the holder's (`obj.member`, `obj->member`,
`obj[k].member` with `obj` declared in the same file with the holder in its type, or `obj` the holder's own name: an
autoload, a static). Several machines may share a member's name; the object's declared type tells them apart. A
declaration of a variable that bears the member's name is no write of it."""
import json

import cpp_syntax
import gd_syntax

ID = "STATE_CONTROLLER"
_ATOMIC_WRITES = {"store", "exchange"}
_DECLARING = {"var", "const", "static", "constexpr", "auto", ">", "*", "&", "&&"}
_EXTENSIONS = (".cpp", ".h", ".gd")
_ACCESS = (".", "->")
_TYPE_ENDS = {"(", ",", ";", "{", "}", "=", ")", ":", "?"}  # where a C++ declaration's type begins, read back from its name
_TYPE_MARKS = {"&", "*", "&&", ">", ">>"}  # what may stand between a C++ declared name and its type
_NOT_TYPES = {"return", "case", "else", "throw", "delete", "new", "sizeof", "goto", "co_return", "co_yield", "do"}
_GD_DECLARING = {"var", "const", "(", ",", "for"}  # what stands before a GDScript name declared `name: Type`


def applies(ctx, path):
    paths = ctx.rules["states"]["paths"]
    return path.endswith(_EXTENSIONS) and any(path.startswith(p.rstrip("/") + "/") for p in paths)


def context(ctx):
    """What a verdict reads besides the file: the machines, the kinds and the suffixes, and which controllers are there."""
    cfg = ctx.rules["states"]
    there = sorted(m["file"] for m in cfg["machines"] if m["file"] in ctx.listed_set)
    return json.dumps([cfg, there], sort_keys=True)


def _spans(src):
    if src.lang == "gd":
        return gd_syntax.function_spans(gd_syntax.logical_lines(src))
    return cpp_syntax.function_spans(src.code())


def _inside(spans, name, line):
    return any(n == name and first <= line < first + length for n, first, length in spans)


def _enums(code):
    """(name, line) of every enum the code declares: `enum Name`, `enum class Name`, `enum struct Name`."""
    for i, t in enumerate(code):
        if t.kind != "ident" or t.text != "enum":
            continue
        j = i + 1
        if j < len(code) and code[j].text in ("class", "struct"):
            j += 1
        if j < len(code) and code[j].kind == "ident":
            yield code[j].text, code[j].line


def _statement(code, i, gd):
    """The tokens of the statement that holds code[i] from there on: to its `;` (C++) or the end of its line (GDScript)."""
    out = []
    for t in code[i:]:
        if (gd and t.line != code[i].line) or (not gd and t.text == ";"):
            break
        out.append(t)
    return out


def _past_index(code, j):
    """The index after the subscripts that follow code[j - 1] (`member[k][l]` steps to what comes after `]`)."""
    while j < len(code) and code[j].text == "[":
        depth = 0
        for k in range(j, len(code)):
            depth += {"[": 1, "]": -1}.get(code[k].text, 0)
            if depth == 0:
                j = k + 1
                break
        else:
            return len(code)
    return j


def _writes(code, member):
    """Indexes of the tokens where `member` is written: assigned with `=` (itself or an element of it), or an atomic's
    store/exchange."""
    n = len(code)
    for i, t in enumerate(code):
        if t.kind != "ident" or t.text != member:
            continue
        before = code[i - 1] if i else None
        if before is not None and (before.text in _DECLARING or (before.kind == "ident" and before.text != "return")):
            continue  # a declaration: a type or a declaring word stands before the name
        j = _past_index(code, i + 1)
        after = code[j].text if j < n else ""
        atomic = after == "." and j + 1 < n and code[j + 1].text in _ATOMIC_WRITES
        if after == "=" or atomic:
            yield i


def _open_bracket(code, k):
    """The index of the `[` that the `]` at code[k] closes."""
    depth = 0
    for j in range(k, -1, -1):
        depth += {"]": 1, "[": -1}.get(code[j].text, 0)
        if depth == 0:
            return j
    return 0


def _object(code, i):
    """The index of the name a write of the member at code[i] goes through (`obj.member`, `obj->member`, `obj[k].member`),
    or None when the member is written bare or through an expression that is no name."""
    k = i - 1
    if k < 0 or code[k].text not in _ACCESS:
        return None
    k -= 1
    while k >= 0 and code[k].text == "]":
        k = _open_bracket(code, k) - 1
    return k if k >= 0 and code[k].kind == "ident" else None


def _cpp_declared(code, d):
    """The identifiers of the type a C++ declaration gives the name at code[d] (read back to where the declaration
    begins), or None when code[d] is not a declared name: a type word, `&`, `*`, `&&` or a template's `>` stands right
    before a declared name, and a use, a qualifier or a member access right after it is no declaration."""
    if d + 1 < len(code) and code[d + 1].text in (".", "->", "::"):
        return None
    prev = code[d - 1] if d else None
    if prev is None or not ((prev.kind == "ident" and prev.text not in _NOT_TYPES) or prev.text in _TYPE_MARKS):
        return None
    out = set()
    for k in range(d - 1, -1, -1):
        if code[k].text in _TYPE_ENDS:
            break
        if code[k].kind == "ident":
            out.add(code[k].text)
    return out


def _gd_declared(code, d):
    """The identifiers of the type a GDScript declaration gives the name at code[d] (`var name: Type`, a parameter
    `name: Type`, `for name: Type in`, a typed array `Array[Type]`), or None when code[d] is not so declared."""
    if d + 2 >= len(code) or code[d + 1].text != ":" or code[d + 2].kind != "ident":
        return None
    if not d or code[d - 1].text not in _GD_DECLARING:
        return None
    out, k = {code[d + 2].text}, d + 3
    if k < len(code) and code[k].text == "[":
        while k < len(code) and code[k].text != "]":
            if code[k].kind == "ident":
                out.add(code[k].text)
            k += 1
    return out


def _holder_written(code, i, holder, gd):
    """Whether the object a write of the member at code[i] goes through is the holder's: named as the holder itself (an
    autoload, a static), or declared in this file (the nearest declaration of its name before the write) with the holder
    in its type (by value, reference or pointer, or a container of it)."""
    o = _object(code, i)
    if o is None:
        return False
    name = code[o].text
    if name == holder:
        return True
    declared = _gd_declared if gd else _cpp_declared
    for d in range(o - 1, -1, -1):
        if code[d].kind == "ident" and code[d].text == name:
            types = declared(code, d)
            if types is not None:
                return holder in types
    return False


def _declared_problems(ctx, src, cfg, err):
    machines = {(m["enum"], m["declared"]) for m in cfg["machines"]}
    controllers = {(m["enum"], m["declared"]): m for m in cfg["machines"]}
    kinds = {(k["enum"], k["declared"]) for k in cfg["kinds"]}
    named = {m["enum"] for m in cfg["machines"]} | {k["enum"] for k in cfg["kinds"]}
    for name, line in _enums(src.code()):
        machine = controllers.get((name, src.path))
        if machine is not None and machine["file"] not in ctx.listed_set:
            err(ID, f"{src.path}:{line}", f"the machine {name} names its controller {machine['file']}, which is not there: "
                f"its one setter {machine['setter']} and its table are to be written in it")
            continue
        if (name.endswith(tuple(cfg["suffixes"])) or name in named) and (name, src.path) not in machines | kinds:
            err(ID, f"{src.path}:{line}", f"the enum {name} reads as a state: declare it in rules.json states, a machine (its "
                "controller's file, member, one setter and transition table) or a kind (why it is a value, never stepped)")


def _controller_problems(src, machine, spans, code, err):
    setter = next((s for s in spans if s[0] == machine["setter"]), None)
    if setter is None:
        err(ID, f"{src.path}:1", f"the controller of {machine['enum']} has no setter {machine['setter']}")
        return
    table = machine.get("table")
    if table is None:
        if not machine.get("why"):
            err(ID, f"{src.path}:{setter[1]}", f"the setter {machine['setter']} of {machine['enum']} reads no transition "
                "table, and its machine does not say why any state may follow any")
        return
    uses = [t.line for t in code if t.kind == "ident" and t.text == table]
    read = any(setter[1] <= line < setter[1] + setter[2] for line in uses)
    declared = any(not setter[1] <= line < setter[1] + setter[2] for line in uses)
    if not (read and declared):
        err(ID, f"{src.path}:{setter[1]}", f"the setter {machine['setter']} of {machine['enum']} reads no transition table "
            f"{table} declared beside it")


def check_states(ctx, src, err):
    cfg = ctx.rules["states"]
    _declared_problems(ctx, src, cfg, err)
    code = src.code()
    gd = src.lang == "gd"
    spans = None
    for machine in cfg["machines"]:
        own = src.path == machine["file"]
        if own or any(t.kind == "ident" and t.text == machine["member"] for t in code):
            spans = spans if spans is not None else _spans(src)
        else:
            continue
        if own:
            _controller_problems(src, machine, spans, code, err)
        for i in _writes(code, machine["member"]):
            line = code[i].line
            if own and _inside(spans, machine["setter"], line):
                continue
            names_enum = any(t.kind == "ident" and t.text == machine["enum"] for t in _statement(code, i, gd))
            if own or names_enum or _holder_written(code, i, machine["holder"], gd):
                err(ID, f"{src.path}:{line}", f"{machine['member']} ({machine['enum']}) is written outside its one setter "
                    f"{machine['setter']} ({machine['file']}): ask the setter, which keeps the transition table")
