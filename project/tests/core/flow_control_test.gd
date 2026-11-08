## The control a junction on automatic gets from its flows (sim/traffic/flow_warrants.h): the measure fed at known rates stands at
## them and decays when left, the rule gives the MUTCD's warrants at their thresholds (a signal by Warrant 1's Conditions A and B,
## a Stop on every leg by 2B.07) with the game's step down, real vehicles are counted, and a junction of the city turns into a signal
## by what passes through it.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
## Past the trend window, with room for the first evaluation after it.
const PUSH_S := 1000
## The game time each run takes, in ticks of the simulation (sim_tick_hz 20, two steps a tick at speed 2): 4 s and 20 s of the
## city running fast, stepped by the test.
const DRIVE_TICKS := 160
const STOP_TICKS := 800
const STOP_LEAST := 40
## AutoControl, in its order (flow_warrants.h).
const PRIORITY := 0
const STOP := 1
const SIGNAL := 2
## The MUTCD (2009) volumes, vehicles an hour: Table 4C-1's 100% column for one lane each way, Condition A and B (major street's
## two approaches, busier minor approach), and 2B.07 paragraph 04 C (major approaches, minor approaches together).
const SIGNAL_A := [500.0, 150.0]
const SIGNAL_B := [750.0, 75.0]
const MULTIWAY := [300.0, 200.0]
## One vehicle an hour under a threshold.
const UNDER := 1.0
## A third of the trend window is the measure's time constant (flow_measure.h).
const WINDOW_PER_TAU := 3.0
const VEHICLE_HOUR_S := 3600.0

func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	_replay()
	_warrants()
	_driven()
	_junction()
	_stop_sign()
	finish()


## The measure fed at known rates, then left quiet, and the choices the script's junction got.
func _replay() -> void:
	var got: Dictionary = CityStore.query(&"flow_replay")
	print("replay: ", got)
	var window: float = DataTables.tuning("flow_trend_window")
	var look: float = DataTables.tuning("flow_look_every_s")
	# A count of one vehicle moves the mean by at most an hour's vehicle over the time constant: the ripple about the fed rate.
	var ripple := VEHICLE_HOUR_S / (window / WINDOW_PER_TAU)
	check(absf(got.busy_major - got.fed_major) <= ripple, "a major leg measures what it is fed: %.1f of %.1f" % [got.busy_major, got.fed_major])
	check(absf(got.busy_minor - got.fed_minor) <= ripple, "the minor leg too: %.1f of %.1f" % [got.busy_minor, got.fed_minor])
	check(got.busy_choice == "signal", "fed over Condition A, the junction is a signal: %s" % got.busy_choice)
	check(got.changed_at == int(ceilf(window / look) * look), "the first change at the first look after a trend window: %d" % got.changed_at)
	var quiet: float = DataTables.tuning("flow_stop_major") * DataTables.tuning("flow_step_down")
	check(is_equal_approx(got.quiet_flow, quiet), "no control under the least step up, stepped down: %.1f" % got.quiet_flow)
	check(got.quiet_major < got.quiet_flow and got.quiet_choice == "priority", "left quiet %d s it decays back to priority: %.3f, %s" % [got.quiet_s, got.quiet_major, got.quiet_choice])


## The rule at the manual's thresholds, and the step down that keeps a control above 80% of them.
func _warrants() -> void:
	var down: float = DataTables.tuning("flow_step_down")
	var cases := [
		[SIGNAL_A[0], SIGNAL_A[1], SIGNAL_A[1], PRIORITY, "signal", "Condition A at its volumes"],
		[SIGNAL_A[0] - UNDER, SIGNAL_A[1], SIGNAL_A[1], PRIORITY, "priority", "Condition A a vehicle short on the major street"],
		[SIGNAL_A[0], SIGNAL_A[1] - UNDER, SIGNAL_A[1] - UNDER, PRIORITY, "priority", "Condition A a vehicle short on the minor"],
		[SIGNAL_B[0], SIGNAL_B[1], SIGNAL_B[1], PRIORITY, "signal", "Condition B at its volumes"],
		[SIGNAL_B[0] - UNDER, SIGNAL_B[1], SIGNAL_B[1], PRIORITY, "priority", "Condition B a vehicle short"],
		[MULTIWAY[0], SIGNAL_B[1], MULTIWAY[1], PRIORITY, "stop", "the multi-way stop at its volumes"],
		[MULTIWAY[0], SIGNAL_B[1], MULTIWAY[1] - UNDER, PRIORITY, "priority", "the multi-way stop a vehicle short on the minor"],
		[MULTIWAY[0] - UNDER, SIGNAL_B[1], MULTIWAY[1], PRIORITY, "priority", "the multi-way stop a vehicle short on the major"],
		[SIGNAL_A[0] * down, SIGNAL_A[1] * down, SIGNAL_A[1] * down, SIGNAL, "signal", "a signal kept at the step down"],
		[MULTIWAY[0] * down, SIGNAL_B[1], MULTIWAY[1] * down, STOP, "stop", "a stop kept at the step down"],
		[MULTIWAY[0] * down - UNDER, SIGNAL_B[1], MULTIWAY[1] * down, STOP, "priority", "a stop under the step down goes"],
	]
	for c: Array in cases:
		var got: String = CityStore.query(&"flow_warrant", [c[0], c[1], c[2], c[3]])
		check(got == c[4], "%s (%.0f, %.0f, %.0f): %s" % [c[5], c[0], c[1], c[2], got])


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
	for n in int(CityStore.snapshot.nodes): # every node of the town: the traffic flows with a Stop at every junction
		signed += 1 if CityStore.dispatch(&"set_node_control", [n, CityStore.code(&"NodeControl", &"CTL_STOP")]).get("ok", false) else 0
	check(signed > 0, "Stop put at %d junctions" % signed)
	ticks(STOP_TICKS)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	check(probe.stop_entries >= STOP_LEAST, "vehicles went through the Stop: %d" % probe.stop_entries)
	check(probe.stop_skipped == 0, "all stopped first: %d passed without stopping" % probe.stop_skipped)


func _junction() -> void:
	var node: int = CityStore.query(&"flow_junction")
	check(node >= 0, "there is a three-arm junction with no signal")
	if node < 0:
		return
	var state: Dictionary = CityStore.query(&"flow_push", [node, PUSH_S])
	print("pushed: ", state)
	check(state.choice == "signal" and state.signal, "the flow puts the signal at the junction")
