"""Packs the looks of the signature buildings (project/assets/lots/materials.sig) a picture at a time.

The cook's table is one deflate stream of every look's record and picture (33 MiB unpacked for 127 looks): to read one picture
the game unpacked all of it and kept it. This layout keeps the records plain and each picture in a stream of its own, so the game
holds the packed file (0.6 MiB) and unpacks the picture a building asks for (native/src/render/buildings/signature_bodies.cpp):

  u32 magic "RCSL", u32 looks, then a record a look: name[24], 10 f32, u32 flags, u32 width, u32 height, u32 packed bytes;
  then each picture's zlib stream (RGBA8, width x height), in the looks' order, none for a look without a picture.

Run on the cook's table (magic "RCSM"): python tools/assets/pack_signature_looks.py <materials.sig> [<out>]. A file already in
this layout is left as it is. Exits 1 on a file that is neither, or that does not end where its looks end.
"""
import struct
import sys
import zlib

COOKED, PACKED = 0x4D534352, 0x4C534352  # "RCSM", "RCSL"
NAME_BYTES, RECORD_FLOATS, RGBA = 24, 10, 4
RECORD = NAME_BYTES + RECORD_FLOATS * 4 + 4  # the name, the floats and the flags
BEST = 9  # zlib's smallest output: the file is packed once and read at every start


def pack(raw: bytes) -> bytes | None:
    magic, size = struct.unpack_from("<II", raw, 0)
    if magic == PACKED:
        return None
    if magic != COOKED:
        raise ValueError("not a table of signature looks")
    data = zlib.decompress(raw[8:])
    if len(data) != size:
        raise ValueError("the table does not unpack to the size it says")
    count, = struct.unpack_from("<I", data, 0)
    at, records, streams = 4, [], []
    for _ in range(count):
        record = data[at:at + RECORD]
        width, height = struct.unpack_from("<II", data, at + RECORD)
        at += RECORD + 8
        pixels = data[at:at + width * height * RGBA]
        if len(record) != RECORD or len(pixels) != width * height * RGBA:
            raise ValueError("the table ends inside a look")
        at += len(pixels)
        stream = zlib.compress(pixels, BEST) if pixels else b""
        records.append(record + struct.pack("<III", width, height, len(stream)))
        streams.append(stream)
    if at != len(data):
        raise ValueError("the table does not end where its looks end")
    return struct.pack("<II", PACKED, count) + b"".join(records) + b"".join(streams)


def main(argv: list[str]) -> int:
    if len(argv) not in (1, 2):
        print(__doc__, file=sys.stderr)
        return 2
    source, target = argv[0], argv[-1]
    try:
        with open(source, "rb") as f:
            packed = pack(f.read())
    except (OSError, ValueError, struct.error, zlib.error) as e:
        print(f"{source}: {e}", file=sys.stderr)
        return 1
    if packed is None:
        print(f"{source}: already packed a picture at a time")
        return 0
    with open(target, "wb") as f:
        f.write(packed)
    print(f"{target}: {len(packed)} bytes")
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
