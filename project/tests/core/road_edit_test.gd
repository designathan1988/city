## Core test of the road edits beyond laying one, through the store: the tool's options on a class road, the Move and
## Control tabs and what an edit may not leave behind, on open ground past the town.
## Run: godot --headless --path project res://tests/core/road_edit_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const URBAN_M := 12.0
const URBAN_PRICE := 1520.0
const LANE_M := 4.0
const BAY_M := 2.0
const WIDE_M := 15.0
const VARIANT_OFF_M := 180.0
const VARIANT_LEN_M := 60.0
const MOVE_OFF_M := 300.0
const MOVE_LEN_M := 60.0
const MOVE_BY_M := 30.0
const CONTROL_OFF_M := 420.0
const TICKS_FOR_GRAPH := 3
const RULE_STOP := 1
const RULES_OFF_M := 540.0
const SHARP_DEG := 15.0
const OPEN_DEG := 40.0
const AVENUE_GAP_M := 20.0
const SIDE_OFF_M := 720.0
const COMPOSED_LANE_M := 4.5
const COMPOSED_WALK_M := 2.0 # a footway the player tries to put on a highway
const SIDE_GAP_M := 8.0
const CURVE_OFF_M := 800.0
const CHORD_M := 100.0
const BEND_M := 20.0
const TIGHT_CHORD_M := 20.0
const TIGHT_CONTROL_M := 30.0
const TIGHTEST_M := 8.7
const NEAR_M := 0.01


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var e := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_variants(e)
	_move_node(e)
	_control(e)
	_rules(e)
	_side_by_side(e)
	_curves(e)
	RoadShapeCases.new(self).stubs(e)
	RoadShapeCases.new(self).run(e)
	RoadShapeCases.new(self).ring_stub(e)
	_composed_road()
	finish()


## The tool's options on a class road: a profile of their own, made once, with the 4.0's widths and prices.
func _variants(e: Vector3) -> void:
	var urban: int = CityStore.first_road()
	var profiles: Array = CityStore.query(&"road_profiles")
	check(profiles[urban].width == URBAN_M and profiles[urban].cost_per_m == URBAN_PRICE, "an urban road as in the 4.0")
	var four: int = CityStore.dispatch(&"road_variant", [urban, 4, 0.0, 0])
	var bays: int = CityStore.dispatch(&"road_variant", [urban, 0, 0.0, 1])
	var wide: int = CityStore.dispatch(&"road_variant", [urban, 0, WIDE_M, 0])
	profiles = CityStore.query(&"road_profiles")
	check(profiles[four].lanes == 4 and profiles[four].width == URBAN_M + 2 * LANE_M, "four lanes: %s" % profiles[four].width)
	check(profiles[four].centre == "solid", "two lanes each way have the 4.0's line: %s" % profiles[four].centre)
	check(profiles[bays].width == URBAN_M + 2 * BAY_M, "bays on both sides: %s" % profiles[bays].width)
	check(profiles[wide].width == WIDE_M and profiles[wide].lanes == 2, "total width: %s" % profiles[wide].width)
	check(CityStore.dispatch(&"road_variant", [urban, 4, 0.0, 0]) == four, "the same variant is the same profile")
	check(CityStore.dispatch(&"road_variant", [urban, 0, 0.0, 0]) == urban, "with no option it is the road itself")
	var card: int = profiles.filter(func(p: Dictionary) -> bool: return p.id == "narrowStreet")[0].index
	check(CityStore.dispatch(&"road_variant", [card, 4, 0.0, 0]) == card, "a catalogue road takes no option")
	var a := e + Vector3(0, 0, VARIANT_OFF_M)
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, a + Vector3(VARIANT_LEN_M, 0, 0), false, Vector3.ZERO, four, PX])
	check(r.get("ok", false), "a road with the variant: %s" % r)


## The Move tab: a node dropped elsewhere takes its roads along, charges only the difference, undoes in one step.
func _move_node(e: Vector3) -> void:
	var a := e + Vector3(0, 0, MOVE_OFF_M)
	var mid := a + Vector3(MOVE_LEN_M, 0, 0)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, mid, false, Vector3.ZERO, urban, PX])
	CityStore.dispatch(&"build_road", [mid, mid + Vector3(MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	var node: int = CityStore.query(&"node_at", [mid, PX]).node
	check(node >= 0, "finds the middle node")
	var s0: int = CityStore.snapshot.segments
	var to := mid + Vector3(0, 0, MOVE_BY_M)
	check(CityStore.query(&"moved_roads", [node, to]).size() == 2, "a preview with both roads")
	var r: Dictionary = CityStore.dispatch(&"move_node", [node, to])
	check(r.get("ok", false), "moves the node: %s" % r)
	CityStore.refresh()
	check(CityStore.snapshot.segments == s0, "the roads stay two")
	check(CityStore.query(&"node_at", [to, PX]).node >= 0 and CityStore.query(&"node_at", [mid, PX]).node < 0, "the node is in the new place")
	check(float(r.money) > 0 and float(r.money) < MOVE_LEN_M * URBAN_PRICE, "charges only the difference: %s" % r.money)
	CityStore.dispatch(&"undo")
	CityStore.refresh()
	check(CityStore.query(&"node_at", [mid, PX]).node >= 0, "undo gives the node back")
	var far: Dictionary = CityStore.dispatch(&"move_node", [node, a + Vector3(1, 0, 0)])
	check(not far.get("ok", false) and far.get("error", "") == "hint.move.tooShort", "too short is refused: %s" % far)


## The Control tab: a junction takes a control and undoes it; a node of two roads is refused.
func _control(e: Vector3) -> void:
	var a := e + Vector3(0, 0, CONTROL_OFF_M)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, a + Vector3(2 * MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	var mid := a + Vector3(MOVE_LEN_M, 0, 0)
	CityStore.dispatch(&"build_road", [mid, mid + Vector3(0, 0, MOVE_LEN_M), false, Vector3.ZERO, urban, PX])
	var hit: Dictionary = CityStore.query(&"node_at", [mid, PX])
	check(hit.node >= 0 and hit.ctl == 0, "a new junction is automatic")
	check(CityStore.query(&"junctions").any(func(j: Dictionary) -> bool: return j.node == hit.node), "the junction is on the list")
	check(CityStore.dispatch(&"set_node_control", [hit.node, 1]).get("ok", false), "signal")
	check(CityStore.query(&"node_at", [mid, PX]).ctl == 1, "the control stayed")
	CityStore.dispatch(&"undo")
	check(CityStore.query(&"node_at", [mid, PX]).ctl == 0, "undo brings the control back")
	var end: Dictionary = CityStore.query(&"node_at", [a, PX])
	var r: Dictionary = CityStore.dispatch(&"set_node_control", [end.node, 1])
	check(r.get("error", "") == "hint.control.notJunction", "a road end has no control: %s" % r)
	_lanes_by_hand(hit.node)
	var legs: Array = CityStore.query(&"node_legs", [hit.node])
	check(legs.size() == 3, "rules: three roads at the junction (%d)" % legs.size())
	var rule: Dictionary = CityStore.dispatch(&"set_approach_rule", [hit.node, legs[0].seg, RULE_STOP])
	check(rule.get("ok", false), "rules: Stop put on a road")
	check(int(CityStore.query(&"node_legs", [hit.node])[0].rule) == RULE_STOP, "rules: the road kept the Stop")
	CityStore.dispatch(&"undo")
	check(int(CityStore.query(&"node_legs", [hit.node])[0].rule) == 0, "rules: undo takes the rule away")


## A lane's connection taken away by hand, the traffic's graph following, and undo bringing it back.
func _lanes_by_hand(node: int) -> void:
	ticks(TICKS_FOR_GRAPH)
	var before: Array = CityStore.query(&"node_lanes", [node]).built
	check(before.size() > 1, "connectors: the junction has links (%d)" % before.size())
	var one: Array = before[0]
	var left := before.filter(func(b: Array) -> bool: return b[0] == one[0] and b[1] == one[1])
	var r: Dictionary = CityStore.dispatch(&"toggle_lane_link", [node, one[0], one[1], one[2], one[3]])
	if left.size() < 2:
		check(r.get("error", "") == "hint.lane.last", "connectors: the lane is not left with no way out: %s" % r)
		return
	check(r.get("ok", false), "connectors: link taken away")
	ticks(TICKS_FOR_GRAPH)
	var now: Array = CityStore.query(&"node_lanes", [node]).built
	check(now.size() == before.size() - 1, "connectors: the graph lost the link (%d -> %d)" % [before.size(), now.size()])
	CityStore.dispatch(&"undo")
	ticks(TICKS_FOR_GRAPH)
	check(CityStore.query(&"node_lanes", [node]).built.size() == before.size(), "connectors: undo brings the link back")


## What an edit may not leave: two roads meeting under 25 degrees, a stretch too short for its two junctions. A refusal
## rolls the whole edit back.
func _rules(e: Vector3) -> void:
	var a := e + Vector3(0, 0, RULES_OFF_M)
	var b := a + Vector3(MOVE_LEN_M, 0, 0)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, urban, PX])
	var tight := b + Vector3(-MOVE_LEN_M * cos(deg_to_rad(SHARP_DEG)), 0, MOVE_LEN_M * sin(deg_to_rad(SHARP_DEG)))
	var r: Dictionary = CityStore.dispatch(&"build_road", [b, tight, false, Vector3.ZERO, urban, PX])
	check(r.get("error", "") == "refuse.road.sharpAngle", "a tight angle is refused: %s" % r)
	var open := b + Vector3(-MOVE_LEN_M * cos(deg_to_rad(OPEN_DEG)), 0, MOVE_LEN_M * sin(deg_to_rad(OPEN_DEG)))
	check(CityStore.dispatch(&"build_road", [b, open, false, Vector3.ZERO, urban, PX]).get("ok", false), "an open angle passes")
	var avenue: int = CityStore.query(&"road_profiles").filter(func(p: Dictionary) -> bool: return p.id == "avenue")[0].index
	var c := e + Vector3(0, 0, RULES_OFF_M + 2 * MOVE_LEN_M)
	var d := c + Vector3(0, 0, AVENUE_GAP_M)
	CityStore.dispatch(&"build_road", [c, c + Vector3(2 * MOVE_LEN_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	CityStore.dispatch(&"build_road", [d, d + Vector3(2 * MOVE_LEN_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	CityStore.refresh()
	var s0: int = CityStore.snapshot.segments
	var m0: float = CityStore.snapshot.money
	var mid := c + Vector3(MOVE_LEN_M, 0, 0)
	r = CityStore.dispatch(&"build_road", [mid, mid + Vector3(0, 0, AVENUE_GAP_M), false, Vector3.ZERO, avenue, PX])
	check(r.get("error", "") == "hint.rule.squeezed", "a tight stretch is refused: %s" % r)
	CityStore.refresh()
	check(CityStore.snapshot.segments == s0 and CityStore.snapshot.money == m0, "a refusal undoes the whole work")


## A road on top of another is refused along its length, and only there: going on from a road's end is not lying beside it.
func _side_by_side(e: Vector3) -> void:
	var a := e + Vector3(0, 0, SIDE_OFF_M)
	var b := a + Vector3(MOVE_LEN_M, 0, 0)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, urban, PX])
	var beside := a + Vector3(0, 0, SIDE_GAP_M)
	var r: Dictionary = CityStore.dispatch(&"build_road", [beside, beside + Vector3(MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	check(r.get("error", "") == "road.reason.overlap", "a road on top of another is refused: %s" % r)
	r = CityStore.dispatch(&"build_road", [b, b + Vector3(MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	check(r.get("ok", false), "going on from the end passes: %s" % r)


## A dragged curve bends as the 4.0's does, short of the farthest point of the drag, and none turns tighter than the design car.
func _curves(e: Vector3) -> void:
	var a := e + Vector3(0, 0, CURVE_OFF_M)
	var b := a + Vector3(CHORD_M, 0, 0)
	var urban: int = CityStore.first_road()
	var far := (a + b) * 0.5 + Vector3(0, 0, BEND_M)
	var q: PackedVector3Array = CityStore.query(&"road_pieces", [a, b, 1, false, Vector3.ZERO, PackedVector3Array([a, far, b]), urban, PX])
	var apex := (q[0] + 2 * q[1] + q[2]) * 0.25
	var bend: float = BEND_M * DataTables.tuning("road_bend_gain") * 0.5
	check(absf(apex.distance_to((a + b) * 0.5) - bend) < NEAR_M, "the curve bends as the 4.0's: %s" % apex.distance_to((a + b) * 0.5))
	b = a + Vector3(TIGHT_CHORD_M, 0, 0)
	var control := (a + b) * 0.5 + Vector3(0, 0, TIGHT_CONTROL_M)
	q = CityStore.query(&"road_pieces", [a, b, 1, true, control, PackedVector3Array(), urban, PX])
	var rise := q[1].distance_to((q[0] + q[2]) * 0.5)
	var radius := TIGHT_CHORD_M * TIGHT_CHORD_M / (4 * rise)
	check(radius >= TIGHTEST_M - NEAR_M, "least radius of the design car: %s" % radius)


## Customise: a road of the catalogue read as elements, what stops one from being built, and a road of the player's laid.
func _composed_road() -> void:
	var base: int = CityStore.first_road()
	var spec: Dictionary = CityStore.query(&"road_spec", [base])
	check(spec.elements[0].kind == "footway" and spec.elements[-1].kind == "footway", "profile: the road read starts and ends in pavement")
	check((CityStore.query(&"road_spec_issues", [base, spec]) as Array).is_empty(), "profile: the catalogue road can be built")
	var bad: Dictionary = spec.duplicate(true)
	bad.elements.remove_at(0)
	check(not (CityStore.query(&"road_spec_issues", [base, bad]) as Array).is_empty(), "profile: with no pavement at the end there is a problem")
	check(int(CityStore.query(&"road_spec_make", [base, bad])) < 0, "profile: what has a problem does not become a road")
	var own: Dictionary = spec.duplicate(true)
	RoadSpecOps.with_width(own, 1, COMPOSED_LANE_M)
	var index: int = CityStore.query(&"road_spec_make", [base, own])
	check(index >= 0, "profile: the composed road becomes a profile")
	check(int(CityStore.query(&"road_spec_make", [base, own])) == index, "profile: the same road is the same profile")
	var back: Dictionary = CityStore.query(&"road_spec", [index])
	check(is_equal_approx(RoadSpecOps.total(back), RoadSpecOps.total(own)), "profile: the width comes back the same (%.2f)" % RoadSpecOps.total(back))
	_composed_highway()


## A highway composed: no footway (the Brazilian traffic code, annex I), its shoulders the class's, kept on what is composed.
func _composed_highway() -> void:
	var base := -1
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if String(p.id) == "expressway":
			base = int(p.index)
	var spec: Dictionary = CityStore.query(&"road_spec", [base])
	check(base >= 0 and (CityStore.query(&"road_spec_issues", [base, spec]) as Array).is_empty(), "profile: the expressway read with no footway can be built")
	var walked: Dictionary = spec.duplicate(true)
	walked.elements.insert(0, {"kind": "footway", "width": COMPOSED_WALK_M, "dir": 0, "bus": false, "solid": false, "flush": false, "pave": 0})
	var problems: Array = (CityStore.query(&"road_spec_issues", [base, walked]) as Array).map(func(i: Dictionary) -> String: return String(i.problem))
	check(problems.has("noFootways"), "profile: a footway on a highway is refused (%s)" % [problems])
	var own: Dictionary = spec.duplicate(true)
	RoadSpecOps.with_width(own, 0, COMPOSED_LANE_M)
	var index: int = CityStore.query(&"road_spec_make", [base, own])
	var strips: Array = CityStore.query(&"road_profiles")[index].strips if index >= 0 else []
	check(not strips.is_empty() and strips[0].kind == "shoulder" and strips[-1].kind == "shoulder", "profile: the composed highway keeps its shoulders (%s)" % [strips.map(func(s: Dictionary) -> String: return String(s.kind))])
