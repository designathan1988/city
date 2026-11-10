## The Move case of street_works_test: what the player put on a road's footways goes with it when its node moves.
class_name StreetMoveCases
extends RefCounted

## A road of two pieces on free ground past the town, its middle node moved aside with a line of poles along it.
const FIELD_M := 400.0
const MOVE_LEN_M := 80.0
const MOVE_BY_M := 12.0
const FOOTWAY_OFF_M := 5.0
const PX := 0.25
const KERB_KEPT_M := 0.1
const REACH := 6.0
const LAMPS_ALL := 2
const URBAN := 0 # drawers.json pole_kinds

var _check: Callable


func _init(check: Callable) -> void:
	_check = check


## Moving a node carries what the player put on its roads' footways (Roadcraft nodeMover.ts carryStreetKit): the poles and
## their wires stay, on the roads laid again; undo puts it all back.
func run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + FIELD_M, 0, (b.min.z + b.max.z) * 0.5)
	var mid := a + Vector3(MOVE_LEN_M, 0, 0)
	var e := mid + Vector3(MOVE_LEN_M, 0, 0)
	var road: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, mid, false, Vector3.ZERO, road, PX])
	CityStore.dispatch(&"build_road", [mid, e, false, Vector3.ZERO, road, PX])
	# the ends as the pole tool clicks them, on the footway of each piece (the native line snaps them, pole_tool.gd)
	var from := a.lerp(mid, 0.3) + Vector3(0, 0, FOOTWAY_OFF_M)
	var to := mid.lerp(e, 0.7) + Vector3(0, 0, FOOTWAY_OFF_M)
	var plan: Dictionary = CityStore.query(&"preview_pole_run", [from, to, REACH, LAMPS_ALL, URBAN])
	_check.call(plan.get("ok", false), "a line on the pavements of both new roads: %s" % plan.get("error", ""))
	if not plan.get("ok", false):
		return
	var run: Dictionary = CityStore.dispatch(&"build_pole_run", [from, to, REACH, LAMPS_ALL, URBAN])
	var strung: Dictionary = CityStore.query(&"street_counts")
	_check.call(run.get("ok", false) and strung.spans > 0, "a line along both: %s" % run)
	var off := _off_axis()
	var node: int = CityStore.query(&"node_at", [mid, PX]).node
	var moved: Dictionary = CityStore.dispatch(&"move_node", [node, mid + Vector3(0, 0, MOVE_BY_M)])
	var now: Dictionary = CityStore.query(&"street_counts")
	_check.call(moved.get("ok", false) and now.poles == strung.poles and now.spans == strung.spans,
		"moving the node takes the poles and wires: %s -> %s %s" % [strung, now, moved.get("error", "")])
	var after := _off_axis()
	var kept := after.size() == off.size()
	for i in mini(after.size(), off.size()):
		kept = kept and absf(after[i] - off[i]) < KERB_KEPT_M
	_check.call(kept, "each pole the same distance from the moved road's axis: %s -> %s" % [off, after])
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"street_counts").poles == strung.poles and CityStore.query(&"street_counts").spans == strung.spans, "undoing the move")


## How far each pole stands from the axis of the road under it, sorted.
func _off_axis() -> Array:
	var out := []
	for p: Vector3 in CityStore.query(&"street_poles"):
		var seg: Dictionary = CityStore.query(&"segment_at", [p])
		out.push_back(Vector2(p.x, p.z).distance_to(Vector2(seg.at.x, seg.at.z)) if seg.get("seg", -1) >= 0 else INF)
	out.sort()
	return out
