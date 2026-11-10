## Core test of public transport through the store: stops on a street, a line through them, buses put on the road and calling at their
## stops; a track laid clear of the streets, stations on it, a train line whose trains run and stop; undo and redo of the plan; and the
## plan back from a save file. Run: godot --headless --path project res://tests/core/transit_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const TRAIN := 1
const STEP_M := 60.0
const RUN_TICKS := 6000
const SLOT := "teste_transporte"
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

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	_buses()
	_trains()
	_level_crossing()
	_undo_and_save()
	print("transit_test: ", "ok" if failures == 0 else "FAIL %d" % failures)
	get_tree().quit(failures)


func _buses() -> void:
	var probe: Dictionary = CityStore.query(&"traffic_probe", [1])
	CityStore.step_ticks(10)
	var ground: Vector3 = probe.busy_crossing
	var placed := 0
	for i in 12:
		var at := ground + Vector3(STEP_M * (i - 6), 0, 0)
		if CityStore.dispatch(&"transit_stop", [at]).get("ok", false):
			placed += 1
	var plan: Dictionary = CityStore.query(&"transit_plan")
	_check(placed >= 2, "paradas postas: %d" % placed)
	var ids := PackedInt32Array()
	for s: Dictionary in plan.stops:
		ids.append(s.id)
	_check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "bus line")
	CityStore.step_ticks(RUN_TICKS)
	var run: Dictionary = CityStore.query(&"transit_run")
	print("buses: ", run)
	_check(int(run.buses) > 0, "buses on the line")
	_check(int(run.stops_called) > 0, "a bus served a stop")
	_check(int(run.alighted) > 0, "people get off the bus: %s" % run.alighted)
	_check(int(run.boarded) > 0, "people get on the bus: %s" % run.boarded)
	var far := Vector3(ground.x, 0, ground.z)
	CityStore.dispatch(&"transit_stop", [far + Vector3(0, 0, STEP_M)]) # an edit of the plan leaves the buses where they are
	CityStore.step_ticks(1)
	var after: Dictionary = CityStore.query(&"transit_run")
	_check(int(after.on_road) >= int(run.on_road), "buses go on after an edit of the plan: %d of %d" % [after.on_road, run.on_road])


## A track across a street near the busiest junction: the lanes it crosses are found, and traffic waits as a train comes.
func _level_crossing() -> void:
	var ground: Vector3 = CityStore.query(&"traffic_probe", [1]).busy_crossing
	var laid := false
	for i in range(-CROSSING_GRID, CROSSING_GRID + 1):
		for k in range(-CROSSING_GRID, CROSSING_GRID + 1):
			if not laid:
				laid = _lay_across(ground + Vector3(i, 0, k) * CROSSING_STEP_M)
	_check(laid, "a line through the level crossing")
	var most := 0
	var seen := 0
	for i in CROSSING_SAMPLES:
		CityStore.step_ticks(CROSSING_SAMPLE_TICKS)
		var run: Dictionary = CityStore.query(&"transit_run")
		most = maxi(most, int(run.held_at_crossing))
		seen = int(run.crossings)
	print("crossing: lanes crossed ", seen, ", vehicles stopped at most ", most)
	_check(seen > 0, "lanes crossed")
	_check(most > 0, "a vehicle waits at the crossing")


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
	_check(CityStore.dispatch(&"transit_track", [PackedVector3Array([a, e]), TRAIN]).get("ok", false), "trilho")
	_check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.1), TRAIN]).get("ok", false), "station 1")
	_check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.9), TRAIN]).get("ok", false), "station 2")
	var ids := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(s.mode) == TRAIN:
			ids.append(s.id)
	_check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "train line")
	CityStore.step_ticks(RUN_TICKS)
	var run: Dictionary = CityStore.query(&"transit_run")
	print("trens: ", run)
	_check(int(run.trains) > 0, "trains on the line")
	_check(int(run.trains_at_stations) > 0, "a train stopped at a station")


func _undo_and_save() -> void:
	var before: Dictionary = CityStore.query(&"transit_plan")
	CityStore.dispatch(&"undo", [])
	_check(CityStore.query(&"transit_plan").lines.size() == before.lines.size() - 1, "undo takes the line away")
	CityStore.dispatch(&"redo", [])
	_check(CityStore.query(&"transit_plan").lines.size() == before.lines.size(), "redo gives the line back")
	CityStore.saves.save_game(SLOT)
	await CityStore.saved
	CityStore.new_map(TOWN_MAP, SEED, true)
	_check(CityStore.query(&"transit_plan").lines.is_empty(), "a new map with no lines")
	CityStore.saves.load_game(SLOT)
	await CityStore.loaded
	var after: Dictionary = CityStore.query(&"transit_plan")
	_check(after.stops.size() == before.stops.size() and after.lines.size() == before.lines.size() and after.tracks == before.tracks, "the plan comes back from the file")


func _check(ok: bool, what: String) -> void:
	if not ok:
		failures += 1
		print("FAIL ", what)
