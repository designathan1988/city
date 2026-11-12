"""The defect hunt, one command: `python tools/hunt/hunt.py` runs project/tests/hunt/hunt.tscn hidden through the orchestrator
(tools/tasks: a private desktop, a deadline, stopped by its PID), and prints every defect the core's probes found in the generated
town, most frequent first, each with how many, where, why and the code that owns its mechanism. The full list, with the first
cases of each class whole, is written to dist/hunt/hunt.json. Exit 1 when a defect is found or the run fails, 0 when none is."""
import json
import os
import subprocess
import sys
import time

ROOT = os.path.abspath(os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))
CLI = os.path.join(ROOT, "tools", "tasks", "cli.py")
SCENE = "res://tests/hunt/hunt.tscn"
OUT = os.path.join(ROOT, "dist", "hunt", "hunt.json")
TIMEOUT_S = 600
SLACK_S = 60  # what the orchestrator may take beyond the run's own deadline
ERRORS = ("ERROR:", "SCRIPT ERROR:", "SHADER ERROR:")  # the engine's error lines in a run's log


def godot():
    done = subprocess.run([sys.executable, CLI, "var", "godot"], cwd=ROOT, capture_output=True, text=True, timeout=60)
    return done.stdout.strip()


def main():
    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    if os.path.exists(OUT):
        os.remove(OUT)
    request = {"id": 1, "name": "hunt", "hidden": True, "timeout_s": TIMEOUT_S, "extra": [str(time.time_ns())],
               "argv": [godot(), "--path", "project", SCENE, "--", "--no-flow", "--out=" + OUT.replace("\\", "/")]}
    done = subprocess.run([sys.executable, CLI, "serve"], cwd=ROOT, input=json.dumps(request) + "\n", capture_output=True,
                          text=True, encoding="utf-8", timeout=TIMEOUT_S + SLACK_S)
    answers = [json.loads(line) for line in done.stdout.splitlines() if line.startswith("{")]
    answer = answers[-1] if answers else {"status": "BROKEN", "error": done.stderr.strip()[-400:]}
    if not os.path.isfile(OUT):
        print(f"hunt: the run wrote no list ({answer.get('status')}; log {answer.get('log', '')}; {answer.get('error', '')})")
        return 1
    with open(OUT, encoding="utf-8") as fh:
        found = json.load(fh)["defects"]
    # the engine's own errors are defects too (a shader that does not compile draws nothing), and the run's checks must pass
    log = answer.get("log", "")
    errors, failed = [], ""
    if log and os.path.isfile(log):
        with open(log, encoding="utf-8", errors="replace") as fh:
            lines = fh.read().splitlines()
        errors = [line.strip() for line in lines if line.lstrip().startswith(ERRORS)]
        failed = next((line for line in lines if line.startswith("FAILURES:")), "")
        for line in lines:
            if line.startswith("HUNT ") or line.startswith("FAIL"):
                print(line)
    for e in dict.fromkeys(errors):
        found.append({"kind": "engine_error", "count": errors.count(e), "why": e, "owner": "see the log", "where": None})
    print(f"hunt: {len(found)} defect class(es) in the town ({failed or 'no FAILURES line'}; log {log}); list: {OUT}")
    for d in found:
        where = f" at {d['where']}" if d.get("where") is not None else ""
        print(f"{d['count']:6d}  {d['kind']}{where}\n        {d['why']}\n        -> {d['owner']}")
    return 1 if found or answer.get("status") != "PASS" or failed != "FAILURES: 0" else 0


if __name__ == "__main__":
    sys.exit(main())
