"""Cooks the signature buildings of the Roadcraft 4.0 into project/assets/lots: what its lot lab draws for each project, geometry and pictures, as data.

Input: the report of a look.mjs run that ran buildSignature on each lot (each step's value a JSON {name, parts}, the last the materials). No code of the 4.0
comes along: only what it drew.

    python tools/assets/cook_signatures.py <report.json> project/assets/lots

Output, little-endian:
  materials.sig: b"RCSM", u32 raw size, then deflate of: u32 count, then each: 24-byte name, f32 r g b (sRGB), roughness, metallic, emission r g b (sRGB, times its intensity), opacity,
                 alpha_test, u32 flags (1 blended, 2 two sided, 4 all of it window, 8 its picture's alpha says what is window), u32 picture width, height, then width * height * 4 bytes of RGBA8
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
BLENDED, TWO_SIDED, WINDOWS, PAINTED_WINDOWS = 1, 2, 4, 8
NORMAL_SCALE = 127.0
ROOM_M = 3.2  # the shader's cell over glass: signature_look.gdshaderinc GLASS_CELL_M
BIGGEST_PANE_M = 8.0  # a piece of glass past this is a wall of many rooms
ROOMS_TO_A_RUN = 3.0  # an open office lit together, along a wall of glass
WELD_M = 1e-3
# Where each style of `facade` paints its glass in the one bay and storey of its picture: x0, y0, x1, y1 from the top left.
ARCHED = ("archBrick", "archStone", "archTerracotta", "gothicStone")
DARKEST_FRAME = 165  # what is lighter than this inside an arch's box is the arch's frame
GLASS_IN_PICTURE = {
    "archBrick": [(0.3, 0.2, 0.7, 0.78)], "archStone": [(0.3, 0.2, 0.7, 0.78)], "archTerracotta": [(0.28, 0.08, 0.72, 0.8)],
    "gothicStone": [(0.4, 0.1, 0.6, 0.78)], "decoStripes": [(0.3, 0, 0.45, 1), (0.55, 0, 0.7, 1)], "loftBrick": [(0.15, 0.13, 0.85, 0.82)],
    "tallStone": [(0.34, 0.08, 0.66, 0.92)], "schoolBrick": [(0.13, 0.19, 0.87, 0.75)], "officeWhite": [(0.08, 0.22, 0.92, 0.7)],
    "ribbonWhite": [(0, 0.18, 1, 0.7)], "ribbonBlack": [(0, 0.18, 1, 0.7)], "squareConcrete": [(0.22, 0.22, 0.78, 0.74)],
    "navyGrid": [(0.08, 0.1, 0.92, 0.78)], "copperFins": [(0, 0, 1, 0.85)], "punchedPink": [(0.25, 0.22, 0.75, 0.77)],
    "curtainBlue": [(0, 0, 1, 0.8)], "curtainDark": [(0, 0, 1, 0.8)], "curtainGreen": [(0, 0, 1, 0.8)], "curtainTeal": [(0, 0, 1, 0.8)],
    "gridDark": [(0.11, 0.2, 0.89, 0.78)], "stoneGrid": [(0.11, 0.2, 0.89, 0.78)], "brick": [(0.2, 0.2, 0.8, 0.78)],
}
PLAIN_PUNCHED = [(0.27, 0.2, 0.73, 0.78)]
OFF_THE_ROW, OFF_THE_COLUMN = 45.0, 100.0  # how far from the glass around it a bar is, the column's wider for the glass's gradient


def array(text, dtype):
    return np.frombuffer(base64.b64decode(text), dtype=dtype)


def picture(url):
    return Image.open(io.BytesIO(base64.b64decode(url.split(",", 1)[1]))).convert("RGBA")


def srgb(hex_colour):
    return [((hex_colour >> shift) & 255) / 255.0 for shift in (16, 8, 0)]


def with_panes(image, kind):
    """The picture with its alpha saying what is glass, for the rooms behind at night: the painted box less the bars across it."""
    rgb = np.asarray(image.convert("RGB"), dtype=np.float32)
    size_y, size_x = rgb.shape[:2]
    mask = np.zeros((size_y, size_x), dtype=np.uint8)
    for x0, y0, x1, y1 in GLASS_IN_PICTURE.get(kind, PLAIN_PUNCHED):
        xa, xb, ya, yb = int(x0 * size_x), int(x1 * size_x), int(y0 * size_y), int(y1 * size_y)
        box = rgb[ya:yb, xa:xb]
        off_row = np.linalg.norm(box - np.median(box, axis=1, keepdims=True), axis=2)
        off_column = np.linalg.norm(box - np.median(box, axis=0, keepdims=True), axis=2)
        glass = (off_row < OFF_THE_ROW) & (off_column < OFF_THE_COLUMN)
        if kind in ARCHED:
            glass &= box.mean(axis=2) < DARKEST_FRAME
        mask[ya:yb, xa:xb] = np.where(glass, 255, 0)
    image.putalpha(Image.fromarray(mask))
    return image


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
        if name.startswith("facade|"):
            image = with_panes(image, name.split("|")[1])
        glow = [c * m["glow"] for c in srgb(m["emissive"])]
        flags = (BLENDED if m["transparent"] else 0) | (TWO_SIDED if m["two_sided"] else 0)
        if name.startswith("facade|"):
            flags |= PAINTED_WINDOWS
        elif is_window(name, m):
            flags |= WINDOWS
        width, height = image.size if image else (0, 0)
        blob += name.encode().ljust(NAME_BYTES, b"\0")
        blob += struct.pack("<3f2f3f2f3I", *srgb(m["color"]), m["rough"], m["metal"], *glow, m["opacity"], m["alpha_test"], flags, width, height)
        if image:
            blob += image.tobytes()
    return names, blob


def is_window(name, m):
    return "lass" in name and "ail" not in name and not m["transparent"] and not m["map"]


def pieces_of(pos, index):
    """Which connected piece each vertex is in, the corners of a box's faces welded."""
    _, spot = np.unique(np.round(pos / WELD_M).astype(np.int64), axis=0, return_inverse=True)
    spot = spot.reshape(-1)
    root = list(range(int(spot.max()) + 1))

    def find(a):
        while root[a] != a:
            root[a] = root[root[a]]
            a = root[a]
        return a

    for tri in spot[index].reshape(-1, 3).tolist():
        a = find(tri[0])
        root[find(tri[1])] = a
        root[find(tri[2])] = a
    return np.array([find(s) for s in spot.tolist()])


def rooms_in_uv(pos, index, uv):
    """A pane is one room, lit or not whole: every corner of a small piece gets a cell of its own. A wall of glass keeps its metres, cut into runs of rooms by the shader."""
    uv = uv.copy()
    piece = pieces_of(pos, index)
    for n, p in enumerate(np.unique(piece).tolist()):
        at = piece == p
        if np.ptp(pos[at], axis=0).max() <= BIGGEST_PANE_M:
            uv[at] = (n + 0.5) * ROOM_M, 0.5 * ROOM_M
        else:
            uv[at, 0] /= ROOMS_TO_A_RUN
    return uv


def body_file(parts, order, mats):
    parts = [p for p in parts if p["p"]]  # a material every part of which another hid
    raw = struct.pack("<I", len(parts))
    for part in parts:
        pos = array(part["p"], "<f4").reshape(-1, 3)
        nor = array(part["n"], "<f4").reshape(-1, 3)
        uv = array(part["u"], "<f4").reshape(-1, 2)
        index = array(part["i"], "<u4")
        if is_window(part["mat"], mats[part["mat"]]):
            uv = rooms_in_uv(pos, index, uv)
        vert = np.zeros(len(pos), dtype=[("p", "<f4", 3), ("n", "i1", 4), ("t", "<f4", 2)])
        vert["p"] = pos
        vert["n"][:, :3] = np.round(nor * NORMAL_SCALE)
        vert["t"] = uv
        raw += struct.pack("<III", order[part["mat"]], len(pos), len(index)) + vert.tobytes() + index.reshape(-1, 3)[:, [0, 2, 1]].astype("<u4").tobytes()  # three's front is Godot's back
    return MAGIC_BODY + struct.pack("<I", len(raw)) + zlib.compress(raw, 9)


def main(report, out):
    steps = [s["value"] for s in json.load(open(report, encoding="utf8"))["steps"] if isinstance(s.get("value"), str) and s["value"].startswith("{")]
    mats = json.loads(steps[-1])
    names, blob = materials_file(mats)
    open(f"{out}/materials.sig", "wb").write(MAGIC_MATERIALS + struct.pack("<I", len(blob)) + zlib.compress(blob, 9))
    order = {n: i for i, n in enumerate(names)}
    total = 0
    for text in steps[:-1]:
        doc = json.loads(text)
        data = body_file(doc["parts"], order, mats)
        total += len(data)
        open(f"{out}/{doc['name']}.sig", "wb").write(data)
    print(f"{len(names)} materials, {len(blob) / 1e6:.1f} MB raw; {len(steps) - 1} bodies, {total / 1e6:.1f} MB")


if __name__ == "__main__":
    if len(sys.argv) != 3:
        raise SystemExit(__doc__)
    main(*sys.argv[1:3])
