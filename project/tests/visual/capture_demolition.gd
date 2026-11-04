## Captures of the bomb in the real game (the store's `strike`, the effects drawn by the native renderer): a house hit by
## a small bomb, the same by a big one, a bomb on a street (it cuts the road), and on bare earth, each at the moments
## after the blow. The time between shots is real time: the effects run on their own clock. Writes PNGs in --capture-dir.
## Run through tools/build/hidden_run.ps1: godot --path project --resolution 1600x900 res://tests/visual/capture_demolition.tscn
##   -- --no-flow --capture-dir=<dir>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const SMALL := 5
const BIG := 45
const VIEW_YAW := 25.0
const VIEW_PITCH := 38.0
const VIEW_DIST := 70.0
const FAR_DIST := 140.0
const EARTH_OUT_M := 500.0
const SCAN_STEP_M := 40.0
const MOMENTS := [0.0, 1.0, 2.0, 5.0]
const SETTLE_FRAMES := 4

var out_dir := "user://captures"
var rig: CityCamera
var ui: UiDriver
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var world := CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	ui = UiDriver.new(rig, get_tree(), out_dir)
	await _run()


func _run() -> void:
	var house := _house()
	await _blow("bomba_casa_f%d" % SMALL, house, SMALL, VIEW_DIST)
	var road := _road()
	await _blow("bomba_rua_f%d" % SMALL, road, SMALL, VIEW_DIST)
	var b: Dictionary = CityStore.query(&"city_bounds")
	var earth := ui.ground(b.max.x + EARTH_OUT_M, (b.min.z + b.max.z) * 0.5)
	await _blow("bomba_terra_f%d" % SMALL, earth, SMALL, VIEW_DIST)
	await _blow("bomba_grande_f%d" % BIG, house, BIG, FAR_DIST)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The first building with floors, as a point on its roof (the blow lands on it).
func _house() -> Vector3:
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors >= 2 and info.zone > 0:
			return info.position + Vector3(0, 6.0, 0)
	return Vector3.INF


func _road() -> Vector3:
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var x: float = bounds.min.x
	while x < bounds.max.x:
		var z: float = bounds.min.z
		while z < bounds.max.z:
			var anchor: Dictionary = CityStore.query(&"snap_road", [Vector3(x, 0, z), 0])
			if anchor.get("seg", -1) >= 0:
				return anchor.position
			z += SCAN_STEP_M
		x += SCAN_STEP_M
	return Vector3.INF


func _blow(label: String, at: Vector3, force: int, dist: float) -> void:
	_check(at != Vector3.INF, "%s: ponto achado" % label)
	if at == Vector3.INF:
		return
	rig.jump(at, VIEW_YAW, VIEW_PITCH, dist)
	await ui.drawn()
	await ui.shot("%s_0_antes" % label)
	var before: Dictionary = CityStore.query(&"damage_probe")
	var r: Dictionary = CityStore.dispatch(&"strike", [at, force])
	_check(r.ok, "%s: golpe" % label)
	await ui.frames(SETTLE_FRAMES)
	var elapsed := 0.0
	for t in MOMENTS:
		if t > elapsed:
			await get_tree().create_timer(t - elapsed).timeout
			elapsed = t
		await ui.drawn()
		await ui.shot("%s_t%d" % [label, int(t)])
	var after: Dictionary = CityStore.query(&"damage_probe")
	print(label, ": ", before, " -> ", after, " tremor ", CityStore.query(&"effects_shake"))


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
