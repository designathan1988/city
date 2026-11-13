"""The register's commands of the atlas (`python tools/atlas/cli.py problems ...`): list with filters, add, update, close, drop,
render, import-gates, stats, rename-rule and rename-path; a filter is checked against the schema's values. Every change goes through tools/atlas/problems.py (the one writer) and the view is
written again after it."""
import json
import os
import sys

TOOLS_DIR = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), ".."))
if TOOLS_DIR not in sys.path:
    sys.path.insert(0, TOOLS_DIR)
from atlas import imports  # noqa: E402
from atlas import problems as register  # noqa: E402
from atlas import render  # noqa: E402


def add_parser(commands, common):
    p = commands.add_parser("problems", parents=[common], help="the register: list (filters), add, update, close, drop, render, "
                            "import-gates, stats (of the filtered problems), rename-rule, rename-path")
    p.add_argument("--area")
    p.add_argument("--owner")
    p.add_argument("--status")
    p.add_argument("--kind")
    p.add_argument("--cause", help="words the cause holds, all of them")
    actions = p.add_subparsers(dest="action")
    add = actions.add_parser("add", parents=[common], help="a new problem, status open, id assigned")
    _fields(add, required=("title", "symptom", "area", "kind", "origin"))
    update = actions.add_parser("update", parents=[common], help="fields set (a list added to); --status moves it by the transitions")
    update.add_argument("id")
    update.add_argument("--status", dest="move_to")
    _fields(update, required=())
    close = actions.add_parser("close", parents=[common], help="fixed, with what changed and why it cannot return")
    close.add_argument("id")
    close.add_argument("--fix", required=True)
    drop = actions.add_parser("drop", parents=[common], help="dropped, with why it is no problem")
    drop.add_argument("id")
    drop.add_argument("--why", required=True)
    actions.add_parser("render", parents=[common], help="write the generated view doc/problems.md")
    gates = actions.add_parser("import-gates", parents=[common], help="the gates' standing findings, one problem per rule and file")
    gates.add_argument("findings", help="the JSON python tools/gates/run_gates.py --json wrote")
    actions.add_parser("stats", parents=[common], help="counts by area, status and origin, and the median minutes to a cause, "
                       "of the problems the filters keep")
    rename = actions.add_parser("rename-rule", parents=[common], help="a gate rule renamed: its problems' origins and words follow")
    rename.add_argument("old")
    rename.add_argument("new")
    moved = actions.add_parser("rename-path", parents=[common], help="a file or folder moved: its problems' evidence and owner follow")
    moved.add_argument("old")
    moved.add_argument("new")
    return p


def _fields(parser, required):
    for field in ("title", "symptom", "cause", "reading", "owner", "area", "kind", "criterion", "spec"):
        parser.add_argument(f"--{field}", required=field in required, dest=f"set_{field}")
    for field in register.LISTS:
        parser.add_argument(f"--{field}", action="append", required=field in required, dest=f"set_{field}",
                            help="repeatable")


def _given(args):
    return {k[len("set_"):]: v for k, v in vars(args).items() if k.startswith("set_") and v is not None}


def _show(found, as_json):
    if as_json:
        print(json.dumps(found, ensure_ascii=False, indent=1))
        return
    for p in found:
        print(f"{p['id']} [{p['status']}] {p['area']}/{p['kind']}: {p['title']}")
        print(f"    owner {p['owner']}; cause: {p['cause']}; closes when: {p['criterion']}")
    print(f"{len(found)} problem(s)")


def run(args, root=register.ROOT):
    """The exit code: 0 an answer, 1 a change the rules refuse (nothing written), 2 a filter no problem can match."""
    reg = register.Register(root=root)
    wrong = reg.wrong_filters(area=args.area, status=args.status, kind=args.kind)
    for field, value, allowed in wrong:
        print(f"problems: --{field} {value!r} is none of the register's: {', '.join(allowed)}")
    if wrong:
        return 2
    kept = reg.query(args.area, args.owner, args.status, args.cause, args.kind)
    if args.action is None:
        _show(kept, args.json)
        return 0
    if args.action == "render":
        return render.main(["--root", root])
    if args.action == "stats":
        doc = imports.stats(reg, kept)
        print(json.dumps(doc, indent=1) if args.json else "\n".join(f"{k}: {v}" for k, v in doc.items()))
        return 0
    if args.action in ("rename-rule", "rename-path"):
        changed = (reg.rename_rule if args.action == "rename-rule" else reg.rename_path)(args.old, args.new)
        try:
            reg.save()
        except register.RegisterError as e:
            print(f"refused: {e}")
            return 1
        print(f"{len(changed)} problem(s) now name {args.new} instead of {args.old}")
        return render.main(["--root", root])
    if args.action == "import-gates":
        with open(args.findings, encoding="utf-8") as fh:
            added = imports.gate_findings(reg, json.load(fh))
        try:
            reg.save()
        except register.RegisterError as e:
            print(f"refused: {e}")
            return 1
        print(f"{len(added)} problem(s) entered from the gates' findings")
        return render.main(["--root", root])
    try:
        if args.action == "add":
            changed = reg.add(**_given(args))
        elif args.action == "update":
            changed = reg.update(args.id, status=args.move_to, **_given(args))
        elif args.action == "close":
            changed = reg.close(args.id, args.fix)
        else:
            changed = reg.drop(args.id, args.why)
        reg.save()
    except register.RegisterError as e:
        print(f"refused: {e}")
        return 1
    _show([changed], args.json)
    return render.main(["--root", root])
