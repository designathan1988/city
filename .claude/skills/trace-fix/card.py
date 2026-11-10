"""The fix card of a cause, for the trace-fix skill: what the reading knew and the fixing needs, computed instead of found again.
For every problem under the cause: the function that holds it with its exact lines (read those, not the whole file), who
calls it and who reads the state it writes (what a change there can break), the tests that reach it, and whether the
change can reach the screen (then the photos of step 5 are due). Callers come from the repository's feature tracer (the tree on disk); and so that no call by name is missed, every other
place that writes the function's name, and every quoted piece the name may be built from, is listed to be checked.
"""
import os
import re
import subprocess
import sys

import store
import views
from store import OPEN, Refused, fail

INCLUDE = ("project/game", "project/ui", "project/render", "project/data", "project/tests", "native/src")
TESTS = "project/tests/"
# Where a change can alter what is drawn (CLAUDE.md, the project's structure).
SCREEN = ("native/src/render/", "native/src/sim/terrain/", "project/render/", "project/data/render/", "project/data/game/", "project/ui/")
FLOW = {"call", "string", "register"}
LISTED = 14  # callers or readers named per function; the rest are counted
PREFIX_MIN = 4  # a name's prefix shorter than this is a common word, not a piece of the name
GREP_TIMEOUT_S = 60


def grep(pattern, fixed):
    """(path, line, text) of every line of the tree, tracked or not, that matches: git's own search, so nothing ignored is read."""
    args = ["git", "-C", store.ROOT, "grep", "-n", "-I", "--untracked", "-F" if fixed else "-E", *(["-w"] if fixed else []), "-e", pattern]
    out = subprocess.run(args, capture_output=True, timeout=GREP_TIMEOUT_S).stdout.decode("utf-8", "replace")
    rows = []
    for row in out.splitlines():
        path, _, rest = row.partition(":")
        line, _, text = rest.partition(":")
        if line.isdigit():
            rows.append((path, int(line), text.strip()))
    return rows


def other_mentions(graph, unit, explained):
    """Every place that writes the function's name and is not the function itself nor a caller already on the card, and every text that is a
    piece a name may be built from at run time (a prefix of it up to an underscore, in quotes): no call by name is left out by silence."""
    node = graph.nodes[unit]
    own = {(p, n) for p, a, b in node.locations for n in range(a, b + 1)}
    whole = [(p, n, t) for p, n, t in grep(node.name, True) if (p, n) not in own and (p, n) not in explained]
    pieces = [node.name[:i + 1] for i, c in enumerate(node.name) if c == "_" and PREFIX_MIN <= i + 1 < len(node.name)]
    built = []
    for piece in pieces:
        built += [(p, n, t) for p, n, t in grep("[\"']" + re.escape(piece) + "[\"'%{]", False) if (p, n) not in own]
    return whole, sorted(set(built))


def graph_of(include=None):
    tools = os.path.join(store.ROOT, "tools", "analysis")
    if not os.path.isdir(os.path.join(tools, "ftrace")):
        raise Refused("this repository has no feature tracer (tools/analysis/ftrace)")
    sys.path.insert(0, tools)
    from ftrace.graph import Graph
    from ftrace.index import build_index
    return Graph(build_index(store.ROOT, cache_dir=os.path.join(store.ROOT, "dist", "cache", "ftrace"), include=list(include or INCLUDE)))


def reaching_tests(graph, unit):
    """Test files with a unit that makes this one run, through calls, upwards."""
    seen, todo, found = {unit}, [unit], set()
    while todo:
        for source, edge in graph.inn[todo.pop()].items():
            if edge.kinds & FLOW and source not in seen:
                seen.add(source)
                todo.append(source)
                if graph.nodes[source].path.startswith(TESTS):
                    found.add(graph.nodes[source].path)
    return found


def named(graph, ids):
    rows = [f"{graph.label(i)}" for i in ids[:LISTED]]
    return rows + ([f"... and {len(ids) - LISTED} more"] if len(ids) > LISTED else [])


def lines_of(state, graph, cause):
    problems = [p for p in state["problems"] if p["cause_id"] == cause["id"] and p["status"] == OPEN]
    out = [f"# Fix card of {cause['id']}: {cause['what']}", "", f"Owner: {cause['owner']}", f"Plan: {cause['plan']}", ""]
    tests, files, units = set(), set(), {}
    for p in problems:
        hit = graph.innermost(p["path"], p["line"])
        if hit:
            units.setdefault(hit[0], []).append(p)
        else:
            out.append(f"- {p['id']} {p['path']}:{p['line']}: the tracer has no unit there (a data file or a file it does not read): {p['what']}")
        files.add(p["path"])
    out.append(f"## Where to edit: {len(units)} function(s) holding {len(problems)} open problem(s). Read these line ranges.")
    for unit, held in units.items():
        node = graph.nodes[unit]
        out += ["", f"### {graph.label(unit)}, lines {node.start}-{node.end}"]
        out += [f"- {p['id']} line {p['line']}: {p['what']}" for p in held]
        callers = sorted(s for s, e in graph.inn[unit].items() if e.kinds & FLOW and not graph.nodes[s].path.startswith(TESTS))
        written = sorted(t for t, e in graph.out[unit].items() if "write" in e.kinds and graph.nodes[t].kind == "data")
        readers = sorted({r for d in written for r, e in graph.inn[d].items() if "read" in e.kinds and r != unit
                          and not graph.nodes[r].path.startswith(TESTS)})
        out.append(f"- called by {len(callers)}: " + "; ".join(named(graph, callers)) if callers else "- called by no unit the tracer resolves (an entry, or a call by name)")
        if written:
            out.append(f"- writes {len(written)}: " + "; ".join(named(graph, written)))
            out.append(f"- that state is read by {len(readers)}: " + "; ".join(named(graph, readers)))
        explained = {(e.where, e.line) for e in graph.inn[unit].values()}
        whole, built = other_mentions(graph, unit, explained)
        out.append(f"- its name is also written in {len(whole)} place(s) no caller above explains (a call by name, a table, a comment: check each):")
        out += [f"    {p}:{n}: {views.clip(t, 140)}" for p, n, t in whole]
        if built:
            out.append(f"- {len(built)} text(s) are a piece its name may be built from at run time (check what is joined to each):")
            out += [f"    {p}:{n}: {views.clip(t, 140)}" for p, n, t in built]
        tests |= reaching_tests(graph, unit)
    scenes = sorted(t[len("project/"):-3] + ".tscn" for t in tests if t.endswith("_test.gd") and os.path.isfile(os.path.join(store.ROOT, t[:-3] + ".tscn")))
    out += ["", f"## How to prove it: {len(scenes)} test scene(s) reach these functions"]
    out += [f"- python tools/tasks/cli.py run fast {s}" for s in scenes] or ["- no test scene reaches them by calls: the fix needs its own failing test first"]
    if any(f.startswith(SCREEN) for f in files):
        out += ["", "## It can reach the screen: after the fix, photograph the game as it is and analyse every photo (step 5 of the skill)."]
    return out


def cmd_card(args):
    state = store.load()
    try:
        cause = store.cause_of(state, args.id)
        text = lines_of(state, graph_of(args.include), cause)
    except Refused as refusal:
        fail(str(refusal))
    path = os.path.join(store.trace_folder(state["slug"]), f"card-{cause['id']}.md")
    with open(path + ".tmp", "w", encoding="utf-8", newline="\n") as f:
        f.write("\n".join(text) + "\n")
    os.replace(path + ".tmp", path)
    out = views.Out()
    for line in text:
        if not out.add(views.clip(line, 700) if line else ""):
            break
    out.flush(f"The whole card is in {path.replace(chr(92), '/')}: read it with the Read tool.")
    if not out.full:
        print(f"(also written to {path.replace(chr(92), '/')})")
