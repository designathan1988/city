"""The graph against the program run: programs made up on every run (tests/linkproof.py), every way one function runs
another, run for real (node; godot, headless), each printing the links that happened. The links the tracer's trace
from the start says (calls, objects made, handlers fired: what runs, not what is registered) must be those, none
missing and none more (a function a decoy hands down the same chain is not reached). `TRACER_SEED`,
`TRACER_PROGRAMS` as for the other made-up programs.
"""
from __future__ import annotations

import os
import random
import shutil
import subprocess
import tempfile
import time
import unittest
from pathlib import Path

from support import HERE, analyse, compiler
from linkproof import made
from tracer.model import LAMBDA
from tracer.vocab import CALL, CLASS, EVERY, FIRES, NEW
from tracer.walk import find, forward

PROGRAMS = int(os.environ.get("TRACER_PROGRAMS", "4"))
SECONDS = 120


def happened(lang: str, folder: Path) -> set | None:
    """The links that happened when the program ran: {(caller, callee)}, or None where it could not run."""
    try:
        if lang == "ts":
            done = subprocess.run([shutil.which("node") or "node", "driver.mts"], cwd=folder, capture_output=True,
                                  text=True, timeout=SECONDS)
        elif lang == "cpp":
            cxx = compiler()
            if cxx is None:
                return None
            built = subprocess.run([cxx, "-std=c++17", "-O0", "-static", "driver.cpp", "sink.cpp", "-o",
                                    "program.exe"], cwd=folder, capture_output=True, text=True, timeout=SECONDS)
            if built.returncode != 0:
                print(built.stderr[:1500])
                return None
            done = subprocess.run([str(folder / "program.exe")], cwd=folder, capture_output=True, text=True,
                                  timeout=SECONDS)
        else:
            engine = shutil.which("godot") or shutil.which("godot4")
            if engine is None:
                return None
            done = subprocess.run([engine, "--headless", "--path", str(folder), "--script", "res://driver.gd"],
                                  cwd=folder, capture_output=True, text=True, timeout=SECONDS)
    except (OSError, subprocess.SubprocessError):
        return None
    if done.returncode != 0:
        print(done.stderr[:1500])
        return None
    out = set()
    for line in done.stdout.splitlines():
        parts = line.split()
        if len(parts) == 3 and parts[0] == "edge" and parts[1] != "-":
            out.add((parts[1], parts[2]))
    return out


def traced(lang: str, folder: Path) -> set:
    """The links the trace from the start says run: {(caller, callee)} by the names the program enters with. A
    lambda enters with no name: what it runs is run by the function running it (as the program prints it)."""
    name = {"ts": "program.mts", "gd": "program.gd", "cpp": "program.cpp"}[lang]
    result = analyse([folder / name], folder)
    p = result.program
    starts = find(result, "start")

    def short(d) -> str:
        label = p.label(d)
        return label.split(".", 1)[1] if lang == "gd" and "." in label else label
    lambda_mark = LAMBDA.split("{")[0]
    nameless = lambda d: p.label(d).rsplit(".", 1)[-1].startswith(lambda_mark)       # noqa: E731
    edges = set()
    for step in forward(result, starts, handed=EVERY):
        link = step.link
        if link is not None and not step.leaf and link.kind in (CALL, FIRES, NEW) and \
                p.defs[link.dst].kind != CLASS and p.defs[link.dst].body:      # (code not read: no link of it)
            edges.add((link.src, link.dst))
    after = {}
    for a, b in edges:
        after.setdefault(a, set()).add(b)
    out = set()
    for a, b in edges:
        if nameless(a):
            continue
        todo, seen = [b], set()
        while todo:
            d = todo.pop()
            if d in seen:
                continue
            seen.add(d)
            if nameless(d):
                todo += after.get(d, ())
            else:
                out.add((short(a), short(d)))
    return out


class LinksRun(unittest.TestCase):
    def test_the_graph_is_what_runs(self):
        seed = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))
        rng = random.Random(seed)
        work = Path(tempfile.mkdtemp(prefix=".links-", dir=HERE))
        counts = {lang: {"programs": 0, "links run": 0, "missing": 0, "more": 0, "not run": 0} for lang in ("ts", "gd", "cpp")}
        wrong = []
        began = time.monotonic()
        try:
            for i in range(PROGRAMS):
                program = made(rng)
                for lang, files in program.files.items():
                    folder = work / f"p{i}" / lang
                    folder.mkdir(parents=True)
                    for name, text in files.items():
                        (folder / name).write_text(text, encoding="utf-8")
                    c = counts[lang]
                    c["programs"] += 1
                    real = happened(lang, folder)
                    if real is None:
                        c["not run"] += 1
                        continue
                    said = traced(lang, folder)
                    missing, more = real - said, said - real
                    c["links run"] += len(real)
                    c["missing"] += len(missing)
                    c["more"] += len(more)
                    if missing or more:
                        wrong.append((lang, i, sorted(missing), sorted(more), program.plan,
                                      files[{"ts": "program.mts", "gd": "program.gd", "cpp": "program.cpp"}[lang]]))
        finally:
            print(f"\n[links] seed {seed}, {time.monotonic() - began:.1f} s")
            for lang, c in counts.items():
                print(f"[links] {lang}: " + ", ".join(f"{k} {v}" for k, v in c.items()))
            if not os.environ.get("TRACER_KEEP"):
                shutil.rmtree(work, ignore_errors=True)
        for lang, i, missing, more, plan, text in wrong[:3]:
            print(f"--- {lang} program {i}: missing {missing}, more {more}\nplan {plan}\n{text}")
        self.assertEqual([], [(lang, i) for lang, i, *_ in wrong], f"seed {seed}")


if __name__ == "__main__":
    unittest.main()
