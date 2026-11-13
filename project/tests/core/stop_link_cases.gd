## The records_roads_test cases of the bus stops a road carries and of a lane link asked of a bad node (T037, sheet
## backbone-records-true items 4, 5 and 14): a stop carried to the half it stands by when its road is split, the stops of a road
## demolished left on no road and served by no road that takes its id elsewhere, and a lane link on a node that is no junction
## standing, or naming a road that is not its arm, refused with its key and no undo step.
class_name StopLinkCases
extends RefCounted

## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const ARM_M := 50.0 # a junction's arms: one piece each (tuning road.max_seg_m 112)
const ROAD_M := 100.0
const CASE_GAP_M := 200.0 # each case on its own ground
const STOP_SIDE_M := 8.0 # a click this far off the road's line: its footway, within transit_stop_reach_m
const STOP_ALONG_M := 80.0 # near the road's b end
const STOP_SAME_M := 18.0 # the stop placed from a click is the plan's nearest within this
const SPLIT_AT_M := 50.0
const SQUARE_OUT := 0.1 # a stop's `out` this close to across its road's line: framed by the road it stands on
const STOPS_ALONG_M := [20.0, 80.0]
const BUS_TICKS := 3
const BAD_NODES := [-1, 1000000]
const NOT_JUNCTION := "hint.control.notJunction"
const STOP_ROAD_LIVE := "stop_road_live"
const STOP_ON_ITS_ROAD := "stop_on_its_road"


## Item 4: a bus stop near the b end of a road is on the second half after a split: framed by the road beside it.
## `t`: the records_roads_test (TestCase), whose map, ticks, check and road helpers these are.
static func stop_after_split(t: Node, k: Vector3) -> void:
	t.road(k, k + Vector3(ROAD_M, 0, 0))
	var click := k + Vector3(STOP_ALONG_M, 0, STOP_SIDE_M)
	t.check(CityStore.dispatch(&"transit_stop", [click]).get("ok", false), "stop split: a stop near the road's b end")
	var stop := _stop_near(click)
	if not t.check(not stop.is_empty(), "stop split: the stop is in the plan"):
		return
	t.check(absf((stop.out as Vector3).x) < SQUARE_OUT, "stop split: the stop faces across its road before the split (out %s)" % [stop.out])
	var cut: Dictionary = CityStore.dispatch(&"split_road", [k + Vector3(SPLIT_AT_M, 0, 0)])
	t.check(cut.get("ok", false), "stop split: the road split: %s" % [cut])
	t.check(not t.broken([STOP_ON_ITS_ROAD, STOP_ROAD_LIVE], int(stop.id)), "stop split: the stop is on a road it stands by")
	var after := _stop_near(click)
	t.check(not after.is_empty() and absf((after.out as Vector3).x) < SQUARE_OUT,
			"stop split: the stop is on the second half, framed across it, not off the first half's end (out %s)" % [after.get("out", null)])
	CityStore.dispatch(&"undo")
	var undone := _stop_near(click)
	t.check(not undone.is_empty() and absf((undone.out as Vector3).x) < SQUARE_OUT, "stop split: undo puts the stop back on the whole road")


## Item 5: the stops of a road demolished are on no road, and the next road that takes its id, elsewhere, does not serve them.
static func stop_after_removal(t: Node, k: Vector3) -> void:
	t.road(k, k + Vector3(ROAD_M, 0, 0))
	var seg: int = CityStore.query(&"segment_at", [k + Vector3(ROAD_M * 0.5, 0, 0)]).get("seg", -1)
	var ids := PackedInt32Array()
	for along: float in STOPS_ALONG_M:
		var click := k + Vector3(along, 0, STOP_SIDE_M)
		CityStore.dispatch(&"transit_stop", [click])
		var stop := _stop_near(click)
		if not stop.is_empty():
			ids.append(int(stop.id))
	if not t.check(ids.size() == STOPS_ALONG_M.size(), "stop removal: two stops on the road (%s)" % [ids]):
		return
	t.ticks(BUS_TICKS)
	var before: int = CityStore.query(&"transit_run").buses
	t.check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "stop removal: a bus line through them")
	t.ticks(BUS_TICKS)
	var with_line: int = CityStore.query(&"transit_run").buses
	t.check(with_line > before, "stop removal: the line runs buses (%d, %d before)" % [with_line, before])
	t.check(CityStore.dispatch(&"demolish", [k + Vector3(ROAD_M * 0.5, 0, 0)]).get("ok", false), "stop removal: the road demolished")
	var named: PackedInt32Array = t.named_broken([STOP_ROAD_LIVE, STOP_ON_ITS_ROAD], ids)
	t.check(named.is_empty(), "stop removal: the stops are on no road, not the dead one (named broken: %s)" % [named])
	var elsewhere := k + Vector3(0, 0, CASE_GAP_M)
	t.road(elsewhere, elsewhere + Vector3(ROAD_M, 0, 0))
	var taken: int = CityStore.query(&"segment_at", [elsewhere + Vector3(ROAD_M * 0.5, 0, 0)]).get("seg", -1)
	t.check(taken == seg, "stop removal: the road laid elsewhere took the freed id (%d, was %d)" % [taken, seg])
	t.ticks(BUS_TICKS)
	var after: int = CityStore.query(&"transit_run").buses
	t.check(after == before, "stop removal: the road elsewhere serves none of the stops: the line runs no bus (%d, %d without it)" % [after, before])


## Item 14: a lane link on a node that is no junction standing, or naming a road that is not its arm, is refused with the key,
## with no trap and no undo step.
static func lane_link_bad_node(t: Node, k: Vector3) -> void:
	t.road(k - Vector3(ARM_M, 0, 0), k + Vector3(ARM_M, 0, 0))
	t.road(k, k + Vector3(0, 0, ARM_M))
	var node: int = CityStore.query(&"node_at", [k, PX]).node
	var end := k + Vector3(ROAD_M, 0, ROAD_M)
	t.road(end, end + Vector3(ARM_M, 0, 0))
	var dead: int = CityStore.query(&"node_at", [end, PX]).node
	var other := end + Vector3(0, 0, ARM_M) # a road standing elsewhere: no arm of the junction (laid first: it takes no freed node)
	t.road(other, other + Vector3(ARM_M, 0, 0))
	var stranger: int = CityStore.query(&"segment_at", [other + Vector3(ARM_M * 0.5, 0, 0)]).get("seg", -1)
	t.check(CityStore.dispatch(&"demolish", [end + Vector3(ARM_M * 0.5, 0, 0)]).get("ok", false), "bad node: a road demolished, its end node gone")
	if not t.check(node >= 0 and dead >= 0 and stranger >= 0, "bad node: a junction, a dead node and a road elsewhere (%d, %d, %d)" % [node, dead, stranger]):
		return
	t.check(int(CityStore.query(&"node_at", [end, PX]).node) < 0, "bad node: the end node is gone")
	var arm: int = t.legs(node)[0].seg
	for bad: int in BAD_NODES + [dead]:
		_refused_link(t, bad, [arm, 0, arm, 0], "node %d" % bad)
	_refused_link(t, node, [stranger, 0, arm, 0], "a from that is not an arm")
	_refused_link(t, node, [arm, 0, stranger, 0], "a to that is not an arm")


## The plan's bus stop nearest a click, within STOP_SAME_M ({} when none).
static func _stop_near(click: Vector3) -> Dictionary:
	var best := {}
	var nearest := STOP_SAME_M
	for stop: Dictionary in CityStore.query(&"transit_plan").stops:
		var d := Vector2((stop.at as Vector3).x, (stop.at as Vector3).z).distance_to(Vector2(click.x, click.z))
		if d < nearest:
			nearest = d
			best = stop
	return best


static func _refused_link(t: Node, node: int, link: Array, what: String) -> void:
	var steps: int = CityStore.query(&"undo_steps")
	var r: Dictionary = CityStore.dispatch(&"flip_lane_link", [node, link[0], link[1], link[2], link[3]])
	t.check(not r.get("ok", false) and r.get("error", "") == NOT_JUNCTION, "bad node: %s refused with the key: %s" % [what, r])
	t.check(int(CityStore.query(&"undo_steps")) == steps, "bad node: %s leaves no undo step" % what)
