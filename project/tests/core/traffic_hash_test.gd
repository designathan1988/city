## The simulation is deterministic: the same map, stepped by hand twice, gives the same hash of the fleet on every tick. The first
## tick that differs is named: the cause is in what that step did (an unordered walk, a clock, an uninitialised field).
extends Node

const MAP := 0
const SEED := 1
const TICKS := 400


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	var runs: Array[PackedStringArray] = []
	for run in 2:
		CityStore.new_map(MAP, SEED, true, true)
		var t0 := Time.get_ticks_msec()
		runs.append(CityStore.query(&"traffic_hashes", [TICKS]))
		print("round %d: %d steps in %d ms, last %s" % [run, TICKS, Time.get_ticks_msec() - t0, runs[run][TICKS - 1]])
	var first := -1
	for k in TICKS:
		if runs[0][k] != runs[1][k]:
			first = k
			break
	if first >= 0:
		print("FAIL the traffic diverged at step ", first)
	print("FAILURES: ", 0 if first < 0 else 1)
	get_tree().quit(0 if first < 0 else 1)
