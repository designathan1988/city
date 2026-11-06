# Parallel suite runner: the scenes, renderers, time limit and failure rule of tools/build/run_tests.sh (read from there: one
# list), all at once, each in its own Godot process on the hidden desktop (tools/build/hidden_run.ps1). It writes the same logs to
# dist/tests that the commit hook checks. The wall time is the longest test's, not the sum: the rigour is the same, the wait is
# not. A test that fails under load runs again alone: passing then, it is reported UNSTABLE (timing-dependent), never hidden.
# Use: python tools/analysis/suite.py [--godot EXE] [--parallel N] [--only=a,b] [--timeout S] [--root DIR]
import argparse
import os
import re
import shutil
import subprocess
import sys
import time
from concurrent.futures import ThreadPoolExecutor

ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), "..", ".."))  # --root points at another copy
FAILED = re.compile(r"Program crashed|SCRIPT ERROR|^FAIL\b|FALHAS: [1-9]|FAILURES: [1-9]", re.M)  # the tests' own failure lines


def listed(name, text):
    m = re.search(r"^" + name + r"=\((.*?)\)", text, re.M)
    return m.group(1).split() if m else []


def cases():
    """(scene, renderer) in run_tests.sh's order: all in gl_compatibility, the BOTH_RENDERERS ones in forward_plus too."""
    text = open(os.path.join(ROOT, "tools", "build", "run_tests.sh"), encoding="utf-8").read()
    both = set(listed("BOTH_RENDERERS", text))
    out = []
    for t in listed("TESTS", text):
        out.append((t, "gl_compatibility"))
        if t in both:
            out.append((t, "forward_plus"))
    return out


def run(godot, scene, method, timeout):
    log = os.path.join(ROOT, "dist", "tests", f"{os.path.basename(scene)}_{method}.log")
    line = f'"{godot}" --path "{os.path.join(ROOT, "project")}" --rendering-method {method} "res://{scene}.tscn" -- --no-flow > "{log}" 2>&1'
    t0 = time.time()
    r = subprocess.run(["powershell", "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", os.path.join(ROOT, "tools", "build", "hidden_run.ps1"),
                        "-Command", line, "-TimeoutS", str(timeout), "-WorkDir", ROOT], capture_output=True, text=True)
    text = open(log, encoding="utf-8", errors="replace").read() if os.path.exists(log) else ""
    failed = r.returncode != 0 or bool(FAILED.search(text))
    why = "; ".join(l for l in text.splitlines() if FAILED.search(l))[:300]
    return scene, method, failed, r.returncode, time.time() - t0, why


def main():
    global ROOT
    ap = argparse.ArgumentParser()
    ap.add_argument("--godot", default=shutil.which("godot") or "godot")
    ap.add_argument("--parallel", type=int, default=max(2, (os.cpu_count() or 4) // 3))
    ap.add_argument("--only", default="")
    ap.add_argument("--timeout", type=int, default=900)
    ap.add_argument("--root", default=ROOT)
    a = ap.parse_args()
    ROOT = os.path.abspath(a.root)
    os.makedirs(os.path.join(ROOT, "dist", "tests"), exist_ok=True)
    wanted = [f for f in a.only.split(",") if f]
    todo = [c for c in cases() if not wanted or any(f in c[0] for f in wanted)]
    t0 = time.time()
    with ThreadPoolExecutor(max_workers=a.parallel) as ex:
        results = list(ex.map(lambda c: run(a.godot, c[0], c[1], a.timeout), todo))
    unstable, failures = [], 0
    for i, (scene, method, failed, code, took, why) in enumerate(results):
        if failed:
            log = os.path.join(ROOT, "dist", "tests", f"{os.path.basename(scene)}_{method}")
            shutil.copyfile(log + ".log", log + ".loaded")  # what failed under load: the run alone writes over the log
            again = run(a.godot, scene, method, a.timeout)  # alone: the log left in dist/tests is this run's
            if not again[2]:
                unstable.append(f"{scene} ({method}): {why}")
                results[i] = again
    for scene, method, failed, code, took, why in results:
        failures += failed
        print(f"{'FAILED' if failed else 'ok'} {scene} ({method}) {took:.0f} s" + (f" exit {code}: {why}" if failed else ""))
    for x in unstable:
        print(f"UNSTABLE under load (passed alone): {x}")
    print(f"{len(results) - failures}/{len(results)} ok, {len(unstable)} unstable, in {time.time() - t0:.0f} s ({a.parallel} in parallel)")
    sys.exit(1 if failures else 0)


if __name__ == "__main__":
    main()
