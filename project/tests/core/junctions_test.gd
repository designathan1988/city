## A junction as the traffic rules it (probe junction_facts), on a crossing of two avenues laid on free ground past the town, with
## the view over it: the gap a driver asks by its turn and how a long wait lowers it; a signal's plan (the stages of opposite
## approaches, the turn stages, every crossing walked in some stage); a signal with nobody asking rests on its stage, a vehicle on
## a red approach turns it, a walker waiting long at a kerb gets the walk; buses first is kept; the panel's data (what the player
## set, what rules it, each leg's flow and the road it is on); a mini-roundabout ranks nobody and is a circle; with no control
## nobody is ranked either.
## Run: godot --path project res://tests/core/junctions_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const AVENUE := 3 # road_profiles.json: four lanes
const OUT_M := 300.0 # past the town's edge
const ARM_M := 100.0
const PX := 0.25
const SETTLE_TICKS := 4 # the lane graph takes an edit up
const VIEW_M := 100.0 # how far the view over the crossing sees: the window of detailed traffic round it
const LOOK_TICKS := 40 # past the window's own look (a second)
const REST_TICKS := 600 # half a minute with nobody asking
const TURN_TICKS := 1600 # the longest a call waits: the running green's least, its intergreen, a stage between
const POLL_TICKS := 4 # a walker held at a kerb asks five times a second: the test says it as often
const HOPS := 4 # the streets on a car set by the test drives
const LONG_WAIT_S := 60.0 # past traffic.json ped_wait_limit_s
const CTL_AUTO := 0 # road_network.h NodeControl
const CTL_NONE := 5
const CTL_MINI := 6
const SIGNAL := 2 # lane_graph_types.h Control
const GREEN := 0 # Light
const LAMP_WALK := 2 # signal_plan.h WalkLamp
const RANK_FREE := 2 # junction_control.cpp
const RANK_YIELD := 1

var _mid := Vector3.ZERO
var _node := -1


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	_lay()
	if _node >= 0:
		_gaps()
		_plan()
		_rests_and_turns()
		_walkers_call()
		_settings()
		_other_controls()
	finish()


## The crossing: two avenues, four arms, on automatic (a signal before the flows speak), the view over it.
func _lay() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var o := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_mid = o + Vector3(ARM_M, 0, 0)
	for ends: Array in [[o, o + Vector3(2.0 * ARM_M, 0, 0)], [_mid + Vector3(0, 0, ARM_M), _mid], [_mid - Vector3(0, 0, ARM_M), _mid]]:
		var r: Dictionary = CityStore.dispatch(&"build_road", [ends[0], ends[1], false, Vector3.ZERO, AVENUE, PX])
		check(r.get("ok", false), "road %s -> %s laid: %s" % [ends[0], ends[1], r])
	CityStore.query(&"far_traffic", [_mid, VIEW_M])
	ticks(LOOK_TICKS)
	var hit: Dictionary = CityStore.query(&"inspect", [_mid])
	if check(hit.get("kind", "") == "node" and hit.get("arms", 0) == 4, "a crossing of four arms stands there: %s" % [hit]):
		_node = hit.id


func _facts() -> Dictionary:
	return CityStore.query(&"junction_facts", [_node])


## The gap by turn: a left across asks the most, then straight over, a right turn, a left off the road that has the way the
## least; after a long wait every gap is at the floor.
func _gaps() -> void:
	var p: Dictionary = DataTables.catalog_table("traffic").params
	var f := _facts()
	var g: Array = f.gaps
	check(g[2] > g[1] and g[1] > g[0] and g[0] > g[3], "the gap asked goes by the turn: right %.1f, straight %.1f, left %.1f, left off the main road %.1f s" % [g[0], g[1], g[2], g[3]])
	var waited: Array = f.gaps_waited
	check(waited[2] < g[2] and is_equal_approx(waited[3], p.gap_floor_s), "a long wait lowers it, never under the floor: left %.1f s, left off the main road %.1f s" % [waited[2], waited[3]])


## The plan: a signal, more stages than the two of its opposite approaches (the turns that give way have their own), each turn
## stage serving an approach of an ordinary one, every crossing walked in some stage.
func _plan() -> void:
	var f := _facts()
	check(f.control == SIGNAL, "four arms of avenues run on a signal: control %d" % f.control)
	if f.control != SIGNAL:
		return
	check(f.turn_stages > 0 and f.stages > f.turn_stages and f.turns_served,
			"the turns that give way have stages of their own: %d stages, %d of them for turns" % [f.stages, f.turn_stages])
	check(f.walked_all, "every crossing walks in some stage (%d of the walkers' own)" % f.walk_only)


## Nobody asks: the signal rests on its stage. A car on an approach that is red: its stage runs and it gets the green.
func _rests_and_turns() -> void:
	var before := _facts()
	ticks(REST_TICKS)
	var after := _facts()
	check(after.run == before.run and after.green_running, "with nobody asking the signal rests on its stage: stage %d, then %d" % [before.run, after.run])
	var red: Dictionary = {}
	for arm: Dictionary in after.arms:
		if arm.light != GREEN:
			red = arm
	if not check(not red.is_empty(), "an approach shows red to turn green for"):
		return
	var car: int = CityStore.query(&"drive_from", [red.seg, red.dir, HOPS])
	var turned := false
	for i in TURN_TICKS / POLL_TICKS:
		ticks(POLL_TICKS)
		for arm: Dictionary in _facts().arms:
			turned = turned or (arm.seg == red.seg and arm.light == GREEN)
		if turned:
			break
	check(car >= 0 and turned, "a vehicle on a red approach turns the signal: car %d on road %d got its green" % [car, red.seg])


## A walker waiting long at the kerb of a crossing that shows stop: the stage that walks it runs and its lamp says walk.
func _walkers_call() -> void:
	var stopped: Dictionary = {}
	for arm: Dictionary in _facts().arms:
		if arm.crossing and arm.lamp != LAMP_WALK:
			stopped = arm
	if not check(not stopped.is_empty(), "a crossing shows stop to call the walk for"):
		return
	var walks := false
	for i in TURN_TICKS / POLL_TICKS:
		CityStore.query(&"kerb_call", [_node, stopped.seg, LONG_WAIT_S])
		ticks(POLL_TICKS)
		for arm: Dictionary in _facts().arms:
			walks = walks or (arm.seg == stopped.seg and arm.lamp == LAMP_WALK)
		if walks:
			break
	check(walks, "a walker who waited long at the kerb gets the walk over road %d" % stopped.seg)


## What the player sets and what the panel reads: buses first is kept; the control set and what rules the junction; each leg's
## flow and the two legs of the road it is on.
func _settings() -> void:
	var info: Dictionary = CityStore.query(&"inspect", [_mid])
	check(info.get("ctl", -1) == CTL_AUTO and info.get("ruled_by", -1) == SIGNAL and info.has("greens") and not info.buses_first,
			"the panel reads an automatic junction ruled by a signal: ctl %s, ruled by %s" % [info.get("ctl"), info.get("ruled_by")])
	if info.has("greens"):
		var r: Dictionary = CityStore.dispatch(&"set_signal_plan", [_node, false, info.greens, 0.0, true])
		check(r.get("ok", false) and CityStore.query(&"inspect", [_mid]).get("buses_first", false), "buses first is set and read back")
	var legs: Array = CityStore.query(&"node_legs", [_node])
	var main := 0
	var flows := true
	for leg: Dictionary in legs:
		main += 1 if leg.main else 0
		flows = flows and leg.has("flow_h") and leg.flow_h >= 0.0
	check(legs.size() == 4 and main == 2 and flows, "each leg has its flow and two of them are the road the junction is on: %d legs, %d main" % [legs.size(), main])


## A mini-roundabout: every leg gives way and whoever is on the circle goes first. No control at all: nobody is ranked.
func _other_controls() -> void:
	check(CityStore.dispatch(&"set_node_control", [_node, CTL_MINI]).get("ok", false), "the junction made a mini-roundabout")
	ticks(SETTLE_TICKS)
	var mini := _facts()
	check(mini.circle and _ranks(mini) == [RANK_YIELD], "on a mini-roundabout every leg gives way and the circle goes first: ranks %s" % [_ranks(mini)])
	check(CityStore.dispatch(&"set_node_control", [_node, CTL_NONE]).get("ok", false), "the junction left with no control")
	ticks(SETTLE_TICKS)
	var none := _facts()
	check(not none.circle and _ranks(none) == [RANK_FREE], "with no control nobody is ranked (no highway meets it): ranks %s" % [_ranks(none)])
	var ruled := 0
	for leg: Dictionary in CityStore.query(&"node_legs", [_node]):
		ruled += 1 if leg.rule != 0 else 0
	check(ruled == 0, "and no leg carries a rule's plate: %d" % ruled)


## The different ranks of a junction's arriving arms.
func _ranks(f: Dictionary) -> Array:
	var out := []
	for arm: Dictionary in f.arms:
		if not out.has(arm.rank):
			out.append(arm.rank)
	return out
