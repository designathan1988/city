"""The flow of a problem found by script, for the trace-fix skill: the repository's feature tracer (tools/analysis/ftrace)
is asked what triggers a seed, what it runs and writes and who reads that, and the answer becomes the flow of the open
trace: every file involved, once, in the order the code runs, each with why it is there and the functions of it that
are on the flow. Where a seed is C or C++, what it runs comes from the architecture analyser (tools/analysis/arch), which reads the
compiler's own tree of the code on disk: calls through base classes, templates, lambdas and jobs are on the route.
"""
import json
import os
import subprocess
import sys
import tempfile
from collections import OrderedDict

import store
import views
from operations import do_flow
from store import Refused, fail

# The code the player runs, as the tracer is given it (CLAUDE.md, the project's structure): the tests are left out, or
# every test that calls a command would stand as an entry of its flow. `--include project/tests` brings them in.
INCLUDE = ("project/game", "project/ui", "project/render", "project/data", "native/src")
# Entry chains shown and followed: the shortest ones say how the seed is reached; the rest repeat them through other callers.
ENTRY_CHAINS = 12
UNITS_KEPT = 40  # functions of one file remembered for the reading batches
NATIVE = "native/src"  # the code the compiler reads (CLAUDE.md, the project's structure)
SCAN_TIMEOUT_S = 1200  # one scan of the working tree by the architecture analyser; its own task has 1800


def fresh_slices():
    """The architecture analyser's published reading of the code on disk (dist/arch/worktree/slices.json), made again when a source is newer
    than it; None where the repository has no analyser."""
    arch = os.path.join(store.ROOT, "tools", "analysis", "arch.py")
    if not os.path.isfile(arch):
        return None
    slices = os.path.join(store.ROOT, "dist", "arch", "worktree", "slices.json")
    newest = max((os.path.getmtime(os.path.join(base, n)) for base, _, names in os.walk(os.path.join(store.ROOT, NATIVE)) for n in names), default=0.0)
    if not os.path.isfile(slices) or os.path.getmtime(slices) < newest:
        done = subprocess.run([sys.executable, arch, "scan", "--worktree"], cwd=store.ROOT, capture_output=True, timeout=SCAN_TIMEOUT_S)
        if done.returncode != 0:
            raise Refused("the architecture analyser could not scan the working tree: " + done.stderr.decode("utf-8", "replace")[-600:])
    return slices


def compiler_route(graph, seeds):
    """For the seeds the compiler knows (C and C++), what each runs, in program order, as the architecture analyser reads it from the compiler's own
    tree: calls through base classes, templates, lambdas and jobs included, which no lookup by name can promise. The analyser's scan of the working
    tree is made again when a source is newer than it. Returns [(path, [(line, last line, name)])], or [] where there is no analyser or no such seed."""
    arch = os.path.join(store.ROOT, "tools", "analysis", "arch.py")
    native = [s for s in seeds if graph.nodes[s].path.startswith(NATIVE + "/") and graph.nodes[s].kind == "callable"]
    if not native or not os.path.isfile(arch):
        return []
    fresh_slices()
    files = OrderedDict()
    for seed in native:
        node = graph.nodes[seed]
        out = os.path.join(tempfile.gettempdir(), f"trace_fix_route_{os.getpid()}.json")
        done = subprocess.run([sys.executable, arch, "trace", f"{node.path}:{node.start}", "--worktree", "--json", out], cwd=store.ROOT,
                              capture_output=True, timeout=SCAN_TIMEOUT_S)
        if done.returncode != 0 or not os.path.isfile(out):
            continue  # a function the compiler's tree does not hold as the tracer names it (a declaration, a macro): the tracer's own route stands
        with open(out, encoding="utf-8") as f:
            route = json.load(f)
        os.remove(out)
        for group in route.get("below", []):
            files.setdefault(group["file"], []).extend((fn[1], fn[2], fn[0]) for fn in group["functions"])
    return list(files.items())


def run_tracer(query, seeds, include):
    tools = os.path.join(store.ROOT, "tools", "analysis")
    if not os.path.isdir(os.path.join(tools, "ftrace")):
        raise Refused("this repository has no feature tracer (tools/analysis/ftrace): put the files on the flow with `flow`")
    sys.path.insert(0, tools)
    from ftrace.graph import Graph
    from ftrace.index import build_index
    from ftrace.seeds import select_by_query, select_by_specs
    from ftrace.trace import trace
    index = build_index(store.ROOT, cache_dir=os.path.join(store.ROOT, "dist", "cache", "ftrace"), include=list(include))
    graph = Graph(index)
    picked = select_by_specs(graph, seeds) if seeds else select_by_query(graph, query)
    if not picked:
        raise Refused("no unit matches: give `--seed Class::method`, `--seed path/file` or `--seed file:line`, or other words")
    return graph, trace(graph, picked)


def steps(graph, result, deep):
    """(node, role, what leads to it) in the order the code runs: the way down from the entries to the seeds, the seeds,
    what they run, the infrastructure they use, the state they write, and who picks that state up later."""
    out, seen = [], set()

    def add(node, role, by=""):
        if node not in seen:
            seen.add(node)
            out.append((node, role, by))

    chains = sorted((result.chain_up(e) for e in result.entries), key=lambda c: (len(c), c))[:ENTRY_CHAINS]
    for chain in chains:
        for at, node in enumerate(chain[:-1]):
            add(node, "entry" if at == 0 else "upstream", f"calls {graph.label(chain[at + 1])}")
    for node in result.seeds:
        add(node, "seed")
    for node in sorted(result.feature - set(result.seeds), key=lambda n: (len(result.chain_down(n)), n)):
        previous, kind = result.down_prev[node]
        add(node, "runs", f"{kind} from {graph.label(previous)}")
    for node in sorted(result.shared):
        add(node, "infrastructure it uses", f"{result.prevalence.get(node, 0)} callers")
    for node in sorted(result.mutated):
        add(node, "state it writes")
    for node in result.consumers:
        chain = result.consumer_up[node]
        for at, link in enumerate(chain):
            last = at == len(chain) - 1
            add(link, "reads what it writes" if last else "runs the reader", "" if last else f"calls {graph.label(chain[at + 1])}")
    if deep:
        for node in sorted(result.executed):
            add(node, "runs inside its calls")
    return out


def files_of(graph, found):
    """path -> [(line, last line, name, role, by)], paths in the order their first unit runs."""
    out = OrderedDict()
    for node, role, by in found:
        unit = graph.nodes[node]
        name = unit.name if unit.kind != "file" else "(the file)"
        out.setdefault(unit.path, []).append((unit.start, unit.end, "::".join(unit.scope + (name,)), role, by))
    return out


def cmd_trace(args):
    """Asks the tracer and puts the route on the flow of the active trace (or only shows it, with --dry)."""
    if not args.query and not args.seed:
        fail("say what to trace: --query \"words of the symptom\" or --seed Class::method | path/file | file:line")
    state = store.load()
    try:
        graph, result = run_tracer(args.query, args.seed, args.include or INCLUDE)
    except Refused as refusal:
        fail(str(refusal))
    found = steps(graph, result, args.deep)
    first = next((i for i, step in enumerate(found) if step[1] == "seed"), len(found))
    files = files_of(graph, found[:first])  # what leads to the seeds, then what the compiler says they run, then the rest of the tracer's answer
    try:
        compiled = compiler_route(graph, result.seeds) if not args.include else []
    except Refused as refusal:
        fail(str(refusal))
    for path, functions in compiled:
        files.setdefault(path, []).extend((line, end, name, "runs (the compiler's route)", "") for line, end, name in functions)
    for path, units in files_of(graph, found[first:]).items():
        files.setdefault(path, []).extend(units)
    out = views.Out()
    out.add(f"ROUTE of {'seeds ' + ', '.join(args.seed) if args.seed else 'query ' + repr(args.query)}: "
            f"{len(result.seeds)} seed(s), {len(result.entries)} entry point(s), {len(result.feature)} unit(s) it runs, "
            f"{len(result.shared)} of infrastructure, {len(result.mutated)} state written, {len(result.consumers)} consumer(s), "
            f"{len(result.executed)} more inside its calls by name{'' if args.deep else ' (--deep adds them)'}; "
            f"the compiler's route of the C++ seeds adds {len(compiled)} file(s) in program order.")
    out.add("SEEDS: " + "; ".join(graph.label(s) for s in result.seeds[:8]) + (" ..." if len(result.seeds) > 8 else ""))
    added = shown = 0
    for path, units in files.items():
        if not os.path.isfile(os.path.join(store.ROOT, path)):
            continue
        line, _, name, role, by = units[0]
        why = f"{role}: {name} (line {line})" + (f", {by}" if by else "") + (f"; and {len(units) - 1} more of it on the route" if len(units) > 1 else "")
        if not args.dry:
            try:
                new = do_flow(state, [path], why)
            except Refused as refusal:
                fail(str(refusal))
            entry = store.find_file(state, path)
            entry["units"] = sorted([u[0], u[1], u[2], u[3]] for u in units)[:UNITS_KEPT]
            added += len(new)
        shown += 1
        out.add(f"  {shown}. {path} | {views.clip(why, 150)}")
    if args.dry:
        out.add(f"{len(files)} file(s) on the route. Nothing was recorded (--dry).")
    else:
        state["route"] = {"query": args.query or "", "seeds": [graph.label(s) for s in result.seeds][:20], "files": len(files)}
        store.save(state)
        out.add(f"{added} file(s) added to the flow in this order ({len(files) - added} were on it already). `next` gives the reading "
                "batches along the route; what the reading shows beyond it goes on with `flow`.")
    out.flush("The route is longer than fits; every file of it is on the flow: `status` shows it by folder.")
