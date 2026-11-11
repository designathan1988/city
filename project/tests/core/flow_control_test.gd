## The control a junction on automatic gets: from its geometry before any flow is measured (junctions drawn on free ground), and
## from its flows: the translated classes against the Roadcraft 4.0 on one script (the numbers below came from its own code,
## tools/converter/scripts/call40.mjs, ORACLE), real vehicles counted, and a junction of the city turned into a signal by what
## passes through it.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## The game keeps floats, the 4.0 doubles: this far apart and no more.
const SAME := 0.00002
const ORACLE := {"busy_major": 872.4241116004741, "busy_minor": 290.64694425165396, "busy_choice": "signal", "quiet_major": 0.038951884536446886,
	"quiet_choice": "priority", "changed_at": 900, "quiet_flow": 240.0, "warranted": "signal"}
## Past the trend window, with room for the first evaluation after it.
const PUSH_S := 1000
## The game time each run takes, in ticks of the simulation (sim_tick_hz 20, two steps a tick at speed 2): 4 s and 20 s of the
## city running fast, stepped by the test.
const DRIVE_TICKS := 160
const STOP_TICKS := 800
const STOP_NODES := 320 # every node of the town: the traffic flows with a Stop at every junction
const STOP_LEAST := 40
const CTL_STOP := 3 # road_network.h NodeControl
## The junctions drawn on free ground past the town: arms of local streets (road_profiles.json localStreet, two lanes) and an
## avenue (avenue, four lanes); a tick or two for the lane graph to take the edit up.
const LOCAL := 0
const AVENUE := 3
const OUT_M := 300.0
const ARM_M := 100.0
const SITES_APART_M := 400.0
const PX := 0.25
const SETTLE_TICKS := 4
const SIGNAL_POST := 0 # furniture_types.h FurnitureKind::SIGNAL
const FURNITURE_SAMPLE := 1 # traffic_probe counts the street furniture only with a sample
const T_ARMS := 3
const T_JOINING := 1 # at a T the stem joins the road the junction is on

func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	_replay()
	var quiet := _geometry()
	_driven()
	_junction(quiet)
	_stop_sign()
	finish()


## Every number and choice of the script, as the 4.0 gave them.
func _replay() -> void:
	var got: Dictionary = CityStore.query(&"flow_replay")
	print("replay: ", got)
	for key: String in ORACLE:
		var want: Variant = ORACLE[key]
		var same: bool = absf(got[key] - want) <= absf(want) * SAME if want is float else got[key] == want
		check(same, "%s: %s, the 4.0 gives %s" % [key, got[key], want])


## Before the flows speak, a junction on automatic runs on signals when it is a crossing of four arms or more, or a T where one
## road carries four lanes or more (two each way); a T of two-lane streets runs without. A signal stands on its posts. Returns
## a T of two-lane streets left as it is, for the flows to push (-1: none was laid).
func _geometry() -> int:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var o := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	var mid := o + Vector3(ARM_M, 0, 0)
	_road(o, o + Vector3(2 * ARM_M, 0, 0), LOCAL)
	_road(mid + Vector3(0, 0, ARM_M), mid, LOCAL)
	_signal_at(mid, 3, false, "a T of two-lane streets")
	var posts := _signal_posts()
	_road(mid - Vector3(0, 0, ARM_M), mid, LOCAL)
	_signal_at(mid, 4, true, "its fourth arm makes a crossing")
	check(_signal_posts() > posts, "the crossing's signal stands on posts: %d -> %d on the map" % [posts, _signal_posts()])
	var p := o + Vector3(0, 0, SITES_APART_M)
	var t := p + Vector3(ARM_M, 0, 0)
	_road(p, p + Vector3(2 * ARM_M, 0, 0), AVENUE)
	_road(t + Vector3(0, 0, ARM_M), t, LOCAL)
	_signal_at(t, 3, true, "a T on a four-lane avenue")
	var q := o - Vector3(0, 0, SITES_APART_M)
	var stem := q + Vector3(ARM_M, 0, 0)
	_road(q, q + Vector3(2 * ARM_M, 0, 0), LOCAL)
	_road(stem - Vector3(0, 0, ARM_M), stem, LOCAL)
	return _signal_at(stem, 3, false, "another T of two-lane streets")


func _road(a: Vector3, e: Vector3, profile: int) -> void:
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, profile, PX])
	check(r.get("ok", false), "road %s -> %s laid: %s" % [a, e, r])


## The junction at `at`: its arms as the inspector counts them, and whether the core runs it on signals. Without one, the road
## the junction is on has the way and every arm joining it gives way (the reference game's rule): at a T the stem alone. Returns
## its node (-1: no junction of those arms there).
func _signal_at(at: Vector3, arms: int, signalled: bool, what: String) -> int:
	ticks(SETTLE_TICKS)
	var node: Dictionary = CityStore.query(&"inspect", [at])
	if not check(node.get("kind", "") == "node" and node.get("arms", 0) == arms, "%s: a junction of %d arms %s" % [what, arms, node]):
		return -1
	var state: Dictionary = CityStore.query(&"flow_state", [node.id])
	check(state.signal == signalled, "%s: %s" % [what, "a signal" if signalled else "no signal"])
	if not signalled and arms == T_ARMS:
		check(int(state.giving_way) == T_JOINING, "%s: the stem gives way, the road it meets has the way: %d arms give way" % [what, state.giving_way])
	return node.id


func _signal_posts() -> int:
	return CityStore.query(&"traffic_probe", [FURNITURE_SAMPLE]).street_furniture[SIGNAL_POST]


## The city running: vehicles entering junctions reach the count.
func _driven() -> void:
	ticks(DRIVE_TICKS)
	var state: Dictionary = CityStore.query(&"flow_state", [-1])
	print("running: ", state)
	check(state.measured > 0.0, "the traffic's time reaches the measure")
	check(state.advised > 0, "vehicles counted at junctions")


## A Stop on every leg: who goes through has come to a stand at the line.
func _stop_sign() -> void:
	var signed := 0
	for n in STOP_NODES:
		signed += 1 if CityStore.dispatch(&"set_node_control", [n, CTL_STOP]).get("ok", false) else 0
	check(signed > 0, "Stop put at %d junctions" % signed)
	ticks(STOP_TICKS)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	check(probe.stop_entries >= STOP_LEAST, "vehicles went through the Stop: %d" % probe.stop_entries)
	check(probe.stop_skipped == 0, "all stopped first: %d passed without stopping" % probe.stop_skipped)


## The flows through a T of two-lane streets, as many as warrant a signal, put one on it.
func _junction(node: int) -> void:
	if not check(node >= 0, "a three-arm junction with no signal to push"):
		return
	var state: Dictionary = CityStore.query(&"flow_push", [node, PUSH_S])
	print("pushed: ", state)
	check(state.choice == "signal" and state.signal, "the flow puts the signal at the junction")
