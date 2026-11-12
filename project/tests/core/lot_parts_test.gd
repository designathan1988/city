## What a lot's layout lays is whole (probe lot_parts: bodies made on the spot as the generator makes a record's, on lots of each
## zone and size, no city generated): a planter is laid with what grows in it, never as a bare box (a terrace's pots, a kitchen
## garden's raised beds, a court's and a forecourt's planters, a car park's and a plaza's tree planters), and the plant stands on the
## planter's soil; and no part stood on the ground (a tree, a shrub, a lamp, a shed) is as tall as what its building holds over it
## there: a roof's eaves, a balcony, an awning, an upper block (sim/buildings/b40/body_air.h).
## Run: godot --path project res://tests/core/lot_parts_test.tscn -- --no-flow
extends TestCase

const LAND_MAP := 0
const SEED := 7
const BODIES := 60 # of each zone and lot size
## zones.json: residential, commercial, industrial, mixed, and the low and high of the first two
const ZONES := [1, 2, 3, 4, 5, 6, 7, 8]
## the lots the town cuts: width and depth, metres
const LOTS := [[10.0, 28.0], [14.0, 32.0], [20.0, 36.0], [30.0, 40.0]]
## the open lots: function (BuildingFn: 5 a square, 6 a park), width and depth
const OPEN_LOTS := [[6, 24.0, 32.0], [6, 44.0, 60.0], [6, 70.0, 70.0], [5, 40.0, 40.0], [5, 60.0, 50.0]]
const OPEN_BODIES := 4
## a sports court (BuildingFn 13): its grove stands round the court's run, outside its fence
const COURTS := [[13, 30.0, 44.0]]
## the lines of an open lot a street runs along besides its front (b40::LotEdges street: 1 the left, 2 the right, 4 the back): the
## front alone (a lot between neighbours), a lot through its block, a whole block
const STREET_SIDES := [0, 4, 7]


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(LAND_MAP, SEED, false)
	var planters := 0
	var benches := 0
	var trees := 0
	var posts := {} # lamps on their posts, by zone
	var by_zone := {}
	for zone: int in ZONES:
		for lot: Array in LOTS:
			var parts: Dictionary = CityStore.query(&"lot_parts", [zone, 0, lot[0], lot[1], 1, BODIES])
			planters += int(parts.planters)
			by_zone[zone] = int(by_zone.get(zone, 0)) + int(parts.planters)
			var where := "zone %d on %.0f x %.0f" % [zone, lot[0], lot[1]]
			check(int(parts.bare) == 0, "%s: no planter stands bare (%d of %d; the first: %s)" % [where, parts.bare, parts.planters, parts.first_bare])
			check(int(parts.off_soil) == 0, "%s: what grows in a planter stands on its soil (%d; the first: %s)" % [where, parts.off_soil, parts.first_off])
			check(int(parts.under_body) == 0, "%s: no part stands as tall as what its building holds over it (%d of %d; the first: %s)" % [where, parts.under_body, parts.standing, parts.first_under])
			check(int(parts.bench_askew) == 0, "%s: a bench's seat runs across the way it looks (%d of %d laid along it; the first: %s)" % [where, parts.bench_askew, parts.benches, parts.first_askew])
			benches += int(parts.benches)
			trees += int(parts.trees)
			posts[zone] = int(posts.get(zone, 0)) + int(parts.posts)
	# The checks above hold something: the layouts lay planters, and the houses' terraces their pots.
	check(planters > 0, "the lots lay planters (%d)" % planters)
	check(int(by_zone.get(1, 0)) > 0, "the houses' lots lay planters (%d)" % int(by_zone.get(1, 0)))
	check(trees > 0, "the lots plant trees (%d)" % trees)
	# A works' apron keeps its lamp beside the door: stood out from under the door's canopy, never left out for standing under it.
	check(int(posts.get(3, 0)) >= LOTS.size() * BODIES, "every works lot has a lamp on its post (%d of %d lots)" % [int(posts.get(3, 0)), LOTS.size() * BODIES])
	check(benches > 0, "the lots lay benches (%d)" % benches)
	# The open lots: a park's benches along its paths, a square's round its fountain. Each looks at the path or the fountain it stands by.
	var open_benches := 0
	for lot: Array in OPEN_LOTS + COURTS:
		for sides: int in STREET_SIDES:
			open_benches += _check_open_lot(lot[0], lot[1], lot[2], sides)
	check(open_benches > 0, "the open lots lay benches (%d)" % open_benches)
	print("lot_parts: lamps on posts by zone %s" % posts)
	print("lot_parts: %d planters, %d trees over %d bodies; planters by zone %s" % [planters, trees, ZONES.size() * LOTS.size() * BODIES, by_zone])
	finish()


## An open lot's bodies made on the spot, a street along its front and the lines `sides` names: its benches, its grove and its ways.
## Returns the benches laid.
func _check_open_lot(fn: int, w: float, d: float, sides: int) -> int:
	var open: Dictionary = CityStore.query(&"open_lot_parts", [fn, w, d, 1, OPEN_BODIES, sides])
	var where := "open lot (function %d) %.0f x %.0f, street sides %d" % [fn, w, d, sides]
	check(int(open.bench_askew) == 0, "%s: a bench's seat runs across the way it looks (%d of %d laid along it; the first: %s)" % [where, open.bench_askew, open.benches, open.first_askew])
	check(int(open.bench_away) == 0, "%s: a bench looks at the path or the fountain it stands by (%d of %d look away; the first: %s)" % [where, open.bench_away, open.benches, open.first_away])
	check(int(open.bench_on_cover) == 0, "%s: a bench stands beside a path, never on one (%d of %d; the first: %s)" % [where, open.bench_on_cover, open.benches, open.first_on])
	# Its trees are parts of the lot: each one's element is the room of its crown, as a lot garden's tree.
	print("%s: %d trees in %d bodies, the narrowest room %.2f m, the most slender %.2f" % [where, open.trees, open.bodies, open.room_least_m, open.slender])
	check(int(open.bare) == 0, "%s: every body has its trees (%d of %d bodies with none)" % [where, open.bare, open.bodies])
	check(int(open.tree_out) == 0, "%s: every tree's room inside the lot's lines (%d of %d past them; the first: %s)" % [where, open.tree_out, open.trees, open.first_out])
	check(int(open.tree_holds) == 0, "%s: no tree's room holds another part (a lamp, a bench, a hedge, a fountain's rim): %d of %d; the first: %s" % [where, open.tree_holds, open.trees, open.first_holds])
	check(int(open.tree_meet) == 0, "%s: no two trees' rooms in one another (%d pairs; the first: %s)" % [where, open.tree_meet, open.first_meet])
	check(int(open.trunk_on_hard) == 0, "%s: every trunk stands on planted ground (%d of %d on a hard cover; the first: %s)" % [where, open.trunk_on_hard, open.trees, open.first_hard])
	# Its ways lead somewhere: a path reaches a line only where a street runs along it, and the hedge is cut only there.
	check(int(open.path_ends_blind) == 0, "%s: no path leads to a line no street runs along (%d; the first (seed, line, the cover): %s)" % [where, open.path_ends_blind, open.first_blind])
	check(int(open.hedge_gaps_blind) == 0, "%s: the hedge runs whole along a line no street runs along (%d lines; the first (seed, line, hedged, length): %s)" % [where, open.hedge_gaps_blind, open.first_gap])
	return int(open.benches)
