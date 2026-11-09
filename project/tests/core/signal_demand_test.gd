## The actuated signals run on demand (sim/traffic/signal_timing.cpp, Roadcraft lab src/sim/signals/fsm.ts): with nobody on the
## streets a signal rests in the green it has, and once the city's traffic and walkers come its stages change by their calls.
extends TestCase

const MAP := 0
const SEED := 1
## Levels of traffic, people and demand (CityStore.set_levels): none, then the city's own.
const EMPTY := 0.0
const FULL := 1.0
## Ticks of the empty city: 60 s at sim_tick_hz 20, past the longest cycle a plan may have shared out (signal_cycle_max_s 120 s
## over two stages), so a signal that cycled by the clock would have changed stage.
const REST_TICKS := 1200
## Ticks of the city with its traffic, watched a stage at a time.
const BUSY_TICKS := 2400
const LOOK_EVERY := 20


func _ready() -> void:
	if not boot():
		finish()
		return
	CityStore.set_levels(EMPTY, EMPTY, EMPTY)
	CityStore.set_sim_switch("pedestrians", false) # the walkers of doors and stops come whatever the levels say
	new_map(MAP, SEED, true)
	var node := _signal()
	if not check(node >= 0, "a signalled junction in the town: %d" % node):
		_full()
		finish()
		return
	var first: int = CityStore.query(&"node_traffic", [node]).get("stage", -1)
	var changes := _watch(node, REST_TICKS)
	check(changes == 0, "nobody asks: the signal rests in stage %d (%d changes in %d ticks)" % [first, changes, REST_TICKS])
	_full()
	node = _signal()
	changes = _watch(node, BUSY_TICKS)
	check(changes > 0, "the city's calls change its stages: %d changes in %d ticks at node %d" % [changes, BUSY_TICKS, node])
	finish()


## The city's own levels and walkers back.
func _full() -> void:
	CityStore.set_levels(FULL, FULL, FULL)
	CityStore.set_sim_switch("pedestrians", true)


## The lit junction the probe names (the one with most people at its kerbs, the first one when nobody walks): -1 for none.
func _signal() -> int:
	var at: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	var was: Dictionary = CityStore.query(&"inspect", [at])
	return int(was.get("id", -1)) if was.get("kind", "") == "node" and was.has("greens") else -1


## How many times the running stage of `node` changed over `count` ticks, looked at every LOOK_EVERY.
func _watch(node: int, count: int) -> int:
	var stage: int = CityStore.query(&"node_traffic", [node]).get("stage", -1)
	var changes := 0
	for k in count / LOOK_EVERY:
		ticks(LOOK_EVERY)
		var now: int = CityStore.query(&"node_traffic", [node]).get("stage", -1)
		if now != stage:
			changes += 1
			stage = now
	return changes
