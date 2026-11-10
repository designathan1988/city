## Where the next building grows (Roadcraft world/growth/zoning.ts pickLot, sim/simulation/city_growth.cpp pick_free): two short streets
## laid far apart on bare land and zoned; each street fills along (every building within sim.grow_on_reach_m of the one before), with one
## jump to the other street once the first is full, both fill, and the same zoning grows the same way again.
## Run: godot --path project res://tests/core/growth_pick_test.tscn -- --no-flow
extends TestCase

const MAP := 0
const SEED := 1
const PX := 0.25 # metres a screen pixel covers: how near a point snaps to a road or a node
const PROFILE := 0
const STREET_FROM_M := 260.0 # each street from this far off the map's middle, west and east: 520 m apart, past any reach
const STREET_M := 50.0 # short enough that its two sides lie within the reach of each other
const ZONE_RADIUS_M := 40.0 # the brush over the street's middle reaches both its sides
const ZONE := "res_low"
const STEP_TICKS := 1
const MOST_STEPS := 4000
const QUIET_STEPS := 200 # this many steps with nothing new: the streets are full


func _ready() -> void:
	if not boot():
		finish()
		return
	var first := _grow()
	var again := _grow()
	var reach := DataTables.tuning("sim_grow_on_reach_m")
	var west := 0
	var east := 0
	var jumps := 0
	for i in first.size():
		if first[i].x < 0.0:
			west += 1
		else:
			east += 1
		if i > 0 and Vector2(first[i].x - first[i - 1].x, first[i].z - first[i - 1].z).length() > reach:
			jumps += 1
	print("grown %d: west %d east %d, jumps past %.0f m: %d" % [first.size(), west, east, reach, jumps])
	check(west > 0 and east > 0, "both streets grew (west %d, east %d)" % [west, east])
	check(jumps == 1, "each street filled along, one jump to the other (%d)" % jumps)
	check(first == again, "the same zoning grew the same way (%d and %d buildings)" % [first.size(), again.size()])
	finish()


## A fresh bare map, the two streets laid and zoned, grown until nothing more grows: where each building stood, in the order grown.
func _grow() -> Array[Vector3]:
	new_map(MAP, SEED, false)
	for sign: float in [-1.0, 1.0]:
		var a := Vector3(sign * STREET_FROM_M, 0, 0)
		var b := Vector3(sign * (STREET_FROM_M + STREET_M), 0, 0)
		var built: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, PROFILE, PX])
		check(built.get("ok", false), "a street is laid (%s)" % built.get("error", ""))
		var zoned: Dictionary = CityStore.dispatch(&"paint_zone", [(a + b) * 0.5, ZONE_RADIUS_M, ZoneTable.code(ZONE)])
		check(zoned.get("ok", false), "a street is zoned (%s)" % zoned.get("error", ""))
	var order: Array[Vector3] = []
	var seen := {}
	var quiet := 0
	var streets: int = CityStore.snapshot.street_revision
	for step in MOST_STEPS:
		ticks(STEP_TICKS)
		var fresh := false
		for id: int in CityStore.query(&"list_buildings"):
			if seen.has(id):
				continue
			seen[id] = true
			fresh = true
			order.push_back(CityStore.query(&"probe_building", [id]).position)
		quiet = 0 if fresh else quiet + 1
		if quiet >= QUIET_STEPS:
			break
	check(order.size() > 0 and CityStore.snapshot.street_revision == streets, "the buildings that grew left the streets' revision where it was: the kept furniture stands")
	return order
