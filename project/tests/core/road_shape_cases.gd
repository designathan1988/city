## The road_edit_test cases of what the Inspector does to a road's shape, through the store: a node's height held to the grade, a road bent and straightened,
## two roads in line joined, each undone to the same city.
class_name RoadShapeCases
extends RefCounted

const OFF_M := 1100.0
const ROAD_M := 200.0
const URBAN := 0
const PX := 0.25
const LIFT_M := 3.0
const TOO_HIGH_M := 500.0
const BEND_M := 30.0
const MIDDLE := 0.5
const SAME_M := 2.0
const TICKS := 5
const UTURN_OFF_M := 80.0
const QUARTER := 0.3 # clear of the cut in the middle of a road this long

const STUB_OFF_M := 900.0
const STUB_LEN_M := 300.0
const STUB_AT_M := 110.0
const STUB_ARM_M := 60.0
const STUB_PX := 0.25

var _check: Callable
var _mid := Vector3.ZERO


func _init(check: Callable) -> void:
	_check = check


func run(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, 0)
	var b := a + Vector3(ROAD_M, 0, 0)
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, URBAN, PX, 0.0])
	_check.call(laid.get("ok", false), "a road for the Inspect edits: %s" % [laid])
	var mid := (a + b) * 0.5
	_mid = mid
	var info: Dictionary = CityStore.query(&"inspect", [mid])
	_check.call(info.get("kind", "") == "road", "Inspect takes the road")
	_height(info)
	_bend(info, mid)
	_join(a + (b - a) * QUARTER)
	_crossing(a + (b - a) * QUARTER)
	_uturn(e)
	_movement(e)


func _height(info: Dictionary) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var up: Dictionary = CityStore.dispatch(&"set_node_height", [info.node_b, LIFT_M])
	_check.call(up.get("ok", false), "height of the end: %s" % [up])
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "undoing the height returns to the same city")
	var limited: Dictionary = CityStore.dispatch(&"set_node_height", [info.node_b, TOO_HIGH_M])
	_check.call(limited.get("ok", false), "asking too much goes to the greatest grade: %s" % [limited])
	var lift: float = CityStore.query(&"inspect", [_mid]).lift_b
	_check.call(lift > LIFT_M and lift < TOO_HIGH_M, "the height stops at the greatest grade: %.1f" % lift)
	CityStore.dispatch(&"undo")


func _bend(info: Dictionary, mid: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var bent: Dictionary = CityStore.dispatch(&"bend_road", [info.id, BEND_M, MIDDLE])
	_check.call(bent.get("ok", false), "bending the road: %s" % [bent])
	var now: Dictionary = CityStore.query(&"inspect", [bent.get("at", mid)])
	_check.call(now.get("kind", "") == "road" and absf(absf(float(now.bend_m)) - BEND_M) < SAME_M, "the road keeps the curve asked for: %s" % [now.get("bend_m", "?")])
	_check.call(float(now.length_m) > float(info.length_m), "bent, the road is longer")
	var back: Dictionary = CityStore.dispatch(&"bend_road", [now.id, 0.0, MIDDLE])
	_check.call(back.get("ok", false), "straightening the road: %s" % [back])
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "undoing the curve returns to the same city")


func _crossing(at: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	for kind: int in [RoadPanel.CROSS_ZEBRA, RoadPanel.CROSS_SIGNAL]:
		var made: Dictionary = CityStore.dispatch(&"add_crossing", [at, kind])
		_check.call(made.get("ok", false), "a mid-block crossing: %s" % [made])
		var node: Dictionary = CityStore.query(&"inspect", [at])
		_check.call(node.get("kind", "") == "node" and int(node.crossing) == kind, "Inspect takes the crossing: %s" % [node.get("crossing", "?")])
		var off: Dictionary = CityStore.dispatch(&"remove_crossing", [node.id])
		_check.call(off.get("ok", false), "taking the crossing away: %s" % [off])
		CityStore.dispatch(&"undo")
		CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "undoing the crossing returns to the same city")
	var near_end: Dictionary = CityStore.dispatch(&"add_crossing", [at - Vector3(ROAD_M * QUARTER - 1.0, 0, 0), RoadPanel.CROSS_ZEBRA])
	_check.call(not near_end.get("ok", true), "with no room for cars to stop, the crossing is refused")
	for kind: int in [RoadPanel.CROSS_ZEBRA, RoadPanel.CROSS_SIGNAL]: # the lanes and the junction are made again round it, with the fleet on
		CityStore.dispatch(&"add_crossing", [at, kind])
		CityStore.step_ticks(TICKS)
		CityStore.dispatch(&"undo")


func _uturn(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, UTURN_OFF_M)
	var b := a + Vector3(ROAD_M, 0, 0)
	var at := a + (b - a) * QUARTER
	var refused: Dictionary = CityStore.dispatch(&"add_uturn", [_lay(a, b, "urbanStreet"), true])
	_check.call(not refused.get("ok", true), "with no median there is no U-turn: %s" % [refused.get("error", "")])
	a += Vector3(0, 0, UTURN_OFF_M)
	b += Vector3(0, 0, UTURN_OFF_M)
	at = a + (b - a) * QUARTER
	_lay(a, b, "cityAvenue")
	var before: Dictionary = CityStore.query(&"state_hash")
	var made: Dictionary = CityStore.dispatch(&"add_uturn", [at, true])
	_check.call(made.get("ok", false), "a U-turn through the median: %s" % [made])
	var node: Dictionary = CityStore.query(&"inspect", [at])
	_check.call(node.get("kind", "") == "node" and int(node.uturn) != 0, "Inspect takes the U-turn")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "undoing the U-turn returns to the same city")
	CityStore.dispatch(&"add_uturn", [at, false])
	CityStore.step_ticks(TICKS)
	CityStore.dispatch(&"undo")


func _movement(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, UTURN_OFF_M * 4.0)
	var b := a + Vector3(ROAD_M, 0, 0)
	var mid := a + (b - a) * QUARTER
	_lay(a, b, "urbanStreet")
	_lay(mid, mid + Vector3(0, 0, UTURN_OFF_M), "urbanStreet")
	var node: int = CityStore.query(&"node_at", [mid, PX]).get("node", -1)
	var legs: Array = CityStore.query(&"node_legs", [node])
	_check.call(legs.size() == 3, "three arms: %d" % legs.size())
	CityStore.step_ticks(1)
	var before: int = CityStore.query(&"node_traffic", [node]).movements
	var hash_before: Dictionary = CityStore.query(&"state_hash")
	var banned: Dictionary = CityStore.dispatch(&"set_movement_banned", [node, legs[0].seg, legs[1].seg, true])
	_check.call(banned.get("ok", false), "forbidding a movement: %s" % [banned])
	CityStore.step_ticks(1)
	_check.call(int(CityStore.query(&"node_traffic", [node]).movements) < before, "forbidden, the movement leaves the junction")
	_check.call(CityStore.query(&"node_legs", [node])[0].banned.size() == 1, "the panel reads the forbidden movement")
	CityStore.dispatch(&"undo")
	CityStore.step_ticks(1)
	_check.call(int(CityStore.query(&"node_traffic", [node]).movements) == before, "undone, the movement comes back")
	var along := a + (mid - a) * 0.5
	var two: Dictionary = CityStore.query(&"inspect", [along])
	var spec: Dictionary = CityStore.query(&"road_spec", [two.profile_id])
	var index: int = CityStore.query(&"road_spec_make", [two.profile_id, RoadSpecOps.directed(spec, 1)])
	_check.call(index >= 0, "the one-way road is a profile")
	var turned: Dictionary = CityStore.dispatch(&"upgrade_road", [along, index, false])
	_check.call(turned.get("ok", false) and CityStore.query(&"inspect", [along]).one_way, "two-way becomes one-way: %s" % [turned])
	CityStore.dispatch(&"undo")
	_check.call(not CityStore.query(&"inspect", [along]).one_way, "undone, it is two-way again")
	var gone: Dictionary = CityStore.dispatch(&"remove_node", [node])
	_check.call(gone.get("ok", false), "taking the node with its roads: %s" % [gone])
	CityStore.dispatch(&"undo")


func _lay(a: Vector3, b: Vector3, id: String) -> Vector3:
	var index := -1
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			index = p.index
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, index, PX, 0.0])
	_check.call(laid.get("ok", false), "road %s: %s" % [id, laid])
	return (a + b) * 0.5


func _join(mid: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var cut: Dictionary = CityStore.dispatch(&"split_road", [mid])
	_check.call(cut.get("ok", false), "a point in the middle of the road: %s" % [cut])
	var node: int = CityStore.query(&"node_at", [mid, PX]).get("node", -1)
	_check.call(node >= 0, "the point is a node")
	var joined: Dictionary = CityStore.dispatch(&"join_roads", [node])
	_check.call(joined.get("ok", false), "joining the roads in line: %s" % [joined])
	_check.call(CityStore.query(&"node_at", [mid, PX]).get("node", -1) < 0, "joined, the node goes")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "undoing the join returns to the same city")


## A T laid beside a place where a long road was cut takes the stub back: no short piece caps its mouth.
func stubs(e: Vector3) -> void:
	var a := e + Vector3(0, 0, STUB_OFF_M)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, a + Vector3(STUB_LEN_M, 0, 0), false, Vector3.ZERO, urban, STUB_PX])
	CityStore.refresh()
	var laid: int = CityStore.snapshot.segments
	var t := a + Vector3(STUB_AT_M, 0, 0)
	CityStore.dispatch(&"build_road", [t, t + Vector3(0, 0, STUB_ARM_M), false, Vector3.ZERO, urban, STUB_PX])
	CityStore.refresh()
	_check.call(CityStore.snapshot.segments == laid + 1, "the stub is a road again: %d against %d" % [CityStore.snapshot.segments, laid + 1])
