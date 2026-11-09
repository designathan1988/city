# The layout of project/assets/people/people.bin as cook_people_lab.py writes it and native/src/render/agents/people_asset.cpp reads it: the
# base a cook keeps (the bodies' meshes, card, faces, palms, helmets and ragdoll joints, from a version 8 or 9 file) and the version 9
# file written whole (the clip table with each clip's kind and loop, each body's fits and strides, the clips' curves, the palette with the
# bodies' blocks in COLUMNS columns).
import struct
from pathlib import Path

import numpy as np

MAGIC = 0x50504352  # "RCPP"
READS = (8, 9)  # the base may be the old layout or this one
VERSION = 9
CLIP_NAME_BYTES = 16
VERTEX_BYTES = 6 * 4 + 4 + 4 + 4 + 2 * 4 + 4  # people_asset.cpp read_mesh: position, normal, colour, bones, weights, uv, face row
PALETTE_TEXELS, BONE_TEXELS, RGBA = 2, 3, 4  # a value's two texels (high bytes, low), a bone's three rows, four channels
PALETTE_SPAN = 3.0  # a palette value is fixed point over [-span, span] (shader_contracts.json PEOPLE_PALETTE_SPAN)
FIXED_TOP = 65535  # sixteen bits
BYTE_STEPS = 256
COLUMNS = 8  # bodies side by side in the palette: sixteen bodies' blocks in two rows of eight, under 16384 texels either way
FACE_TEXEL_BYTES, FACE_TEXELS = 8, 8 * 13  # people_asset.cpp: half floats, PEOPLE_FACE_SPAN x PEOPLE_FACE_CHANNELS
JOINTS = 18  # ragdoll.h RAGDOLL_JOINTS


class Reader:
    def __init__(self, data):
        self.data, self.at = data, 0

    def take(self, fmt):
        out = struct.unpack_from('<' + fmt, self.data, self.at)
        self.at += struct.calcsize('<' + fmt)
        return out

    def raw(self, n):
        out = self.data[self.at:self.at + n]
        self.at += n
        return out


def read_base(path):
    """What the cook keeps from a people.bin: each body's meshes, flags, mood, palm, helmet and joints; the card, faces and ragdoll."""
    r = Reader(Path(path).read_bytes())
    magic, version, count, bones, lods, pal_w, block_rows, clip_count, hand, head = r.take('10I')
    if magic != MAGIC or version not in READS:
        raise ValueError(f'{path}: not a people.bin this cook reads (version {version})')
    columns = r.take('I')[0] if version == 9 else 1
    frames = []
    for _ in range(clip_count):
        r.raw(CLIP_NAME_BYTES)
        _, n = r.take('2I')
        r.take('2f')
        if version == 9:
            r.take('2I')
        frames.append(n)
    card = r.raw(20)
    bodies = []
    for _ in range(count):
        if version == 8:
            r.take('I f')
        height, flags, alpha = r.take('f I f')
        seat = r.take('3f')
        mood = r.take('f')[0]
        palm = r.take('12f')
        helmet = r.take('12f')
        sizes = r.take(f'{lods * 2}I')
        r.raw(clip_count * (7 if version == 8 else 8) * 4)
        bodies.append(dict(height=height, flags=flags, alpha=alpha, seat=seat, mood=mood, palm=palm, helmet=helmet, sizes=sizes))
    if version == 9:
        r.raw(count * sum(n + 1 for n in frames) * 4)
    for b in bodies:
        b['meshes'] = b''.join(r.raw(b['sizes'][2 * l] * VERTEX_BYTES + b['sizes'][2 * l + 1] * 2) for l in range(lods))
    body_rows = -(-count // columns)
    r.raw(pal_w * PALETTE_TEXELS * columns * RGBA * block_rows * body_rows)
    face_lines = r.take('I')[0]
    faces = r.raw(face_lines * FACE_TEXELS * FACE_TEXEL_BYTES)
    bone_part = r.raw(bones)
    joint_bone = r.raw(JOINTS)
    for b in bodies:
        b['joints'] = r.raw(JOINTS * 3 * 4)
    if r.at != len(r.data):
        raise ValueError(f'{path}: {len(r.data) - r.at} bytes left over')
    return dict(count=count, bones=bones, lods=lods, hand=hand, head=head, card=card, bodies=bodies, face_lines=face_lines, faces=faces,
                bone_part=bone_part, joint_bone=joint_bone)


def palette_bytes(rows):
    """A block of rows (rows, bones, 3, 4) as the palette's texels: each value's high byte, then its low byte, a texel each."""
    fixed = np.clip(np.round((rows + PALETTE_SPAN) / (2 * PALETTE_SPAN) * FIXED_TOP), 0, FIXED_TOP).astype(np.uint32)
    n, bones = fixed.shape[0], fixed.shape[1]
    values = fixed.reshape(n, bones * BONE_TEXELS, RGBA)
    out = np.zeros((n, bones * BONE_TEXELS, PALETTE_TEXELS, RGBA), dtype=np.uint8)
    out[:, :, 0, :] = values // BYTE_STEPS
    out[:, :, 1, :] = values % BYTE_STEPS
    return out.reshape(n, bones * BONE_TEXELS * PALETTE_TEXELS * RGBA)


def write(path, base, clips, baked):
    count, bones, lods = base['count'], base['bones'], base['lods']
    block_rows = sum(c['frames'] for c in clips)
    pal_w = bones * BONE_TEXELS
    out = bytearray()
    out += struct.pack('<11I', MAGIC, VERSION, count, bones, lods, pal_w, block_rows, len(clips), base['hand'], base['head'], COLUMNS)
    row = 0
    for c in clips:
        name = c['name'].encode()
        if len(name) > CLIP_NAME_BYTES:
            raise ValueError(f'clip name too long: {c["name"]}')
        out += name.ljust(CLIP_NAME_BYTES, b'\0')
        out += struct.pack('<2I2f2I', row, c['frames'], c['seconds'][0], c['seconds'][1], c['kind'], 1 if c['loop'] else 0)
        row += c['frames']
    out += base['card']
    for b, (_, fits, _) in zip(base['bodies'], baked):
        out += struct.pack('<fIf', b['height'], b['flags'], b['alpha'])
        out += struct.pack('<3f', *b['seat'])
        out += struct.pack('<f', b['mood'])
        out += struct.pack('<12f', *b['palm'])
        out += struct.pack('<12f', *b['helmet'])
        out += struct.pack(f'<{lods * 2}I', *b['sizes'])
        out += fits.astype('<f4').tobytes()
    for _, _, curves in baked:
        for curve in curves:
            out += np.asarray(curve, dtype='<f4').tobytes()
    for b in base['bodies']:
        out += b['meshes']
    line = pal_w * PALETTE_TEXELS * RGBA
    body_rows = -(-count // COLUMNS)
    image = np.zeros((body_rows * block_rows, COLUMNS * line), dtype=np.uint8)
    for v, (rows, _, _) in enumerate(baked):
        r0, c0 = (v // COLUMNS) * block_rows, (v % COLUMNS) * line
        image[r0:r0 + block_rows, c0:c0 + line] = palette_bytes(rows)
    out += image.tobytes()
    out += struct.pack('<I', base['face_lines'])
    out += base['faces']
    out += base['bone_part'] + base['joint_bone']
    for b in base['bodies']:
        out += b['joints']
    Path(path).write_bytes(bytes(out))
    return block_rows
