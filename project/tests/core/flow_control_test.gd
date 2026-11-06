## The control a junction on automatic gets from its flows: the translated classes against the Roadcraft 4.0 on one script (the
## numbers below came from its own code, tools/converter/scripts/call40.mjs, ORACLE), real vehicles counted, and a junction of the
## city turned into a signal by what passes through it.
extends Node3D

const TOWN_MAP := 0
const SEED := 1
## The game keeps floats, the 4.0 doubles: this far apart and no more.
const SAME := 0.00002
const ORACLE := {"busy_major": 872.4241116004741, "busy_minor": 290.64694425165396, "busy_choice": "signal", "quiet_major": 0.038951884536446886,
	"quiet_choice": "priority", "changed_at": 900, "quiet_flow": 240.0, "warranted": "signal"}
## Past the trend window, with room for the first evaluation after it.
const PUSH_S := 1000
const DRIVE_FRAMES := 240
const STOP_FRAMES := 1200
const STOP_NODES := 320 # every node of the town: the traffic flows with a Stop at every junction
const STOP_LEAST := 40
const CTL_STOP := 3 # road_network.h NodeControl

var failures := 0


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	_replay()
	await _driven()
	_junction()
	await _stop_sign()
	print("FAILURES: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## Every number and choice of the script, as the 4.0 gave them.
func _replay() -> void:
	var got: Dictionary = CityStore.query(&"flow_replay")
	print("replay: ", got)
	for key: String in ORACLE:
		var want: Variant = ORACLE[key]
		var same: bool = absf(got[key] - want) <= absf(want) * SAME if want is float else got[key] == want
		_check(same, "%s: %s, the 4.0 gives %s" % [key, got[key], want])


## The city running: vehicles entering junctions reach the count.
func _driven() -> void:
	CityStore.dispatch(&"set_speed", [2])
	for i in DRIVE_FRAMES:
		await get_tree().process_frame
	CityStore.dispatch(&"set_speed", [0])
	var state: Dictionary = CityStore.query(&"flow_state", [-1])
	print("rodando: ", state)
	_check(state.measured > 0.0, "the traffic's time reaches the measure")
	_check(state.advised > 0, "vehicles counted at junctions")


## A Stop on every leg: who goes through has come to a stand at the line.
func _stop_sign() -> void:
	var signed := 0
	for n in STOP_NODES:
		signed += 1 if CityStore.dispatch(&"set_node_control", [n, CTL_STOP]).get("ok", false) else 0
	_check(signed > 0, "Stop put at %d junctions" % signed)
	CityStore.dispatch(&"set_speed", [2])
	for i in STOP_FRAMES:
		await get_tree().process_frame
	CityStore.dispatch(&"set_speed", [0])
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	_check(probe.stop_entries >= STOP_LEAST, "vehicles went through the Stop: %d" % probe.stop_entries)
	_check(probe.stop_skipped == 0, "all stopped first: %d passed without stopping" % probe.stop_skipped)


func _junction() -> void:
	var node: int = CityStore.query(&"flow_junction")
	_check(node >= 0, "there is a three-arm junction with no signal")
	if node < 0:
		return
	var state: Dictionary = CityStore.query(&"flow_push", [node, PUSH_S])
	print("empurrado: ", state)
	_check(state.choice == "signal" and state.signal, "the flow puts the signal at the junction")
