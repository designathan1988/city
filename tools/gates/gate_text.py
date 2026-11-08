"""Visible text goes through tr(): every assignment to a Control's text properties and every call that sets visible
text in GDScript is given a translated value. A value is judged on its tokens and followed to where it comes from, over
every script (gd_index): a parameter makes its function a text helper, whose every call (an `emit` of a signal connected
to it included) is judged at the call site; a local or a member is judged at each assignment; a call is translated when
every function it may run returns only translated values; a read of a gated text property was judged where written."""
import re

import gd_index
from context import digest
from gd_syntax import access_path, receiver, split_args, unbound, value_of
from tokens import string_body

TEXT_PROPERTIES = {"text", "tooltip_text", "placeholder_text", "title"}
ASSIGN = {"=", "+="}
TEXT_CALLS = {  # method -> index of its visible-text argument
    "set_text": 0, "set_tooltip_text": 0, "set_placeholder": 0, "set_title": 0, "add_item": 0, "add_check_item": 0,
    "add_radio_check_item": 0, "add_separator": 0, "add_submenu_item": 0, "add_tab": 0, "add_text": 0,
    "append_text": 0, "add_icon_item": 1, "add_icon_check_item": 1, "add_icon_radio_check_item": 1,
    "set_item_text": 1, "set_item_tooltip": 1, "set_tab_title": 1, "set_tab_tooltip": 1,
}
TRANSLATE = {"tr", "tr_n", "atr", "atr_n"}
SERVER_LOOKUPS = {"translate", "translate_plural", "get_message", "get_plural_message"}
SHOWN_OWNER = "Formatters"  # the one owner of shown numbers, units and dates (tools/gates/owners.json)
NOT_CONTROLS = {"Dictionary", "Array", "Variant", "Object", "String", "StringName", "Callable", None}
VERBATIM = "verbatim_text"  # the owners.json concept whose function shows text that is not the game's own, as given
INPUTS = {"LineEdit", "TextEdit", "CodeEdit"}  # their `text` is what the player types (Godot translates only the placeholder)
_FORMAT_SPEC = re.compile(r"%[-+ 0#*]*\d*(?:\.\d+)?[a-zA-Z%]")
_ESCAPE = re.compile(r"\\(?:u[0-9a-fA-F]{4}|U[0-9a-fA-F]{6}|.)")  # an escape names a character, its letters are not text


def _top_level(toks):
    depth = 0
    for t in toks:
        x = t.text if t.kind == "punct" else None
        if x in ("(", "[", "{"):
            depth += 1
        elif x in (")", "]", "}"):
            depth -= 1
        elif depth == 0:
            yield t


def _letter_free(tok):
    body = _ESCAPE.sub("", _FORMAT_SPEC.sub("", string_body(tok)))
    return tok.kind == "string" and not re.search(r"[^\W\d_]", body)


def _branches(expr):
    """The values an expression shows: both sides of each `a if c else b`, never the condition."""
    top = list(_top_level(expr))
    cond = next((t for t in top if t.kind == "ident" and t.text == "if"), None)
    other = next((t for t in top if t.kind == "ident" and t.text == "else"), None)
    if cond is None or other is None:
        return [expr]
    return _branches(expr[:expr.index(cond)]) + _branches(expr[expr.index(other) + 1:])


def _call_head(expr):
    """Index of the called name when the expression starts with a call that makes shown text, reached through an access
    path: tr/tr_n/atr/atr_n (an object's own too, `node.tr`), any function of the shown-value owner, or a lookup of the
    engine's translation owner (`TranslationServer.translate`, `TranslationServer.get_translation_object(l).get_message`)."""
    k = 0
    while k < len(expr) and expr[k].kind == "ident":
        called = k + 1 < len(expr) and expr[k + 1].text == "("
        if called and (expr[k].text in TRANSLATE or k == 2 and expr[0].text == SHOWN_OWNER
                       or k and expr[0].text == "TranslationServer" and expr[k].text in SERVER_LOOKUPS):
            return k
        nxt = split_args(expr, k + 1)[1] + 1 if called else k + 1
        if nxt + 1 >= len(expr) or expr[nxt].text != ".":
            return None
        k = nxt + 1
    return None


def translated(expr, leaf=None):
    """The expression is a translation (tr/tr_n/atr/atr_n, TranslationServer.translate or a Formatters function, then
    only `%` formatting or method calls on the result), an empty string, or a literal without letters formatted with
    `%`; a concatenation with `+` is never translated. `leaf(operand)` judges an operand the tokens alone cannot."""
    if not expr:
        return True
    shown = _branches(expr)
    if len(shown) > 1:
        return all(translated(b, leaf) for b in shown)
    if any(t.text == "+" for t in _top_level(expr)):
        return False
    if expr[0].text == "(":  # `(value)`, then only `%` formatting or method calls on it
        close = split_args(expr, 0)[1]
        rest = expr[close + 1:]
        return (not rest or rest[0].text in ("%", ".")) and translated(expr[1:close], leaf)
    head = _call_head(expr)
    if head is not None:
        if head + 1 < len(expr) and expr[head + 1].text == "(":
            rest = expr[split_args(expr, head + 1)[1] + 1:]
            return not rest or rest[0].text in ("%", ".")
        return False
    if expr[0].kind == "string":
        rest = expr[1:]
        if not _letter_free(expr[0]) or not rest:
            return _letter_free(expr[0])
        specs = [m.group(0)[-1] for m in _FORMAT_SPEC.finditer(string_body(expr[0])) if m.group(0) != "%%"]
        args, close = split_args(rest, 1) if len(rest) > 1 and rest[1].text == "[" else ([rest[1:]], len(rest) - 1)
        args = args if close == len(rest) - 1 else [rest[1:]]
        return rest[0].text == "%" and len(specs) == len(args) and all(_shown_value(c, a, leaf) for c, a in zip(specs, args))
    return leaf is not None and leaf(expr)


def _shown_value(conversion, arg, leaf):
    """A value formatted as text (%s) is translated; one formatted as a number carries no string literal with letters."""
    if conversion == "s":
        return translated(arg, leaf)
    return not any(t.kind == "string" and not _letter_free(t) for t in arg)


def _sinks(s, ix, verbatim):
    """(line, value, function) of each text sink of a script: a text property assigned or `set`, a text argument of a
    Control's method. A LineEdit's (or TextEdit's) `text` is the player's own input, not a display; the declared owner of
    verbatim text (owners.json, VERBATIM: path and function) shows what it is given as given."""
    for toks, fn in s.lines:
        if fn is not None and (s.path, fn.name) == verbatim:
            continue
        for i, t in enumerate(toks):
            prev = toks[i - 1].text if i else ""
            nxt = toks[i + 1].text if i + 1 < len(toks) else ""
            args = split_args(toks, i + 1)[0] if nxt == "(" else []
            named = t.text == "set" and len(args) > 1 and args[0] and args[0][0].kind == "string"
            prop = string_body(args[0][0]) if named else t.text
            if t.kind != "ident" or prev == "func" or prop not in TEXT_PROPERTIES and t.text not in TEXT_CALLS:
                continue
            if named or nxt in ASSIGN and prev not in ("var", "const"):
                typed = prop == "text" and prev == "." and ix.type_of(receiver(toks, i - 1), s, fn) in INPUTS
                bare = prev != "." and not named and (i == 0 or toks[i - 1].kind == "punct")
                declared = t.text in s.members or fn is not None and t.text in fn.locals
                if not typed and (prev == "." or named or bare and not declared):
                    yield t.line, args[1] if named else value_of(toks[i + 2:]), fn
            elif TEXT_CALLS.get(t.text, len(args)) < len(args):
                yield t.line, args[TEXT_CALLS[t.text]], fn


class _Flow:
    """The text helpers {function id: {("text" or "callable", parameter index)}}, the translating functions and the
    findings {path: lines} of an index, each at its fixpoint."""

    def __init__(self, ix, verbatim=None):
        self.ix, self.verbatim = ix, verbatim
        self.helpers, self.translators, self.findings, self._members = {}, set(), {}, {}
        grew = True
        while grew:  # least fixpoint: a function joins once all its returns are translated by what is known so far
            more = {id(fn) for fns in ix.by_name.values() for fn in fns if fn.returns and id(fn) not in self.translators
                    and all(self._value(v, fn.script, fn, None, frozenset()) for _line, v in fn.returns)}
            self.translators |= more
            grew = bool(more)
        self._solve()

    def _value(self, v, s, fn, report, seen):
        """report: None judges only (a parameter is unknown); False also gathers helpers; True also records findings."""
        return v is not None and translated(v, lambda b: self._leaf(b, s, fn, report, seen))

    def _leaf(self, b, s, fn, report, seen):
        if len(b) == 1 and b[0].kind == "ident":
            return self._name(b[0].text, s, fn, report, seen)
        k = fn.param(b[0].text) if fn is not None and len(b) > 3 and b[1].text == "." and b[2].text == "call" else None
        close = split_args(b, 3)[1] if k is not None and b[3].text == "(" else None
        if close is not None and (close == len(b) - 1 or b[close + 1].text in ("%", ".")):
            self.helpers.setdefault(id(fn), set()).update([("callable", k)] if report is not None else [])
            return report is not None  # a Callable parameter called: its callers must give a translating function
        return self._translating_call(b, s, fn) or self._gated_read(b, s, fn) or self._member_read(b, s, fn, report, seen)

    def _member_read(self, b, s, fn, report, seen):
        """`x.member` of an object or class whose class is told: judged at each assignment to that member."""
        chain = access_path(b)
        cls = self.ix.type_of(b[:-2], s, fn) if chain and len(chain) > 1 and not chain[-1][1] else None
        return cls is not None and self._member(self.ix.classes.get(cls), chain[-1][0], report, seen)

    def _member(self, s, name, report, seen):
        """A member of script s or of a base: judged at each assignment to it."""
        owner = next((x for x in self.ix.chain_of(s) if name in x.members), None)
        return owner is not None and self._values(self._member_values(owner, name), report, seen, (id(owner), name))

    def _function_ref(self, arg, s, fn, seen=frozenset()):
        """A function given as a Callable (`f`, `obj.f`, `f.bind(...)`, a function of the shown-value owner, a local
        holding one, either side of `a if c else b`) that returns only translated values, or a Callable parameter of the
        caller, which defers to its own callers."""
        if len(_branches(arg)) > 1:
            return all(self._function_ref(b, s, fn, seen) for b in _branches(arg))
        arg = unbound(arg)
        own = fn.param(arg[0].text) if fn is not None and len(arg) == 1 else None
        if own is not None:
            self.helpers.setdefault(id(fn), set()).add(("callable", own))
            return True
        if fn is not None and len(arg) == 1 and arg[0].text in fn.locals and (id(fn), arg[0].text) not in seen:
            values = fn.assigns.get(arg[0].text, [])
            return bool(values) and all(v and self._function_ref(v, s, fn, seen | {(id(fn), arg[0].text)}) for _l, v, _f in values)
        chain = access_path(arg)
        if chain and len(chain) == 2 and chain[0] == (SHOWN_OWNER, False) and not chain[1][1]:
            return True
        targets = self.ix.targets("func", chain[-1][0], arg[:-2] or None, s, fn) if chain and not chain[-1][1] else []
        return bool(targets) and all(id(t) in self.translators for t in targets)

    def _name(self, name, s, fn, report, seen):
        """A parameter defers to the function's callers; a local or a member is judged at each of its assignments."""
        k = fn.param(name) if fn is not None else None
        if k is not None and report is None:
            return False
        if k is not None:
            self.helpers.setdefault(id(fn), set()).add(("text", k))
        if fn is not None and (k is not None or name in fn.locals):
            return self._values([(line, v, f, s) for line, v, f in fn.assigns.get(name, [])], report, seen, (id(fn), name))
        return self._member(s, name, report, seen)

    def _member_values(self, owner, name):
        """Every assignment to a member: in its script or a subclass, and through another object of its class."""
        key = (id(owner), name)
        if key not in self._members:
            own = [(line, v, fn, x) for x in self.ix.scripts if owner in self.ix.chain_of(x) for line, v, fn in x.assigns.get(name, [])]
            out = [(line, v, fn, x) for x, recv, _n, line, v, fn in self.ix.writes.get(name, [])
                   if self.ix.type_of(recv, x, fn) is None or self.ix.is_a(self.ix.type_of(recv, x, fn), owner)]
            self._members[key] = own + out
        return self._members[key]

    def _values(self, values, report, seen, key):
        """(line, value, function, script) of each assignment; with `report`, an untranslated one is a finding there."""
        if key in seen:
            return True
        if any(v is None for _line, v, _fn, _s in values):
            return False
        good = True
        for line, v, fn, at in values:
            ok = self._value(v, at, fn, report, seen | {key})
            if not ok and report:
                self._find(at, line)
            good &= ok
        return good if report is None else True

    def _translating_call(self, b, s, fn):
        j = next((j for j, t in enumerate(b) if t.text == "("), 0)
        recv = b[:j - 2] if j > 1 else None
        if j == 0 or b[j - 1].kind != "ident" or recv is not None and (b[j - 2].text != "." or not access_path(recv)):
            return False
        args, close = split_args(b, j)
        rest = b[close + 1:]
        targets = [] if rest and rest[0].text not in ("%", ".") else self.ix.targets("func", b[j - 1].text, recv, s, fn, args)
        return bool(targets) and all(id(t) in self.translators for t in targets)

    def _gated_read(self, expr, s, fn):
        """`x.text` (or `x.text.format(...)`) of an object of a known class that is no container: every write to a text
        property is judged by this rule, so its value was."""
        j = next((j for j in range(len(expr) - 3) if expr[j].text == "." and expr[j + 1].text == "format"
                  and expr[j + 2].text == "(" and split_args(expr, j + 2)[1] == len(expr) - 1), None)
        expr = expr[:j] if j is not None else expr
        if len(expr) < 3 or expr[-2].text != "." or expr[-1].text not in TEXT_PROPERTIES or not access_path(expr):
            return False
        return self.ix.type_of(expr[:-2], s, fn) not in NOT_CONTROLS

    def _find(self, s, line):
        self.findings.setdefault(s.path, set()).add(line)

    def _judge(self, s, line, v, fn, report):
        if not self._value(v, s, fn, report, frozenset()) and report:
            self._find(s, line)

    def _solve(self):
        sites = [(s, line, v, fn) for s in self.ix.scripts for line, v, fn in _sinks(s, self.ix, self.verbatim)]
        calls = [(s, line, args, fn, self.ix.targets(kind, name, recv, s, fn, args))
                 for s in self.ix.scripts for kind, name, line, args, recv, fn in s.calls]
        calls = [c for c in calls if c[-1]]
        for report in (False, True):
            size = -1
            while size != sum(map(len, self.helpers.values())):
                size = sum(map(len, self.helpers.values()))
                for s, line, v, fn in sites:
                    self._judge(s, line, v, fn, report)
                for s, line, args, fn, targets in calls:
                    for kind, k in sorted({h for t in targets for h in self.helpers.get(id(t), ()) if h[1] < len(args)}):
                        if kind == "text":
                            self._judge(s, line, args[k], fn, report)
                        elif not self._function_ref(args[k], s, fn) and report:
                            self._find(s, line)


def findings(ctx, rule):
    """{path: sorted lines} over every script in scope, cached by their content; each file the rule judges reports its own."""
    paths = sorted(p for p in ctx.files if p.endswith(".gd"))
    owner = next(((c["owner"], c["symbol"]) for c in ctx.owners["concepts"] if c["id"] == VERBATIM), None)

    def build():
        flow = _Flow(gd_index.Index([(p, ctx.source(p)) for p in paths]), owner)
        return {p: sorted(lines) for p, lines in flow.findings.items()}
    return ctx.lazy("text_findings", lambda: ctx.cache.get(digest("text_findings", rule, owner, ctx.cache.key_of(paths)), build))


def untranslated(ctx, src, rule, err):
    if src.lang != "gd":
        return
    for line in findings(ctx, rule).get(src.path, []):
        err(rule["id"], f"{src.path}:{line}", rule["message"])
