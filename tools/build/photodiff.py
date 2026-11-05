# photodiff.py <before dir> <after dir> <out dir> [limit %]: every photo present in both, compared pixel by pixel. With a limit
# only the photos past it are told, and any of them ends in exit 1.
# Per pair: mean absolute difference (0..255), the share of pixels changed by more than 24 levels, and the box of the
# changed pixels; a diff image (changes in red over the after photo, greyed) for every pair that changed.
# The HUD bar (top 60 px) and the dock (bottom 80 px) carry live numbers (clock, vehicles): masked.
import sys
from pathlib import Path

import numpy as np
from PIL import Image

before, after, out = map(Path, sys.argv[1:4])
limit = float(sys.argv[4]) if len(sys.argv) > 4 else None
out.mkdir(parents=True, exist_ok=True)
CHANGED = 24
HUD_TOP, HUD_BOTTOM = 60, 80
rows = []
for a in sorted(after.glob("*.png")):
    b = before / a.name
    if not b.exists():
        rows.append((a.name, None, None, None))
        continue
    x = np.asarray(Image.open(b).convert("RGB"), dtype=np.int16)
    y = np.asarray(Image.open(a).convert("RGB"), dtype=np.int16)
    if x.shape != y.shape:
        rows.append((a.name, "size", None, None))
        continue
    d = np.abs(x - y).max(axis=2)
    d[:HUD_TOP] = 0
    d[-HUD_BOTTOM:] = 0
    mask = d > CHANGED
    share = mask.mean()
    box = None
    if mask.any():
        ys, xs = np.nonzero(mask)
        box = (int(xs.min()), int(ys.min()), int(xs.max()), int(ys.max()))
        grey = (y.mean(axis=2, keepdims=True) * 0.5).repeat(3, axis=2)
        grey[mask] = [255, 0, 0]
        Image.fromarray(grey.astype(np.uint8)).save(out / a.name)
    rows.append((a.name, float(d.mean()), float(share), box))
over = [r for r in rows if not isinstance(r[2], float) or r[2] * 100 > (limit or 0)] if limit is not None else rows
for name, mean, share, box in sorted(over, key=lambda r: -(r[2] or 0) if isinstance(r[2], float) else 1):
    if mean is None:
        print(f"{name:32s} no pair")
    elif mean == "size":
        print(f"{name:32s} different size")
    else:
        print(f"{name:32s} mean {mean:6.2f}  changed {share * 100:6.2f}%  box {box}")
if limit is not None:
    print(f"{len(rows) - len(over)} of {len(rows)} photos equal")
    sys.exit(1 if over else 0)
