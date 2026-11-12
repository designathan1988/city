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


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(LAND_MAP, SEED, false)
	var planters := 0
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
			trees += int(parts.trees)
			posts[zone] = int(posts.get(zone, 0)) + int(parts.posts)
	# The checks above hold something: the layouts lay planters, and the houses' terraces their pots.
	check(planters > 0, "the lots lay planters (%d)" % planters)
	check(int(by_zone.get(1, 0)) > 0, "the houses' lots lay planters (%d)" % int(by_zone.get(1, 0)))
	check(trees > 0, "the lots plant trees (%d)" % trees)
	# A works' apron keeps its lamp beside the door: stood out from under the door's canopy, never left out for standing under it.
	check(int(posts.get(3, 0)) >= LOTS.size() * BODIES, "every works lot has a lamp on its post (%d of %d lots)" % [int(posts.get(3, 0)), LOTS.size() * BODIES])
	print("lot_parts: lamps on posts by zone %s" % posts)
	print("lot_parts: %d planters, %d trees over %d bodies; planters by zone %s" % [planters, trees, ZONES.size() * LOTS.size() * BODIES, by_zone])
	finish()
