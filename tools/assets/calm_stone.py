"""The stone finish as stone is: dressed blocks of one quarry differ by a few percent, and the 4.0's recipe (bake/finishRecipes.ts,
`cellHash * 0.2`) sets them almost twice the light apart, which reads as camouflage from the street. Only the tone of each block
moves, to a spread of BLOCK_SPREAD: its joints and grain stay. The same run twice gives the same file.
Usage: python tools/assets/calm_stone.py [map png]"""
import sys

import numpy as np
from PIL import Image

STONE_LAYER = 4  # as it was baked, not the order of finishRecipes.ts LOOKS: the one of 3.2 m in materials.json finish_m with joints
ACROSS, ROWS = 5, 8  # the recipe's `bond`: five blocks a course, eight courses
JOINT_PX = 1.5  # what `bond` calls the joint, by the edge of a block
BLOCK_SPREAD = 4.0  # standard deviation of the blocks' tone, of 255 in sRGB: about 3% of the light


def block_of(size):
    """Each pixel's block, -1 on a joint: `bond` of the recipe, whose rows the bake laid from the bottom up."""
    v, u = np.mgrid[0:size, 0:size].astype(float)
    v = size - 1 - v
    h, w = size / ROWS, size / ACROSS
    row = np.floor(v / h)
    shifted = u + np.where(row % 2 == 1, w / 2, 0)
    col = np.floor(shifted / w) % ACROSS
    fu = shifted - np.floor(shifted / w) * w
    fv = v - row * h
    block = (row * ACROSS + col).astype(int)
    block[np.minimum(np.minimum(fu, w - fu), np.minimum(fv, h - fv)) < JOINT_PX] = -1
    return block


def main():
    path = sys.argv[1] if len(sys.argv) > 1 else "project/assets/surfaces/building-finishes.map.png"
    image = np.asarray(Image.open(path).convert("RGBA")).copy()
    size = image.shape[1]
    layer = image[STONE_LAYER * size:(STONE_LAYER + 1) * size].astype(float)
    block = block_of(size)
    tone = layer[..., :3].mean(axis=2)
    ids = np.unique(block[block >= 0])
    means = np.array([tone[block == b].mean() for b in ids])
    inside = np.array([tone[block == b].std() for b in ids]).mean()
    keep = min(1.0, BLOCK_SPREAD / means.std())
    print("blocks %d, spread %.1f -> %.1f, inside a block %.1f" % (ids.size, means.std(), means.std() * keep, inside))
    middle = means.mean()
    for b, m in zip(ids, means):
        layer[block == b, :3] += (middle + (m - middle) * keep) - m
    image[STONE_LAYER * size:(STONE_LAYER + 1) * size] = np.clip(np.round(layer), 0, 255).astype(np.uint8)
    Image.fromarray(image).save(path)


if __name__ == "__main__":
    main()
