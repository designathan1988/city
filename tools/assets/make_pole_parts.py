"""The utility pole of each street style in project/data/render/streets.json, from the Roadcraft 4.0's structures (its
src/world/utilities.ts and src/render/utilities.ts placePole, after CELESC E-313.0002, 2024): the urban pole, its cross-arm held out
over the street from the mast (structure B1, Figura 43) with the secondary rack under it (S1, Figura 71), and the rural pole, its
cross-arm across the crown (N1, Figura 31). Every part is in the pole's frame: y up from its foot, +z toward the street, x along
the line. Written in place: each style's pole parts (all but the lighting column, part 16), its wires' seats and the height of its
lamp. Run from the repository root: python tools/assets/make_pole_parts.py
"""
import json
import math
import re
import sys

STREETS = "project/data/render/streets.json"
HEIGHTS = {"roadcraft": 8.5, "brasil": 10.2}  # the mast, crown above the foot: the 4.0's POLE_HEIGHT; brasil's from its _doc

# The sets of parts a pole may show (sim/catalog/furniture_types.h PoleSet).
MAST, ARM, CAN, LAMP, COLUMN, ARM_RURAL, CAN_RURAL = 1, 2, 4, 8, 16, 32, 64

TOP_R, BASE_R = 0.11, 0.18  # POLE_TOP_RADIUS, POLE_BASE_RADIUS
ARM_DROP, ARM_THICK = 0.15, 0.1  # POLE_ARM_DROP (its top under the crown), POLE_ARM_THICK
ARM_SPAN = {"urban": (0.0, 2.0), "rural": (-1.0, 1.0)}  # POLE_ARM_SPAN
PRIMARY = {"urban": [0.55, 1.25, 1.85], "rural": [-0.95, 0.35, 0.95]}  # POLE_PRIMARY
INSULATOR_R, INSULATOR_TALL = 0.07, 0.2  # INSULATOR_RADIUS, POLE_INSULATOR_TALL; the bell's top 0.55 of its foot
BELL_TOP = 0.55
BRACE_DROP, BRACE_REACH, BRACE_THICK = 0.5, 0.45, 0.05  # POLE_BRACE_DROP, POLE_BRACE_REACH, BRACE_THICK
SECONDARY_OUT, SECONDARY_DROPS, SPOOL_WIDE, RACK_PAST, RACK_DEEP = 0.22, [1.3, 1.5], 0.08, 0.1, 0.06  # S1 rack and spools
CAN_R, CAN_TALL, CAN_UNDER_ARM = 0.28, 0.95, 0.75  # the transformer: radius, height, its middle under the arm's middle
LAMP_REACH, LAMP_DROP, LAMP_LONG, LAMP_TALL, LAMP_WIDE = 1.6, 2.0, 0.5, 0.14, 0.26  # POLE_LAMP_*
LAMP_TILT = math.radians(8)  # the bracket's rise
LENS_SHARE, LENS_THICK = 0.8, 0.01  # the lens under the head: 0.8 of it, as a thin box
CAP_R, CAP_TOP_R, CAP_TALL, CAP_SINK = 0.1265, 0.044, 0.08, 0.01  # the crown's cap: its foot 1 cm under the crown
MAST_SIDES, BELL_SIDES, CAN_SIDES, CAP_SIDES = 12, 10, 14, 10
WOOD, ARM_WOOD, STEEL, PORCELAIN, METAL, HOUSING, LENS = "#4e3b2a", "#3a2c20", "#8c9590", "#d8dcd6", "#4a4a46", "#a9b0ad", "#ffdba0"


def r(v):
    return round(v, 4)


def mast_radius_at(h, height):
    return BASE_R + (TOP_R - BASE_R) * min(1.0, max(0.0, h / height))


def frustum(base, radius, top, tall, sides, color, part):
    return {"shape": "frustum", "base": [r(x) for x in base], "radius": r(radius), "top_radius": r(top), "height": r(tall),
            "segments": sides, "color": color, "part": part}


def arm(kind, height, part):
    out = []
    lo, hi = ARM_SPAN[kind]
    arm_top = height - ARM_DROP
    out.append({"shape": "box", "min": [-ARM_THICK / 2, r(arm_top - ARM_THICK), lo], "max": [ARM_THICK / 2, r(arm_top), hi],
                "color": ARM_WOOD, "wood": True, "part": part})
    for along in PRIMARY[kind]:
        out.append(frustum([0, arm_top, along], INSULATOR_R, INSULATOR_R * BELL_TOP, INSULATOR_TALL, BELL_SIDES, PORCELAIN, part))
    # The brace from the mast under the arm up to it: on the street's side (urban), on the lone insulator's (rural).
    side = 1 if kind == "urban" else -1
    strap, under = arm_top - BRACE_DROP, arm_top - ARM_THICK
    foot = mast_radius_at(strap, height)
    out.append({"shape": "beam", "from": [0, r(strap), r(side * foot)], "to": [0, r(under), r(side * BRACE_REACH)],
                "width": BRACE_THICK, "thick": BRACE_THICK / 2, "color": STEEL, "part": part})
    if kind != "urban":
        return out
    # The secondary rack bolted to the mast's street side, a spool on it under each secondary wire.
    high, low = height - SECONDARY_DROPS[0], height - SECONDARY_DROPS[-1]
    face = mast_radius_at((high + low) / 2, height)
    out.append({"shape": "box", "min": [-RACK_DEEP / 2, r(low - RACK_PAST), r(face)], "max": [RACK_DEEP / 2, r(high + RACK_PAST), r(face + BRACE_THICK)],
                "color": STEEL, "part": part})
    for drop in SECONDARY_DROPS:
        out.append(frustum([0, height - drop - SPOOL_WIDE / 2, SECONDARY_OUT], SPOOL_WIDE / 2, SPOOL_WIDE / 2 * BELL_TOP, SPOOL_WIDE,
                           BELL_SIDES, PORCELAIN, part))
    return out


def can(kind, height, part):
    # Just under the arm, on the side its brace is not: away from the street on an urban pole, toward it on a rural one.
    away = -1 if kind == "urban" else 1
    middle = height - ARM_DROP - ARM_THICK / 2 - CAN_UNDER_ARM
    return [{"shape": "cylinder", "base": [0, r(middle - CAN_TALL / 2), r(away * (BASE_R + CAN_R))], "radius": CAN_R, "height": CAN_TALL,
             "segments": CAN_SIDES, "color": STEEL, "part": part}]


def lamp(height):
    head = height - LAMP_DROP
    lens_w, lens_l = LAMP_WIDE * LENS_SHARE / 2, LAMP_LONG * LENS_SHARE / 2
    return [
        {"shape": "beam", "from": [0, r(head - math.tan(LAMP_TILT) * LAMP_REACH), 0], "to": [0, r(head), LAMP_REACH],
         "width": ARM_THICK * 0.8, "thick": ARM_THICK * 0.8, "color": METAL, "part": LAMP},
        {"shape": "box", "min": [-LAMP_WIDE / 2, r(head - LAMP_TALL), r(LAMP_REACH - LAMP_LONG / 2)], "max": [LAMP_WIDE / 2, r(head), r(LAMP_REACH + LAMP_LONG / 2)],
         "color": HOUSING, "part": LAMP},
        {"shape": "box", "min": [r(-lens_w), r(head - LAMP_TALL - LENS_THICK), r(LAMP_REACH - lens_l)], "max": [r(lens_w), r(head - LAMP_TALL), r(LAMP_REACH + lens_l)],
         "color": LENS, "glow": "night", "part": LAMP},
    ]


def pole(height):
    parts = [frustum([0, 0, 0], BASE_R, TOP_R, height, MAST_SIDES, WOOD, MAST), frustum([0, height - CAP_SINK, 0], CAP_R, CAP_TOP_R, CAP_TALL, CAP_SIDES, STEEL, MAST)]
    parts[0]["wood"] = True
    return parts + arm("urban", height, ARM) + arm("rural", height, ARM_RURAL) + can("urban", height, CAN) + can("rural", height, CAN_RURAL) + lamp(height)


def wires(height):
    # The primary wires rest on their insulators' tops; the secondary ones on the rack's spools (the 4.0's primarySeats, secondarySeats).
    return {"y_m": [r(height - ARM_DROP + INSULATOR_TALL)] + [r(height - d) for d in SECONDARY_DROPS],
            "rural_z_m": PRIMARY["rural"], "secondary_z_m": SECONDARY_OUT}


def main():
    lines = open(STREETS, encoding="utf-8").read().split("\n")
    style = None
    out = []
    i = 0
    while i < len(lines):
        line = lines[i]
        m = re.match(r'^  "(\w+)": \{$', line)
        if m and m.group(1) in HEIGHTS:
            style = m.group(1)
        if style and line.strip() == '"pole": [':
            out.append(line)
            indent = re.match(r"\s*", lines[i + 1]).group(0)
            i += 1
            kept = []
            while not lines[i].strip().startswith("]"):
                part = json.loads(lines[i].strip().rstrip(","))
                if part.get("part") == COLUMN:
                    kept.append(part)
                i += 1
            parts = pole(HEIGHTS[style]) + kept
            out.extend(indent + json.dumps(p) + ("," if k + 1 < len(parts) else "") for k, p in enumerate(parts))
            continue
        if style and line.strip().startswith('"wires": {'):
            indent = re.match(r"\s*", line).group(0)
            out.append(indent + '"wires": ' + json.dumps(wires(HEIGHTS[style])) + ",")
            # the light from the lens's face: under the head (render/props/street_props.cpp push_light)
            out.append(indent + '"lamp_light": ' + json.dumps({"lamp_height_m": r(HEIGHTS[style] - LAMP_DROP - LAMP_TALL)}) + ",")
            if lines[i + 1].strip().startswith('"lamp_light"'):
                i += 1
            i += 1
            style = None
            continue
        out.append(line)
        i += 1
    text = "\n".join(out)
    json.loads(text)
    open(STREETS, "w", encoding="utf-8", newline="").write(text)
    return 0


if __name__ == "__main__":
    sys.exit(main())
