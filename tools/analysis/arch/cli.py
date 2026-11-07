"""The commands. scan writes dist/arch; explain, next, done and status read what it wrote; predict uses the repository's own past."""
import argparse
import json
import os
import sys

os.environ.setdefault("OPENBLAS_NUM_THREADS", "1")  # no matrix is ever multiplied here: its thread pool would only reserve memory

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
    run = Run(args.repo, cache=not args.no_cache)
    run.home = os.path.normpath(program_root())
    run.key = digest(run.repo.root)
    run.read()
    result = analyse(run)
    base, _ = publish(run, result)
    run.cache.save()
    print(f"{len(run.graph.units)} units, {len(run.graph.edges)} edges, {len(result['groups'])} groups, {len(result['queue'])} causes -> {base}")
    return 0


def _queue(args):
    return read_json(os.path.join(_base(args.repo), "queue.json"), [])


def _find(queue, ident):
    return next((q for q in queue if q["id"].startswith(ident)), None)


def show(q):
    print(f"{q['id']}  {q['cause']}  context {q['context']}  reach {q['reach']}  commits {q['commits']}")
    print(" detail:", json.dumps(q["detail"], ensure_ascii=False))
    print(" entry:", q["entry"])
    for s in q["proof"]:
        print(f"  {s['file']}:{s['line']} {s['kind']} {s['from']} -> {s['to']}")
    print(" places:")
    for p in q["places"]:
        print(f"  {p['file']}:{p['line']} {p['unit']}")
    print(" owner:", q["owner"])
    if q["generated"] or q["made_by"]:
        print(" generated, fix in the generator:", q["generated"], q["made_by"])
    print(" vanish:", q["vanish"], " children:", len(q["children"]))


def explain(args):
    q = _find(_queue(args), args.id)
    if q is None:
        print(f"no cause {args.id}")
        return 1
    show(q)
    return 0


def next_cause(args):
    done = read_json(os.path.join(_base(args.repo), "done.json"), {})
    for q in _queue(args):
        if q["id"] not in done:
            show(q)
            return 0
    print("nothing left in the queue")
    return 0


def done(args):
    base = _base(args.repo)
    table = read_json(os.path.join(base, "done.json"), {})
    q = _find(_queue(args), args.id)
    if q is None:
        print(f"no cause {args.id}")
        return 1
    table[q["id"]] = q["cause"]
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
    residue = read_json(os.path.join(base, "residual.json"), [])
    print(f"commit {cover['commit']}: {cover['units']} units, {cover['edges']} edges, {len(queue)} causes")
    check = read_json(os.path.join(base, "selfcheck.json"), {})
    proved = set(check.get("injection", {}).get("proved", ()))
    print("coverage (covered: found by injection in the last selfcheck):")
    open_rows = 0
    for row in [*({"category": c["id"], "title": c["title"], "found": c["count"]} for c in cover["classes"]), *cover["table"]]:
        mark = "covered" if row["category"] in proved else "NOT COVERED"
        open_rows += mark != "covered"
        print(f"  {row['category']:<22} {mark:<12} {row['found']:>7}  {row['title']}")
    print(f"categories not covered: {open_rows}")
    print("points:", json.dumps(cover["points"], sort_keys=True))
    print(f"residue: {len(residue)}")
    for r in residue:
        print(f"  {r['file']}:{r['line']}:{r['column']} {r['kind']} {r['text']} candidates {r['candidates']} confidence {r['confidence']}")
    return 0


def selfcheck(args):
    from .check.run import main as check
    return check(args)


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
        if name == "selfcheck":
            p.add_argument("--only", default="")
            p.add_argument("--job", default="", help=argparse.SUPPRESS)
    args = parser.parse_args(argv)
    return args.fn(args)


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
