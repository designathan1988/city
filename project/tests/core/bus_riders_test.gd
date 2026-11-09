## Core test of who rides the buses, through the store: a line along a street, its buses calling at their stops. Each bus carries
## between none and its seats, and what it carries moves as people get off and on (counted, not drawn from the bus's id); whoever
## gets off walks on from the footway, nobody is left held at the kerb. And the picks over the plan's cells: each stop picked at
## its own place, a track's end snapped to.
## Run: godot --headless --path project res://tests/core/bus_riders_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const STEP_M := 60.0
const STOPS_TRIED := 12
const SAMPLES := 60
const SAMPLE_TICKS := 100
const TRACK_OUT_M := 80.0
const TRACK_LONG_M := 400.0
const SNAP_OFF_M := 1.5 # within the joining reach of the track's end


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	_riders()
	_picks()
	finish()


func _riders() -> void:
	var probe: Dictionary = CityStore.query(&"traffic_probe", [1])
	ticks(10)
	var ground: Vector3 = probe.busy_crossing
	for i in STOPS_TRIED:
		CityStore.dispatch(&"transit_stop", [ground + Vector3(STEP_M * (i - STOPS_TRIED * 0.5), 0, 0)])
	var ids := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		ids.append(s.id)
	check(ids.size() >= 2, "stops placed: %d" % ids.size())
	check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "bus line")
	var in_range := true
	var stranded := 0
	var changed := false
	var last := PackedInt32Array()
	var run: Dictionary = {}
	for k in SAMPLES:
		ticks(SAMPLE_TICKS)
		run = CityStore.query(&"transit_run")
		var riders: PackedInt32Array = run.riders
		for n in riders:
			in_range = in_range and n >= 0 and n <= int(run.riders_most)
		if riders.size() == last.size() and riders != last:
			changed = true
		last = riders
		stranded = maxi(stranded, int(run.stranded_walkers))
	print("riders: ", run.riders, " boarded ", run.boarded, " alighted ", run.alighted, " stranded at most ", stranded)
	check(int(run.alighted) > 0 and int(run.boarded) > 0, "people get off and on: %s off, %s on" % [run.alighted, run.boarded])
	check(in_range, "every bus carries between none and its seats")
	check(changed, "who rides a bus moves as people get off and on")
	check(stranded == 0, "nobody is left held at the kerb: %d" % stranded)


func _picks() -> void:
	var bus := CityStore.code(&"TransitMode", &"BUS")
	var all_found := true
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		all_found = all_found and int(CityStore.query(&"transit_pick", [s.at, bus, true])) == int(s.id)
	check(all_found, "each stop picked at its own place")
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + TRACK_OUT_M, 0, b.min.z)
	var e := a + Vector3(0, 0, TRACK_LONG_M)
	var train := CityStore.code(&"TransitMode", &"TRAIN")
	check(CityStore.dispatch(&"transit_track", [PackedVector3Array([a, e]), train]).get("ok", false), "track")
	var snapped: Vector3 = CityStore.query(&"transit_snap", [e + Vector3(SNAP_OFF_M, 0, 0), train])
	check(Vector2(snapped.x, snapped.z).distance_to(Vector2(e.x, e.z)) < 0.01, "a point near the track's end snaps to it: %s" % snapped)
	var mid := a.lerp(e, 0.5) + Vector3(SNAP_OFF_M, 0, 0)
	var on: Vector3 = CityStore.query(&"transit_snap", [mid, train])
	check(absf(on.x - a.x) < 0.01, "a point beside the track snaps onto it: %s" % on)
