## Core test of public transport through the store: stops on a street, a line through them, buses put on the road and calling at their
## stops; a track laid clear of the streets, stations on it, a train line whose trains run and stop; undo and redo of the plan; and the
## plan back from a save file. Run: godot --headless --path project res://tests/core/transit_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const TRAIN := 1
const STEP_M := 60.0
const RUN_TICKS := 6000
const SLOT := "transit_test"
const TRACK_OUT_M := 80.0
const TRACK_LONG_M := 400.0
const CROSSING_STEP_M := 60.0
const CROSSING_GRID := 10
const CROSSING_HALVES := [120.0, 80.0]
const CROSSING_REACH_M := 18.0
const CROSSING_OFF_M := 6.0
const CROSSING_STATION := 0.7
const CROSSING_SAMPLES := 200
const CROSSING_SAMPLE_TICKS := 20
const MODE_KERB := 3 # PedMode::KERB: a walker a kerb stop or a bus's call holds
const HELD_REACH_M := 600.0 # round the busy crossing: the bus line's stops
const HELD_LONGEST_S := 40.0 # past traffic.json kerb_give_up_s + kerb_delivery_s (25 + 7): no errand holds anybody this long
const HELD_SPOT_M := 0.01
const HOUR := 8.0 # the morning's walkers: the buses call where people are out on foot
const WARM_TICKS := 1200 # the town fills before the busiest crossing is read (walker_motion_test's warm-up)
const KERB_SET_DOWN := 1 # traffic_probe kerb: [planned, set down, picked up, delivered, given up]
const KERB_PICKED := 2
const KERB_DELIVERED := 3


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	_buses()
	_trains()
	_level_crossing()
	await _undo_and_save()
	finish()


## The line runs through the junction with the most people waiting or crossing, read once the town is out on foot: before that no junction
## has anybody, and the stops would stand wherever, maybe where nobody walks.
func _buses() -> void:
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(WARM_TICKS)
	var ground: Vector3 = CityStore.query(&"traffic_probe", [1]).busy_crossing
	var placed := 0
	for i in 12:
		var at := ground + Vector3(STEP_M * (i - 6), 0, 0)
		if CityStore.dispatch(&"transit_stop", [at]).get("ok", false):
			placed += 1
	var plan: Dictionary = CityStore.query(&"transit_plan")
	check(placed >= 2, "stops placed: %d" % placed)
	var ids := PackedInt32Array()
	for s: Dictionary in plan.stops:
		ids.append(s.id)
	check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "bus line")
	ticks(RUN_TICKS)
	var run: Dictionary = CityStore.query(&"transit_run")
	print("buses: ", run)
	check(int(run.buses) > 0, "buses on the line")
	check(int(run.stops_called) > 0, "a bus served a stop")
	check(int(run.alighted) > 0, "people get off the bus: %s" % run.alighted)
	check(int(run.boarded) > 0, "people get on the bus: %s (on the footway in reach %s, called %s, came to the door's arc %s)" % [run.boarded, run.in_reach, run.called, run.came])
	var kerb: Array = CityStore.query(&"traffic_probe", [0]).kerb # planned, set down, picked up, delivered, given up
	check(int(kerb[KERB_PICKED]) > 0, "a car stops for somebody on the footway, who walks up and gets in: %d (stops planned, set down, picked up, delivered, given up: %s)" % [kerb[KERB_PICKED], kerb])
	# every errand's whole visit (kerb_phase.cpp KERB_PHASES; the checking DLL stops on any other move): pulled up, door open, the move
	# between seat and footway, door shut; the one picked up fetched first, the truck's mate away with the load and back
	check(int(kerb[KERB_SET_DOWN]) > 0 and int(kerb[KERB_DELIVERED]) > 0,
			"cars set somebody down and trucks deliver at the kerb: %d set down, %d delivered" % [kerb[KERB_SET_DOWN], kerb[KERB_DELIVERED]])
	_nobody_left_held(ground)
	var far := Vector3(ground.x, 0, ground.z)
	CityStore.dispatch(&"transit_stop", [far + Vector3(0, 0, STEP_M)]) # an edit of the plan leaves the buses where they are
	ticks(1)
	var after: Dictionary = CityStore.query(&"transit_run")
	check(int(after.on_road) >= int(run.on_road), "buses go on after an edit of the plan: %d of %d" % [after.on_road, run.on_road])


## Whoever a stop holds (getting off a bus or a car, walking up to one) is let go when its errand is over: nobody is still under
## a stop's hand, on the same spot, after longer than the longest errand lasts.
func _nobody_left_held(ground: Vector3) -> void:
	var held := _held(ground)
	ticks(int(HELD_LONGEST_S * float(DataTables.tuning("sim_tick_hz"))))
	var still := _held(ground)
	var left := 0
	for id: int in held:
		if still.has(id) and (still[id] as Vector2).distance_to(held[id]) < HELD_SPOT_M:
			left += 1
	check(left == 0, "nobody is left standing under a stop's hand: %d of %d held" % [left, held.size()])


## The walkers a stop holds round a point (probe walker_bodies: id, x, z, pace, mode), by id: where each stands.
func _held(ground: Vector3) -> Dictionary:
	var out := {}
	for w: Array in CityStore.query(&"walker_bodies", [ground, HELD_REACH_M]).bodies:
		if int(w[4]) == MODE_KERB:
			out[int(w[0])] = Vector2(w[1], w[2])
	return out


## A track across a street near the busiest junction: the lanes it crosses are found, and traffic waits as a train comes.
func _level_crossing() -> void:
	var ground: Vector3 = CityStore.query(&"traffic_probe", [1]).busy_crossing
	var laid := false
	for i in range(-CROSSING_GRID, CROSSING_GRID + 1):
		for k in range(-CROSSING_GRID, CROSSING_GRID + 1):
			if not laid:
				laid = _lay_across(ground + Vector3(i, 0, k) * CROSSING_STEP_M)
	check(laid, "a line through the level crossing")
	var most := 0
	var seen := 0
	for i in CROSSING_SAMPLES:
		ticks(CROSSING_SAMPLE_TICKS)
		var run: Dictionary = CityStore.query(&"transit_run")
		most = maxi(most, int(run.held_at_crossing))
		seen = int(run.crossings)
	print("crossing: lanes crossed ", seen, ", vehicles stopped at most ", most)
	check(seen > 0, "lanes crossed")
	check(most > 0, "a vehicle waits at the crossing")


## A track across the street nearest a point, if the core lets it through (a wide avenue it does not), two stations and a line.
func _lay_across(at: Vector3) -> bool:
	var way: Dictionary = CityStore.query(&"transit_footway", [at, CROSSING_REACH_M])
	if way.is_empty():
		return false
	var road: Vector3 = way.at - way.out * CROSSING_OFF_M
	for half: float in CROSSING_HALVES:
		var run := PackedVector3Array([road - way.out * half, road + way.out * half])
		if not CityStore.dispatch(&"transit_track", [run, TRAIN]).get("ok", false):
			continue
		var before: Array = CityStore.query(&"transit_plan").stops.map(func(q: Dictionary) -> int: return q.id)
		for side: float in [-1.0, 1.0]:
			CityStore.dispatch(&"transit_station", [road + way.out * side * half * CROSSING_STATION, TRAIN])
		var ids := PackedInt32Array()
		for q: Dictionary in CityStore.query(&"transit_plan").stops:
			if not before.has(q.id):
				ids.append(q.id)
		return CityStore.dispatch(&"transit_line", [ids]).get("ok", false)
	return false


func _trains() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + TRACK_OUT_M, 0, b.min.z)
	var e := a + Vector3(0, 0, TRACK_LONG_M)
	check(CityStore.dispatch(&"transit_track", [PackedVector3Array([a, e]), TRAIN]).get("ok", false), "track")
	check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.1), TRAIN]).get("ok", false), "station 1")
	check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.9), TRAIN]).get("ok", false), "station 2")
	var ids := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(s.mode) == TRAIN:
			ids.append(s.id)
	check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "train line")
	ticks(RUN_TICKS)
	var run: Dictionary = CityStore.query(&"transit_run")
	print("trains: ", run)
	check(int(run.trains) > 0, "trains on the line")
	check(int(run.trains_at_stations) > 0, "a train stopped at a station")


## The plan's undo and redo, then the plan written to a file, a new map over it and the file read back (load_game is
## synchronous: its answer is the outcome).
func _undo_and_save() -> void:
	var before: Dictionary = CityStore.query(&"transit_plan")
	CityStore.dispatch(&"undo", [])
	check(CityStore.query(&"transit_plan").lines.size() == before.lines.size() - 1, "undo takes the line away")
	CityStore.dispatch(&"redo", [])
	check(CityStore.query(&"transit_plan").lines.size() == before.lines.size(), "redo gives the line back")
	var saved: Dictionary = await save(SLOT)
	check(saved.get("ok", false), "the city is written: %s" % saved)
	new_map(TOWN_MAP, SEED, true)
	check(CityStore.query(&"transit_plan").lines.is_empty(), "a new map with no lines")
	var opened: Dictionary = CityStore.saves.load_game(SLOT, true)
	check(opened.get("ok", false), "the file opens: %s" % opened)
	var after: Dictionary = CityStore.query(&"transit_plan")
	check(after.stops.size() == before.stops.size() and after.lines.size() == before.lines.size() and after.tracks == before.tracks, "the plan comes back from the file")
	DirAccess.remove_absolute(CityStore.saves.save_path(SLOT))
