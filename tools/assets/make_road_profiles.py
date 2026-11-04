"""Writes project/data/catalog/road_profiles*.json: the road catalogue of Roadcraft 4.0 (src/world/roads/catalog.ts and the
classes of src/world/roadTypes.ts), kept in the order the saves index it by. A width here is the 4.0's: a footway counts its
kerb, a median its two kerbs. "rank" is the card's place in its tab; "antigos" holds the two roads the 4.0 dropped, for old
saves only. Strips are listed from the right edge (negative u) to the left edge looking from node a to node b; lanes with
dir 1 go a -> b. Run once when the catalogue changes: python3 tools/assets/make_road_profiles.py
"""
import json

CONCRETE, PAVERS, STONE, GRASS = 0, 1, 2, 3  # paving
ASPHALT, CONCRETE_S, COBBLE, BALLAST = 0, 1, 2, 3  # carriageway surface


def strip(kind, width, **flags):
    out = {"kind": kind, "width": width}
    out.update({key: value for key, value in flags.items() if value})
    return out


KERB = 0.15


def sw(w=3, pave=CONCRETE):
    return strip("sidewalk", w, pave=pave)


CURB = strip("curb", KERB)
PARK = strip("parking", 2)


def bike(w=2):
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
    """Two-way road: footways and median at their whole width, kerbs taken out of them."""
    walk_strip = sw(walk if flush else walk - KERB, pave)
    if flush:
        walk_strip["flush"] = True
    s = [walk_strip] + ([] if flush else [CURB]) + ([edge] if edge else []) + lanes(1, n, w, bus)
    if med:
        s += [CURB, med, CURB]
    s += lanes(-1, n, w, bus) + ([edge] if edge else []) + ([] if flush else [CURB]) + [walk_strip]
    return s


def one(walk, n, w):
    return [sw(walk - KERB), CURB] + lanes(1, n, w) + [CURB, sw(walk - KERB)]


def planted(width, pave=GRASS):
    return median(width - 2 * KERB, pave)


def profile(name, cls, speed, one_way, prio, cat, strips, ring=False, surface=ASPHALT, centre="none"):
    return {"id": name, "name_key": "profile." + name, "road_class": cls, "rank": 0, "speed": speed, "one_way": one_way,
            "priority": prio, "category": cat, "ring": ring, "surface": surface, "centre": centre, "strips": strips}


# The order is the saves' index: new roads go at the end.
BUILTIN = [
    profile("localStreet", "local", 30, False, 0, "ruas", two(2, 1, 3)),
    profile("streetWithParking", "urban", 40, False, 1, "ruas", two(2, 1, 3, edge=PARK), centre="dashed"),
    profile("oneWayWithParking", "urban", 50, True, 1, "antigos", [sw(2.85), CURB, PARK, lane(1, 3), lane(1, 3), CURB, sw(2.85)]),
    profile("avenue", "avenue", 60, False, 2, "avenidas", two(2, 2, 3.5), centre="solid"),
    profile("avenueWithCycleTrack", "avenue", 60, False, 2, "avenidas", two(3, 2, 3, edge=bike(), med=planted(2))),
    profile("expressway", "highway", 100, False, 4, "rodovias", two(1, 2, 4, med=planted(2))),
    profile("roundabout", "urban", 30, True, 3, "especiais", [sw(2.85), CURB, lane(1, 4), lane(1, 4), CURB, median(1.35)], ring=True),
    profile("urbanStreet", "urban", 50, False, 1, "ruas", two(2, 1, 4), centre="dashed"),
    profile("oneWay1Lane", "local", 30, True, 0, "ruas", one(2, 1, 3)),
    profile("oneWay2Lanes", "urban", 40, True, 1, "ruas", one(2, 2, 3)),
    profile("parkingOneSide", "urban", 40, False, 1, "ruas", [sw(1.85), CURB, PARK, lane(1, 3), lane(-1, 3), CURB, sw(1.85)], centre="dashed"),
    profile("streetWithBikeLane", "urban", 40, False, 1, "ruas", two(2, 1, 3, edge=bike()), centre="dashed"),
    profile("streetWithMedian", "urban", 40, False, 1, "ruas", two(3, 1, 3, med=planted(2))),
    profile("wideSidewalks", "local", 30, False, 0, "ruas", two(4, 1, 3)),
    profile("narrowStreet", "local", 30, False, 0, "ruas", two(1, 1, 3)),
    profile("sharedStreet", "local", 20, False, 0, "ruas", two(3, 1, 3, flush=True, pave=STONE), surface=COBBLE),
    profile("cobblestoneStreet", "local", 30, False, 0, "ruas", two(2, 1, 3), surface=COBBLE),
    profile("avenue4Lanes", "avenue", 60, False, 2, "avenidas", two(3, 2, 3, med=planted(2))),
    profile("avenue6LanesMedian", "avenue", 60, False, 2, "avenidas", two(3, 3, 3, med=planted(3))),
    profile("avenueWithParking", "avenue", 50, False, 2, "avenidas", two(3, 2, 3, edge=PARK), centre="solid"),
    profile("curbsideBusLane", "avenue", 60, False, 2, "avenidas", two(3, 2, 3, med=planted(2), bus="outer")),
    profile("centralBusway", "avenue", 60, False, 2, "avenidas", two(3, 3, 3, med=planted(2, CONCRETE), bus="inner")),
    profile("oneWayAvenue3Lanes", "avenue", 60, True, 2, "avenidas", one(3, 3, 3)),
    profile("oneWayAvenue4Lanes", "avenue", 60, True, 2, "avenidas", one(3, 4, 3)),
    profile("boulevard", "boulevard", 60, False, 3, "avenidas", two(3, 2, 4, med=planted(2))),
    profile("highway6Lanes", "highway", 110, False, 4, "rodovias", two(1, 3, 4, med=planted(3, CONCRETE))),
    profile("oneWayHighway2Lanes", "highway", 100, True, 4, "rodovias", one(1, 2, 4)),
    profile("oneWayHighway3Lanes", "highway", 110, True, 4, "rodovias", one(1, 3, 4)),
    profile("dualCarriageway2Lanes", "highway", 80, False, 4, "rodovias", two(1, 1, 4, med=planted(2, CONCRETE))),
    profile("countryRoad", "urban", 80, False, 3, "antigos", [shoulder(1.5), lane(1, 3.5), lane(-1, 3.5), shoulder(1.5)], centre="dashed"),
    profile("ramp", "ramp", 60, True, 3, "especiais", one(1, 1, 4)),
    profile("ramp2Lanes", "ramp", 60, True, 3, "especiais", one(1, 2, 4)),
    profile("ruralCobblestoneRoad", "local", 40, False, 0, "especiais", two(1, 1, 3), surface=COBBLE),
    profile("concreteRoad", "urban", 60, False, 1, "especiais", two(2, 1, 4), surface=CONCRETE_S, centre="dashed"),
    profile("doubleRailway", "rail", 90, False, 6, "trilhos",
            [shoulder(1.6), lane(1, 4.3, rail=True), lane(-1, 4.3, rail=True), shoulder(1.6)], surface=BALLAST),
    profile("singleRailway", "rail", 80, True, 6, "trilhos", [shoulder(1.6), lane(1, 4.3, rail=True), shoulder(1.6)], surface=BALLAST),
    profile("metroDouble", "rail", 80, False, 6, "trilhos",
            [shoulder(0.8), lane(1, 4.0, rail=True), lane(-1, 4.0, rail=True), shoulder(0.8)], surface=BALLAST),
]

# The cards of each tab, in the 4.0's order.
ORDER = {
    "ruas": ["localStreet", "urbanStreet", "oneWay1Lane", "oneWay2Lanes", "streetWithParking", "parkingOneSide",
             "streetWithBikeLane", "streetWithMedian", "wideSidewalks", "narrowStreet", "sharedStreet", "cobblestoneStreet"],
    "avenidas": ["avenue", "avenue4Lanes", "avenue6LanesMedian", "avenueWithCycleTrack", "avenueWithParking", "curbsideBusLane",
                 "centralBusway", "oneWayAvenue3Lanes", "oneWayAvenue4Lanes", "boulevard"],
    "rodovias": ["expressway", "highway6Lanes", "oneWayHighway2Lanes", "oneWayHighway3Lanes", "dualCarriageway2Lanes"],
    "especiais": ["ramp", "ramp2Lanes", "ruralCobblestoneRoad", "concreteRoad"],
    "trilhos": ["doubleRailway", "singleRailway", "metroDouble"],
}
for ids in ORDER.values():
    for i, name in enumerate(ids):
        next(p for p in BUILTIN if p["id"] == name)["rank"] = i

# The catalogue's three files: a data file stays under the gate's line limit.
PARTS = (("road_profiles.json", 0, 12), ("road_profiles_a.json", 12, 24), ("road_profiles_b.json", 24, len(BUILTIN)))
OUT = "project/data/catalog/"


def write(name, data):
    with open(OUT + name, "w", encoding="utf-8", newline="\n") as f:
        json.dump(data, f, indent=1)
        f.write("\n")


if __name__ == "__main__":
    prices = {"carriageway": 150, "median": 40, "footway": 80}
    structure = {"ground": 1, "embankment": 1.4, "trench": 1.8, "bridge": 2.5, "tunnel": 6}
    for name, a, b in PARTS:
        part = {"profiles": BUILTIN[a:b]}
        write(name, {"prices": prices, "structure": structure, **part} if a == 0 else part)
