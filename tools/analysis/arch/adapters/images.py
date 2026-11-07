"""How many pixels an image file holds, from its own header: what its texture costs to read does not depend on what it is called."""
import re
import struct

SIZE = re.compile(rb'''<svg[^>]*?\bwidth="([\d.]+)[^"]*"[^>]*?\bheight="([\d.]+)''', re.S)
BOX = re.compile(rb'''viewBox="[\d.\s-]*?([\d.]+)\s+([\d.]+)"''')


def _jpeg(data):
    i = 2
    while i + 9 < len(data):
        if data[i] != 0xFF:
            return None
        kind, size = data[i + 1], struct.unpack(">H", data[i + 2:i + 4])[0]
        if 0xC0 <= kind <= 0xCF and kind not in (0xC4, 0xC8, 0xCC):
            high, wide = struct.unpack(">HH", data[i + 5:i + 9])
            return high * wide
        i += 2 + size
    return None


def pixels(data):
    """Width times height, or None for a format with no size to read."""
    if data[:8] == b"\x89PNG\r\n\x1a\n":
        wide, high = struct.unpack(">II", data[16:24])
        return wide * high
    if data[:2] == b"\xff\xd8":
        return _jpeg(data)
    found = SIZE.search(data) or BOX.search(data)
    return float(found.group(1)) * float(found.group(2)) if found else None
