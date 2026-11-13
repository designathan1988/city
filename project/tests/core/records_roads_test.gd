## Core test of what names a road kept true through its removal and re-laying (T037, sheet backbone-records-true items 1-5 and
## 14): a junction's bans and hand links dropped with the arm they name and carried to the piece that meets the same node when a
## road is moved, split, turned round or bent; the bus stops and the lane link asked of a bad node in stop_link_cases.gd (items 4,
## 5, 14), on this scene's helpers. Each case asks the core's invariants (probe broken_records) besides the symptom.
## Run: godot --headless --path project res://tests/core/records_roads_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0 # past the town's edge: open ground
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const ARM_M := 50.0 # a junction's arms: one piece each (tuning road.max_seg_m 112)
const ROAD_M := 100.0
const CASE_GAP_M := 200.0 # each case on its own ground
const MOVE_BY_M := 10.0
const FAR_SAME_M := 1.0 # a far node read back through the legs query
const WAY_DOT := 0.9 # a leg leaves toward a way when its direction is this close to it
const TICKS_FOR_GRAPH := 3
const ONE_WAY := "oneWay1Lane" # data/catalog/road_profiles.json: a one-way the Inspector turns round
const BEND_M := 8.0
const BEND_AT := 0.5
const LINKS_ON_ARMS := "links_on_arms"

var _e := Vector3.ZERO
var _urban := 0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var b: Dictionary = CityStore.query(&"city_bounds")
	_e = Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_urban = CityStore.first_road()
	_bans_after_removal(_e)
	_bans_after_move(_e + Vector3(0, 0, CASE_GAP_M))
	_links_after_split(_e + Vector3(0, 0, 2 * CASE_GAP_M))
	StopLinkCases.stop_after_split(self, _e + Vector3(0, 0, 3 * CASE_GAP_M))
	StopLinkCases.stop_after_removal(self, _e + Vector3(0, 0, 4 * CASE_GAP_M))
	StopLinkCases.lane_link_bad_node(self, _e + Vector3(0, 0, 6 * CASE_GAP_M))
	_ban_after_reverse(_e + Vector3(0, 0, 8 * CASE_GAP_M))
	_ban_after_bend(_e + Vector3(0, 0, 9 * CASE_GAP_M))
	finish()


## Item 1: a ban at a junction goes with the arm it names. A road laid again into the junction takes the arm's freed id, and must
## not come banned.
func _bans_after_removal(j: Vector3) -> void:
	road(j - Vector3(ARM_M, 0, 0), j + Vector3(ARM_M, 0, 0))
	road(j - Vector3(0, 0, ARM_M), j + Vector3(0, 0, ARM_M)) # crosses the first at j: four arms
	var node: int = CityStore.query(&"node_at", [j, PX]).node
	if not check(node >= 0 and legs(node).size() == 4, "removal: a junction of four arms at the crossing"):
		return
	var west := _leg_toward(node, Vector2(-1, 0))
	var north := _leg_toward(node, Vector2(0, -1))
	if not check(not west.is_empty() and not north.is_empty(), "removal: the west and north arms"):
		return
	var ban: Dictionary = CityStore.dispatch(&"set_movement_banned", [node, west.seg, north.seg, true])
	check(ban.get("ok", false), "removal: west to north banned: %s" % [ban])
	var gone: Dictionary = CityStore.dispatch(&"demolish", [j - Vector3(ARM_M * 0.5, 0, 0)])
	check(gone.get("ok", false), "removal: the west arm demolished: %s" % [gone])
	check(not broken([LINKS_ON_ARMS], node), "removal: no link of the junction names a road gone")
	check(_banned_count(node) == 0, "removal: the ban went with its arm")
	road(j, j - Vector3(ARM_M, 0, 0)) # back into the junction where the arm was: it takes the freed id
	var again := _leg_toward(node, Vector2(-1, 0))
	check(int(again.get("seg", -1)) == int(west.seg), "removal: the road laid again took the arm's id (%s, was %s)" % [again.get("seg", -1), west.seg])
	check(_banned_count(node) == 0, "removal: the road laid again is banned nowhere (legs %s)" % [legs(node)])


## Item 2: a ban between two arms stands between the same two roads after the node is moved, read by the far node each arm goes to;
## undo brings it back as it was.
func _bans_after_move(j: Vector3) -> void:
	var p := j - Vector3(ARM_M, 0, 0)
	var q := j + Vector3(ARM_M, 0, 0)
	road(p, q)
	road(j, j + Vector3(0, 0, ARM_M)) # from the middle of the first: three arms
	var node: int = CityStore.query(&"node_at", [j, PX]).node
	if not check(node >= 0 and legs(node).size() == 3, "move: a junction of three arms"):
		return
	var to_p := _leg_to_far(j, p)
	var to_q := _leg_to_far(j, q)
	if not check(not to_p.is_empty() and not to_q.is_empty(), "move: the arms to P and Q"):
		return
	check(CityStore.dispatch(&"set_movement_banned", [node, to_p.seg, to_q.seg, true]).get("ok", false), "move: the arm to P banned to the arm to Q")
	_ban_between(j, p, q, "move before")
	var where := j - Vector3(0, 0, MOVE_BY_M)
	var moved: Dictionary = CityStore.dispatch(&"move_node", [node, where])
	check(moved.get("ok", false), "move: the junction moved: %s" % [moved])
	var now: int = CityStore.query(&"node_at", [where, PX]).node
	if not check(now >= 0 and legs(now).size() == 3, "move: the junction stands where it was dropped with its three arms"):
		return
	check(not broken([LINKS_ON_ARMS], now), "move: no link of the junction names a road that is not its arm")
	_ban_between(where, p, q, "move")
	CityStore.dispatch(&"undo")
	if check(int(CityStore.query(&"node_at", [j, PX]).node) >= 0, "move: undo puts the junction back"):
		_ban_between(j, p, q, "move undone")


## Item 3: a hand link at node b of a road stays on the half that meets node b when the road is split.
func _links_after_split(j: Vector3) -> void:
	road(j - Vector3(ARM_M, 0, 0), j + Vector3(ARM_M, 0, 0))
	road(j, j + Vector3(0, 0, ARM_M)) # a junction at j: the west arm runs into it (its node b)
	var node: int = CityStore.query(&"node_at", [j, PX]).node
	if not check(node >= 0, "split: a junction"):
		return
	ticks(TICKS_FOR_GRAPH)
	var west: int = _leg_toward(node, Vector2(-1, 0)).get("seg", -1)
	var built: Array = CityStore.query(&"junction_lane_map", [node]).built
	var one: Array = []
	var lane_links := 0
	for link: Array in built:
		var same: Array = built.filter(func(b: Array) -> bool: return b[0] == link[0] and b[1] == link[1])
		if one.is_empty() and link[0] == west and link[2] != west and same.size() >= 2:
			one = link
			lane_links = same.size()
	if not check(not one.is_empty(), "split: a lane of the west arm with two ways out (%s)" % [built]):
		return
	check(CityStore.dispatch(&"flip_lane_link", [node, one[0], one[1], one[2], one[3]]).get("ok", false), "split: a link taken away by hand")
	ticks(TICKS_FOR_GRAPH)
	check(not _has_link(node, one), "split: the graph lost the link")
	var cut: Dictionary = CityStore.dispatch(&"split_road", [j - Vector3(ARM_M * 0.5, 0, 0)])
	check(cut.get("ok", false), "split: the west arm split: %s" % [cut])
	check(not broken([LINKS_ON_ARMS], node), "split: no link of the junction names a road that does not meet it")
	ticks(TICKS_FOR_GRAPH)
	var half: int = _leg_toward(node, Vector2(-1, 0)).get("seg", -1)
	var on_half := [half, one[1], one[2], one[3]]
	check(not _has_link(node, on_half), "split: the link taken away stays away on the half that meets the junction")
	var kept: Array = CityStore.query(&"junction_lane_map", [node]).built.filter(func(b: Array) -> bool: return b[0] == half and b[1] == one[1])
	check(kept.size() == lane_links - 1, "split: the lane keeps the links left by hand (%d, %d before the toggle)" % [kept.size(), lane_links])
	CityStore.dispatch(&"undo")
	ticks(TICKS_FOR_GRAPH)
	check(not _has_link(node, one), "split: undo gives the road back with the link still away")


## A one-way arm turned round (reverse_road takes it up and lays it again): the ban from the west arm onto it stays on it.
func _ban_after_reverse(j: Vector3) -> void:
	var p := j - Vector3(ARM_M, 0, 0)
	var south := j + Vector3(0, 0, ARM_M)
	road(p, j + Vector3(ARM_M, 0, 0))
	var one_way: int = CityStore.query(&"road_profiles").filter(func(r: Dictionary) -> bool: return r.id == ONE_WAY)[0].index
	var r: Dictionary = CityStore.dispatch(&"build_road", [j, south, false, Vector3.ZERO, one_way, PX])
	check(r.get("ok", false), "reverse: a one-way arm off the junction: %s" % [r])
	var node: int = CityStore.query(&"node_at", [j, PX]).node
	var to_p := _leg_to_far(j, p)
	var to_south := _leg_to_far(j, south)
	if not check(node >= 0 and not to_p.is_empty() and not to_south.is_empty(), "reverse: a junction with the west and the one-way arms"):
		return
	check(CityStore.dispatch(&"set_movement_banned", [node, to_p.seg, to_south.seg, true]).get("ok", false), "reverse: west to the one-way banned")
	var turned: Dictionary = CityStore.dispatch(&"reverse_road", [to_south.seg])
	check(turned.get("ok", false), "reverse: the one-way turned round: %s" % [turned])
	check(not broken([LINKS_ON_ARMS], node), "reverse: no link of the junction names a road that is not its arm")
	_ban_between(j, p, south, "reverse")
	CityStore.dispatch(&"undo")
	_ban_between(j, p, south, "reverse undone")


## An arm bent (set_road_bend takes the road up and lays it again along the curve): the ban from it to the east arm stays on it.
func _ban_after_bend(j: Vector3) -> void:
	var p := j - Vector3(ARM_M, 0, 0)
	var q := j + Vector3(ARM_M, 0, 0)
	road(p, q)
	road(j, j + Vector3(0, 0, ARM_M))
	var node: int = CityStore.query(&"node_at", [j, PX]).node
	var to_p := _leg_to_far(j, p)
	var to_q := _leg_to_far(j, q)
	if not check(node >= 0 and not to_p.is_empty() and not to_q.is_empty(), "bend: a junction with the arms to P and Q"):
		return
	check(CityStore.dispatch(&"set_movement_banned", [node, to_p.seg, to_q.seg, true]).get("ok", false), "bend: the arm to P banned to the arm to Q")
	var bent: Dictionary = CityStore.dispatch(&"bend_road", [to_p.seg, BEND_M, BEND_AT])
	check(bent.get("ok", false), "bend: the arm to P bent: %s" % [bent])
	check(not broken([LINKS_ON_ARMS], node), "bend: no link of the junction names a road that is not its arm")
	_ban_between(j, p, q, "bend")
	CityStore.dispatch(&"undo")
	_ban_between(j, p, q, "bend undone")


# --- helpers ---

func road(a: Vector3, b: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, _urban, PX])
	check(r.get("ok", false), "a road from %s to %s: %s" % [a, b, r])


func legs(node: int) -> Array:
	return CityStore.query(&"node_legs", [node])


## The leg of `node` whose road leaves toward `way` ({} when none).
func _leg_toward(node: int, way: Vector2) -> Dictionary:
	for leg: Dictionary in legs(node):
		if Vector2(leg.dx, leg.dz).normalized().dot(way) > WAY_DOT:
			return leg
	return {}


## The leg of the junction standing at `node_at` whose road goes to the node standing at `far` ({} when none).
func _leg_to_far(node_at: Vector3, far: Vector3) -> Dictionary:
	var hit: Dictionary = CityStore.query(&"node_at", [node_at, PX])
	if int(hit.get("node", -1)) < 0:
		return {}
	var at: Vector3 = hit.at
	for leg: Dictionary in legs(hit.node):
		if Vector2(at.x + leg.dx, at.z + leg.dz).distance_to(Vector2(far.x, far.z)) < FAR_SAME_M:
			return leg
	return {}


func _banned_count(node: int) -> int:
	var count := 0
	for leg: Dictionary in legs(node):
		count += (leg.banned as Array).size()
	return count


## The only ban at the junction standing at `node_at` runs from the arm going to far node `p` to the arm going to far node `q`.
func _ban_between(node_at: Vector3, p: Vector3, q: Vector3, what: String) -> void:
	var node: int = CityStore.query(&"node_at", [node_at, PX]).node
	var to_p := _leg_to_far(node_at, p)
	var to_q := _leg_to_far(node_at, q)
	if not check(not to_p.is_empty() and not to_q.is_empty(), "%s: the arms to P and Q read by their far nodes" % what):
		return
	check((to_p.banned as Array) == [to_q.seg] and _banned_count(node) == 1,
			"%s: the one ban runs from the arm to P to the arm to Q (legs %s)" % [what, legs(node)])


func _has_link(node: int, link: Array) -> bool:
	return CityStore.query(&"junction_lane_map", [node]).built.any(func(b: Array) -> bool: return b == link)


## Whether the core's invariants name `record` broken under one of `check_names` (probe broken_records).
func broken(check_names: Array, record: int) -> bool:
	return not named_broken(check_names, PackedInt32Array([record])).is_empty()


## The records of `records` the probe names broken under one of `check_names`. Asking drains what the debug DLL collected after
## each command, so every other entry it gives fails here, as TestCase.finish would fail it.
func named_broken(check_names: Array, records: PackedInt32Array) -> PackedInt32Array:
	var named := PackedInt32Array()
	for b: Dictionary in CityStore.query(&"broken_records"):
		var listed: PackedInt32Array = b.records
		var ours := check_names.has(String(b.check)) and not listed.is_empty()
		for r: int in listed:
			ours = ours and records.has(r)
		if ours:
			named.append_array(listed)
		else:
			check(false, "records broken: %s.%s %s" % [b.feature, b.check, b.records])
	return named
