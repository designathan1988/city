"""Cooks Roadcraft's cast (public/models/citizens/people/<id>.bin and .ktx2: MakeHuman CC0 bodies with their clothes, one texture atlas
each, Microsoft Rocketbox MIT skeleton) into project/assets/people/people.bin, read by native/src/render/agents/people_asset.cpp, and
its LICENSES.json. The atlases go out as PNG for tools/assets/compress_atlas.gd. The clips come baked per body (`clips_dir`: <id>.clips,
index.json) as bone palettes; each body has one block of palette rows with its clips at the same rows, named in the header, so the
shader finds a person's clip from the person's block.

Usage: python3 tools/assets/cook_people.py <roadcraft dir> <clips dir> <out dir> <png dir>

Output layout (little-endian):
  header  : magic b"RCPP", u32 version, u32 variations, u32 bones, u32 lods, u32 palette_w, u32 block_rows, u32 clips
  per clip: 16-byte name, u32 row, u32 frames (rows within a block; a walk's own frames are the variation's)
  impostor: u32 frames, f32 x, y, width, height (the strip of yaw views in the atlas, uv with v down)
  per variation: u32 walk_frames, f32 idle_seconds, f32 walk_advance, f32 height, u32 flags (1 female, 2 senior, 4 child),
                 f32 alpha_test, f32 head x y z of the moto clip (where a helmet goes), f32 mood, the palm's three rows, the helmet's three rows, per lod: u32 vertex_count, u32 index_count,
                 per clip, at its first frame: f32 pelvis x y z (root space, metres), f32 above, below, ahead, side (the body's
                 extent from the pelvis)
  per variation, per lod: vertices (f32 x y z, f32 nx ny nz, u8 r g b look, u8 bones[4], u8 weights[4], f32 u v, u32 face row),
                 then u16 indices; look: bits 6-7 region (0 cloth, 1 skin, 2 hair, 3 eye), bits 0-5 roughness
  palette : palette_w * variations * block_rows * 4 f32 (bone b, frame f of a clip: texels 3b..3b+2 of row
            variation * block_rows + the clip's row + f = the rows of the 3x4 bind -> pose matrix)
  face    : u32 lines, then lines * FACE_SPAN rows of the channels' moves at rest, one half-float RGBA texel a channel (row 0 moves nothing)
"""
import json
import os
import struct
import subprocess
import sys

import numpy as np

from cook_ragdoll import ragdoll

MAGIC = b"RCPP"
VERSION = 8
FEMALE, SENIOR, CHILD = 1, 2, 4
BONE_TEXELS = 3
IDLE_FRAMES = 60  # an idle of 12 to 15 s moves slowly
SEATED = ("carDrive", "carRide", "carRearRide", "cabDrive", "cabRide", "chairSit")
SEATED_FRAMES = 12  # somebody sitting hardly moves: the breath of the baked 36 frames, the shader mixes between them
CLIP_NAME_BYTES = 16
PALETTE_SPAN = 3.0  # people.gdshader BODY_SPAN: palette values lie within it, the cook prints them
FIXED_MAX = 65535
STAND_FRAMES = {"talk": 120, "listen": 90, "look": 32, "phone": 110, "headphones": 100, "bag": 100, "crouchDown": 24, "crouchIdle": 40, "crouchUp": 24, "photo": 60}  # what one does waiting: 3 to 7 frames a second
HAND = "Bip01_R_Hand"
HEAD = "Bip01_Head"
HELMET_SEGMENTS = (24, 16)  # riderPoses.ts: the facets the shell is padded for
HELMET_PAD = 0.012  # room between the head and the shell, metres
FINGERS = ("Bip01_R_Finger1", "Bip01_R_Finger2", "Bip01_R_Finger3", "Bip01_R_Finger4")  # index to little finger
# name in the file, name of the baked clip for an adult and for a senior
FACE_CHANNELS = ["faceBlink", "faceJoy", "faceSadness", "faceAnger", "faceSurprise", "faceBrowRaise", "mouthOpen", "mouthRound", "mouthPuckered", "mouthSpread",
                 "mouthShut", "lookLeft", "lookRight"]  # people_face.gdshaderinc, in that order
FACE_SPAN = 8  # rows side by side in the texture: the rows of the sixteen bodies pass the tallest texture
CLIPS = [("walk", "walk", "walkElder"), ("idle", "idle", "idle"), ("carDrive",) * 3, ("carRide",) * 3, ("carRearRide",) * 3, ("cabDrive",) * 3,
         ("cabRide",) * 3, ("chairSit",) * 3, ("motoRide",) * 3, ("motoStop",) * 3, ("bikePedal",) * 3, ("bikeStop",) * 3,
         ("talk",) * 3, ("listen",) * 3, ("look",) * 3, ("phone",) * 3, ("headphones",) * 3, ("bag",) * 3,
         ("crouchDown",) * 3, ("crouchIdle",) * 3, ("crouchUp",) * 3, ("photo",) * 3]


def face_rows(b, first):
    """A body's face: what each vertex that has one moves by, channel by channel, and each vertex's row, counted from `first` (0: no face)."""
    touched = np.unique(np.concatenate([v for v, _ in b["face"]]))
    row_of = np.zeros(len(b["rest"]), dtype="<u4")
    row_of[touched] = first + np.arange(len(touched))
    table = np.zeros((len(touched), len(FACE_CHANNELS), 3), dtype=np.float64)
    for c, (verts, moves) in enumerate(b["face"]):
        table[row_of[verts] - first, c] = moves
    return row_of, table


def face_texels(tables):
    """The rows FACE_SPAN to a line of the texture, a channel a half-float texel."""
    rows = np.concatenate([np.zeros((1, len(FACE_CHANNELS), 3))] + tables)
    lines = -(-len(rows) // FACE_SPAN)
    tex = np.zeros((lines * FACE_SPAN, len(FACE_CHANNELS), 4), dtype="<f2")
    tex[:len(rows), :, :3] = rows
    return lines, tex.reshape(lines, FACE_SPAN * len(FACE_CHANNELS), 4).tobytes()


def buffer(raw, base, ref):
    kinds = {"f32": "<f4", "u32": "<u4", "u16": "<u2", "i16": "<i2", "u8": "u1", "i8": "i1"}
    return np.frombuffer(raw, dtype=kinds[ref["kind"]], count=ref["length"], offset=base + ref["at"])


def body(path):
    raw = open(path, "rb").read()
    n = struct.unpack("<I", raw[:4])[0]
    head = json.loads(raw[4:4 + n])
    base = 4 + ((n + 3) & ~3)
    m = head["mesh"]
    pos, nor, col, uv, bones, weights = (buffer(raw, base, r) for r in m["attributes"])
    count = m["vertexCount"]
    pos = pos.reshape(count, 3).astype(np.float64) / 32767.0
    bind = np.array(m["bind"], dtype=np.float64).reshape(4, 4).T  # column-major in the file
    rest = (bind @ np.c_[pos, np.ones(count)].T).T[:, :3]
    normals = nor.reshape(count, 4)[:, :3].astype(np.float64) / 127.0
    normals = (bind[:3, :3] @ normals.T).T
    normals /= np.maximum(np.linalg.norm(normals, axis=1, keepdims=True), 1e-6)
    morphs = {k["name"]: (buffer(raw, base, k["vertices"]), buffer(raw, base, k["deltas"]).reshape(-1, 3).astype(np.float64) / 32767.0) for k in m["morphs"]}
    face = [(morphs[name][0], (bind[:3, :3] @ morphs[name][1].T).T) for name in FACE_CHANNELS]
    return {
        "face": face,
        "head": head, "rest": rest, "normals": normals, "col": col.reshape(count, 4), "uv": uv.reshape(count, 2).astype(np.float64) / 65535.0,
        "bones": bones.reshape(count, 4), "weights": weights.reshape(count, 4).astype(np.float64) / 255.0, "index": buffer(raw, base, m["index"]),
    }


def palm(head):
    """The hand bone's place in the file's bones, and the palm at rest as three rows: X along the fingers, Y to the thumb, Z out of the back."""
    m = head["mesh"]
    names = [head["nodes"][i].get("name") for i in m["bones"]]

    def at(name):
        return np.linalg.inv(np.array(m["inverses"], dtype=np.float64)[names.index(name) * 16:][:16].reshape(4, 4).T)[:3, 3]

    wrist = at(HAND)
    x = np.mean([at(f) for f in FINGERS], axis=0) - wrist
    x /= np.linalg.norm(x)
    y = at(FINGERS[0]) - at(FINGERS[-1])
    y -= x * y.dot(x)
    y /= np.linalg.norm(y)
    return names.index(HAND), np.c_[x, y, np.cross(x, y), wrist]


def helmet(b):
    """The head bone's place in the file's bones, and a shell of width 1 fitted to this head as three rows at rest (riderPoses.ts helmetShape): round
    every point of the head above the bone's joint, in the bone's own frame."""
    m = b["head"]["mesh"]
    bone = [b["head"]["nodes"][i].get("name") for i in m["bones"]].index(HEAD)
    to_bone = np.array(m["inverses"], dtype=np.float64)[bone * 16:][:16].reshape(4, 4).T
    weight = (b["weights"] * (b["bones"] == bone)).sum(axis=1)
    points = (to_bone @ np.c_[b["rest"], np.ones(len(b["rest"]))].T).T[weight >= 0.5, :3]
    points = points[points[:, 1] > 0]
    low, high = points.min(axis=0), points.max(axis=0)
    centre, half = (low + high) / 2, (high - low) / 2
    reach = np.sqrt((((points - centre) / half) ** 2).sum(axis=1)).max()
    facet = 1 / (np.cos(np.pi / HELMET_SEGMENTS[1]) * np.cos(np.pi / HELMET_SEGMENTS[0]))
    width = (half * reach * facet + HELMET_PAD) * 2
    shell = np.eye(4)
    shell[:3, :3] = np.diag(width)
    shell[:3, 3] = centre
    return bone, (np.linalg.inv(to_bone) @ shell)[:3]


def fixed_point(tex):
    """The palette as the shader reads it: 16-bit fixed point, texel 2x the high bytes of texel x's channels, texel 2x + 1 the low."""
    q = np.round((np.clip(tex, -PALETTE_SPAN, PALETTE_SPAN) + PALETTE_SPAN) / (2 * PALETTE_SPAN) * FIXED_MAX).astype("<u2")
    return np.stack([(q >> 8).astype("u1"), (q & 255).astype("u1")], axis=2)


def clip_rows(blob, clip, bones):
    """A clip's frames as bind -> pose rows, frames x bones x 12 (row x, row y, row z)."""
    data = np.frombuffer(blob, dtype="<f4", count=clip["floats"], offset=clip["at"]).reshape(-1, bones, 4, 3)  # column-major: 4 columns of 3
    data = data[:max(clip["frames"], 1)]
    return np.ascontiguousarray(data.transpose(0, 1, 3, 2)).reshape(-1, bones, 12)


def resampled(rows, count):
    """A looped clip at `count` evenly spaced frames, each a mix of the two frames either side, as the shader mixes them."""
    at = np.arange(count) * (len(rows) / count)
    first = np.floor(at).astype(int)
    mix = (at - first)[:, None, None]
    return rows[first % len(rows)] * (1 - mix) + rows[(first + 1) % len(rows)] * mix


def posed(frame, b):
    """The body's vertices in one frame."""
    rows = frame.reshape(-1, 3, 4)
    p = np.c_[b["rest"], np.ones(len(b["rest"]))]
    out = np.zeros((len(p), 3))
    for k in range(4):
        out += b["weights"][:, k:k + 1] * np.einsum("vij,vj->vi", rows[b["bones"][:, k]], p)
    return out


def licenses(root, out):
    text = open(f"{root}/public/models/citizens/LICENSE-MICROSOFT.txt", encoding="utf8").read().strip()
    doc = {
        "_doc": "Provenance of people.bin and atlas.bin (made by tools/assets/cook_people.py from Roadcraft public/models/citizens/people).",
        "makehuman": {
            "what": "bodies, clothes, hair, shoes and their textures (MakeHuman hm08 base mesh, MPFB2 system assets)",
            "source": "https://github.com/makehumancommunity/mpfb2 @ afb9f530a7c2741dedb8df0ebae2e0b183caec21",
            "license": "CC0-1.0",
            "holders": "Data Collection AB, Joel Palmius, Jonas Hauquier (MakeHuman team)",
        },
        "rocketbox": {
            "what": "skeleton, walk and idle animation (Microsoft Rocketbox), retargeted and baked to bone palettes",
            "source": "https://github.com/microsoft/Microsoft-Rocketbox",
            "license": "MIT",
            "copyright": "Copyright (c) 2020 Microsoft",
            "license_text": text,
        },
        "riders": {"what": "the clips of people in and on vehicles: poses solved on the same skeleton by inverse kinematics"},
    }
    json.dump(doc, open(f"{out}/LICENSES.json", "w", encoding="utf8", newline="\n"), indent=1, ensure_ascii=False)


def cook(root, clips_dir, out, png):
    index = json.load(open(f"{clips_dir}/index.json"))
    os.makedirs(png, exist_ok=True)
    moods = {c["id"]: c["mood"] for c in json.load(open(f"{root}/src/people/cast.json", encoding="utf8"))}
    bodies, blobs, face_tables = [], [], []
    next_face = 1
    for e in index:
        bodies.append(body(f"{root}/public/models/citizens/people/{e['id']}.bin"))
        blobs.append(open(f"{clips_dir}/{e['id']}.clips", "rb").read())
    nb = index[0]["bones"]
    frames_of = {}
    for name, adult, senior in CLIPS:
        counts = {max(e["clips"][senior if e["ageBand"] == "senior" else adult]["frames"], 1) for e in index}
        frames_of[name] = max(counts)
    frames_of["idle"] = IDLE_FRAMES
    for name in SEATED:
        frames_of[name] = SEATED_FRAMES
    frames_of.update(STAND_FRAMES)
    table, row = [], 0
    for name, _, _ in CLIPS:
        table.append((name, row, frames_of[name]))
        row += frames_of[name]
    seconds = {name: [float(np.mean([e["clips"][name if name != "walk" else adult]["duration"] for e in index if e["gender"] == g])) for g in "mf"]
               for name, adult, _ in CLIPS}
    block_rows = row
    heads, meshes, rows = [], [], []
    alpha = None
    for e, b, blob in zip(index, bodies, blobs):
        senior = e["ageBand"] == "senior"
        block = np.zeros((block_rows, nb, 12), dtype="<f4")
        fits = []
        walk_frames = 1
        for (name, adult, old), (_, at, count) in zip(CLIPS, table):
            clip = e["clips"][old if senior else adult]
            data = clip_rows(blob, clip, nb)
            if name == "walk":
                walk_frames = len(data)
            elif len(data) != count:
                data = resampled(data, count)
            block[at:at + len(data)] = data
            shape = posed(block[at], b)
            pelvis = np.array(clip["pelvis"])
            fits.append(struct.pack("<7f", *pelvis, shape[:, 1].max() - pelvis[1], pelvis[1] - shape[:, 1].min(), shape[:, 2].max() - pelvis[2],
                                    np.abs(shape[:, 0] - pelvis[0]).max()))
        rows.append(block)
        m = b["head"]["mesh"]
        alpha = m["alphaTest"]
        walk, moto = e["clips"]["walkElder" if senior else "walk"], e["clips"]["motoRide"]
        hx, hy, hz = (moto["head"][12], moto["head"][13], moto["head"][14]) if moto.get("head") else (0.0, 0.0, 0.0)
        flags = (FEMALE if e["gender"] == "f" else 0) | (SENIOR if senior else 0) | (CHILD if e["ageBand"] == "child" else 0)
        hand, rows_of_palm = palm(b["head"])
        head = struct.pack("<IffIfffff", walk_frames, walk["stride"], e["height"], flags, alpha, hx, hy, hz, moods[e["id"]]) + rows_of_palm.astype("<f4").tobytes()
        head_bone, shell = helmet(b)
        head += shell.astype("<f4").tobytes()
        face_row, face_table = face_rows(b, next_face)
        face_tables.append(face_table)
        next_face += len(face_table)
        for near, level in zip([True] + [False] * len(m["levels"]), m["levels"]):
            tri = b["index"][level["start"]:level["start"] + level["count"]].reshape(-1, 3)[:, [0, 2, 1]].reshape(-1)  # three's front is Godot's back
            used, local = np.unique(tri, return_inverse=True)
            head += struct.pack("<II", len(used), len(tri))
            vert = np.zeros(len(used), dtype=[("p", "<f4", 3), ("n", "<f4", 3), ("c", "u1", 4), ("b", "u1", 4), ("w", "u1", 4), ("t", "<f4", 2), ("f", "<u4")])
            vert["p"], vert["n"], vert["c"], vert["b"] = b["rest"][used], b["normals"][used], b["col"][used], b["bones"][used]
            vert["w"], vert["t"] = np.round(b["weights"][used] * 255).astype("u1"), b["uv"][used]
            vert["f"] = face_row[used] if near else 0
            meshes.append(vert.tobytes() + local.astype("<u2").tobytes())
        heads.append(head + b"".join(fits))
        subprocess.run(["node", os.path.join(os.path.dirname(__file__), "ktx2_to_png.mjs"), f"{root}/public/models/citizens/people/{e['id']}.ktx2",
                        f"{png}/{e['id']}.png"], check=True, stdout=subprocess.DEVNULL)
    pal = np.concatenate(rows, axis=0)
    tex = pal.reshape(pal.shape[0], nb * BONE_TEXELS, 4).astype("<f4")
    m = bodies[0]["head"]["mesh"]
    side = float(bodies[0]["head"]["atlas"]["size"])
    imp = m["impostor"]
    blob = struct.pack("<4sIIIIIIIII", MAGIC, VERSION, len(index), nb, len(m["levels"]), nb * BONE_TEXELS, block_rows, len(table), hand, head_bone)
    blob += b"".join(struct.pack(f"<{CLIP_NAME_BYTES}sIIff", n.encode(), r, f, *seconds[n]) for n, r, f in table)
    # v over the atlas's rows as the mesh's uv reads them: a frame's y counts from the other end (cookedPerson.ts CookedImpostor)
    blob += struct.pack("<Iffff", imp["frames"], imp["x"] / side, 1 - (imp["y"] + imp["height"]) / side, imp["width"] / side, imp["height"] / side)
    face_lines, face_bytes = face_texels(face_tables)
    blob += b"".join(heads) + b"".join(meshes) + fixed_point(tex).tobytes() + struct.pack("<I", face_lines) + face_bytes
    parts, joint_bones, _ = ragdoll(bodies[0]["head"])
    blob += bytes(parts) + bytes(joint_bones) + b"".join(ragdoll(b["head"])[2].tobytes() for b in bodies)
    open(f"{out}/people.bin", "wb").write(blob)
    licenses(root, out)
    print(f"palette values within {float(np.abs(pal).max()):.3f}")
    print(f"{len(index)} bodies, palette {nb * BONE_TEXELS}x{pal.shape[0]}, {len(blob) / 1e6:.2f} MB; rest height {bodies[0]['rest'][:, 1].min():.3f}..{bodies[0]['rest'][:, 1].max():.3f}")
    print(f"face: {next_face} rows, {face_lines} lines, {len(face_bytes) / 1e6:.2f} MB")
    print("frames", {n: f for n, _, f in table}, "alpha", alpha)


if __name__ == "__main__":
    if len(sys.argv) != 5:
        raise SystemExit(__doc__)
    cook(*sys.argv[1:5])
