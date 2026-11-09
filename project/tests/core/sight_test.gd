## Nobody is taken away where the player may see them (sim/traffic/sight.h): on the town with its clock running for some minutes, no
## near vehicle and no walker leaves the street inside the radius the render draws it in, but into a door or a vehicle; a trip that
## ends there drives on or stands, a walk goes on or stands. The camera's eye is the city's middle (no camera in a test: CityState's
## default), so the whole 640 m town lies inside both radii and every end of a trip or a walk is in sight.
## Run: godot --path project res://tests/core/sight_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const MINUTES := 5
const TICKS_A_MINUTE := 1200 # sim.tick_hz 20 (tuning.json) x 60 s
const FILL_TICKS := 200 # the town is filled in its first spawn_fresh_s (traffic.json)
## The longest a near vehicle may stand: a red light, a queue behind it and a turn across traffic, never the whole run (a gridlock, or a
## trip that ended and stands with nowhere to go).
const STOOD_MOST_S := 180.0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	ticks(FILL_TICKS)
	var first: Dictionary = CityStore.query(&"traffic_probe", [1])
	check(int(first.vehicles) > 0 and int(first.people) > 0, "the town has traffic and walkers: %d vehicles, %d people" % [first.vehicles, first.people])
	for m in MINUTES:
		ticks(TICKS_A_MINUTE)
	var p: Dictionary = CityStore.query(&"traffic_probe", [1])
	print("after %d minutes: %d vehicles, %d people, %d trips arrived, %d walkers arrived; longest stood %.0f s" % [
		MINUTES, p.vehicles, p.people, p.trips_arrived, p.people_arrived, p.stuck_longest_s])
	check(int(p.people_arrived) > 0, "walks end meanwhile: %d" % p.people_arrived)
	check(float(p.stuck_longest_s) < STOOD_MOST_S, "no near vehicle stands for good: the longest %.0f s" % p.stuck_longest_s)
	check(int(p.gone_in_sight[0]) == 0, "no vehicle taken away in sight: %d" % p.gone_in_sight[0])
	check(int(p.gone_in_sight[1]) == 0, "no walker taken away in sight: %d" % p.gone_in_sight[1])
	check(int(p.vehicles) > 0 and int(p.people) > 0, "the town keeps traffic and walkers: %d vehicles, %d people" % [p.vehicles, p.people])
	finish()
