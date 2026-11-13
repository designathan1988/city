"""The tracer's command: reads the code under the roots and shows, from a start, what runs after it and what reaches
it.

    python trace.py <start> [--root DIR]... [--depth N] [--back N] [--exact] [--guesses near|deep|none]
                    [--handed inline|every] [--among N] [--state] [--fresh] [--json] [--out FILE]
    python trace.py --gaps [--root DIR]... [--json]
    python trace.py [<start>] --cases [NAME] [--range LOW:HIGH] [--conditions] [--root DIR]... [--json]
    python trace.py [<start>] --wrong [--range LOW:HIGH] [--root DIR]... [--json]
    python trace.py --list [--root DIR]...
    python trace.py --evaluate "A;B*3" [--given NAME=VALUE]... [--situation FILE] [--repeat N] [--why PATH:LINE]
                    [--why-not PATH:LINE] [--root DIR]... [--json] [--html FILE]
    python trace.py --broken | --feature POINT[,POINT] | --cost ROOT [--root DIR]... [--json] [--html FILE]
    python trace.py --compare MAP MAP [--shared NAME=VALUE]... [--sweep NAME:LOW:HIGH:N] [--plane C:I,C:I] [--html F]
    python trace.py --versions OLD_ROOT --evaluate START [--given NAME=VALUE]... --root NEW_ROOT [--html FILE]

A start is a label (`Shop.open`), the end of one (`open`) or `path:line`. Exit 1 when the roots hold no code the
tracer reads or the start names nothing.
"""
from __future__ import annotations

import argparse
import json
import sys
import time
from fractions import Fraction
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))

from tracer import AMONG, analyse, beliefs, compare, cost, decide, evaluate, feature, index, invariants, limits, links  # noqa: E402,E501
from tracer import situations, views  # noqa: E402
from tracer.gaps import summary, text_report, trigger_text  # noqa: E402
from tracer.report import as_data, as_text, listing, state_data, state_text  # noqa: E402
from tracer.vocab import DEEP, EVERY, EXACT, INLINE, NEAR, NONE, RUNNING  # noqa: E402
from tracer.walk import backward, find, forward  # noqa: E402

FORWARD, BACKWARD = "What runs from it:", "What reaches it:"


def arguments(argv):
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("start", nargs="?", help="a label, the end of one, or path:line")
    parser.add_argument("--root", action="append", help="a folder or a file to read (default: the current folder)")
    parser.add_argument("--depth", type=int, help="how far forward (default: to the end)")
    parser.add_argument("--back", type=int, help="how far backward (default: to the end)")
    parser.add_argument("--exact", action="store_true", help="leave out what was found by a name or a text alone")
    parser.add_argument("--guesses", choices=(NEAR, DEEP, NONE), default=NEAR,
                        help="what was found by name alone: followed one step (default), on, or not at all")
    parser.add_argument("--handed", choices=(INLINE, EVERY), default=INLINE,
                        help="functions handed to code not read: gone into where written there (default), or all")
    parser.add_argument("--among", type=int, default=AMONG,
                        help=f"a call found by name among more functions than this is ambiguous (default {AMONG})")
    parser.add_argument("--state", action="store_true",
                        help="also the state each step reads, and who reads the state the trace writes")
    parser.add_argument("--fresh", action="store_true", help="analyse again, not reading a saved analysis")
    parser.add_argument("--gaps", action="store_true",
                        help="what the trace cannot follow in the whole program, why, and where")
    parser.add_argument("--json", action="store_true", help="the trace as data")
    parser.add_argument("--out", help="write the trace to this file")
    parser.add_argument("--list", action="store_true", help="every definition code is written in")
    parser.add_argument("--conditions", action="store_true",
                        help="when each effect the trace runs happens and what it writes, in the start's inputs")
    parser.add_argument("--cases", nargs="?", const="", metavar="NAME",
                        help="the alternatives (effects of one name on sibling targets): a case where none happens, "
                             "where more than one does, where a value leaves --range; of that name, or all")
    parser.add_argument("--range", metavar="LOW:HIGH", help="the range the values of --cases must keep (either may be "
                                                            "left out)")
    parser.add_argument("--seconds", type=float, default=limits.SECONDS,
                        help="the longest the questions may take (default: no limit); past it, what was not "
                             "answered is said")
    parser.add_argument("--memory", type=float, default=limits.MEGABYTES,
                        help="the most memory in MB the questions may hold (default: no limit); past it, "
                             "what was not answered is said")
    parser.add_argument("--evaluate", metavar="STEPS",
                        help="what the program produces from these starts, in turn (`A;B*3`: A, then B three times), "
                             "the state passed on; every output with its values and why")
    parser.add_argument("--given", action="append", default=[], metavar="NAME=VALUE",
                        help="an input of the situation: a parameter, a state, what a call of code not read gives back "
                             "(by `path:line` or what is called); the value as JSON (else a text)")
    parser.add_argument("--situation", metavar="FILE", help="the inputs of the situation, as one JSON object")
    parser.add_argument("--repeat", type=int, default=1, help="how many times the steps of --evaluate run")
    parser.add_argument("--why", metavar="PATH:LINE", help="with --evaluate: where the values of the outputs there "
                                                         "come from, down to the inputs")
    parser.add_argument("--why-not", metavar="PATH:LINE", help="with --evaluate: why the code there did not run")
    parser.add_argument("--broken", action="store_true",
                        help="links that lead nowhere: state never read or never written, events with no handler, "
                             "handlers never fired, data keys missing or never read, uniforms never set or read")
    parser.add_argument("--feature", metavar="POINTS",
                        help="the functionality of these outputs (`path:line` or a function's label, by commas): what "
                             "decides them, what sets them off, and its sheet")
    parser.add_argument("--cost", metavar="ROOT", help="what one run of this root costs: its loops, what they go over, "
                                                      "what is made and called at each turn, and the order of each")
    parser.add_argument("--compare", nargs=2, metavar="MAP", help="two programs (each a map file) compared in the "
                                                                  "same situations")
    parser.add_argument("--shared", action="append", default=[], metavar="NAME=VALUE",
                        help="a variable the two maps of --compare compute their inputs from")
    parser.add_argument("--sweep", metavar="NAME:LOW:HIGH:N", help="--compare over a range of a shared variable")
    parser.add_argument("--plane", metavar="WHAT:I,WHAT:I", help="with --compare and --sweep: argument I of output "
                                                                 "WHAT against argument I of another, on a plane")
    parser.add_argument("--versions", metavar="OLD_ROOT", help="with --evaluate: the same situation in an older version "
                                                              "(the roots are the newer): what changed, by which line")
    parser.add_argument("--html", metavar="FILE", help="also write the answer as one HTML file to look at")
    parser.add_argument("--dot", metavar="FILE", help="also write the graph (of what the start runs, or all) as DOT")
    parser.add_argument("--wrong", action="store_true",
                        help="behaviour that contradicts what the program itself states: checks most peers make, "
                             "missing at one; writes breaking an invariant the other writes keep; one value given to "
                             "two objects' same operation; the alternatives' gaps and overlaps")
    parser.add_argument("--text-only", action="store_true",
                        help="leave the compilers out (clang, the TypeScript compiler): what each call runs as the "
                             "tracer reads it from the text alone")
    return parser.parse_args(argv)


def bounds(written: str | None) -> tuple:
    """The two ends of a range written `LOW:HIGH` (either may be left out)."""
    if not written:
        return None, None
    low, _, high = written.partition(":")
    return (Fraction(low) if low.strip() else None), (Fraction(high) if high.strip() else None)


def situation_of(args) -> dict:
    """The inputs given: a JSON file, then each NAME=VALUE (the value as JSON, else a text)."""
    out = json.loads(Path(args.situation).read_text(encoding="utf-8")) if args.situation else {}
    for given in args.given + args.shared:
        name, _, value = given.partition("=")
        try:
            out[name] = json.loads(value)
        except ValueError:
            out[name] = value
    return out


def steps_of(result, written: str) -> list:
    """`A;B*3` as [(start, [])]: each start, repeated as written."""
    out = []
    for part in written.split(";"):
        label, _, times = part.strip().partition("*")
        found = find(result, label.strip())
        if not found:
            raise SystemExit(f"nothing is named {label.strip()}: --list shows every name")
        out += [(found[0], [])] * (int(times) if times.strip().isdigit() else 1)
    return out


def write(args, text: str) -> int:
    if args.out:
        Path(args.out).write_bytes((text + "\n").encode("utf-8"))
    else:
        sys.stdout.buffer.write((text + "\n").encode("utf-8"))
    return 0


def evaluating(args, roots, say) -> int:
    """The questions that work the program out (they read the files' facts: a fresh analysis, kept)."""
    ceiling = limits.Ceiling(args.seconds * 5, args.memory)
    situation = situation_of(args)
    pages = []
    if args.compare:
        sweep = None
        if args.sweep:
            name, low, high, count = args.sweep.split(":")
            sweep = (name, float(low), float(high), int(count))
        done = compare.compare(args.compare[0], args.compare[1], situation, sweep)
        text = json.dumps(compare.data(done), indent=1, default=str) if args.json else compare.text_report(done)
        if sweep:
            pages.append((views.BANDS, "Where each output happens", views.bands(done)))
        if args.plane and sweep:
            x, y = [(part.split(":")[0], int(part.split(":")[1])) for part in args.plane.split(",")]
            pages.append((views.PLANE, "The outputs' values",
                          views.plane_view(x, y, [r[2] for r in done.rows] + [r[3] for r in done.rows])))
        pages.append((views.WORDS, "The comparison", compare.text_report(done)))
        return finish(args, text, pages)
    result = analyse(roots, say=say, among=args.among, keep=True, ceiling=ceiling,
                      compilers=not args.text_only)
    if result.cut:
        say(f"the analysis stopped at the ceiling ({result.cut}): the answers are partial")
    if args.evaluate:
        steps = steps_of(result, args.evaluate)
        run = evaluate.evaluate(result, steps, situation, args.repeat)
        if args.versions:
            old = analyse([args.versions], args.versions, keep=True, ceiling=ceiling, compilers=not args.text_only)
            old_steps = steps_of(old, args.evaluate)
            old_run = evaluate.evaluate(old, old_steps, situation, args.repeat)
            changed = compare.changed_lines(args.versions, roots[0])
            first = [(o.callee, [evaluate.plain(a) for a in o.args], o) for o in old_run.outputs]
            second = [(o.callee, [evaluate.plain(a) for a in o.args], o) for o in run.outputs]
            lines = ["The older version against the newer, in the same situation:"]
            for kind, a, b in compare.differences(first, second):
                lines.append(f"  {kind}: {a.callee if a else ''} {b.callee if b else ''}")
                lines += [f"    older: {row}" for row in compare.why_side(old_run, a)]
                lines += [f"    newer: {row}" for row in compare.why_side(run, b)]
                lines += [f"    changed line: {w}" for w in compare.blamed(changed, b or a)]
            text = "\n".join(lines)
        elif args.json:
            text = json.dumps(evaluate.run_data(result, run), indent=1, default=str)
        else:
            text = evaluate.run_text(result, run)
        if args.why:
            rows = [o for o in run.outputs if o.where.endswith(args.why) or args.why.endswith(o.where)]
            text += "\n\nWhy, at " + args.why + ":\n" + "\n".join(
                line for o in rows for a in o.args for line in evaluate.why_lines(a.why)) if rows else \
                f"\n\nNo output at {args.why} in this run."
        if args.why_not:
            text += "\n\nWhy not, at " + args.why_not + ":\n" + "\n".join(evaluate.why_not(run, args.why_not))
        pages += [(views.TIMELINE, "The sequence", views.timeline(run)), (views.WORDS, "The run", text)]
        units = situations.reach(result, [d for d, _ in steps])
        pages.append((views.GRAPH, "The graph of what it runs", views.graph(result, units)))
        return finish(args, text, pages, result, units)
    units = None                                 # the graph's part: the functionality's slice, the root's reach, or all
    if args.broken:
        text = json.dumps(links.data(result), indent=1) if args.json else links.text_report(result)
        pages.append((views.WORDS, "Links that lead nowhere", text))
    elif args.feature:
        wanted = [w.strip() for w in args.feature.split(",") if w.strip()]
        text = json.dumps(feature.data_of(result, wanted), indent=1, default=str) if args.json else \
            feature.text_report(result, wanted)
        pages.append((views.WORDS, "The functionality", text))
        units = feature.units_of(result, wanted)
    else:
        roots_found = find(result, args.cost)
        if not roots_found:
            print(f"nothing is named {args.cost}: --list shows every name", file=sys.stderr)
            return 1
        text = json.dumps(cost.data(result, roots_found), indent=1) if args.json else \
            cost.text_report(result, roots_found)
        pages.append((views.WORDS, "The cost", text))
        units = situations.reach(result, roots_found)
    pages.append((views.GRAPH, "The graph", views.graph(result, units)))
    return finish(args, text, pages, result, units)


def finish(args, text: str, pages: list, result=None, units=None) -> int:
    if args.html:
        Path(args.html).write_text(views.page("tracer", pages), encoding="utf-8")
    if args.dot and result is not None:
        Path(args.dot).write_text(views.dot(views.graph(result, units)), encoding="utf-8")
    return write(args, text)


def main(argv=None) -> int:
    args = arguments(argv)
    began = time.monotonic()

    def say(words: str) -> None:
        print(f"[{time.monotonic() - began:6.1f} s] {words}", file=sys.stderr, flush=True)
    roots = args.root or ["."]
    if args.evaluate or args.compare or args.broken or args.feature or args.cost:
        return evaluating(args, roots, say)
    name = index.key(roots, None, args.among, not args.text_only)
    result = None if args.fresh else index.load(name)
    if result is None:
        ceiling = limits.Ceiling(limits.ANALYSIS_SECONDS, args.memory)
        result = index.kept(analyse(roots, say=say, among=args.among, ceiling=ceiling,
                                     compilers=not args.text_only))
        if result.cut:                   # stopped at the ceiling: answered as far as it went, not kept as whole
            say(f"the analysis stopped at the ceiling ({result.cut}): what follows is partial, and not saved")
        else:
            index.save(name, result)
            say("analysis saved")
    else:
        say("analysis read back")
    program = result.program
    say(f"{sum(len(found) for found in result.out.values())} links")
    if not program.files:
        print("the roots hold no code the tracer reads", file=sys.stderr)
        return 1
    low, high = bounds(args.range)
    if args.gaps:
        text = json.dumps(summary(result), indent=1) if args.json else text_report(result)
    elif args.wrong:
        starts = find(result, args.start) if args.start else None
        asked = (args.start, args.range, args.json)
        result.answers = result.answers or {}
        if asked not in result.answers:          # answered once, then read back with the saved analysis
            units = situations.reach(result, starts) if starts else None     # a start: what it runs, only
            decide.MEMO.update(index.load_answers())
            known = len(decide.MEMO)
            ceiling = limits.Ceiling(args.seconds, args.memory)
            if args.json:
                data = {"checks missing": beliefs.data(result, units, ceiling),
                        "the invariants": invariants.data(result, units, ceiling),
                        "the alternatives": situations.data(result, starts, None, low, high, ceiling),
                        "partial (not followed on)": [f"{where}  {what}" for where, what in result.cases.partial],
                        "not answered": [f"{what} ({why})" for what, why in ceiling.left_out]}
                result.answers[asked] = json.dumps(data, indent=1)
            else:
                parts = [beliefs.text_report(result, units=units, ceiling=ceiling),
                         invariants.text_report(result, units=units, ceiling=ceiling),
                         situations.text_report(result, starts, None, low, high, ceiling)]
                if result.cases.partial:
                    parts.append("Partial: not followed on here (answers through it are partial):\n" +
                                 "\n".join(f"  {where}  {what}" for where, what in result.cases.partial[:50]))
                if ceiling.left_out:
                    parts.append(ceiling.text())
                result.answers[asked] = "\n\n".join(parts)
            text = result.answers[asked]
            if ceiling.left_out:                 # cut by the ceiling: said, and not kept as the answer
                say(f"the ceiling was reached ({ceiling.why}): {len(ceiling.left_out)} questions not answered")
                del result.answers[asked]
            index.save(name, result)
            if len(decide.MEMO) > known:
                index.save_answers(decide.MEMO)
            say(f"answers saved ({len(decide.MEMO) - known} formulas solved, {known} known)")
        else:
            text = result.answers[asked]
    elif args.cases is not None or args.conditions:
        starts = find(result, args.start) if args.start else None
        if args.start and not starts:
            print(f"nothing is named {args.start}: --list shows every name", file=sys.stderr)
            return 1
        name = args.cases or None
        if args.json:
            text = json.dumps(situations.data(result, starts, name, low, high), indent=1)
        else:
            parts = [situations.conditions_text(result, starts)] if args.conditions else []
            if args.cases is not None:
                parts.append(situations.text_report(result, starts, name, low, high))
            text = "\n\n".join(parts)
    elif args.list or not args.start:
        text = listing(result, RUNNING)
    else:
        starts = find(result, args.start)
        if not starts:
            print(f"nothing is named {args.start}: --list shows every name", file=sys.stderr)
            return 1
        keep = (lambda link: link.how == EXACT) if args.exact else (lambda link: True)
        ahead = forward(result, starts, args.depth, keep, args.guesses, args.handed)
        behind = backward(result, starts, args.back, keep, args.guesses)
        if args.json:
            data = {"start": args.start, "forward": as_data(result, ahead), "backward": as_data(result, behind)}
            if args.state:
                data["state"] = state_data(result, ahead)
            text = json.dumps(data, indent=1)
        else:
            text = as_text(result, FORWARD, ahead, True, args.state) + "\n\n" + as_text(result, BACKWARD, behind, False)
            if args.state:
                text += "\n\n" + state_text(result, ahead)
            text += "\n\n" + trigger_text(result, behind)
    if args.out:
        Path(args.out).write_bytes((text + "\n").encode("utf-8"))
    else:
        sys.stdout.buffer.write((text + "\n").encode("utf-8"))
    return 0


if __name__ == "__main__":
    sys.exit(main())
