"""The tracer's evaluation of shaders against the shaders run: shaders made up on every run (tests/shaders.py), each
compiled as C++ with the shading language's types and functions (a header writing each as GLSL defines it) and run in
software for each situation (the uniforms it sets), and evaluated by the tracer from the shader's code alone. The
colour written must be the same, component by component, as reals (a zero's sign is not kept by the shading
languages: GLSL 4.60 §4.7.1); a result the tracer says partial is counted apart, and none may be. `TRACER_SEED`,
`TRACER_PROGRAMS` as for the other made-up programs.
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
from shaders import driver, header, made
from tracer.evaluate import evaluate
from tracer.walk import find

PROGRAMS = int(os.environ.get("TRACER_PROGRAMS", "6"))
SECONDS = 120


def ran(folder: Path, shader) -> list | None:
    """The colour of each situation, run for real; None where the shader could not be built."""
    cxx = compiler()
    if cxx is None:
        return None
    built = subprocess.run([cxx, "-std=c++17", "-O0", "-ffp-contract=off", "-static", "driver.cpp", "-o",
                            "shader.exe"], cwd=folder, capture_output=True, text=True, timeout=SECONDS)
    if built.returncode != 0:
        print(built.stderr[:2000])
        return None
    out = []
    for situation in shader.situations:
        done = subprocess.run([str(folder / "shader.exe")] + [repr(situation[u]) for u in shader.uniforms],
                              cwd=folder, capture_output=True, text=True, timeout=SECONDS)
        if done.returncode != 0:
            return None
        out.append([struct.unpack("<f", bytes.fromhex(word)[::-1])[0] for word in done.stdout.split()])
    return out


def evaluated(folder: Path, shader) -> tuple:
    """The colour of each situation as the tracer evaluates it: ([[4 reals] or None], partial reasons)."""
    result = analyse([folder / "shader.frag"], folder, keep=True)
    start = find(result, "main")
    out, partial = [], []
    for situation in shader.situations:
        run = evaluate(result, [(start[0], [])], situation)
        partial += run.partial
        colour = next((v for k, v in run.state.items() if k.split(".")[-1] == "frag"), None)
        out.append(list(colour) if isinstance(colour, (tuple, list)) and len(colour) == 4 else None)
    return out, partial


def same(one: list, two: list) -> bool:
    return len(one) == len(two) and all(x == y or (x != x and y != y) for x, y in zip(one, two))


class ShadersRun(unittest.TestCase):
    def test_the_evaluation_is_what_the_shaders_write(self):
        seed = int(os.environ.get("TRACER_SEED") or random.SystemRandom().randrange(1 << 30))
        rng = random.Random(seed)
        work = Path(tempfile.mkdtemp(prefix=".shaders-", dir=HERE))
        counts = {"shaders": 0, "situations": 0, "compared": 0, "divergences": 0, "partial": 0, "not run": 0}
        wrong, began = [], time.monotonic()
        try:
            for i in range(PROGRAMS):
                shader = made(rng)
                folder = work / f"s{i}"
                folder.mkdir(parents=True)
                (folder / "shader.frag").write_text(shader.text, encoding="utf-8")
                (folder / "shading.h").write_text(header(), encoding="utf-8")
                (folder / "driver.cpp").write_text(driver(shader.uniforms), encoding="utf-8")
                counts["shaders"] += 1
                counts["situations"] += len(shader.situations)
                real = ran(folder, shader)
                if real is None:
                    counts["not run"] += 1
                    continue
                guess, partial = evaluated(folder, shader)
                if partial:
                    counts["partial"] += 1
                    wrong.append((i, "partial", partial[:3], shader.text))
                    continue
                for situation, r, g in zip(shader.situations, real, guess):
                    counts["compared"] += 1
                    if g is None or not same(r, g):
                        counts["divergences"] += 1
                        wrong.append((i, situation, (r, g), shader.text))
        finally:
            print(f"\n[shaders] seed {seed}, {time.monotonic() - began:.1f} s: " +
                  ", ".join(f"{k} {v}" for k, v in counts.items()))
            if not os.environ.get("TRACER_KEEP"):
                shutil.rmtree(work, ignore_errors=True)
        for i, situation, what, text in wrong[:3]:
            print(f"--- shader {i} {situation}: {what}\n{text}")
        self.assertEqual([], [(i, s) for i, s, *_ in wrong], f"seed {seed}")
        self.assertTrue(counts["compared"], "no shader ran: nothing was compared")


if __name__ == "__main__":
    unittest.main()
