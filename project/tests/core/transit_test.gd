## Core test of public transport through the store: stops on a street, a line through them, buses put on the road and calling at their
## stops; a track laid clear of the streets, stations on it, a train line whose trains run and stop; undo and redo of the plan; and the
## plan back from a save file. Run: godot --headless --path project res://tests/core/transit_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const STEP_M := 60.0
const RUN_TICKS := 6000
const SAMPLE_TICKS := 20 # how often the bus run is read while it goes: somebody between a door and the footway lasts longer than this
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


func _buses() -> void:
	var probe: Dictionary = CityStore.query(&"traffic_probe", [1])
	ticks(10)
	var ground: Vector3 = probe.busy_crossing
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
	ticks(1) # the step reads the plan: the line's buses are made, with who rides in them
	var start: Dictionary = CityStore.query(&"transit_run")
	check(int(start.buses) > 0 and int(start.riders) > 0, "the buses start with people aboard: %d in %d" % [start.riders, start.buses])
	var stepping_off := 0
	var walking_up := 0
	var least := int(start.riders)
	var most := least
	for _tick in range(0, RUN_TICKS, SAMPLE_TICKS):
		ticks(SAMPLE_TICKS)
		var now: Dictionary = CityStore.query(&"transit_run")
		stepping_off = maxi(stepping_off, int(now.stepping_off))
		walking_up = maxi(walking_up, int(now.walking_up))
		least = mini(least, int(now.riders))
		most = maxi(most, int(now.riders))
	var run: Dictionary = CityStore.query(&"transit_run")
	print("buses: ", run)
	check(stepping_off > 0, "somebody seen stepping down from a bus's door to the footway")
	check(walking_up > 0, "somebody seen walking up to a bus's door")
	check(int(run.set_down) > 0, "those off walk out on to the footway and on: %s of %s" % [run.set_down, run.alighted])
	check(int(run.riders) - int(start.riders) == int(run.boarded) - int(run.alighted),
			"who rides is counted: %d aboard at first, %d now, %d on, %d off" % [start.riders, run.riders, run.boarded, run.alighted])
	check(least != most, "the count aboard changed: %d .. %d" % [least, most])
	check(int(run.buses) > 0, "buses on the line")
	check(int(run.stops_called) > 0, "a bus served a stop")
	check(int(run.alighted) > 0, "people get off the bus: %s" % run.alighted)
	check(int(run.boarded) > 0, "people get on the bus: %s" % run.boarded)
	var far := Vector3(ground.x, 0, ground.z)
	CityStore.dispatch(&"transit_stop", [far + Vector3(0, 0, STEP_M)]) # an edit of the plan leaves the buses where they are
	ticks(1)
	var after: Dictionary = CityStore.query(&"transit_run")
	check(int(after.on_road) >= int(run.on_road), "buses go on after an edit of the plan: %d of %d" % [after.on_road, run.on_road])


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
		if not CityStore.dispatch(&"transit_track", [run, CityStore.code(&"TransitMode", &"TRAIN")]).get("ok", false):
			continue
		var before: Array = CityStore.query(&"transit_plan").stops.map(func(q: Dictionary) -> int: return q.id)
		for side: float in [-1.0, 1.0]:
			CityStore.dispatch(&"transit_station", [road + way.out * side * half * CROSSING_STATION, CityStore.code(&"TransitMode", &"TRAIN")])
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
	check(CityStore.dispatch(&"transit_track", [PackedVector3Array([a, e]), CityStore.code(&"TransitMode", &"TRAIN")]).get("ok", false), "track")
	check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.1), CityStore.code(&"TransitMode", &"TRAIN")]).get("ok", false), "station 1")
	check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.9), CityStore.code(&"TransitMode", &"TRAIN")]).get("ok", false), "station 2")
	var ids := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(s.mode) == CityStore.code(&"TransitMode", &"TRAIN"):
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
	var opened: Dictionary = await CityStore.saves.load_game(SLOT, true)
	check(opened.get("ok", false), "the file opens: %s" % opened)
	var after: Dictionary = CityStore.query(&"transit_plan")
	check(after.stops.size() == before.stops.size() and after.lines.size() == before.lines.size() and after.tracks == before.tracks, "the plan comes back from the file")
	DirAccess.remove_absolute(CityStore.saves.save_path(SLOT))
