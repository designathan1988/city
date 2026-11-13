"""One command: the exact graphs of the reference game and of this game (kept; only what changed is read again), then
one functionality from both, side by side, with the values one has that the other has not and the cost of each.

  python compare.py --game-points P[,P] --ref-points P[,P] [--frame NAME,...] [--depth N] [--out FILE.html]

The game: C++ through clang with the build's compile commands, GDScript through its grammar, joined by the names the
C++ registers; data in project/data. The reference: TypeScript through its own compiler. Nothing is run but the
compilers' readers; nothing of the reference is copied: it is read for what it does.
"""
from __future__ import annotations

import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
GAME = HERE.parent.parent
REF = Path(r"C:\Users\Jonathan\Documents\Projects\lab\roadcraft")
TOOLCHAIN = r"C:\Projects\toolchains\llvm-mingw-20261020-ucrt-x86_64"
OUT = GAME / "dist" / "feature"          # the graphs kept between runs and the sheets (suites.json lifecycle)


def run(cmd: list, what: str):
    t0 = time.monotonic()
    p = subprocess.run(cmd, capture_output=True, text=True)
    print(f"{what}: {p.stderr.strip().splitlines()[-1] if p.stderr.strip() else ''} ({time.monotonic() - t0:.0f} s)")
    if p.returncode != 0:
        print(p.stderr[-3000:])
        sys.exit(p.returncode)


def main() -> int:
    a = sys.argv[1:]
    opt = lambda k, d=None: a[a.index(k) + 1] if k in a else d
    OUT.mkdir(parents=True, exist_ok=True)
    cpp, gd, ts = OUT / "game_cpp.json", OUT / "game_gd.json", OUT / "ref_ts.json"
    run([sys.executable, str(HERE / "cpp_graph.py"), str(GAME / "native" / "build-win"), str(GAME / "native" / "src"),
         TOOLCHAIN, str(cpp), "--jobs", "8"], "game C++ (clang)")
    run([sys.executable, str(HERE / "gd_graph.py"), str(GAME / "project"), str(gd), "--cpp", str(cpp)], "game GDScript")
    newest = max([p.stat().st_mtime for p in (REF / "src").rglob("*.ts")] + [(HERE / "ts_graph.js").stat().st_mtime])
    if not ts.exists() or "--fresh" in a or ts.stat().st_mtime < newest:     # its code or the reader changed
        run(["node", "--max-old-space-size=4096", str(HERE / "ts_graph.js"), str(REF), str(REF / "src"), str(ts)],
            "reference TypeScript (tsc)")
    cmd = [sys.executable, str(HERE / "feature.py"), f"{cpp}+{gd}", opt("--game-points"), "--data", str(GAME / "project" / "data"),
           "--vs", str(ts), opt("--ref-points"), "--depth", opt("--depth", "3"), "--out", opt("--out", str(OUT / "feature.html"))]
    if opt("--frame"):
        cmd += ["--frame", opt("--frame")]
    p = subprocess.run(cmd, capture_output=True, text=True)
    print(p.stdout[-200:] if p.returncode == 0 else p.stderr[-3000:])
    return p.returncode


if __name__ == "__main__":
    sys.exit(main())
