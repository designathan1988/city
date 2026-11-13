"""The whole bench: for each project of `projects.json` whose folders exist, the tracer measured against the
compiler's answer (made when missing), compared with the recorded baseline, and its golden traces.

    python bench/run.py [--only NAME]... [--update] [--strict]

A path a project names is from the repository, or `@name/...` where tools/tasks/locations.py finds `name` on this machine (the
reference's tree, the toolchain): no machine's path is written in projects.json.

Exit 1 when a figure is worse than the baseline beyond the tolerance, or a golden trace fails, or no project is measured
(a bench of nothing proves nothing); with --strict, also when a target is not met. --update records the figures as the new baseline. Not part of the tracer.
"""
from __future__ import annotations

import argparse
import json
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
REPO = HERE.parents[2]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(REPO / "tools"))

import callers  # noqa: E402
import compare  # noqa: E402
import golden  # noqa: E402

# How much a figure may move against the baseline before it is a regression: the share for recall and precision
# (down), the ratio for the noise and the time (up).
TOLERANCE = {"recall": 0.005, "precision": 0.005, "links at platform calls (noise)": 0.10, "seconds": 0.25}


def place(path: str) -> str:
    """A path a project names, on this machine: `@name/rest` under what tools/tasks/locations.py finds for `name`, any other
    from the repository."""
    if path.startswith("@"):
        from tasks.locations import locate
        name, _, rest = path[1:].partition("/")
        return str(Path(locate(name)) / rest) if rest else locate(name)
    return path if Path(path).is_absolute() else str(REPO / path)


def placed(spec: dict) -> dict:
    """A project with every path it names placed on this machine (its oracle's files stay the bench's own)."""
    out = {**spec, "roots": [place(r) for r in spec["roots"]]}
    if spec.get("base"):
        out["base"] = place(spec["base"])
    for part in ("oracle", "callers"):
        if part in spec:
            out[part] = {k: place(v) if k in ("project", "build", "toolchain", "libclang") else v
                         for k, v in spec[part].items()}
    return out


def oracle(name: str, spec: dict) -> Path | None:
    """The oracle's file, made when it is missing."""
    out = HERE / spec["file"]
    if out.exists():
        return out
    out.parent.mkdir(parents=True, exist_ok=True)
    if spec["kind"] == "ts":
        command = ["node", str(HERE / "oracle" / "ts_calls.cjs"), spec["project"], str(out), spec.get("keep", "")]
    else:
        database = HERE / spec["database"]
        if not database.exists():
            database.parent.mkdir(parents=True, exist_ok=True)
            made = subprocess.run(["ninja", "-t", "compdb"], cwd=spec["build"], capture_output=True)
            if made.returncode:
                print(f"{name}: no compile database ({made.stderr.decode(errors='replace')[:200]})", file=sys.stderr)
                return None
            database.write_bytes(made.stdout)
        command = [sys.executable, str(HERE / "oracle" / "cpp_calls.py"), str(database), spec["project"], str(out)]
        if spec.get("toolchain"):
            command += ["--toolchain", spec["toolchain"]]
        if spec.get("libclang"):
            command += ["--libclang", spec["libclang"]]
    print(f"{name}: making the oracle ...", file=sys.stderr)
    done = subprocess.run(command)
    return out if done.returncode == 0 and out.exists() else None


def callers_oracle(name: str, spec: dict) -> Path:
    """The pairs of methods exposed across languages and their callers, read from the code when missing."""
    out = HERE / spec["file"]
    if not out.exists():
        command = [sys.executable, str(HERE / "oracle" / "godot_callers.py"), spec["project"], str(out)]
        for table in spec["table"]:
            command += ["--table", table]
        command += [word for via in spec.get("via", ()) for word in ("--via", via)]
        command += [word for one in spec.get("direct", ()) for word in ("--direct", one)]
        command += [word for one in spec.get("skip", ()) for word in ("--skip", one)]
        subprocess.run(command, check=True)
    return out


def figure(figures: dict, path: str):
    at = figures
    for part in path.split("."):
        at = at.get(part) if isinstance(at, dict) else None
    return at


def regressions(now: dict, before: dict) -> list:
    out = []
    for mode in ("exact", "all"):
        for key in ("recall", "precision"):
            a, b = figure(now, f"{mode}.{key}"), figure(before, f"{mode}.{key}")
            if a is not None and b is not None and a < b - TOLERANCE[key]:
                out.append(f"{mode} {key} {b} -> {a}")
        key = "links at platform calls (noise)"
        a, b = figure(now, f"{mode}.{key}"), figure(before, f"{mode}.{key}")
        if a is not None and b is not None and a > b * (1 + TOLERANCE[key]) + 10:
            out.append(f"{mode} noise {b} -> {a}")
    for key, worse in (("found", -1), ("false", 1)):
        a, b = figure(now, f"callers.{key}"), figure(before, f"callers.{key}")
        if a is not None and b is not None and (a - b) * worse > 0.005:
            out.append(f"callers {key} {b} -> {a}")
    a, b = now.get("seconds"), before.get("seconds")
    if a is not None and b is not None and a > b * (1 + TOLERANCE["seconds"]) + 2:
        out.append(f"seconds {b} -> {a}")
    return out


def unmet(now: dict, targets: dict) -> list:
    """Targets: a positive number is a floor, a negative one a ceiling (its absolute value)."""
    out = []
    for path, goal in targets.items():
        value = figure(now, path)
        if value is None:
            continue
        if (goal >= 0 and value < goal) or (goal < 0 and value > -goal):
            out.append(f"{path} = {value} ({'at least' if goal >= 0 else 'at most'} {abs(goal)})")
    return out


def summary(figures: dict) -> str:
    parts = []
    for mode in ("exact", "all"):
        m = figures.get(mode) or {}
        parts.append(f"{mode}: recall {m.get('recall')} precision {m.get('precision')} "
                     f"noise {m.get('links at platform calls (noise)')}")
    return " | ".join(parts) + f" | {figures.get('links')} links, {figures.get('seconds')} s"


def main(argv=None) -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument("--only", action="append")
    parser.add_argument("--update", action="store_true")
    parser.add_argument("--strict", action="store_true")
    args = parser.parse_args(argv)
    projects = json.loads((HERE / "projects.json").read_text(encoding="utf-8"))
    failed, measured = False, 0
    for name, spec in projects.items():
        if args.only and name not in args.only:
            continue
        try:
            spec = placed(spec)
        except Exception as e:           # a place this machine does not name (tasks.locations.Missing): said, skipped
            print(f"{name}: skipped, {e}", file=sys.stderr)
            continue
        if not all(Path(root).exists() for root in spec["roots"]):
            print(f"{name}: skipped, a root is missing", file=sys.stderr)
            continue
        print(f"== {name}")
        measured += 1
        figures, result = None, None
        if "oracle" in spec:
            found = oracle(name, spec["oracle"])
            if found is None:                    # no compiler's answer: nothing to measure the links against, said
                print("   FAILED: the compiler's answer could not be made (its errors above): nothing measured")
                failed = True
                continue
            else:
                rows = [json.loads(line) for line in found.read_text(encoding="utf-8").splitlines() if line.strip()]
                links, result, seconds = compare.links_by_call(spec["roots"], spec.get("base"))
                figures = compare.measure(rows, links)
                figures["seconds"] = round(seconds, 1)
                figures["links"] = sum(len(found) for found in result.out.values())
        if result is None:
            began = time.monotonic()
            result = golden.analysed(spec["roots"], spec.get("base"))
            figures = {"seconds": round(time.monotonic() - began, 1),
                       "links": sum(len(found) for found in result.out.values())}
        golden._DONE[(tuple(spec["roots"]), spec.get("base"))] = result
        if "callers" in spec:
            figures["callers"] = callers.measure(result, callers_oracle(name, spec["callers"]),
                                                 spec["callers"]["language"])
            c = figures["callers"]
            print(f"   callers across languages: {c['pairs']} pairs, found {c['found']}, false {c['false']} "
                  f"of {c['callers presented straight']} presented")
        print("   " + summary(figures))
        baseline = HERE / "baseline" / f"{name}.json"
        if baseline.exists() and not args.update:
            worse = regressions(figures, json.loads(baseline.read_text(encoding="utf-8")))
            for line in worse:
                print(f"   REGRESSION {line}")
            failed = failed or bool(worse)
        missing = unmet(figures, spec.get("targets", {}))
        for line in missing:
            print(f"   target not met: {line}")
        failed = failed or (args.strict and bool(missing))
        if spec.get("golden"):
            failed = golden.run([HERE / one for one in spec["golden"]]) != 0 or failed
        if args.update:
            baseline.parent.mkdir(parents=True, exist_ok=True)
            baseline.write_text(json.dumps(figures, indent=1, ensure_ascii=False) + "\n", encoding="utf-8")
            print(f"   baseline recorded: {baseline.relative_to(HERE)}")
    if not measured:
        print("nothing measured: no project of bench/projects.json has its roots here", file=sys.stderr)
        return 1
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main())
