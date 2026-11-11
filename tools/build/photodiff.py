# photodiff.py <before dir> <after dir> <out dir>: where two photo sets differ, a help to find where to look and never a
# verdict (two runs of the same code differ in pixels; a photo is judged on its own, CLAUDE.md). Every photo name of either
# folder is compared pixel by pixel and every difference reported: a photo on one side only, a different size, and per pair
# that changed the share of changed pixels, the mean absolute difference (0..255) and the mean shift of each channel
# (after - before), so a uniform shift is told apart from a local change, with a "where to look" image: the after photo greyed,
# pixels changed by more than LOOK_LEVELS in red. The HUD bar (top 60 px) and the dock (bottom 80 px) carry live numbers
# (clock, vehicles): masked. Exit 0 once it compared; 1 when it has nothing to compare (a missing folder, an empty set);
# 2 on a usage error.
import sys
from pathlib import Path

import numpy as np
from PIL import Image

LOOK_LEVELS = 24  # only for the red image: below it a change is real but not worth the eye's search
HUD_TOP, HUD_BOTTOM = 60, 80
GREY = 0.5


def compare(before: Path, after: Path, out: Path) -> tuple[list[tuple], int]:
    """(rows, differing): one row per photo name, (name, what, details)."""
    rows, differing = [], 0
    names = sorted({p.name for p in before.glob("*.png")} | {p.name for p in after.glob("*.png")})
    for name in names:
        b, a = before / name, after / name
        if not b.exists() or not a.exists():
            rows.append((name, "MISSING", f"only in {'after' if a.exists() else 'before'}"))
            differing += 1
            continue
        x = np.asarray(Image.open(b).convert("RGB"), dtype=np.int16)
        y = np.asarray(Image.open(a).convert("RGB"), dtype=np.int16)
        if x.shape != y.shape:
            rows.append((name, "SIZE", f"{x.shape[1]}x{x.shape[0]} -> {y.shape[1]}x{y.shape[0]}"))
            differing += 1
            continue
        keep = np.ones(x.shape[:2], dtype=bool)
        keep[:HUD_TOP] = False
        keep[-HUD_BOTTOM:] = False
        signed = (y - x)[keep]  # (pixels, 3)
        changed = np.any(signed != 0, axis=1)
        if not changed.any():
            rows.append((name, "EQUAL", ""))
            continue
        differing += 1
        shift = signed.mean(axis=0)
        level = np.abs(y - x).max(axis=2)
        level[~keep] = 0
        look = level > LOOK_LEVELS
        ys, xs = np.nonzero(level)
        box = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
        rows.append((name, "CHANGED", f"changed {changed.mean() * 100:.3f}%  mean {np.abs(signed).mean():.3f}  "
                                      f"shift R {shift[0]:+.3f} G {shift[1]:+.3f} B {shift[2]:+.3f}  box {box}"))
        grey = (y.mean(axis=2, keepdims=True) * GREY).repeat(3, axis=2)
        grey[look] = [255, 0, 0]
        out.mkdir(parents=True, exist_ok=True)
        Image.fromarray(grey.astype(np.uint8)).save(out / name)
    return rows, differing


def main(argv: list[str]) -> int:
    if len(argv) != 3:
        print("usage: photodiff.py <before dir> <after dir> <out dir>", file=sys.stderr)
        return 2
    before, after, out = map(Path, argv)
    missing = [str(d) for d in (before, after) if not d.is_dir()]
    if missing:
        print(f"FAIL no folder: {', '.join(missing)}")
        return 1
    out.mkdir(parents=True, exist_ok=True)
    rows, differing = compare(before, after, out)
    if not rows:
        print(f"FAIL no photo in {before} nor {after}: an empty set proves nothing")
        return 1
    for name, what, details in rows:
        if what != "EQUAL":
            print(f"DIFF {name:32s} {what}  {details}")
    print(f"{len(rows) - differing} of {len(rows)} photos equal; where they differ is in {out} (a help to look, not a verdict)")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
