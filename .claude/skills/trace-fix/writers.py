"""Every place that writes a piece of state, for the trace-fix skill: how a behaviour or a picture is judged from the code.
A rule of the game's world (rules.json: two bodies never stand in one another, nothing floats, nothing vanishes in sight, ...) limits
some state (a position, a height, a visibility); every function that writes that state must keep the rule, and one that does not is
a cause. For the core's C and C++ the writers and readers are the compiler's own (the architecture analyser's reading of the code on
disk: every access resolved, through references, templates and lambdas); for scripts and other languages they come from the feature tracer; and every
other line of the tree that assigns to a member of that name is listed, so no writer is left out. Each goes on the flow with the rule.
"""
import json
import os
import re

import store
import views
from card import graph_of, grep
from operations import do_flow
from route import fresh_slices
from store import Refused, fail

COMPILED = "native/src/"  # what the compiler reads (CLAUDE.md, the project's structure): its members are asked of the analyser
# An assignment to a member through an object: `.name =`, `->name +=`, `.name++` (never a comparison).
WRITE = r"(\.|->){name}[[:space:]]*(=[^=]|\+=|-=|\*=|/=|\+\+|--)"


def compiled(specs):
    """(member, writers, readers) of the members the compiler knows by these names; writers and readers as (name, file, line, last line)."""
    slices = fresh_slices()
    if slices is None:
        return []
    with open(slices, encoding="utf-8") as f:
        data = json.load(f)
    table, out = data["functions"], []
    for member, uses in data.get("members", {}).items():
        if any(member == spec or member.endswith("::" + spec) for spec in specs):
            out.append((member, [tuple(table[i]) for i in uses["w"]], [tuple(table[i]) for i in uses["r"]]))
    return out


def scripted(graph, specs):
    """Functions that write a variable of these names in code the compiler does not read (scripts, other languages), by the tracer:
    (variable label, [(name, file, line, last line)])."""
    out = []
    for spec in specs:
        for d in graph.find(spec):
            node = graph.nodes[d]
            if node.kind != "data" or node.path.startswith(COMPILED):
                continue
            found = [(graph.label(s).split(" (")[0], graph.nodes[s].path, graph.nodes[s].start, graph.nodes[s].end)
                     for s, e in graph.inn[d].items() if "write" in e.kinds and graph.nodes[s].kind != "data"]
            out.append((graph.label(d), sorted(found, key=lambda w: (w[1], w[2]))))
    return out


def cmd_writers(args):
    """Puts every writer of a piece of state on the flow of the active trace, with the rule they must keep."""
    state = store.load()
    try:
        members = compiled(args.state)
        scripts = scripted(graph_of(args.include), args.state) if args.include or not members else []
    except Refused as refusal:
        fail(str(refusal))
    if not members and not scripts:
        fail(f"no state named {', '.join(args.state)}: give it as the code names it (`member`, `Class::member`)")
    out = views.Out()
    out.add(f"RULE: {args.rule}")
    spans, on_flow = set(), []
    for label, found, readers in [*members, *[(label, found, None) for label, found in scripts]]:
        source = "the compiler" if readers is not None else "the tracer"
        out.add(f"STATE {label}: {len(found)} writer(s)" + (f", {len(readers)} reader(s)" if readers is not None else "") + f" (by {source}).")
        for name, file, line, end in found:
            spans.update((file, n) for n in range(line, end + 1))
            on_flow.append((file, f"{name} (lines {line}-{end}) writes {label}; rule: {args.rule}"))
            out.add(f"  writes it: {name}, {file}:{line}-{end}")
        for name, file, line, end in (readers or [])[:views.FOLDER_ROWS]:
            out.add(f"  reads it:  {name}, {file}:{line}-{end}")
    names = sorted({label.split(" (")[0].rsplit("::", 1)[-1].rsplit(".", 1)[-1] for label, _, _ in [*members, *[(a, b, None) for a, b in scripts]]})
    loose = [(p, n, t) for name in names for p, n, t in grep(WRITE.format(name=re.escape(name)), False) if (p, n) not in spans]
    out.add(f"{len(loose)} other line(s) assign to a member of that name outside the writers above (another class's member of the same "
            "name, a script, a macro): check each:")
    for path, line, text in loose:
        on_flow.append((path, f"assigns to .{'/'.join(names)} at line {line}; rule: {args.rule}"))
        out.add(f"  {path}:{line}: {views.clip(text, 140)}")
    if not args.dry:
        for path, why in on_flow:
            if os.path.isfile(os.path.join(store.ROOT, path)) and not store.find_file(state, path):
                do_flow(state, [path], why)
        store.save(state)
        out.add("Their files are on the flow: `next` gives them to read. A writer that does not keep the rule is a problem; the rule then "
                "becomes a check of the core that fails while any writer breaks it.")
    out.flush("More lines than fit: every writer's file is on the flow.")
