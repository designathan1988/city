"""Fills the colour under the transparent part of a cut-out texture from the visible part next to it, alpha untouched.
The pack ships near white under the holes of Leaves.png, so each mip level averaged the white into the leaf and a plant turned pale
from afar. Pull-push (Gortler et al., The Lumigraph, 1996): the visible colours are averaged down a pyramid and each hole takes the
nearest level that holds any, so the fill reaches every texel; the import's fix_alpha_border only reaches the rim.
Run once per texture: python3 tools/assets/bleed_alpha.py <file.png> [<cut 0..1>]
"""
import sys

import numpy as np
from PIL import Image

DEFAULT_CUT = 0.2  # the alphaCutoff of the pack's glTF materials
BYTE_TOP = 255.0


def halve(a: np.ndarray) -> np.ndarray:
    h, w = a.shape[0] // 2 * 2, a.shape[1] // 2 * 2
    a = a[:h, :w]
    return (a[0::2, 0::2] + a[1::2, 0::2] + a[0::2, 1::2] + a[1::2, 1::2]) * 0.25


def grow(a: np.ndarray, shape: tuple[int, int]) -> np.ndarray:
    big = np.repeat(np.repeat(a, 2, axis=0), 2, axis=1)
    pad = ((0, max(0, shape[0] - big.shape[0])), (0, max(0, shape[1] - big.shape[1])), (0, 0))
    return np.pad(big, pad, mode='edge')[:shape[0], :shape[1]]


def fill(colour: np.ndarray, seen: np.ndarray) -> np.ndarray:
    """The colour wherever `seen` is 1, and the pyramid's colour where it is 0."""
    levels = [(colour * seen, seen)]
    while min(levels[-1][1].shape[:2]) > 1:
        c, w = levels[-1]
        levels.append((halve(c), halve(w)))
    c, w = levels[-1]
    flat = c / np.maximum(w, 1e-6)
    for c, w in reversed(levels[:-1]):
        up = grow(flat, w.shape[:2])
        flat = np.where(w > 0, c / np.maximum(w, 1e-6), up)
    return np.where(seen > 0, colour, flat)


def main() -> None:
    path = sys.argv[1]
    cut = float(sys.argv[2]) if len(sys.argv) > 2 else DEFAULT_CUT
    image = np.array(Image.open(path).convert('RGBA')).astype(np.float64)
    seen = (image[..., 3:4] >= cut * BYTE_TOP).astype(np.float64)
    image[..., :3] = fill(image[..., :3], seen)
    Image.fromarray(np.clip(image + 0.5, 0, BYTE_TOP).astype(np.uint8), mode='RGBA').save(path)


if __name__ == '__main__':
    main()
