## The yards of works, wherever a player puts them: a new street out of the town in each of several directions, zoned industrial,
## grown, the camera over it. The generated town has no industry, so nothing else tells a yard without a way from one that works:
## nearly every dock has its ways, a lorry fits its own lane at the stop before a gate, and one truck goes the whole round.
## Run: godot --path project res://tests/core/yard_test.tscn -- --no-flow [--sites=N]
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SITES := 2
const WARM_TICKS := 2400
const SAMPLE := 400
const ZONE_INDUSTRIAL := 3 # catalog/zones.json order
const ZONE_RADIUS_M := 160.0
const STREET_M := 300.0
const PROFILE := 0
const STEP_TICKS := 200
const GROW_STEPS := 60
const PLAN_STEPS := 30
const ROUND_STEPS := 120
const CAMERA_UP_M := 30.0
const NO_STOP := 1 # traffic::YardReach YARD_NO_STOP
const WAY_SHARE := 0.9 # measured 0.99 (105 of 106 docks); 0.1 with a lorry too wide for its lane and one search for each way

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var camera := Camera3D.new()
	add_child(camera)
	camera.make_current()
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(WARM_TICKS)
	var sites := _arg("--sites=", SITES)
	var sample: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	for i in sites:
		await _site(camera, _far_in(sample, TAU * i / sites, TAU / sites))
	await _round()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The vehicle farthest out inside a slice of the compass.
func _far_in(sample: Array, angle: float, width: float) -> Vector3:
	var far := Vector3.ZERO
	for p: Vector3 in sample:
		var flat := Vector2(p.x, p.z)
		if absf(angle_difference(flat.angle(), angle)) <= width * 0.5 and flat.length() > Vector2(far.x, far.z).length():
			far = p
	return far


func _site(camera: Camera3D, far: Vector3) -> void:
	var before: int = CityStore.query(&"dock_count", [])
	var end := far + Vector3(far.x, 0, far.z).normalized() * STREET_M
	var built: Dictionary = CityStore.dispatch(&"build_road", [far, end, false, Vector3.ZERO, PROFILE])
	var at := (far + end) * 0.5
	var zoned: Dictionary = CityStore.dispatch(&"paint_zone", [at, ZONE_RADIUS_M, ZONE_INDUSTRIAL])
	print("lugar ", at, " via ", built.ok, " ", built.error, " zona ", zoned.zoned_sides)
	camera.global_position = at + Vector3.UP * CAMERA_UP_M
	for i in GROW_STEPS:
		await get_tree().process_frame
		CityStore.step_ticks(STEP_TICKS)
		if CityStore.query(&"dock_count", []) > before:
			break
	print("  docas ", CityStore.query(&"dock_count", []) - before)
	for i in PLAN_STEPS:
		await get_tree().process_frame
		CityStore.step_ticks(STEP_TICKS)
	var s: Dictionary = CityStore.query(&"park_sample", [])
	_check(s.docks_planned > 0, "nenhuma doca planejada em %s" % at)
	_check(s.docks_reached[NO_STOP] == 0, "%d docas sem lugar para o caminhão na própria faixa em %s" % [s.docks_reached[NO_STOP], at])
	_check(s.docks_with_way >= s.docks_planned * WAY_SHARE, "%d de %d docas com caminho em %s" % [s.docks_with_way, s.docks_planned, at])
	print("  pátios ", s.yards, " portões ", s.yard_gates, " planejadas ", s.docks_planned, " com caminho ", s.docks_with_way, " parou em ", s.docks_reached, " chamadas ", s.lorry_calls, " pior plano ", s.yard_slowest_ms, " ms")


## A lorry called, backed onto its dock, unloaded and gone: the camera stays over the last works.
func _round() -> void:
	var s: Dictionary = CityStore.query(&"park_sample", [])
	for i in ROUND_STEPS:
		if s.lorries_left > 0:
			break
		await get_tree().process_frame
		CityStore.step_ticks(STEP_TICKS)
		s = CityStore.query(&"park_sample", [])
	print("caminhões (chamados, encostados, saíram) ", s.lorries_sent, " ", s.lorries_docked, " ", s.lorries_left)
	_check(s.lorries_left > 0, "nenhum caminhão fez o ciclo")


func _arg(prefix: String, fallback: int) -> int:
	for a in OS.get_cmdline_user_args():
		if a.begins_with(prefix):
			return int(a.trim_prefix(prefix))
	return fallback


func _check(ok: bool, what: String) -> void:
	if ok:
		return
	failures += 1
	printerr("FALHOU: ", what)
