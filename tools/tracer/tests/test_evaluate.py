"""The tracer's evaluation against the programs run for real: programs made up on every run (tests/programs.py), the
same program in TypeScript, C++ and GDScript, each compiled and run with its situation (node; clang++; godot,
headless), and evaluated by the tracer from its code alone. The outputs must be the same, in order and to the bit.
A result the tracer says partial is counted apart (never as a match); a language whose compiler or runtime is not
there is said not run. `TRACER_SEED`, `TRACER_PROGRAMS` as for the other made-up programs.
"""
from __future__ import annotations

import os
import random
import shutil
import struct
import subprocess
import tempfile
import time
import unittest
from pathlib import Path

from support import HERE, analyse, compiler
from programs import made
from tracer.evaluate import Unknown, evaluate
from tracer.walk import find

PROGRAMS = int(os.environ.get("TRACER_PROGRAMS", "3"))
SECONDS = 120


def ran(lang: str, folder: Path) -> list | None:
    """The outputs of the program run for real: [(tag, kind, value)], or None where it could not run."""
    try:
        if lang == "ts":
            done = subprocess.run([shutil.which("node") or "node", "driver.mts"], cwd=folder, capture_output=True,
                                  text=True, timeout=SECONDS)
        elif lang == "cpp":
            cxx = compiler()
            if cxx is None:
                return None
            built = subprocess.run([cxx, "-std=c++17", "-O0", "-static", "driver.cpp", "sink.cpp", "-o",
                                    "program.exe"],
                                   cwd=folder, capture_output=True, text=True, timeout=SECONDS)
            if built.returncode != 0:
                print(built.stderr[:2000])
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
    out = []
    for line in done.stdout.splitlines():
        parts = line.strip().split()
        if len(parts) == 3 and parts[0].startswith("o") and parts[0][1:].isdigit() and parts[1] in ("i", "f"):
            value = int(parts[2]) if parts[1] == "i" else struct.unpack("<d", bytes.fromhex(parts[2]))[0]
            out.append((parts[0], parts[1], value))
    return out if done.returncode == 0 else None


def predicted(lang: str, folder: Path, steps: list) -> tuple:
    """The tracer's outputs from the program's code alone: ([(tag, kind, value)], partial)."""
    name = {"ts": "program.mts", "cpp": "program.cpp", "gd": "program.gd"}[lang]
    result = analyse([folder / name], folder, keep=True)
    plan = []
    for label, args in steps:
        found = find(result, label)
        if not found:
            return [], True
        plan.append((found[0], args))
    run = evaluate(result, plan)
    out, partial = [], bool(run.partial)
    for o in run.outputs:
        if len(o.args) != 2:
            continue
        tag, value = o.args[0].value, o.args[1].value
        if isinstance(value, Unknown) or o.partial:
            partial = True
            continue
        if lang == "ts" or isinstance(value, float):
            out.append((tag, "f", float(value)))
        else:
            out.append((tag, "i", int(value)))
    return out, partial


def same(one: list, two: list) -> bool:
    if len(one) != len(two):
        return False
    for (t1, k1, v1), (t2, k2, v2) in zip(one, two):
        if t1 != t2 or k1 != k2:
            return False
        if k1 == "f" and struct.pack("<d", v1) != struct.pack("<d", v2) and not (v1 != v1 and v2 != v2):
            return False
        if k1 == "i" and v1 != v2:
            return False
    return True


class Evaluate(unittest.TestCase):
    def test_the_evaluation_is_what_the_programs_do(self):
        seed = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))
        rng = random.Random(seed)
        work = Path(tempfile.mkdtemp(prefix=".evaluate-", dir=HERE))
        counts = {lang: {"programs": 0, "situations": 0, "outputs": 0, "divergences": 0, "partial": 0,
                         "not run": 0} for lang in ("ts", "cpp", "gd")}
        began = time.monotonic()
        wrong = []
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
                    c["situations"] += len(program.steps)
                    real = ran(lang, folder)
                    if real is None:
                        c["not run"] += 1
                        continue
                    guess, partial = predicted(lang, folder, program.steps)
                    if partial:
                        c["partial"] += 1
                        continue
                    c["outputs"] += len(real)
                    if not same(real, guess):
                        c["divergences"] += 1
                        wrong.append((lang, i, real, guess, files["program." + {"ts": "mts", "cpp": "cpp",
                                                                                   "gd": "gd"}[lang]]))
        finally:
            print(f"\n[evaluate] seed {seed}, {time.monotonic() - began:.1f} s")
            for lang, c in counts.items():
                print(f"[evaluate] {lang}: " + ", ".join(f"{k} {v}" for k, v in c.items()))
            if not os.environ.get("TRACER_KEEP"):
                shutil.rmtree(work, ignore_errors=True)
        for lang, i, real, guess, text in wrong[:3]:
            print(f"--- {lang} program {i}\n{text}\nreal  {real}\nguess {guess}")
        self.assertEqual([], [(lang, i) for lang, i, *_ in wrong], f"seed {seed}")
        self.assertTrue(any(c["outputs"] for c in counts.values()), "no language ran: nothing was compared")


if __name__ == "__main__":
    unittest.main()
