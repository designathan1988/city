"""Writes project/data/catalog/road_profiles.json: Roadcraft's 37 built-in road profiles (src/roads/profiles.ts BUILTIN),
rebuilt with the same helper functions (sw, curb, park, bike, lane, median, shoulder, barrier, lanes, two, one).
Strips are listed from the right edge (negative u) to the left edge looking from node a to node b; lanes with dir 1
go a -> b. Run once when the catalogue changes: python3 tools/assets/make_road_profiles.py <out.json>
"""
import json
import sys

CONCRETE, PAVERS, STONE, GRASS = 0, 1, 2, 3  # paving
ASPHALT, CONCRETE_S, COBBLE, BALLAST = 0, 1, 2, 3  # carriageway surface


def strip(kind, width, **flags):
    out = {"kind": kind, "width": width}
    out.update({key: value for key, value in flags.items() if value})
    return out


def sw(w=3, pave=CONCRETE):
    return strip("sidewalk", w, pave=pave)


CURB = strip("curb", 0.2)
PARK = strip("parking", 2.3)


def bike(w=1.6):
    return strip("bike", w)


def lane(d, w=3.2, **flags):
    return strip("lane", w, dir=d, **flags)


def median(w, pave=GRASS):
    return strip("median", w, pave=pave)


def shoulder(w):
    return strip("shoulder", w)


def barrier(w):
    return strip("barrier", w)


def lanes(d, n, w, bus=None):
    out = [lane(d, w, bus=bool(bus and n > 1 and ((bus == "outer" and i == 0) or (bus == "inner" and i == n - 1))))
           for i in range(n)]
    return out if d == 1 else out[::-1]


def two(walk, n, w, edge=None, med=None, bus=None, pave=CONCRETE, flush=False):
    walk_strip = sw(walk, pave)
    if flush:
        walk_strip["flush"] = True
    s = [walk_strip] + ([] if flush else [CURB]) + ([edge] if edge else []) + lanes(1, n, w, bus)
    if med:
        s += [CURB, med, CURB]
    s += lanes(-1, n, w, bus) + ([edge] if edge else []) + ([] if flush else [CURB]) + [walk_strip]
    return s


def one(walk, n, w):
    return [sw(walk), CURB] + lanes(1, n, w) + [CURB, sw(walk)]


def profile(name, speed, one_way, prio, cat, strips, ring=False, surface=ASPHALT, centre="double"):
    return {"id": name, "name_key": "profile." + name, "speed": speed, "one_way": one_way, "priority": prio,
            "category": cat, "ring": ring, "surface": surface, "centre": centre, "strips": strips}


BUILTIN = [
    profile("localStreet", 40, False, 0, "ruas", [sw(2.6), CURB, lane(1, 3.1), lane(-1, 3.1), CURB, sw(2.6)], centre="dashed"),
    profile("streetWithParking", 40, False, 1, "ruas", [sw(2.4), CURB, PARK, lane(1, 3.0), lane(-1, 3.0), PARK, CURB, sw(2.4)]),
    profile("oneWayWithParking", 50, True, 1, "ruas", [sw(3), CURB, PARK, lane(1), lane(1), CURB, sw(3)]),
    profile("avenue", 60, False, 2, "avenidas",
            [sw(3.5), CURB, lane(1), lane(1), CURB, median(3), CURB, lane(-1), lane(-1), CURB, sw(3.5)]),
    profile("avenueWithCycleTrack", 60, False, 3, "avenidas",
            [sw(4), CURB, bike(1.6)] + [lane(1, 3.3)] * 3 + [CURB, median(4), CURB] + [lane(-1, 3.3)] * 3 + [bike(1.6), CURB, sw(4)]),
    profile("expressway", 90, False, 4, "rodovias",
            [barrier(0.5), shoulder(2.5), lane(1, 3.6), lane(1, 3.6), shoulder(1), barrier(0.8), shoulder(1),
             lane(-1, 3.6), lane(-1, 3.6), shoulder(2.5), barrier(0.5)]),
    profile("roundabout", 30, True, 3, "especiais", [sw(3), CURB, lane(1, 4), lane(1, 4), CURB, median(1.5)], ring=True),
    profile("urbanStreet", 50, False, 1, "ruas", two(2.5, 1, 3.3)),
    profile("oneWay1Lane", 30, True, 0, "ruas", one(2, 1, 3.2)),
    profile("oneWay2Lanes", 40, True, 1, "ruas", one(2, 2, 3)),
    profile("parkingOneSide", 40, False, 1, "ruas", [sw(2.2), CURB, PARK, lane(1, 3), lane(-1, 3), CURB, sw(2.2)]),
    profile("streetWithBikeLane", 40, False, 1, "ruas", two(2.2, 1, 3, edge=bike(1.8))),
    profile("streetWithMedian", 40, False, 1, "ruas", two(3, 1, 3.2, med=median(2))),
    profile("wideSidewalks", 30, False, 0, "ruas", two(4.5, 1, 3, pave=PAVERS), centre="dashed"),
    profile("narrowStreet", 30, False, 0, "ruas", two(1.4, 1, 2.8), centre="dashed"),
    profile("sharedStreet", 20, False, 0, "ruas", two(3, 1, 3, flush=True, pave=STONE), surface=COBBLE, centre="dashed"),
    profile("cobblestoneStreet", 30, False, 0, "ruas", two(2, 1, 3), surface=COBBLE, centre="dashed"),
    profile("avenue4Lanes", 60, False, 2, "avenidas", two(3, 2, 3.2)),
    profile("avenue6LanesMedian", 60, False, 3, "avenidas", two(3.5, 3, 3.3, med=median(3))),
    profile("avenueWithParking", 50, False, 2, "avenidas", two(3, 2, 3.2, edge=PARK)),
    profile("curbsideBusLane", 60, False, 3, "avenidas", two(3.5, 2, 3.4, med=median(2), bus="outer")),
    profile("centralBusway", 60, False, 3, "avenidas", two(3.5, 3, 3.4, med=median(2, CONCRETE), bus="inner")),
    profile("oneWayAvenue3Lanes", 60, True, 2, "avenidas", one(3, 3, 3.3)),
    profile("oneWayAvenue4Lanes", 60, True, 3, "avenidas", one(3.5, 4, 3.3)),
    profile("boulevard", 60, False, 3, "avenidas", two(4, 2, 3.3, med=median(6), pave=PAVERS)),
    profile("highway6Lanes", 110, False, 5, "rodovias",
            [barrier(0.5), shoulder(3)] + lanes(1, 3, 3.7) + [shoulder(1), barrier(0.8), shoulder(1)] + lanes(-1, 3, 3.7)
            + [shoulder(3), barrier(0.5)]),
    profile("oneWayHighway2Lanes", 100, True, 5, "rodovias",
            [barrier(0.5), shoulder(2.5)] + lanes(1, 2, 3.6) + [shoulder(1), barrier(0.5)]),
    profile("oneWayHighway3Lanes", 110, True, 5, "rodovias",
            [barrier(0.5), shoulder(2.5)] + lanes(1, 3, 3.6) + [shoulder(1), barrier(0.5)]),
    profile("dualCarriageway2Lanes", 80, False, 4, "rodovias",
            [shoulder(2), lane(1, 3.6), shoulder(0.6), barrier(0.8), shoulder(0.6), lane(-1, 3.6), shoulder(2)]),
    profile("countryRoad", 80, False, 3, "rodovias", [shoulder(1.5), lane(1, 3.5), lane(-1, 3.5), shoulder(1.5)], centre="dashed"),
    profile("ramp", 60, True, 4, "especiais", [barrier(0.5), shoulder(1.5), lane(1, 4), shoulder(1), barrier(0.5)]),
    profile("ramp2Lanes", 60, True, 4, "especiais", [barrier(0.5), shoulder(1.5)] + lanes(1, 2, 3.6) + [shoulder(1), barrier(0.5)]),
    profile("ruralCobblestoneRoad", 40, False, 0, "especiais", [shoulder(0.8), lane(1, 3), lane(-1, 3), shoulder(0.8)],
            surface=COBBLE, centre="dashed"),
    profile("concreteRoad", 60, False, 1, "especiais", two(2, 1, 3.8), surface=CONCRETE_S),
    profile("doubleRailway", 90, False, 6, "trilhos",
            [shoulder(1.6), lane(1, 4.3, rail=True), lane(-1, 4.3, rail=True), shoulder(1.6)], surface=BALLAST),
    profile("singleRailway", 80, True, 6, "trilhos", [shoulder(1.6), lane(1, 4.3, rail=True), shoulder(1.6)], surface=BALLAST),
    profile("metroDouble", 80, False, 6, "trilhos",
            [shoulder(0.8), lane(1, 4.0, rail=True), lane(-1, 4.0, rail=True), shoulder(0.8)], surface=BALLAST),
]


def cost_per_metre(p):
    carriage = med = foot = 0.0
    for s in p["strips"]:
        if s["kind"] in ("sidewalk", "curb"):
            foot += s["width"]
        elif s["kind"] in ("median", "barrier"):
            med += s["width"]
        else:
            carriage += s["width"]
    return round(carriage * 150 + med * 40 + foot * 80)


if __name__ == "__main__":
    for p in BUILTIN:
        print(p["id"], round(sum(s["width"] for s in p["strips"]), 2), cost_per_metre(p), file=sys.stderr)
    with open(sys.argv[1], "w", encoding="utf-8") as f:
        # The styles block of road_profiles.json is written by hand: this script only makes the profile list.
        json.dump({"profiles": BUILTIN}, f, indent=1)
        f.write("\n")
