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
const RING_OFF_M := 1500.0
const RING_ROAD_M := 300.0
const RING_RADIUS_M := 40.0
const RING_STUB_M := 4.0 # from the ring to the junction: nothing of a road
const RING_CLEAR_M := 80.0
const QUARTER := 0.3 # clear of the cut in the middle of a road this long

const STUB_OFF_M := 900.0
const STUB_LEN_M := 300.0
const STUB_AT_M := 110.0
const STUB_ARM_M := 60.0
const STUB_PX := 0.25

var t: Node # the test (TestCase): its check and ticks
var _mid := Vector3.ZERO


func _init(test: Node) -> void:
	t = test


func run(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, 0)
	var b := a + Vector3(ROAD_M, 0, 0)
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, URBAN, PX, 0.0])
	t.check(laid.get("ok", false), "a road for the Inspect edits: %s" % [laid])
	var mid := (a + b) * 0.5
	_mid = mid
	var info: Dictionary = CityStore.query(&"inspect", [mid])
	t.check(info.get("kind", "") == "road", "Inspect takes the road")
	_height(info)
	_bend(info, mid)
	_join(a + (b - a) * QUARTER)
	_crossing(a + (b - a) * QUARTER)
	_uturn(e)
	_movement(e)


func _height(info: Dictionary) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var up: Dictionary = CityStore.dispatch(&"set_node_height", [info.node_b, LIFT_M])
	t.check(up.get("ok", false), "height of the end: %s" % [up])
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"state_hash").whole == before.whole, "undoing the height returns to the same city")
	var limited: Dictionary = CityStore.dispatch(&"set_node_height", [info.node_b, TOO_HIGH_M])
	t.check(limited.get("ok", false), "asking too much goes to the greatest grade: %s" % [limited])
	var lift: float = CityStore.query(&"inspect", [_mid]).lift_b
	t.check(lift > LIFT_M and lift < TOO_HIGH_M, "the height stops at the greatest grade: %.1f" % lift)
	CityStore.dispatch(&"undo")


func _bend(info: Dictionary, mid: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var bent: Dictionary = CityStore.dispatch(&"bend_road", [info.id, BEND_M, MIDDLE])
	t.check(bent.get("ok", false), "bending the road: %s" % [bent])
	var now: Dictionary = CityStore.query(&"inspect", [bent.get("at", mid)])
	t.check(now.get("kind", "") == "road" and absf(absf(float(now.bend_m)) - BEND_M) < SAME_M, "the road keeps the curve asked for: %s" % [now.get("bend_m", "?")])
	t.check(float(now.length_m) > float(info.length_m), "bent, the road is longer")
	var back: Dictionary = CityStore.dispatch(&"bend_road", [now.id, 0.0, MIDDLE])
	t.check(back.get("ok", false), "straightening the road: %s" % [back])
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"state_hash").whole == before.whole, "undoing the curve returns to the same city")


func _crossing(at: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	for kind: int in [RoadPanel.CROSS_ZEBRA, RoadPanel.CROSS_SIGNAL]:
		var made: Dictionary = CityStore.dispatch(&"add_crossing", [at, kind])
		t.check(made.get("ok", false), "a mid-block crossing: %s" % [made])
		var node: Dictionary = CityStore.query(&"inspect", [at])
		t.check(node.get("kind", "") == "node" and int(node.crossing) == kind, "Inspect takes the crossing: %s" % [node.get("crossing", "?")])
		var off: Dictionary = CityStore.dispatch(&"remove_crossing", [node.id])
		t.check(off.get("ok", false), "taking the crossing away: %s" % [off])
		CityStore.dispatch(&"undo")
		CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"state_hash").whole == before.whole, "undoing the crossing returns to the same city")
	var near_end: Dictionary = CityStore.dispatch(&"add_crossing", [at - Vector3(ROAD_M * QUARTER - 1.0, 0, 0), RoadPanel.CROSS_ZEBRA])
	t.check(not near_end.get("ok", true), "with no room for cars to stop, the crossing is refused")
	for kind: int in [RoadPanel.CROSS_ZEBRA, RoadPanel.CROSS_SIGNAL]: # the lanes and the junction are made again round it, with the fleet on
		CityStore.dispatch(&"add_crossing", [at, kind])
		t.ticks(TICKS)
		CityStore.dispatch(&"undo")


func _uturn(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, UTURN_OFF_M)
	var b := a + Vector3(ROAD_M, 0, 0)
	var at := a + (b - a) * QUARTER
	var refused: Dictionary = CityStore.dispatch(&"add_uturn", [_lay(a, b, "urbanStreet"), true])
	t.check(not refused.get("ok", true), "with no median there is no U-turn: %s" % [refused.get("error", "")])
	a += Vector3(0, 0, UTURN_OFF_M)
	b += Vector3(0, 0, UTURN_OFF_M)
	at = a + (b - a) * QUARTER
	_lay(a, b, "cityAvenue")
	var before: Dictionary = CityStore.query(&"state_hash")
	var made: Dictionary = CityStore.dispatch(&"add_uturn", [at, true])
	t.check(made.get("ok", false), "a U-turn through the median: %s" % [made])
	var node: Dictionary = CityStore.query(&"inspect", [at])
	t.check(node.get("kind", "") == "node" and int(node.uturn) != 0, "Inspect takes the U-turn")
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"state_hash").whole == before.whole, "undoing the U-turn returns to the same city")
	CityStore.dispatch(&"add_uturn", [at, false])
	t.ticks(TICKS)
	CityStore.dispatch(&"undo")


func _movement(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, UTURN_OFF_M * 4.0)
	var b := a + Vector3(ROAD_M, 0, 0)
	var mid := a + (b - a) * QUARTER
	_lay(a, b, "urbanStreet")
	_lay(mid, mid + Vector3(0, 0, UTURN_OFF_M), "urbanStreet")
	var node: int = CityStore.query(&"node_at", [mid, PX]).get("node", -1)
	var legs: Array = CityStore.query(&"node_legs", [node])
	t.check(legs.size() == 3, "three arms: %d" % legs.size())
	t.ticks(1)
	var before: int = CityStore.query(&"node_traffic", [node]).movements
	var hash_before: Dictionary = CityStore.query(&"state_hash")
	var banned: Dictionary = CityStore.dispatch(&"set_movement_banned", [node, legs[0].seg, legs[1].seg, true])
	t.check(banned.get("ok", false), "forbidding a movement: %s" % [banned])
	t.ticks(1)
	t.check(int(CityStore.query(&"node_traffic", [node]).movements) < before, "forbidden, the movement leaves the junction")
	t.check(CityStore.query(&"node_legs", [node])[0].banned.size() == 1, "the panel reads the forbidden movement")
	CityStore.dispatch(&"undo")
	t.ticks(1)
	t.check(int(CityStore.query(&"node_traffic", [node]).movements) == before, "undone, the movement comes back")
	var along := a + (mid - a) * 0.5
	var two: Dictionary = CityStore.query(&"inspect", [along])
	var spec: Dictionary = CityStore.query(&"road_spec", [two.profile_id])
	var made: Dictionary = CityStore.dispatch(&"road_spec_make", [two.profile_id, RoadSpecOps.directed(spec, 1)])
	var index: int = made.get("index", -1)
	t.check(made.get("ok", false) and index >= 0, "the one-way road is a profile: %s" % [made])
	var turned: Dictionary = CityStore.dispatch(&"upgrade_road", [along, index, false])
	t.check(turned.get("ok", false) and CityStore.query(&"inspect", [along]).one_way, "two-way becomes one-way: %s" % [turned])
	CityStore.dispatch(&"undo")
	t.check(not CityStore.query(&"inspect", [along]).one_way, "undone, it is two-way again")
	var gone: Dictionary = CityStore.dispatch(&"remove_node", [node])
	t.check(gone.get("ok", false), "taking the node with its roads: %s" % [gone])
	CityStore.dispatch(&"undo")


func _lay(a: Vector3, b: Vector3, id: String) -> Vector3:
	var index := -1
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			index = p.index
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, index, PX, 0.0])
	t.check(laid.get("ok", false), "road %s: %s" % [id, laid])
	return (a + b) * 0.5


func _join(mid: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var cut: Dictionary = CityStore.dispatch(&"split_road", [mid])
	t.check(cut.get("ok", false), "a point in the middle of the road: %s" % [cut])
	var node: int = CityStore.query(&"node_at", [mid, PX]).get("node", -1)
	t.check(node >= 0, "the point is a node")
	var joined: Dictionary = CityStore.dispatch(&"join_roads", [node])
	t.check(joined.get("ok", false), "joining the roads in line: %s" % [joined])
	t.check(CityStore.query(&"node_at", [mid, PX]).get("node", -1) < 0, "joined, the node goes")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"state_hash").whole == before.whole, "undoing the join returns to the same city")


## A T laid beside a place where a long road was cut takes the stub back: no short piece caps its mouth.
func stubs(e: Vector3) -> void:
	var a := e + Vector3(0, 0, STUB_OFF_M)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, a + Vector3(STUB_LEN_M, 0, 0), false, Vector3.ZERO, urban, STUB_PX])
	CityStore.refresh()
	var laid: int = CityStore.snapshot.segments
	var tee := a + Vector3(STUB_AT_M, 0, 0)
	CityStore.dispatch(&"build_road", [tee, tee + Vector3(0, 0, STUB_ARM_M), false, Vector3.ZERO, urban, STUB_PX])
	CityStore.refresh()
	t.check(CityStore.snapshot.segments == laid + 1, "the stub is a road again: %d against %d" % [CityStore.snapshot.segments, laid + 1])


## A ring laid over a road cuts it: what is left up to the next junction must still be a road, or the ring is refused like any tight stretch.
func ring_stub(e: Vector3) -> void:
	var a := e + Vector3(0, 0, RING_OFF_M)
	var cross := a + Vector3(RING_ROAD_M, 0, 0)
	var laid: bool = CityStore.dispatch(&"build_road", [a, a + Vector3(RING_ROAD_M * 2, 0, 0), false, Vector3.ZERO, URBAN, PX]).get("ok", false)
	laid = laid and CityStore.dispatch(&"build_road", [cross - Vector3(0, 0, ROAD_M), cross + Vector3(0, 0, ROAD_M), false, Vector3.ZERO, URBAN, PX]).get("ok", false)
	t.check(laid, "the roads a ring will cut")
	CityStore.refresh()
	var before: int = CityStore.snapshot.segments
	var r: Dictionary = CityStore.dispatch(&"roundabout", [cross - Vector3(RING_RADIUS_M + RING_STUB_M, 0, 0), RING_RADIUS_M, true])
	t.check(r.get("error", "") == "hint.rule.squeezed", "a ring leaving a stub at a junction is refused: %s" % r)
	CityStore.refresh()
	t.check(CityStore.snapshot.segments == before, "the refused ring leaves the roads as they were")
	r = CityStore.dispatch(&"roundabout", [cross - Vector3(RING_RADIUS_M + RING_CLEAR_M, 0, 0), RING_RADIUS_M, true])
	t.check(r.get("ok", false), "a ring with room to its next junction is built: %s" % r)
	CityStore.dispatch(&"undo")
