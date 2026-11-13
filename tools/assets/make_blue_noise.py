"""Writes project/assets/noise/blue_noise.png: a tile of blue noise, the start of each ray of the atmosphere pass.
The pass has no frames to average over, so its start must look like nothing on its own: interleaved gradient noise leaves a
diagonal weave at 0.44, 0.31 cycles a pixel, and this has no line of any kind. Void and cluster (Ulichney, SPIE 1913, 1993): the
tightest cluster and the largest void of a binary pattern under a Gaussian on the torus, ranked one pixel at a time, so every
threshold of the tile is itself blue and the tile repeats without a seam. Run once: python3 tools/assets/make_blue_noise.py <out.png>
"""
import sys

import numpy as np
from PIL import Image

SIDE = 64  # 4096 ranks, 16 to each of the 256 levels of the file
SIGMA = 1.5  # the paper's filter
START_SHARE = 0.1  # of the pixels set in the first pattern
SEED = 7
LEVELS = 256


def gaussian() -> np.ndarray:
    """The filter on the torus, centred on pixel 0, 0."""
    d = np.minimum(np.arange(SIDE), SIDE - np.arange(SIDE)).astype(float)
    return np.exp(-(d[:, None] ** 2 + d[None, :] ** 2) / (2.0 * SIGMA**2))


def energy_of(pattern: np.ndarray, kernel: np.ndarray) -> np.ndarray:
    return np.real(np.fft.ifft2(np.fft.fft2(pattern.astype(float)) * np.fft.fft2(kernel)))


def stamp(energy: np.ndarray, kernel: np.ndarray, at: tuple[int, int], sign: float) -> None:
    energy += sign * np.roll(kernel, at, axis=(0, 1))


def tightest(energy: np.ndarray, pattern: np.ndarray) -> tuple[int, int]:
    return np.unravel_index(np.argmax(np.where(pattern, energy, -np.inf)), energy.shape)


def largest_void(energy: np.ndarray, pattern: np.ndarray) -> tuple[int, int]:
    return np.unravel_index(np.argmin(np.where(pattern, np.inf, energy)), energy.shape)


def first_pattern(kernel: np.ndarray) -> np.ndarray:
    """A random pattern evened out: the tightest point moves to the largest void until it would land where it left."""
    rng = np.random.default_rng(SEED)
    pattern = np.zeros(SIDE * SIDE, dtype=bool)
    pattern[rng.choice(SIDE * SIDE, int(SIDE * SIDE * START_SHARE), replace=False)] = True
    pattern = pattern.reshape(SIDE, SIDE)
    energy = energy_of(pattern, kernel)
    while True:
        cluster = tightest(energy, pattern)
        pattern[cluster] = False
        stamp(energy, kernel, cluster, -1.0)
        void = largest_void(energy, pattern)
        pattern[void] = True
        stamp(energy, kernel, void, 1.0)
        if void == cluster:
            return pattern


def ranks(kernel: np.ndarray) -> np.ndarray:
    start = first_pattern(kernel)
    out = np.zeros((SIDE, SIDE), dtype=np.int32)
    ones = int(start.sum())
    # Down from the first pattern: the tightest point goes first and takes the highest rank left.
    pattern = start.copy()
    energy = energy_of(pattern, kernel)
    for rank in range(ones - 1, -1, -1):
        at = tightest(energy, pattern)
        pattern[at] = False
        stamp(energy, kernel, at, -1.0)
        out[at] = rank
    # Up to half: the largest void fills next.
    pattern = start.copy()
    energy = energy_of(pattern, kernel)
    half = SIDE * SIDE // 2
    for rank in range(ones, half):
        at = largest_void(energy, pattern)
        pattern[at] = True
        stamp(energy, kernel, at, 1.0)
        out[at] = rank
    # Past half the holes are the minority: the tightest hole fills next.
    energy = energy_of(~pattern, kernel)
    for rank in range(half, SIDE * SIDE):
        at = tightest(energy, ~pattern)
        pattern[at] = True
        stamp(energy, kernel, at, -1.0)
        out[at] = rank
    return out


def main() -> None:
    tile = ranks(gaussian())
    assert sorted(tile.ravel().tolist()) == list(range(SIDE * SIDE))
    Image.fromarray((tile * LEVELS // (SIDE * SIDE)).astype(np.uint8)).save(sys.argv[1])  # 2-D uint8: mode L


if __name__ == "__main__":
    main()
