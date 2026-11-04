"""Converts Roadcraft's cooked people (people.bin/people.json: MakeHuman CC0 bodies, Microsoft Rocketbox MIT walk) into
project/assets/people/people.bin, read by native/src/render/agents/people_asset.cpp, and its LICENSES.json. The clips a
walker and a rider play are kept (walk, idle, pedal) as a float palette (3 RGBA texels per bone per frame), and two LODs per
variation, re-indexed per variation. Each variation has one block of palette rows with its clips at the same rows, so the
shader finds a person's idle from the person's block.

Usage: python3 -I tools/assets/cook_people.py <roadcraft public/cooked/people dir> <out dir>
(the Rocketbox licence is read from ../../models/citizens/LICENSE-MICROSOFT.txt beside the cooked pack)

Output layout (little-endian):
  header  : magic b"RCPP", u32 version, u32 variations, u32 bones, u32 lods, u32 palette_w, u32 block_rows,
            u32 idle_row, u32 idle_frames, u32 pedal_row, u32 pedal_frames (rows within a block)
  per variation: u32 walk_frames, f32 idle_seconds, f32 walk_advance, f32 height, u32 flags (1 female, 2 senior, 4 child),
                 per lod: u32 vertex_count, u32 index_count
  per variation, per lod: vertices (f32 x y z, f32 nx ny nz, u8 r g b region, u8 bones[4], u8 weights[4], f32 detail),
                 then u16 indices
  palette : palette_w * variations * block_rows * 4 f32 (bone b, frame f of a clip: texels 3b..3b+2 of row
            variation * block_rows + the clip's row + f = the 3x4 rows); a block's walk starts at its row 0
"""
import json
import struct
import sys

import numpy as np

MAGIC = b"RCPP"
VERSION = 2
LODS = (0, 1)  # Roadcraft LOD0 (~4k tris) near, LOD1 (~600 tris) far
FEMALE, SENIOR, CHILD = 1, 2, 4
BONE_TEXELS = 3
IDLE_FRAMES = 60  # an idle of 12 to 15 s moves slowly: the cook prints what the resampling costs
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


def clip_rows(palette, start, clip):
    return palette[start + clip["start"]:start + clip["start"] + clip["frames"]]


def resampled(rows, count):
    """A looped clip at `count` evenly spaced frames, each a mix of the two frames either side, as the shader mixes them."""
    at = np.arange(count) * (len(rows) / count)
    first = np.floor(at).astype(int)
    mix = (at - first)[:, None, None]
    return rows[first % len(rows)] * (1 - mix) + rows[(first + 1) % len(rows)] * mix


def resampling_error(rows, kept):
    """The most a pose matrix entry strays from the source when `kept` is played back."""
    at = np.arange(len(rows)) * (len(kept) / len(rows))
    first = np.floor(at).astype(int)
    mix = (at - first)[:, None, None]
    back = kept[first % len(kept)] * (1 - mix) + kept[(first + 1) % len(kept)] * mix
    return float(np.abs(back - rows).max())


def licenses(src, out):
    text = open(f"{src}/../../models/citizens/LICENSE-MICROSOFT.txt", encoding="utf8").read().strip()
    doc = {
        "_doc": "Provenance of people.bin (made by tools/assets/cook_people.py from Roadcraft public/cooked/people).",
        "makehuman": {
            "what": "bodies, clothes, hair, shoes and skin colours (MakeHuman hm08 base mesh, MPFB2 system assets, game_engine rig)",
            "source": "https://github.com/makehumancommunity/mpfb2 @ afb9f530a7c2741dedb8df0ebae2e0b183caec21",
            "license": "CC0-1.0",
            "holders": "Data Collection AB, Joel Palmius, Jonas Hauquier (MakeHuman team)",
        },
        "rocketbox": {
            "what": "walk and idle animation (Microsoft Rocketbox), retargeted and baked to bone palettes",
            "source": "https://github.com/microsoft/Microsoft-Rocketbox",
            "license": "MIT",
            "copyright": "Copyright (c) 2020 Microsoft",
            "license_text": text,
        },
        "pedal": {"what": "the pedalling clip: a pose Roadcraft builds on the same skeleton by inverse kinematics, no third-party animation"},
    }
    json.dump(doc, open(f"{out}/LICENSES.json", "w", encoding="utf8", newline="\n"), indent=1, ensure_ascii=False)


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
    clips = [{c["name"]: c for c in s} for s in meta["clipSets"]]
    idle_row = max(c["walk"]["frames"] for c in clips)
    pedal_row = idle_row + IDLE_FRAMES
    pedal_frames = clips[0]["pedal"]["frames"]
    if any(c["pedal"]["frames"] != pedal_frames for c in clips):
        raise SystemExit("every clip set must pedal in the same frames")
    block_rows = pedal_row + pedal_frames
    heads, bodies, rows = [], [], []
    worst = 0.0
    for v in variations:
        clip = clips[v["clipSet"]]
        block = np.zeros((block_rows, nb, 12), dtype="<f4")
        walk, idle = clip_rows(palette, v["paletteStart"], clip["walk"]), clip_rows(palette, v["paletteStart"], clip["idle"])
        kept = resampled(idle, IDLE_FRAMES)
        worst = max(worst, resampling_error(idle, kept))
        block[:len(walk)] = walk
        block[idle_row:pedal_row] = kept
        block[pedal_row:] = clip_rows(palette, v["paletteStart"], clip["pedal"])
        rows.append(block)
        flags = (FEMALE if v["sex"] == "female" else 0) | (SENIOR if v["band"] == "senior" else 0) | (CHILD if v["band"] == "child" else 0)
        head = struct.pack("<IfffI", len(walk), clip["idle"]["frames"] / clip["idle"]["fps"], v["walkAdvance"], v["height"], flags)
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
    blob = struct.pack("<4sIIIIIIIIII", MAGIC, VERSION, len(variations), nb, len(LODS), nb * BONE_TEXELS, block_rows, idle_row,
                       IDLE_FRAMES, pedal_row, pedal_frames)
    blob += b"".join(heads) + b"".join(bodies) + tex.tobytes()
    open(f"{out}/people.bin", "wb").write(blob)
    licenses(src, out)
    print(f"{len(variations)} variations, palette {nb * BONE_TEXELS}x{pal.shape[0]}, {len(blob) / 1e6:.2f} MB, idle resampled to within {worst:.4f}")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    cook(sys.argv[1], sys.argv[2])
