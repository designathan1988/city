#!/usr/bin/env python3
"""Project gates: python tools/gates/run_gates.py [--file PATH ...] [--files-from LIST] [--summary] [--selftest] [--json]
[--register log/problems.jsonl]
Exit 0 clean; 1 when any rule reports a problem or crashes; 2 when no file is given or the scope is empty, or a given
path is unknown (not on disk and not in git). Given paths that every gate leaves out by a declared rule are listed and
skipped; when all of them are, the exit is 0. Each rule runs on its own: one crashing rule is a failure and the others
still run. --register joins the findings with the register (a block's close, FR-052): each finding shown with the open
problem whose origin names it (gate:<RULE>:<file>) or as NEW, and the exit is 1 only for a NEW finding; the findings
themselves are the same, never filtered, baselined or silenced. --json prints the findings as JSON instead of text."""
import argparse
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import registry  # noqa: E402
import scope  # noqa: E402
from context import REPO, Cache, Context, digest  # noqa: E402
from lex_other import LexError  # noqa: E402

sys.path.insert(0, os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..")))
from atlas import problems as register  # noqa: E402  the register's one owner

SUMMARY_SHOWN = 15
WHERE_LINE = re.compile(r"^(.*?):(\d+)$")


def load_config(root):
    with open(os.path.join(root, "tools", "gates", "rules.json"), encoding="utf-8") as fh:
        rules = json.load(fh)
    with open(os.path.join(root, "tools", "gates", "owners.json"), encoding="utf-8") as fh:
        owners = json.load(fh)
    return rules, owners


def run_rules(ctx, targets, rules_list, whole_filter=None):
    """Problems as (id, where, message). `targets`: files the per-file rules judge; whole-repo rules run when
    `whole_filter` is None or one of their inputs is in it."""
    problems = []
    per_file = [r for r in rules_list if isinstance(r, registry.PerFile)]
    whole = [r for r in rules_list if isinstance(r, registry.Whole)]
    contexts = {}
    for f in targets:
        active = []
        for r in per_file:
            try:
                if r.applies(ctx, f):
                    active.append(r)
            except Exception as e:  # noqa: BLE001  a crash is a failure, reported and counted
                problems.append(("GATE_CRASH", f, f"{r.name}: {e!r}"))
        if not active:
            continue
        for r in active:
            if r.context and r.name not in contexts:
                try:
                    contexts[r.name] = r.context(ctx)
                except Exception as e:  # noqa: BLE001
                    contexts[r.name] = None
                    problems.append(("GATE_CRASH", r.name, f"context: {e!r}"))
        key = digest("per_file", ctx.cache.file_key(f), [r.name for r in active],
                     [contexts.get(r.name) for r in active], ctx.changed.get(f) if f in ctx.changed else "-")
        problems += [tuple(p) for p in ctx.cache.get(key, lambda: _judge(ctx, f, active))]
    for r in whole:
        inputs = [f for f in ctx.listed if r.inputs(ctx, f)]
        if whole_filter is not None and not any(r.inputs(ctx, f) for f in whole_filter):
            continue
        try:
            key = digest("whole", r.name, ctx.cache.key_of(inputs, extra=(json.dumps(r.extra(ctx) if r.extra else None),)))
            problems += [tuple(p) for p in ctx.cache.get(key, lambda: _collect(lambda err: r.check(ctx, err)))]
        except Exception as e:  # noqa: BLE001
            problems.append(("GATE_CRASH", r.name, repr(e)))
    return problems


def _collect(fn):
    out = []
    fn(lambda gid, where, msg: out.append([gid, where, msg]))
    return out


def _judge(ctx, path, active):
    out = []
    src = ctx.source(path)
    for r in active:
        def err(gid, where, msg):
            out.append([gid, where, msg])
        try:
            r.check(ctx, src, err)
        except LexError as e:
            out.append([r.ids[0], path, f"cannot be tokenized: {e}"])
        except Exception as e:  # noqa: BLE001
            out.append(["GATE_CRASH", path, f"{r.name}: {e!r}"])
    return out


def build_context(root, requested):
    rules, owners = load_config(root)
    cache = Cache(root, enabled=True)
    listed = scope.listed_files(root)
    files = [f for f in listed if not scope.excluded(f, rules["exclude_dirs"])]
    changed = scope.changed_lines(root, requested)
    return Context(root, files, listed, rules, owners, changed, cache)


def requested_paths(args):
    """Repo-relative paths from --file and --files-from (one path per line), or None when neither was given."""
    if not args.file and args.files_from is None:
        return None
    raw = list(args.file)
    if args.files_from is not None:
        with open(args.files_from, encoding="utf-8") as fh:
            raw += [line.strip() for line in fh if line.strip()]
    return sorted({os.path.relpath(os.path.join(REPO, f), REPO).replace(os.sep, "/") for f in raw})


def classify(ctx, requested, rules_list):
    """(judged, ignored [(path, reason)], unknown) of the given paths. A path outside every gate by a declared rule
    (rules.json/exclude_dirs, .gitignore, deleted in git) is ignored and listed; one that git does not know and that is
    not on disk is a usage error."""
    wholes = [r for r in rules_list if isinstance(r, registry.Whole)]
    tracked = scope.tracked_files(ctx.root)
    judged, ignored, unknown = [], [], []
    for f in requested:
        if f in ctx.file_set or (f in ctx.listed_set and any(r.inputs(ctx, f) for r in wholes)):
            judged.append(f)
        elif f in ctx.listed_set:
            ignored.append((f, "in a folder rules.json/exclude_dirs keeps out of the gates"))
        elif os.path.isfile(os.path.join(ctx.root, f)):
            ignored.append((f, "ignored by .gitignore"))
        elif f in tracked:
            ignored.append((f, "deleted in the worktree"))
        else:
            unknown.append(f)
    return judged, ignored, unknown


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--file", action="append", default=[], help="judge only this file (repeatable)")
    ap.add_argument("--files-from", help="judge only the files listed in this file, one repo-relative path per line")
    ap.add_argument("--summary", action="store_true", help="print a count per rule and the first problems")
    ap.add_argument("--selftest", action="store_true", help="run every rule on its must-fail and must-pass fixtures")
    ap.add_argument("--json", action="store_true", help="print the findings as JSON (rule, where, file, line, message)")
    ap.add_argument("--register", help="join the findings with this register: only a finding no open problem names fails")
    args = ap.parse_args(argv)
    sys.stdout.reconfigure(encoding="utf-8")  # paths and rules.json messages are UTF-8, whatever the console's code page
    if args.selftest:
        import selftest
        return selftest.main()
    requested = requested_paths(args)
    if requested is not None and not requested:
        print("[GATE_SCOPE] no file given: nothing to judge")
        return 2
    ctx = build_context(REPO, requested)
    rules_list = registry.all_rules(ctx.rules)
    if requested is None:
        targets = ctx.files
        if not targets:
            print("[GATE_SCOPE] the scope is empty: nothing to judge")
            return 2
    else:
        targets, ignored, unknown = classify(ctx, requested, rules_list)
        for f, why in ignored:
            print(f"[GATE_IGNORED] {f}: {why}")
        if unknown:
            for f in unknown:
                print(f"[GATE_SCOPE] {f}: no such file, and git knows no file of that name")
            return 2
        if not targets:
            print(f"Gates: none of the {len(requested)} given file(s) is in a gate's scope; nothing to judge.")
            return 0
    ctx.cache.warm(ctx.listed)
    problems = run_rules(ctx, [t for t in targets if t in ctx.file_set], rules_list, None if requested is None else targets)
    problems.sort(key=lambda p: (p[0], p[1]))
    findings = [finding(*p) for p in problems]
    if args.register:
        join(findings, os.path.join(REPO, args.register))
    if args.json:
        print(json.dumps(findings, ensure_ascii=False, indent=1))
    elif args.register:
        report_joined(findings, len(targets))
    else:
        report(problems, len(targets), args.summary)
    if args.register:
        return 1 if any(f["problem"] is None for f in findings) else 0
    return 1 if problems else 0


def finding(gid, where, msg):
    """A finding as data: its rule, where it stands (a file, a file:line or a whole rule's place) and the file alone."""
    m = WHERE_LINE.match(where)
    return {"rule": gid, "where": where, "file": m.group(1) if m else where, "line": int(m.group(2)) if m else None,
            "message": msg}


def join(findings, register_path):
    """Each finding with the id of the open problem whose origin names it (gate:<RULE>:<file>), or None: a NEW finding."""
    named = {}
    for p in register.read(register_path):
        if p.get("status") not in register.FINAL:
            for origin in p.get("origin") or []:
                named.setdefault(origin, p["id"])
    for f in findings:
        f["problem"] = named.get(register.gate_origin(f["rule"], f["file"]))


def report_joined(findings, judged):
    new = [f for f in findings if f["problem"] is None]
    for f in new:
        print(f"NEW [{f['rule']}] {f['where']}: {f['message']}")
    known = {}
    for f in findings:
        if f["problem"]:
            known[f["rule"]] = known.get(f["rule"], 0) + 1
    if known:
        print("registered (open problems of the register): " + ", ".join(f"{g} {n}" for g, n in sorted(known.items())))
    verdict = "FAILED" if new else "OK"
    print(f"Gates against the register {verdict}: {len(findings)} finding(s) in {judged} file(s), {len(new)} NEW, "
          f"{len(findings) - len(new)} registered as open problems.")


def report(problems, judged, summary):
    counts = {}
    for gid, _w, _m in problems:
        counts[gid] = counts.get(gid, 0) + 1
    shown = problems[:SUMMARY_SHOWN] if summary else problems
    for gid, where, msg in shown:
        print(f"[{gid}] {where}: {msg}")
    if summary and len(problems) > len(shown):
        print(f"... and {len(problems) - len(shown)} more")
    if problems:
        print("by rule: " + ", ".join(f"{g} {n}" for g, n in sorted(counts.items())))
        print(f"Gates FAILED: {len(problems)} problem(s) in {judged} file(s). Rules: CLAUDE.md; map: log/map.md.")
    else:
        print(f"Gates OK: {judged} file(s).")


if __name__ == "__main__":
    sys.exit(main())
