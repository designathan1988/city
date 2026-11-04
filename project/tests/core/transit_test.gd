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

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	_buses()
	_trains()
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
	_check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "linha de ônibus")
	CityStore.step_ticks(RUN_TICKS)
	var run: Dictionary = CityStore.query(&"transit_run")
	print("ônibus: ", run)
	_check(int(run.buses) > 0, "ônibus na linha")
	_check(int(run.stops_called) > 0, "ônibus atendeu parada")


func _trains() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + TRACK_OUT_M, 0, b.min.z)
	var e := a + Vector3(0, 0, TRACK_LONG_M)
	_check(CityStore.dispatch(&"transit_track", [PackedVector3Array([a, e]), TRAIN]).get("ok", false), "trilho")
	_check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.1), TRAIN]).get("ok", false), "estação 1")
	_check(CityStore.dispatch(&"transit_station", [a.lerp(e, 0.9), TRAIN]).get("ok", false), "estação 2")
	var ids := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		if int(s.mode) == TRAIN:
			ids.append(s.id)
	_check(CityStore.dispatch(&"transit_line", [ids]).get("ok", false), "linha de trem")
	CityStore.step_ticks(RUN_TICKS)
	var run: Dictionary = CityStore.query(&"transit_run")
	print("trens: ", run)
	_check(int(run.trains) > 0, "trens na linha")
	_check(int(run.trains_at_stations) > 0, "trem parou em estação")


func _undo_and_save() -> void:
	var before: Dictionary = CityStore.query(&"transit_plan")
	CityStore.dispatch(&"undo", [])
	_check(CityStore.query(&"transit_plan").lines.size() == before.lines.size() - 1, "desfazer tira a linha")
	CityStore.dispatch(&"redo", [])
	_check(CityStore.query(&"transit_plan").lines.size() == before.lines.size(), "refazer devolve a linha")
	CityStore.save_game(SLOT)
	await CityStore.saved
	CityStore.new_map(TOWN_MAP, SEED, true)
	_check(CityStore.query(&"transit_plan").lines.is_empty(), "mapa novo sem linhas")
	CityStore.load_game(SLOT)
	await CityStore.loaded
	var after: Dictionary = CityStore.query(&"transit_plan")
	_check(after.stops.size() == before.stops.size() and after.lines.size() == before.lines.size() and after.tracks == before.tracks, "plano volta do arquivo")


func _check(ok: bool, what: String) -> void:
	if not ok:
		failures += 1
		print("FAIL ", what)
