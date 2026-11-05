# hem_posed.py SRC: the walk skinned on the CPU exactly as people.gdshader does (pos = sum w_i * M_i * p, M_i the 3x4
# palette rows), per variation: in the bind pose how far the bottom garment's top edge lies from the top garment's hem
# round the body, and per frame how many bottom-garment vertices end up above the deformed hem and outside the top.
import json
import sys

import numpy as np

src = sys.argv[1]
meta = json.load(open(f"{src}/people.json"))
raw = open(f"{src}/people.bin", "rb").read()
lay = meta["layout"]


def section(name, dtype):
    s = lay[name]
    return np.frombuffer(raw, dtype=dtype, count=s["count"], offset=s["byteOffset"])


vpos = section("vpos", "<f4").reshape(-1, 4)
vattr = section("vattr", "<u4").reshape(-1, 4)
index = section("index", "<u4")
nb = len(meta["bones"])
palette = section("palette", "<f4").reshape(-1, nb, 12)
TOP, BOTTOM, BINS = 2, 3, 24


def skin(ids, frame):
    p = np.concatenate([vpos[ids, :3], np.ones((len(ids), 1), np.float32)], axis=1)
    b = np.stack([(vattr[ids, 2] >> (8 * k)) & 0xFF for k in range(4)], axis=1)
    w = np.stack([(vattr[ids, 3] >> (8 * k)) & 0xFF for k in range(4)], axis=1).astype(np.float32)
    w /= np.maximum(w.sum(axis=1, keepdims=True), 1)
    out = np.zeros((len(ids), 3), np.float32)
    for k in range(4):
        m = frame[np.minimum(b[:, k], nb - 1)].reshape(-1, 3, 4)
        out += w[:, k:k + 1] * np.einsum("nij,nj->ni", m, p)
    return out


for vi, v in enumerate(meta["variations"][:10]):
    first, tris = v["lod"][0]
    used = np.unique(index[first:first + tris * 3])
    reg = vattr[used, 1] >> 24
    tops, bots = used[reg == TOP], used[reg == BOTTOM]
    if len(tops) == 0 or len(bots) == 0:
        continue
    walk = meta["clipSets"][v["clipSet"]][0]
    start = v["paletteStart"] + walk["start"]
    gaps, pokes = [], []
    for f in [None] + list(range(walk["frames"])):
        tp = vpos[tops, :3] if f is None else skin(tops, palette[start + f])
        bp = vpos[bots, :3] if f is None else skin(bots, palette[start + f])
        cx, cz = bp[:, 0].mean(), bp[:, 2].mean()
        sec = lambda q: ((np.arctan2(q[:, 2] - cz, q[:, 0] - cx) + np.pi) / (2 * np.pi) * BINS).astype(int) % BINS
        ts, bs = sec(tp), sec(bp)
        # the torso part of the top only (sleeves hang far from the axis): within the pants' reach of the axis
        reach = np.hypot(bp[:, 0] - cx, bp[:, 2] - cz).max() * 1.6
        torso = np.hypot(tp[:, 0] - cx, tp[:, 2] - cz) < reach
        hem = np.full(BINS, np.inf)
        rad = np.zeros(BINS)
        for k in range(BINS):
            sel = (ts == k) & torso
            if np.any(sel):
                hem[k] = tp[sel, 1].min()
                low = sel & (tp[:, 1] < hem[k] + 0.08)
                rad[k] = np.hypot(tp[low, 0] - cx, tp[low, 2] - cz).max()
        if f is None:
            top_edge = np.array([bp[bs == k, 1].max() if np.any(bs == k) else -np.inf for k in range(BINS)])
            ok = np.isfinite(hem) & np.isfinite(top_edge)
            gaps = top_edge[ok] - hem[ok]  # > 0: the bottom reaches up under the top
        else:
            above = bp[:, 1] > hem[bs]
            outside = np.hypot(bp[:, 0] - cx, bp[:, 2] - cz) > rad[bs] - 0.005
            pokes.append(int((above & outside).sum()))
    print(f"var {vi}: borda da calça menos barra na referência: min {gaps.min():+.3f} med {np.median(gaps):+.3f} m; "
          f"vértices da calça acima da barra e por fora, por quadro: max {max(pokes)} média {np.mean(pokes):.1f}")
