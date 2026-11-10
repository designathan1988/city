# photodiff.py <before dir> <after dir> <out dir>: every photo name of either folder, compared pixel by pixel. A photo on one
# side only, a different size, an empty set or any changed pixel fails (exit 1): the frozen photos are bit-exact. Per pair:
# the share of changed pixels, the mean absolute difference (0..255) and the mean shift of each channel (after - before), so a
# uniform shift is told apart from a local change; for a pair that changed, a "where to look" image: the after photo greyed,
# pixels changed by more than LOOK_LEVELS in red. The HUD bar (top 60 px) and the dock (bottom 80 px) carry live numbers
# (clock, vehicles): masked. Usage error: exit 2.
import sys
from pathlib import Path

import numpy as np
from PIL import Image

LOOK_LEVELS = 24  # only for the red image: below it a change is real but not worth the eye's search
HUD_TOP, HUD_BOTTOM = 60, 80
GREY = 0.5


def compare(before: Path, after: Path, out: Path) -> tuple[list[tuple], int]:
    """(rows, failures): one row per photo name, (name, verdict, details)."""
    rows, failures = [], 0
    names = sorted({p.name for p in before.glob("*.png")} | {p.name for p in after.glob("*.png")})
    for name in names:
        b, a = before / name, after / name
        if not b.exists() or not a.exists():
            rows.append((name, "MISSING", f"only in {'after' if a.exists() else 'before'}"))
            failures += 1
            continue
        x = np.asarray(Image.open(b).convert("RGB"), dtype=np.int16)
        y = np.asarray(Image.open(a).convert("RGB"), dtype=np.int16)
        if x.shape != y.shape:
            rows.append((name, "SIZE", f"{x.shape[1]}x{x.shape[0]} -> {y.shape[1]}x{y.shape[0]}"))
            failures += 1
            continue
        keep = np.ones(x.shape[:2], dtype=bool)
        keep[:HUD_TOP] = False
        keep[-HUD_BOTTOM:] = False
        signed = (y - x)[keep]  # (pixels, 3)
        changed = np.any(signed != 0, axis=1)
        if not changed.any():
            rows.append((name, "EQUAL", ""))
            continue
        failures += 1
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
    return rows, failures


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
    rows, failures = compare(before, after, out)
    if not rows:
        print(f"FAIL no photo in {before} nor {after}: an empty set proves nothing")
        return 1
    for name, verdict, details in rows:
        if verdict != "EQUAL":
            print(f"FAIL {name:32s} {verdict}  {details}")
    print(f"{len(rows) - failures} of {len(rows)} photos equal")
    return 1 if failures else 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
