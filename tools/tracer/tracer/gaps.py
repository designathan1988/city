"""What the trace cannot follow, found without being told where to look: every call the code read writes that no
definition answers, with why; every function nothing in the code read calls, with what may run it instead (the
platform, a text naming it) or that nothing does; and, for a start, what makes it run: the roots its callers come
from, each with the kind of trigger and the path from it, each step saying whether it calls, registers a handler or
fires one. Nothing here names a program, a library or a platform.
"""
from __future__ import annotations

from collections import Counter

from .vocab import FIRES, FUNCTION, HANDED, MODULE

# Why a call is not followed.
PLATFORM_CALL = "the platform's"             # it calls code not read (its object or name is the platform's)
UNKNOWN_CALL = "nothing known of what is called"   # neither the scopes, the types, the flow nor a name says what runs
TEXT_CALL = "a text names what runs, no definition has that name"
HANDS_CALL = "hands a function to code not read (a handler registered: it runs later)"
FIRES_CALL = "code not read runs what the object keeps (followed: fires)"
# What may run a function no code read calls.
BY_PLATFORM = "run by the platform"          # a member of a class below a type not defined here, or a file's code
BY_TEXT_NAME = "run by its name in a text"   # its name is written as a text somewhere: looked up as the program runs
EXPORTED = "offered to other code"           # imported by another file, or a public member others may use
NO_CALLER = "no caller found"                # nothing in the code read reaches it: dead, or a flow not seen


def unresolved(result) -> list:
    """Every call no definition answers: (unit, file, line, callee as written, why)."""
    return list(result.unresolved or ())


def uncalled(result) -> list:
    """Every function written with a body that no link reaches: (function, why it may still run)."""
    p = result.program
    texts = result.texts or set()
    return [(d.id, why_uncalled(result, d, texts)) for d in p.defs
            if d.kind == FUNCTION and d.body and not result.into.get(d.id)]


def why_uncalled(result, d, texts) -> str:
    p = result.program
    if d.name and d.name in texts:
        return BY_TEXT_NAME
    owner = p.defs[d.owner] if d.owner >= 0 else None
    if owner is not None and owner.kind == MODULE and not p.is_class(owner.id):
        return EXPORTED if d.name and d.name in (result.imported or set()) else NO_CALLER
    if owner is not None and p.is_class(owner.id):
        if owner.id in (result.open_classes or set()):
            return BY_PLATFORM
        return EXPORTED if d.name and not d.name.startswith("_") else NO_CALLER
    return NO_CALLER


def path_text(result, path) -> str:
    """A path from a root to the start: each step and how the next is reached (a handler registered runs when the
    code it was handed to fires it)."""
    p = result.program
    words = [p.label(path[0][0])]
    for (_, link), (at, _) in zip(path, path[1:]):   # the link kept with a step leads from it to the next
        arrow = " =registers=> " if link is not None and link.kind == HANDED else \
            " =fires=> " if link is not None and link.kind == FIRES else " -> "
        words.append(arrow + p.label(at))
    return "".join(words)


def triggers(result, steps: list) -> list:
    """What makes a definition run, read off its backward trace (which keeps only the callers that reach it as it is
    called): the steps nothing reaches further, each as (root, why, [(definition, link to the next)] from the root
    to the start)."""
    p = result.program
    texts = result.texts or set()
    roots, stack = [], []
    for i, step in enumerate(steps):
        del stack[step.depth:]
        stack.append((step.at, step.link))
        last = i + 1 == len(steps) or steps[i + 1].depth <= step.depth
        if step.again or step.leaf or not last:
            continue
        d = p.defs[step.at]
        why = why_uncalled(result, d, texts) if d.kind == FUNCTION else BY_PLATFORM
        roots.append((step.at, why, list(reversed(stack))))
    return roots


def trigger_text(result, steps: list, limit: int = 40) -> str:
    p = result.program
    lines = ["When it runs (the roots its callers start from, and what runs them):"]
    for root, why, path in triggers(result, steps)[:limit]:
        lines.append(f"  {p.where(root)}  [{why}]  {path_text(result, path)}")
    return "\n".join(lines)


def summary(result) -> dict:
    """The gaps of the whole program, counted, with where."""
    from .links import data as links_data
    p = result.program
    calls = unresolved(result)
    funcs = uncalled(result)
    return {"calls not followed": dict(Counter(why for *_, why in calls)),
            "functions no code read calls": dict(Counter(why for _, why in funcs)),
            "of them, no caller found": [p.where(d) + "  " + p.label(d) for d, why in funcs if why == NO_CALLER],
            "calls of which nothing is known": [f"{p.files[f].display}:{line}  {shown}  (in {p.label(u)})"
                                                for u, f, line, shown, why in calls if why == UNKNOWN_CALL],
            "links that lead nowhere": links_data(result) if result.cases is not None else []}


def text_report(result, limit: int = 200) -> str:
    from .links import text_report as links_text
    s = summary(result)
    lines = ["Calls the trace does not follow, by why:"]
    lines += [f"  {n:6d}  {why}" for why, n in sorted(s["calls not followed"].items(), key=lambda kv: -kv[1])]
    lines.append("Functions no code read calls, by what may run them:")
    lines += [f"  {n:6d}  {why}" for why, n in sorted(s["functions no code read calls"].items(), key=lambda kv: -kv[1])]
    for title in ("of them, no caller found", "calls of which nothing is known"):
        rows = s[title]
        lines.append(f"{title.capitalize()} ({len(rows)}):")
        lines += ["  " + row for row in rows[:limit]]
        if len(rows) > limit:
            lines.append(f"  ... {len(rows) - limit} more (--json for all)")
    if result.cases is not None:         # the links that lead nowhere, by kind
        lines.append(links_text(result))
    if result.cut:
        lines.append(f"The analysis stopped at the ceiling ({result.cut}): what is above is partial.")
    return "\n".join(lines)
