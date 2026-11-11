#!/usr/bin/env python3
"""The scan of what already stands: every file of LAB_COPY's paths judged whole against the roadcraft, as the gate judges a
change (tools/gates/gate_lab.py), each finding with this game's place and the reference's, counted by folder and by
reference file. python tools/gates/lab_scan.py [--out dist/lab_scan.json]; exit 1 when anything is found, 0 when nothing, 2
without the reference."""
import argparse
import json
import os
import sys
import time

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import gate_lab  # noqa: E402
import lab_prints  # noqa: E402
import scope  # noqa: E402
from context import REPO, Cache, Context  # noqa: E402
from run_gates import load_config  # noqa: E402

PROGRESS_EVERY = 100  # files between two progress lines on a long scan


def walk(root):
    """Every file under `root`, relative and posix (a repository git does not know, as the self-test's are)."""
    out = []
    for dirpath, _dirs, names in os.walk(root):
        out += [os.path.relpath(os.path.join(dirpath, n), root).replace(os.sep, "/") for n in names]
    return sorted(out)


def judged_files(root, rules, listed):
    """The files the scan judges: of LAB_COPY's paths and extensions, outside the gates' excluded folders."""
    return [f for f in listed if not scope.excluded(f, rules["exclude_dirs"]) and gate_lab.in_paths(rules["lab_copy"], f)]


def scan(root, rules, owners, files):
    """[(LAB_COPY, 'file:line', what (the reference's place))] of every file judged whole."""
    ctx = Context(root, files, files, rules, owners, {f: None for f in files}, Cache(root, enabled=root == REPO))
    out = []
    for n, f in enumerate(files, 1):
        src = ctx.source(f)
        if not src.binary:
            for line, reasons in sorted(gate_lab.findings(ctx, src).items()):
                out += [("LAB_COPY", f"{f}:{line}", f"{what} ({ref})") for what, ref in reasons]
        if n % PROGRESS_EVERY == 0:
            print(f"lab_scan: {n}/{len(files)} files, {len(out)} findings", file=sys.stderr, flush=True)
    return out


def report(found):
    """The findings, and their counts by folder and by reference file."""
    by_folder, by_reference = {}, {}
    for _gid, where, msg in found:
        folder = os.path.dirname(where.rsplit(":", 1)[0])
        by_folder[folder] = by_folder.get(folder, 0) + 1
        ref = msg[msg.rfind("(") + 1:-1].rsplit(":", 1)[0]
        by_reference[ref] = by_reference.get(ref, 0) + 1
    order = lambda d: dict(sorted(d.items(), key=lambda kv: (-kv[1], kv[0])))  # noqa: E731
    return {"findings": [{"where": w, "what": m} for _g, w, m in found], "by_folder": order(by_folder),
            "by_reference": order(by_reference)}


def run(root, rules, owners, listed, out_path):
    files = judged_files(root, rules, listed)
    try:
        lab_prints.reference_root(Context(root, files, files, rules, owners, {}, Cache(root, enabled=False)))
    except lab_prints.NoReference as e:
        print(f"[LAB_COPY] cannot scan without the reference: {e}")
        return 2
    started = time.monotonic()
    found = scan(root, rules, owners, files)
    data = report(found)
    os.makedirs(os.path.dirname(os.path.abspath(out_path)), exist_ok=True)
    with open(out_path, "w", encoding="utf-8", newline="\n") as fh:
        json.dump(data, fh, indent=1, ensure_ascii=False)
    for folder, n in list(data["by_folder"].items())[:20]:
        print(f"  {n:5d}  {folder}")
    print(f"lab_scan: {len(found)} finding(s) in {len(files)} file(s), {time.monotonic() - started:.0f} s; report {out_path}")
    return 1 if found else 0


def main(argv=None):
    ap = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    ap.add_argument("--out", default=os.path.join(REPO, "dist", "lab_scan.json"), help="where the report is written")
    args = ap.parse_args(argv)
    sys.stdout.reconfigure(encoding="utf-8")
    rules, owners = load_config(REPO)
    return run(REPO, rules, owners, scope.listed_files(REPO), args.out)


if __name__ == "__main__":
    sys.exit(main())
