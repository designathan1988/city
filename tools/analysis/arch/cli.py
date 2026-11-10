"""The commands. scan writes dist/arch; explain, next, done and status read what it wrote; predict uses the repository's own past; selfcheck proves each
class of defect against its gold and its mutation."""
import argparse
import json
import os
import sys

from .core import digest, program_root, read_json, write_json


def _base(path):
    from .repo import toplevel
    root = toplevel(path)
    home = os.path.join(program_root(), "dist", "arch")
    if os.path.normcase(root) == os.path.normcase(os.path.normpath(program_root())):
        return home
    return os.path.join(home, "repos", digest(root))


def scan(args):
    from .pipeline import analyse, publish
    from .scan import Run
    base = _base(args.repo)
    run = Run(args.repo, rev=args.rev, cache=not args.no_cache, out=os.path.join(program_root(), "dist", "arch"))
    try:
        run.read()
        result = analyse(run)
        publish(run, result, base)
        run.cache.save()
    finally:
        run.close()
    counts = {c["id"]: c["found"] for c in read_json(os.path.join(base, "coverage.json"))["classes"]}
    print(f"{len(result['program'].fn)} functions, {len(result['entries'])} entries, {len(result['findings'])} findings {counts} -> {base}")
    return 0


def _queue(args):
    return read_json(os.path.join(_base(args.repo), "queue.json"), [])


def _find(queue, ident):
    return next((q for q in queue if q["id"].startswith(ident)), None)


def show(q):
    print(f"{q['id']}  {q['entry']}  [{q['kind']}, {q['freq']}]  {q['file']}:{q['line']}  commits {q['commits']}")
    print(" why:", "; ".join(q["why"]))
    for v in q.get("via", []):
        print(f" via script {v['script']} ({v['freq']}): {' -> '.join(v['path'])}")
    print(" cost:", json.dumps(q["cost"], ensure_ascii=False, sort_keys=True))
    print(f" violations: {len(q['violations'])}")
    for v in q["violations"]:
        print(f"  [{v['class']}] {v['file']}:{v['line']} {v['detail']}")
        for step in v["path"]:
            print(f"      {step['file']}:{step['line']} {step['what']}")
    print(" places:")
    for file, line, what in q["places"]:
        print(f"  {file}:{line} {what}")
    print(" owner:", q["owner"] or "-")
    print(" vanish with the fix:", len(q["vanish"]), "findings in other slices")


def explain(args):
    q = _find(_queue(args), args.id)
    if q is None:
        print(f"no slice {args.id}")
        return 1
    show(q)
    return 0


def next_cause(args):
    done = read_json(os.path.join(_base(args.repo), "done.json"), {})
    for q in _queue(args):
        if q["id"] not in done and q["violations"]:
            show(q)
            return 0
    print("nothing left in the queue")
    return 0


def done(args):
    base = _base(args.repo)
    table = read_json(os.path.join(base, "done.json"), {})
    q = _find(_queue(args), args.id)
    if q is None:
        print(f"no slice {args.id}")
        return 1
    table[q["id"]] = q["entry"]
    write_json(os.path.join(base, "done.json"), table)
    print(f"{q['id']} marked done")
    return 0


def predict(args):
    from .predict import predict_for
    for path, score in predict_for(args.repo, args.id):
        print(f"{score:.3f} {path}")
    return 0


def status(args):
    base = _base(args.repo)
    cover = read_json(os.path.join(base, "coverage.json"))
    if cover is None:
        print("no scan yet: run scan")
        return 1
    queue = read_json(os.path.join(base, "queue.json"), [])
    check = read_json(os.path.join(program_root(), "dist", "arch", "selfcheck.json"), {})
    recall = check.get("classes", {})
    print(f"commit {cover['commit']}: {cover['functions']} functions, entries {json.dumps(cover['entries'], sort_keys=True)}, {len(queue)} slices, "
          f"{sum(1 for q in queue if q['violations'])} with violations")
    print(f"selfcheck: {'none yet' if not check else ('PASS' if check.get('pass') else 'FAIL')} (gold commit {check.get('commit', '-')})")
    print(f"  {'class':<20} {'found':>6} {'gold recall':>12} {'mutation':>9}  title")
    for c in cover["classes"]:
        r = recall.get(c["id"])
        gold = f"{r['gold_found']}/{r['gold_total']}" if r else "unproven"
        mutation = r["mutation"] if r else "unproven"
        print(f"  {c['id']:<20} {c['found']:>6} {gold:>12} {mutation:>9}  {c['title']}")
    for name, r in sorted(check.get("checks", {}).items()):
        print(f"  check {name}: {r['status']}")
    print("first slices:")
    for q in queue[:10]:
        print(f"  {q['id']} {q['entry']} [{q['freq']}] violations {len(q['violations'])} cost {q['cost']['deepest']}/{q['cost']['sized_loops']}")
    return 0


def selfcheck(args):
    from .check.run import main as check
    return check(args)


def step(args):
    from .check.steps import main as run_step
    return run_step(args.name, args.out)


def main(argv):
    parser = argparse.ArgumentParser(prog="arch.py", description="Finds causes of architecture and performance problems in a git repository.")
    sub = parser.add_subparsers(dest="command", required=True)
    for name, fn, extra in (("scan", scan, ()), ("explain", explain, ("id",)), ("next", next_cause, ()), ("predict", predict, ("id",)), ("done", done, ("id",)),
                            ("status", status, ()), ("selfcheck", selfcheck, ())):
        p = sub.add_parser(name)
        for e in extra:
            p.add_argument(e)
        p.add_argument("repo", nargs="?", default=".")
        p.set_defaults(fn=fn)
        if name == "scan":
            p.add_argument("--no-cache", action="store_true")
            p.add_argument("--rev", default="HEAD")
        if name == "selfcheck":
            p.add_argument("--only", default="")
    p = sub.add_parser("step", help=argparse.SUPPRESS)
    p.add_argument("name")
    p.add_argument("out")
    p.set_defaults(fn=step)
    args = parser.parse_args(argv)
    return args.fn(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
