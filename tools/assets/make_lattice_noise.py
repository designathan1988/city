"""Writes project/assets/noise/lattice.png: the values of a noise lattice, a tile the building shaders read in one filtered fetch.
Value noise is four hashes and a blend; with its values in a texture the blend is the sampler's bilinear filter, and a wall's
weathering costs four fetches where it cost sixteen integer hashes a pixel (building_common.gdshaderinc vnoise). Run once:
python3 tools/assets/make_lattice_noise.py
"""
import sys

import numpy as np
from PIL import Image

SIDE = 256  # lattice cells before it repeats: the widest use, 0.021 cells a metre, repeats every 12 km
SEED = 11
LEVELS = 256


def main(path: str) -> None:
    values = np.random.default_rng(SEED).integers(0, LEVELS, (SIDE, SIDE), dtype=np.uint8)
    Image.fromarray(values).save(path)  # 2-D uint8: mode L


if __name__ == "__main__":
    main(sys.argv[1] if len(sys.argv) > 1 else "project/assets/noise/lattice.png")
