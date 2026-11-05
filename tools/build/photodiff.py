# photodiff.py <before dir> <after dir> <out dir>: every photo present in both, compared pixel by pixel.
# Per pair: mean absolute difference (0..255), the share of pixels changed by more than 24 levels, and the box of the
# changed pixels; a diff image (changes in red over the after photo, greyed) for every pair that changed.
# The HUD bar (top 60 px) and the dock (bottom 80 px) carry live numbers (clock, vehicles): masked.
import sys
from pathlib import Path

import numpy as np
from PIL import Image

before, after, out = map(Path, sys.argv[1:4])
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
for name, mean, share, box in sorted(rows, key=lambda r: -(r[2] or 0) if isinstance(r[2], float) else 1):
    if mean is None:
        print(f"{name:32s} sem par")
    elif mean == "size":
        print(f"{name:32s} tamanho diferente")
    else:
        print(f"{name:32s} média {mean:6.2f}  mudou {share * 100:6.2f}%  caixa {box}")
