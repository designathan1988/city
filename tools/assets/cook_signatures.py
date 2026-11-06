"""Cooks the signature buildings of the Roadcraft 4.0 into project/assets/lots: what its lot lab draws for each project, geometry and pictures, as data.

Input: the report of a look.mjs run that ran buildSignature on each lot (each step's value a JSON {name, parts}, the last the materials). No code of the 4.0
comes along: only what it drew.

    python tools/assets/cook_signatures.py <report.json> project/assets/lots

Output, little-endian:
  materials.sig: b"RCSM", u32 raw size, then deflate of: u32 count, then each: 24-byte name, f32 r g b (sRGB), roughness, metallic, emission r g b (sRGB, times its intensity), opacity,
                 alpha_test, u32 flags (1 blended, 2 two sided), u32 picture width, height, then width * height * 4 bytes of RGBA8
  <name>.sig   : b"RCSB", u32 raw size, then deflate of: u32 surfaces, each: u32 material, u32 vertices, u32 indices, vertices (f32 x y z, i8 nx ny nz 0, f32 u v),
                 u32 indices. Axes as drawn: x the plan's x, y height, z minus the plan's y.
"""
import base64
import io
import json
import struct
import sys
import zlib

import numpy as np
from PIL import Image

MAGIC_MATERIALS = b"RCSM"
MAGIC_BODY = b"RCSB"
NAME_BYTES = 24
BLENDED, TWO_SIDED = 1, 2
NORMAL_SCALE = 127.0


def array(text, dtype):
    return np.frombuffer(base64.b64decode(text), dtype=dtype)


def picture(url):
    return Image.open(io.BytesIO(base64.b64decode(url.split(",", 1)[1]))).convert("RGBA")


def srgb(hex_colour):
    return [((hex_colour >> shift) & 255) / 255.0 for shift in (16, 8, 0)]


def materials_file(mats):
    names = sorted(mats)
    blob = struct.pack("<I", len(names))
    for name in names:
        m = mats[name]
        assert len(name) < NAME_BYTES, name
        image = None
        if m["map"]:
            image = picture(m["map"])
        if m["alpha_map"]:
            cut = picture(m["alpha_map"]).convert("L")
            image = (image or Image.new("RGBA", cut.size, (255, 255, 255, 255))).resize(cut.size)
            image.putalpha(cut)
        glow = [c * m["glow"] for c in srgb(m["emissive"])]
        flags = (BLENDED if m["transparent"] else 0) | (TWO_SIDED if m["two_sided"] else 0)
        width, height = image.size if image else (0, 0)
        blob += name.encode().ljust(NAME_BYTES, b"\0")
        blob += struct.pack("<3f2f3f2f3I", *srgb(m["color"]), m["rough"], m["metal"], *glow, m["opacity"], m["alpha_test"], flags, width, height)
        if image:
            blob += image.tobytes()
    return names, blob


def body_file(parts, order):
    parts = [p for p in parts if p["p"]]  # a material every part of which another hid
    raw = struct.pack("<I", len(parts))
    for part in parts:
        pos = array(part["p"], "<f4").reshape(-1, 3)
        nor = array(part["n"], "<f4").reshape(-1, 3)
        uv = array(part["u"], "<f4").reshape(-1, 2)
        index = array(part["i"], "<u4")
        vert = np.zeros(len(pos), dtype=[("p", "<f4", 3), ("n", "i1", 4), ("t", "<f4", 2)])
        vert["p"] = pos
        vert["n"][:, :3] = np.round(nor * NORMAL_SCALE)
        vert["t"] = uv
        raw += struct.pack("<III", order[part["mat"]], len(pos), len(index)) + vert.tobytes() + index.reshape(-1, 3)[:, [0, 2, 1]].astype("<u4").tobytes()  # three's front is Godot's back
    return MAGIC_BODY + struct.pack("<I", len(raw)) + zlib.compress(raw, 9)


def main(report, out):
    steps = [s["value"] for s in json.load(open(report, encoding="utf8"))["steps"] if isinstance(s.get("value"), str) and s["value"].startswith("{")]
    names, blob = materials_file(json.loads(steps[-1]))
    open(f"{out}/materials.sig", "wb").write(MAGIC_MATERIALS + struct.pack("<I", len(blob)) + zlib.compress(blob, 9))
    order = {n: i for i, n in enumerate(names)}
    total = 0
    for text in steps[:-1]:
        doc = json.loads(text)
        data = body_file(doc["parts"], order)
        total += len(data)
        open(f"{out}/{doc['name']}.sig", "wb").write(data)
    print(f"{len(names)} materials, {len(blob) / 1e6:.1f} MB raw; {len(steps) - 1} bodies, {total / 1e6:.1f} MB")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    main(*sys.argv[1:3])
