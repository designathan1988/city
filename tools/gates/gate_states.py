"""STATE_CONTROLLER: every state with its controller. An enum whose name ends in one of rules.json states.suffixes (a
State, a Mode, a Phase, a Speed, a Tool), or that states declares, is declared there as one of two things: a machine (the
file of its controller, the member that holds it, the one setter that writes it, and the transition table the setter
reads, or why any state may follow any) or a kind (a value chosen, never stepped, with why). A machine's member is written
only inside its setter: an assignment to it (`member =`, `member.store(`, `member.exchange(`) anywhere else is refused,
in the controller's own file whatever the value, elsewhere when the statement names the machine's enum. A declaration of
a variable that bears the member's name is no write of it."""
import json

import cpp_syntax
import gd_syntax

ID = "STATE_CONTROLLER"
_ATOMIC_WRITES = {"store", "exchange"}
_DECLARING = {"var", "const", "static", "constexpr", "auto", ">", "*", "&", "&&"}
_EXTENSIONS = (".cpp", ".h", ".gd")


def applies(ctx, path):
    paths = ctx.rules["states"]["paths"]
    return path.endswith(_EXTENSIONS) and any(path.startswith(p.rstrip("/") + "/") for p in paths)


def context(ctx):
    """The declarations a verdict reads besides the file: the machines, the kinds and the suffixes."""
    return json.dumps(ctx.rules["states"], sort_keys=True)


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


def _declared_problems(src, cfg, err):
    machines = {(m["enum"], m["declared"]) for m in cfg["machines"]}
    kinds = {(k["enum"], k["declared"]) for k in cfg["kinds"]}
    named = {m["enum"] for m in cfg["machines"]} | {k["enum"] for k in cfg["kinds"]}
    for name, line in _enums(src.code()):
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
    _declared_problems(src, cfg, err)
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
            if own or any(t.kind == "ident" and t.text == machine["enum"] for t in _statement(code, i, gd)):
                err(ID, f"{src.path}:{line}", f"{machine['member']} ({machine['enum']}) is written outside its one setter "
                    f"{machine['setter']} ({machine['file']}): ask the setter, which keeps the transition table")
