"""Converts Roadcraft's cooked people (people.bin/people.json: MakeHuman CC0 bodies, Microsoft Rocketbox MIT walk) into
project/assets/people/people.bin, read by native/src/render/agents/people_asset.cpp. Only the walk clip is kept, as a
float palette (3 RGBA texels per bone per frame), and two LODs per variation, re-indexed per variation.

Usage: python3 -I tools/assets/cook_people.py <roadcraft public/cooked/people dir> <out dir>

Output layout (little-endian):
  header  : magic b"RCPP", u32 version, u32 variations, u32 bones, u32 lods, u32 palette_w, u32 palette_h
  per variation: u32 palette_row, u32 frames, f32 fps, f32 walk_advance, f32 height, u32 flags (1 female, 2 senior),
                 per lod: u32 vertex_count, u32 index_count
  per variation, per lod: vertices (f32 x y z, f32 nx ny nz, u8 r g b region, u8 bones[4], u8 weights[4], f32 detail),
                 then u16 indices
  palette : palette_w * palette_h * 4 f32 (bone b, frame f: texels 3b..3b+2 of row palette_row + f = the 3x4 rows)
"""
import json
import struct
import sys

import numpy as np

MAGIC = b"RCPP"
VERSION = 1
LODS = (0, 1)  # Roadcraft LOD0 (~4k tris) near, LOD1 (~600 tris) far
WALK = 0  # clip index of the walk in every clip set
FEMALE, SENIOR = 1, 2
BONE_TEXELS = 3
FIRST_PROP = 8  # regions >= 8 are held props (phone, cup, bag, paper, umbrella, helmet): the walk carries none


def octa_decode(w0):
    x = (w0 & 0xFFFF).astype(np.float32) / 65535.0 * 2.0 - 1.0
    y = (w0 >> 16).astype(np.float32) / 65535.0 * 2.0 - 1.0
    z = 1.0 - np.abs(x) - np.abs(y)
    t = np.clip(-z, 0.0, None)
    x = x + np.where(x >= 0, -t, t)
    y = y + np.where(y >= 0, -t, t)
    n = np.stack([x, y, z], axis=1)
    return n / np.maximum(np.linalg.norm(n, axis=1, keepdims=True), 1e-6)


def section(raw, layout, name, dtype):
    s = layout[name]
    return np.frombuffer(raw, dtype=dtype, count=s["count"], offset=s["byteOffset"])


def cook(src, out):
    meta = json.load(open(f"{src}/people.json"))
    raw = open(f"{src}/people.bin", "rb").read()
    lay = meta["layout"]
    vpos = section(raw, lay, "vpos", "<f4").reshape(-1, 4)
    vattr = section(raw, lay, "vattr", "<u4").reshape(-1, 4)
    index = section(raw, lay, "index", "<u4")
    nb = len(meta["bones"])
    palette = section(raw, lay, "palette", "<f4").reshape(-1, nb, 12)
    normals = octa_decode(vattr[:, 0])
    variations = meta["variations"]
    heads, bodies, rows = [], [], []
    row = 0
    for v in variations:
        walk = meta["clipSets"][v["clipSet"]][WALK]
        start = v["paletteStart"] + walk["start"]
        rows.append(palette[start:start + walk["frames"]])
        flags = (FEMALE if v["sex"] == "female" else 0) | (SENIOR if v["band"] == "senior" else 0)
        head = struct.pack("<IIfffI", row, walk["frames"], walk["fps"], v["walkAdvance"], v["height"], flags)
        row += walk["frames"]
        for lod in LODS:
            first, tris = v["lod"][lod]
            tri = index[first:first + tris * 3].reshape(-1, 3)
            region = (vattr[tri, 1] >> 24).max(axis=1)
            tri = tri[region < FIRST_PROP].reshape(-1)
            used, local = np.unique(tri, return_inverse=True)
            if len(used) > 65535:
                raise SystemExit(f"variation with {len(used)} vertices does not fit u16 indices")
            head += struct.pack("<II", len(used), len(tri))
            vert = np.zeros(len(used), dtype=[("p", "<f4", 3), ("n", "<f4", 3), ("c", "<u4"), ("b", "<u4"), ("w", "<u4"), ("d", "<f4")])
            vert["p"] = vpos[used, :3]
            vert["n"] = normals[used]
            vert["c"] = vattr[used, 1]
            vert["b"] = vattr[used, 2]
            vert["w"] = vattr[used, 3]
            vert["d"] = vpos[used, 3]
            bodies.append(vert.tobytes() + local.astype("<u2").tobytes())
        heads.append(head)
    pal = np.concatenate(rows, axis=0)  # frames x bones x 12
    tex = pal.reshape(pal.shape[0], nb * BONE_TEXELS, 4).astype("<f4")
    blob = struct.pack("<4sIIIIII", MAGIC, VERSION, len(variations), nb, len(LODS), nb * BONE_TEXELS, pal.shape[0])
    blob += b"".join(heads) + b"".join(bodies) + tex.tobytes()
    open(f"{out}/people.bin", "wb").write(blob)
    print(f"{len(variations)} variations, palette {nb * BONE_TEXELS}x{pal.shape[0]}, {len(blob) / 1e6:.2f} MB")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    cook(sys.argv[1], sys.argv[2])
