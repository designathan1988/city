## The junctions' conflicts are found with work that follows where bodies meet, not with every sample of one way against every
## sample of the other: the town's full build tests few body rectangles a pair of connectors (probe traffic_probe, sweep_*).
## Run: godot --path project res://tests/core/junction_sweep_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
# A pair is settled by its blocks' boxes and the samples where the bodies come near: a few hundred tests. Every sample against
# every sample is some forty thousand.
const TESTS_PER_PAIR_MOST := 2000.0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	var pairs: int = probe.get("sweep_pairs", 0)
	var tests: int = probe.get("sweep_tests", 0)
	var each := float(tests) / maxf(1.0, float(pairs))
	print("junction sweeps: %d pairs, %d rectangles tested, %.1f a pair" % [pairs, tests, each])
	check(pairs > 0, "the town's junctions were swept: %d pairs of connectors" % pairs)
	check(each <= TESTS_PER_PAIR_MOST, "a pair of connectors costs few rectangle tests: %.1f (at most %.0f)" % [each, TESTS_PER_PAIR_MOST])
	finish()
