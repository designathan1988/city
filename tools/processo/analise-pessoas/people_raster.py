# people_raster.py BIN OUT.png [frame]: draws every variation of our cooked people.bin (project/assets/people) LOD0,
# skinned on the CPU exactly as people.gdshader does, back faces culled, z-buffered, coloured by region (or by the
# vertex colour with --colour), from the front and from the back, around the waist. Analysis only (never shipped).
import struct
import sys

import numpy as np
from PIL import Image

path, out = sys.argv[1], sys.argv[2]
frame = int(sys.argv[3]) if len(sys.argv) > 3 and sys.argv[3].isdigit() else 0
colour_mode = '--colour' in sys.argv
raw = open(path, 'rb').read()
magic, version, nvar, nb, nlod, pw, ph = struct.unpack_from('<4sIIIIII', raw, 0)
off = struct.calcsize('<4sIIIIII')
heads = []
for v in range(nvar):
    row, frames, fps, adv, height, flags = struct.unpack_from('<IIfffI', raw, off)
    off += struct.calcsize('<IIfffI')
    lods = []
    for _ in range(nlod):
        vc, ic = struct.unpack_from('<II', raw, off)
        off += 8
        lods.append((vc, ic))
    heads.append((row, frames, lods))
vdt = np.dtype([('p', '<f4', 3), ('n', '<f4', 3), ('c', '<u4'), ('b', '<u4'), ('w', '<u4'), ('d', '<f4')])
meshes = []
for v in range(nvar):
    per = []
    for vc, ic in heads[v][2]:
        vert = np.frombuffer(raw, dtype=vdt, count=vc, offset=off)
        off += vc * vdt.itemsize
        idx = np.frombuffer(raw, dtype='<u2', count=ic, offset=off).astype(np.int64)
        off += ic * 2
        per.append((vert, idx))
    meshes.append(per)
pal = np.frombuffer(raw, dtype='<f4', count=pw * ph * 4, offset=off).reshape(ph, pw // 3, 12)
REGION = np.array([[230, 60, 40], [80, 220, 80], [40, 60, 230], [240, 230, 120], [230, 80, 230], [80, 230, 230], [255, 255, 255]] + [[128, 128, 128]] * 10)
W, H, SCALE = 160, 200, 300.0  # pixels, pixels per metre


def skin(vert, row):
    p = np.concatenate([vert['p'], np.ones((len(vert), 1), np.float32)], axis=1)
    b = np.stack([(vert['b'] >> (8 * k)) & 0xFF for k in range(4)], axis=1)
    w = np.stack([(vert['w'] >> (8 * k)) & 0xFF for k in range(4)], axis=1).astype(np.float32)
    w /= np.maximum(w.sum(axis=1, keepdims=True), 1)
    q = np.zeros((len(vert), 3), np.float32)
    for k in range(4):
        m = pal[row, np.minimum(b[:, k], pal.shape[1] - 1)].reshape(-1, 3, 4)
        q += w[:, k:k + 1] * np.einsum('nij,nj->ni', m, p)
    return q


LAYER = {}


def draw(q, idx, cols, facing, centre, reg=None):
    centre_z = centre[2]
    img = np.full((H, W, 3), 30, np.uint8)
    zb = np.full((H, W), np.inf)
    zt, zbot = np.full((H, W), np.inf), np.full((H, W), np.inf)
    x = (q[:, 0] - centre[0]) * facing
    sx, sy, sz = W / 2 + x * SCALE, H / 2 - (q[:, 1] - centre[1]) * SCALE, -q[:, 2] * facing
    for t in idx.reshape(-1, 3):
        x0, y0, x1, y1, x2, y2 = sx[t[0]], sy[t[0]], sx[t[1]], sy[t[1]], sx[t[2]], sy[t[2]]
        area = (x1 - x0) * (y2 - y0) - (x2 - x0) * (y1 - y0)
        if area >= 0:  # counter-clockwise in screen space (y down): a back face
            continue
        lo_x, hi_x = int(max(0, min(x0, x1, x2))), int(min(W - 1, max(x0, x1, x2)) + 1)
        lo_y, hi_y = int(max(0, min(y0, y1, y2))), int(min(H - 1, max(y0, y1, y2)) + 1)
        if lo_x >= hi_x or lo_y >= hi_y:
            continue
        gx, gy = np.meshgrid(np.arange(lo_x, hi_x) + 0.5, np.arange(lo_y, hi_y) + 0.5)
        w0 = ((x1 - gx) * (y2 - gy) - (x2 - gx) * (y1 - gy)) / area
        w1 = ((x2 - gx) * (y0 - gy) - (x0 - gx) * (y2 - gy)) / area
        w2 = 1 - w0 - w1
        inside = (w0 >= 0) & (w1 >= 0) & (w2 >= 0)
        z = w0 * sz[t[0]] + w1 * sz[t[1]] + w2 * sz[t[2]]
        sub = zb[lo_y:hi_y, lo_x:hi_x]
        win = inside & (z < sub)
        sub[win] = z[win]
        col = cols[t[0]]
        if reg is not None and '--sides' in sys.argv and reg[t].max() == 3:
            col = np.array([240, 230, 120] if sz[t].mean() < centre_z * facing * -1 + 1e9 and q[t, 2].mean() * facing > centre[2] * facing else [255, 140, 20], np.uint8)
        img[lo_y:hi_y, lo_x:hi_x][win] = col
        if reg is not None and reg[t].max() in (2, 3):
            layer = zt if reg[t].max() == 2 else zbot
            sl = layer[lo_y:hi_y, lo_x:hi_x]
            near = inside & (z < sl)
            sl[near] = z[near]
    LAYER['top'], LAYER['bottom'] = zt, zbot
    return img


tiles = []
DEPTH = []
for v in range(nvar):
    vert, idx = meshes[v][0]
    row = heads[v][0] + frame % max(1, heads[v][1])
    q = vert['p'].copy() if '--bind' in sys.argv else skin(vert, row)
    reg = (vert['c'] >> 24) & 0xFF
    if colour_mode:
        cols = np.stack([vert['c'] & 0xFF, (vert['c'] >> 8) & 0xFF, (vert['c'] >> 16) & 0xFF], axis=1).astype(np.uint8)
    else:
        cols = REGION[np.minimum(reg, len(REGION) - 1)].astype(np.uint8)
    bot = q[reg == 3]
    centre = (bot[:, 0].mean() if len(bot) else 0, bot[:, 1].max() if len(bot) else 0.9, bot[:, 2].mean() if len(bot) else 0)
    pair = []
    for facing in (1, -1):
        pair.append(draw(q, idx, cols, facing, centre, reg))
        both = np.isfinite(LAYER['top']) & np.isfinite(LAYER['bottom'])
        ahead = (LAYER['top'] - LAYER['bottom'])[both]  # > 0: the bottom garment in front of the top
        ahead = ahead[ahead > 0] * 100  # cm
        DEPTH.append((v, facing, int(both.sum()), len(ahead), float(np.percentile(ahead, 95)) if len(ahead) else 0.0, float(ahead.max()) if len(ahead) else 0.0))
    tiles.append(np.concatenate(pair, axis=1))
cols_n = 8
rows = [np.concatenate(tiles[i:i + cols_n] + [np.zeros_like(tiles[0])] * (cols_n - len(tiles[i:i + cols_n])), axis=1)
        for i in range(0, len(tiles), cols_n)]
Image.fromarray(np.concatenate(rows, axis=0)).save(out)
print('ok', nvar, 'variações, quadro', frame, 'referência' if '--bind' in sys.argv else '')
px = sum(d[3] for d in DEPTH)
allp = [d[5] for d in DEPTH if d[3]]
print(f"pixels com calça à frente da jaqueta: {px}; penetração p95 máxima entre vistas {max(d[4] for d in DEPTH):.1f} cm, máx {max(allp) if allp else 0:.1f} cm")
