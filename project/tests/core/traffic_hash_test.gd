## The simulation is deterministic: the same map, stepped by hand twice, gives the same hash of the fleet on every tick. The first
## tick that differs is named: the cause is in what that step did (an unordered walk, a clock, an uninitialised field).
extends TestCase

const MAP := 0
const SEED := 1
const TICKS := 400


func _ready() -> void:
	if not boot():
		finish()
		return
	var runs: Array[PackedStringArray] = []
	for run in 2:
		new_map(MAP, SEED, true)
		runs.append(CityStore.query(&"traffic_hashes", [TICKS]))
		if not check(runs[run].size() == TICKS, "round %d: %d steps hashed of %d" % [run, runs[run].size(), TICKS]):
			finish()
			return
		print("round %d: last %s" % [run, runs[run][TICKS - 1]])
	var first := -1
	for k in TICKS:
		if runs[0][k] != runs[1][k]:
			first = k
			break
	check(first < 0, "the two rounds agree on every step (first apart: %d)" % first)
	finish()
