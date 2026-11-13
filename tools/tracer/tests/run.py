"""Every test of the tracer, each file in a process of its own, its lines shown as they come (each test's name and
result, each proof's numbers), with a time limit: a file that fails, breaks or runs past its limit is said so and the
next one runs anyway. At the end, one line per file and the total; the exit code is 0 only when every file passed.

    python tests/run.py                  every file
    python tests/run.py links shaders    the files whose names hold these words
    TRACER_LIMIT=300                     seconds one file may take (default 600)
"""
from __future__ import annotations

import os
import subprocess
import sys
import threading
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
LIMIT = float(os.environ.get("TRACER_LIMIT", "600"))


def run(path: Path) -> tuple:
    """One test file run, its output shown line by line as it comes: (outcome, seconds)."""
    began = time.monotonic()
    env = dict(os.environ, PYTHONUNBUFFERED="1")
    proc = subprocess.Popen([sys.executable, "-u", "-m", "unittest", "-v", path.stem], cwd=HERE, env=env,
                            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, text=True, encoding="utf-8",
                            errors="replace")
    timer = threading.Timer(LIMIT, proc.kill)
    timer.start()
    last = ""
    try:
        for line in proc.stdout:
            print(f"  {line}", end="", flush=True)
            if line.strip():
                last = line.strip()
        proc.wait()
    finally:
        timer.cancel()
    seconds = time.monotonic() - began
    if seconds >= LIMIT:
        return f"STOPPED past {LIMIT:.0f} s", seconds
    if proc.returncode == 0:
        return ("OK" + (" (skipped some)" if "skipped" in last else "")), seconds
    return f"FAILED ({last[:80]})", seconds


def main(words: list) -> int:
    files = sorted(HERE.glob("test_*.py"))
    if words:
        files = [f for f in files if any(w in f.stem for w in words)]
    done = []
    for i, path in enumerate(files, 1):
        print(f"\n=== [{i}/{len(files)}] {path.stem}", flush=True)
        outcome, seconds = run(path)
        print(f"=== {path.stem}: {outcome}, {seconds:.1f} s", flush=True)
        done.append((path.stem, outcome, seconds))
    print("\n" + "=" * 72)
    for name, outcome, seconds in done:
        print(f"{outcome[:60]:<60} {seconds:7.1f} s  {name}")
    failed = [name for name, outcome, _ in done if not outcome.startswith("OK")]
    print(f"{len(done) - len(failed)} of {len(done)} files passed" + (f"; not passed: {', '.join(failed)}"
                                                                        if failed else ""))
    return 1 if failed else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
