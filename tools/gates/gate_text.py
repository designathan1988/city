"""Visible text goes through tr(): every assignment to a Control's text properties and every call that sets visible
text in GDScript must be given a translated expression, judged on the expression's tokens. A call of the owner of shown
values (owners.json, SHOWN_CONCEPT: the class that formats the numbers, units and dates the interface shows, its words
through TranslationServer) is translated text as a tr() call is."""
import re

import gd_syntax
from tokens import string_body

SHOWN_CONCEPT = "shown_values"
TEXT_PROPERTIES = {"text", "tooltip_text", "placeholder_text", "title"}
ASSIGN = {"=", "+="}
TEXT_CALLS = {  # method -> index of its visible-text argument
    "set_text": 0, "set_tooltip_text": 0, "set_placeholder": 0, "set_title": 0, "add_item": 0, "add_check_item": 0,
    "add_radio_check_item": 0, "add_separator": 0, "add_submenu_item": 0, "add_tab": 0, "add_text": 0,
    "append_text": 0, "add_icon_item": 1, "add_icon_check_item": 1, "add_icon_radio_check_item": 1,
    "set_item_text": 1, "set_item_tooltip": 1, "set_tab_title": 1, "set_tab_tooltip": 1,
}
TRANSLATE = {"tr", "tr_n", "atr", "atr_n"}
_FORMAT_SPEC = re.compile(r"%[-+ 0#*]*\d*(?:\.\d+)?[a-zA-Z%]")


def _split_args(toks, open_idx):
    """Argument token lists of the call whose '(' is toks[open_idx], and the index of its ')'."""
    args, cur, depth = [], [], 0
    for k in range(open_idx + 1, len(toks)):
        x = toks[k].text if toks[k].kind == "punct" else None
        if x in ("(", "[", "{"):
            depth += 1
        elif x in (")", "]", "}"):
            if depth == 0:
                if cur:
                    args.append(cur)
                return args, k
            depth -= 1
        elif x == "," and depth == 0:
            args.append(cur)
            cur = []
            continue
        cur.append(toks[k])
    if cur:
        args.append(cur)
    return args, len(toks) - 1


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
    return tok.kind == "string" and not re.search(r"[^\W\d_]", _FORMAT_SPEC.sub("", string_body(tok)))


def shown_owner(ctx):
    """The class_name of the owner of shown values (owners.json), or None when the registry or the tree has none."""
    concept = next((c for c in ctx.owners["concepts"] if c["id"] == SHOWN_CONCEPT), None)
    if concept is None or concept["owner"] not in ctx.listed_set:
        return None
    return ctx.facts(concept["owner"]).get("class_name")


def _owner_call(expr, owner):
    """`Owner.function(` at the start of the expression."""
    return (owner is not None and len(expr) > 3 and expr[0].text == owner and expr[1].text == "."
            and expr[2].kind == "ident" and expr[3].text == "(")


def translated(expr, owner=None):
    """The expression is a translation (tr/tr_n/atr/atr_n, TranslationServer.translate or a function of `owner`, the
    owner of shown values, then only `%` formatting or method calls on the result), an empty string, or a literal
    without letters formatted with `%`; a concatenation with `+` is never translated."""
    if not expr:
        return True
    top = list(_top_level(expr))
    cond = next((t for t in top if t.kind == "ident" and t.text == "if"), None)
    other = next((t for t in top if t.kind == "ident" and t.text == "else"), None)
    if cond is not None and other is not None:  # `a if c else b`: both values are shown, the condition is not
        i, j = expr.index(cond), expr.index(other)
        return translated(expr[:i], owner) and translated(expr[j + 1:], owner)
    if any(t.text == "+" for t in top):
        return False
    head = 0
    if (expr[0].text == "TranslationServer" and len(expr) > 2 and expr[1].text == "." and expr[2].text == "translate"
            or _owner_call(expr, owner)):
        head = 2
    if expr[head].kind == "ident" and (expr[head].text in TRANSLATE or head == 2):
        if head + 1 < len(expr) and expr[head + 1].text == "(":
            _args, close = _split_args(expr, head + 1)
            rest = expr[close + 1:]
            return not rest or rest[0].text in ("%", ".")
        return False
    if expr[0].kind == "string" and _letter_free(expr[0]):
        rest = expr[1:]
        if not rest:
            return True
        specs = [m.group(0)[-1] for m in _FORMAT_SPEC.finditer(string_body(expr[0])) if m.group(0) != "%%"]
        args = _format_args(rest[1:])
        return rest[0].text == "%" and len(specs) == len(args) and all(_shown_value(c, a, owner) for c, a in zip(specs, args))
    return False


def _format_args(toks):
    """The values a `%` format receives: the elements of an array literal, or the one operand."""
    if toks and toks[0].text == "[":
        args, close = _split_args(toks, 0)
        if close == len(toks) - 1:
            return args
    return [toks]


def _shown_value(conversion, arg, owner):
    """A value formatted as text (%s) is translated; one formatted as a number carries no string literal with letters."""
    if conversion == "s":
        return translated(arg, owner)
    return not any(t.kind == "string" and not _letter_free(t) for t in arg)


def _local_names(lines):
    return {d.name for d in gd_syntax.declarations(lines)}


def untranslated(ctx, src, rule, err):
    if src.lang != "gd":
        return
    lines = gd_syntax.logical_lines(src)
    declared = _local_names(lines)
    owner = shown_owner(ctx)
    for ln in lines:
        toks = ln.toks
        for i, t in enumerate(toks):
            if t.kind != "ident":
                continue
            prev = toks[i - 1].text if i else ""
            nxt = toks[i + 1].text if i + 1 < len(toks) else ""
            if t.text in TEXT_PROPERTIES and nxt in ASSIGN and prev not in ("var", "const"):
                bare = prev != "." and (i == 0 or toks[i - 1].kind == "punct")
                if (prev == "." or bare and t.text not in declared) and not translated(toks[i + 2:], owner):
                    err(rule["id"], f"{src.path}:{t.line}", rule["message"])
            elif t.text in TEXT_CALLS and nxt == "(" and prev not in ("func",):
                args, _close = _split_args(toks, i + 1)
                index = TEXT_CALLS[t.text]
                if index < len(args) and not translated(args[index], owner):
                    err(rule["id"], f"{src.path}:{t.line}", rule["message"])
