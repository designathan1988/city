"""What the people's pack needs before it is drawn close up (tools/assets/cook_people.py).

The head's skin: the inside of the head (mouth, eye sockets, ear canals) has a dark red texture, and simplifying the mesh dragged
that colour onto the face, up to nine vertices in ten on some bodies. The body's own skin, measured on the arms and legs of the
same variation, takes its place.

The waist: the garments were cut apart, and in the bind pose the bottom's top edge lies a median 3 to 8 cm below the top's hem
(analise-pessoas/hem_posed.py: no vertex of the bottom comes through the top, two at most in any frame). What shows there is the
body's skin in patches, under a shirt. The trunk's skin below the middle of the top is painted as the top; the neckline above
it stays skin.
"""
import numpy as np

SKIN, TOP = 0, 2
HEAD_BONES = ("neck_01", "head")
TRUNK_BONES = ("pelvis", "spine_01", "spine_02", "spine_03")
REGION_SHIFT = 24
RGB_MASK = 0x00FFFFFF


def _rgb(colour):
    return np.stack([(colour >> s) & 0xFF for s in (0, 8, 16)], axis=1)


def _packed(rgb):
    r, g, b = (int(v) for v in rgb)
    return r | g << 8 | b << 16


def _on(bones, names, wanted):
    return np.isin(bones & 0xFF, [i for i, n in enumerate(names) if n in wanted])


def clean_head(colour, bones, names):
    """`colour` (u32 r g b region) with the head's skin in the colour of the rest of the body's; how many it took."""
    skin = (colour >> REGION_SHIFT) == SKIN
    head = _on(bones, names, HEAD_BONES)
    body = skin & ~head
    if not body.any():
        return colour, 0
    out = colour.copy()
    fixed = skin & head
    out[fixed] = (out[fixed] & np.uint32(~RGB_MASK & 0xFFFFFFFF)) | np.uint32(_packed(np.median(_rgb(colour[body]), axis=0)))
    return out, int(fixed.sum())


def clean_waist(colour, height, detail, bones, names):
    """`colour` and `detail` with the trunk's skin under the middle of the top dressed as the top; how many it took."""
    region = colour >> REGION_SHIFT
    top = region == TOP
    if not top.any():
        return colour, detail, 0
    under = (region == SKIN) & _on(bones, names, TRUNK_BONES) & (height < np.median(height[top]))
    out, shade = colour.copy(), detail.copy()
    out[under] = np.uint32(TOP << REGION_SHIFT | _packed(np.median(_rgb(colour[top]), axis=0)))
    shade[under] = np.median(detail[top])
    return out, shade, int(under.sum())
