## What the Inspector does to a road, in the sorting of edit_fuzz_test.gd: reverse it, bend it, lift a node, join, a crossing, a U-turn,
## a movement banned, a node removed. Each returns what it dispatched ("" for nothing), which the fuzz counts.

const REVERSE := 9
const BEND := 18
const LIFT := 19
const JOIN := 20
const CROSSING := 21
const UTURN := 22
const BAN := 23
const REMOVE := 24
const BEND_M := 30.0
const BEND_AT_LEAST := 0.2
const BEND_LIFT_M := 4.0


## `pick_px`: how near the node a pick takes (edit_fuzz_test.gd PX).
static func run(rng: RandomNumberGenerator, kind: int, road: Vector3, pick_px: float) -> String:
	var seg: int = int(CityStore.query(&"segment_at", [road]).get("seg", -1)) # segment_at's key (city_core_edit.cpp); "id" never was
	var node: int = CityStore.query(&"node_at", [road, pick_px]).get("node", -1)
	if kind == REVERSE and seg >= 0:
		CityStore.dispatch(&"reverse_road", [seg])
		return "reverse_road"
	if kind == BEND and seg >= 0:
		CityStore.dispatch(&"bend_road", [seg, rng.randf_range(-BEND_M, BEND_M), rng.randf_range(BEND_AT_LEAST, 1.0 - BEND_AT_LEAST)])
		return "bend_road"
	if kind == LIFT and node >= 0:
		CityStore.dispatch(&"set_node_height", [node, rng.randf_range(-BEND_LIFT_M, BEND_LIFT_M)])
		return "set_node_height"
	if kind == JOIN and node >= 0:
		CityStore.dispatch(&"join_roads", [node])
		return "join_roads"
	if kind == CROSSING:
		CityStore.dispatch(&"add_crossing", [road, rng.randi_range(1, 2)])
		return "add_crossing"
	if kind == UTURN:
		CityStore.dispatch(&"add_uturn", [road, rng.randf() < 0.5])
		return "add_uturn"
	if kind == BAN and node >= 0:
		var legs: Array = CityStore.query(&"node_legs", [node])
		if legs.size() >= 2:
			CityStore.dispatch(&"set_movement_banned", [node, legs[0].seg, legs[1].seg, legs[0].banned.is_empty()])
			return "set_movement_banned"
	if kind == REMOVE and node >= 0:
		CityStore.dispatch(&"remove_node", [node])
		return "remove_node"
	return ""
