## Captures of the bomb and the pistol in the real game (the store's `strike` and `shoot`, the effects drawn by the native
## renderer), in phases picked by --phase=: `pareada` (the views the
## reference game was photographed from: a low house hit by force 5 and 45), `people` (a walker shot in the trunk, the leg
## and the head: bodies, tracer, blood), `car` (a car shot through its glass until it bursts). The time between shots is real
## time: the effects run on their own clock. Writes PNGs in --capture-dir.
## Run through tools/build/hidden_run.ps1: godot --path project --resolution 1600x900 res://tests/visual/capture_demolition.tscn
##   -- --no-flow --phase=pareada,people,car --capture-dir=<dir>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const VIEW_DIST := 70.0
const LOW_DIST := 45.0
const WALKER_DIST := 8.0
const CAR_DIST := 18.0
const WALKER_PITCH := 25.0
const PAIRED_MOMENTS := [0.1, 0.5, 1.0, 2.0, 5.0, 12.0]
const SETTLE_FRAMES := 4
const WARM_TICKS := 600
const OPEN_M := 70.0 # no building taller than FLOORS_LOW this close: nothing between the camera and the target
const LOOKUP_FACTOR := 1.0
const TARGETS := 80
const CHEST_M := 1.2
const HEAD_M := 1.7
const LEG_M := 0.4
const GLASS_M := 1.2
const BURST_SHOTS := 14
const NORMAL_SPEED := 2 # SimSpeed::NORMAL
const MOTO := 5
const FIRST_HEAVY := 6
const FLOORS_LOW := 2

var out_dir := "user://captures"
var phases := ["pareada"]
var rig: CityCamera
var ui: UiDriver
var blast: CaptureBlast
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		elif arg.begins_with("--phase="):
			phases = arg.trim_prefix("--phase=").split(",")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport()) # the game's own quality preset (MSAA, shadows), as game/play.gd
	print("captura: preset ", Settings.preset, ", msaa_3d ", get_viewport().msaa_3d, ", escala ", get_viewport().scaling_3d_scale)
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.step_ticks(WARM_TICKS)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	blast = CaptureBlast.new(rig, ui)
	await _run()


func _run() -> void:
	if "pareada" in phases:
		await _paired()
	if "earth" in phases:
		await blast.earth()
	if "topo" in phases:
		await blast.topdown()
	if "people" in phases:
		await _people()
	if "car" in phases:
		await _car()
	print("FALHAS: ", failures + blast.failures)
	get_tree().quit(1 if failures > 0 else 0)


## The reference game's views: a low house, force 5 and then force 45 on another, the same moments.
func _paired() -> void:
	var lows := _houses(1, FLOORS_LOW)
	if lows.size() < 3:
		_check(false, "casas baixas para a pareada")
		return
	await blast.blow("bomba_f%d" % CaptureBlast.SMALL, lows[lows.size() / 2], CaptureBlast.SMALL, LOW_DIST, PAIRED_MOMENTS)
	await blast.blow("bomba_f%d_grande" % CaptureBlast.BIG, lows[lows.size() / 3], CaptureBlast.BIG, LOW_DIST, [0.1, 0.5, 1.0, 2.0, 5.0, 17.0])


func _people() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [TARGETS])
	var walkers: Array = found.walkers
	if walkers.size() < 3:
		_check(false, "pedestres para as fotos")
		return
	await _shoot_walker("ped_tronco", walkers[0], CHEST_M, [0.1, 1.0])
	await _shoot_walker("ped_perna", walkers[1], LEG_M, [0.1, 1.0, 3.0])
	await _shoot_walker("ped_cabeca", walkers[2], HEAD_M, [0.1, 1.0, 8.0])


func _car() -> void:
	var tall := []
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.floors > FLOORS_LOW:
			tall.append(info.position)
	var cars: Array = CityStore.query(&"shot_targets", [TARGETS * 8]).vehicles.filter(func(v): return v.type != MOTO and v.type < FIRST_HEAVY and _clear_of(v.position, tall))
	if cars.is_empty():
		_check(false, "um carro para as fotos")
		return
	var car: Dictionary = cars[0]
	CityStore.dispatch(&"set_speed", [0])
	rig.jump(car.position, CaptureBlast.VIEW_YAW, WALKER_PITCH, CAR_DIST)
	await ui.drawn()
	await ui.shot("carro_antes")
	_fire(car.position + Vector3(0, GLASS_M, 0))
	await ui.frames(SETTLE_FRAMES)
	await get_tree().create_timer(0.3).timeout
	await ui.drawn()
	await ui.shot("carro_tiro")
	for k in BURST_SHOTS:
		var now := _vehicle(car.id)
		if now.is_empty():
			break
		if _fire(now.position + Vector3(0, GLASS_M, 0)).get("burst", false):
			break
	await ui.frames(SETTLE_FRAMES)
	for t in [0.0, 2.0]:
		await get_tree().create_timer(t if t > 0.0 else 0.1).timeout
		await ui.drawn()
		await ui.shot("carro_estouro_t%d" % int(t))


func _walker(id: int) -> Dictionary:
	for w in CityStore.query(&"shot_targets", [TARGETS * 4]).walkers:
		if w.id == id:
			return w
	return {}


## Nothing in `tall` within OPEN_M of the point: nothing between the camera and the target.
func _clear_of(at: Vector3, tall: Array) -> bool:
	for other in tall:
		if at.distance_to(other) < OPEN_M:
			return false
	return true


func _vehicle(id: int) -> Dictionary:
	for v in CityStore.query(&"shot_targets", [TARGETS * 4]).vehicles:
		if v.id == id:
			return v
	return {}


## A round at a point of the scene from where the camera is: the ray through it.
func _fire(at: Vector3) -> Dictionary:
	var origin := rig.camera.global_position
	return CityStore.dispatch(&"shoot", [origin, (at - origin).normalized()])


func _shoot_walker(label: String, first: Dictionary, aim_m: float, moments: Array) -> void:
	CityStore.dispatch(&"set_speed", [0]) # nobody moves while the camera comes and aims
	var w := _walker(first.id)
	rig.jump(w.position, CaptureBlast.VIEW_YAW, WALKER_PITCH, WALKER_DIST)
	await ui.drawn()
	await ui.shot("%s_antes" % label)
	var r := _fire(w.position + Vector3(0, aim_m * w.scale, 0))
	_check(r.get("hit", 0) == 1, "%s: acertou %s" % [label, r])
	CityStore.dispatch(&"set_speed", [NORMAL_SPEED]) # the clocks of the wound run: the fallen get up, the bleeding bleeds
	await ui.frames(SETTLE_FRAMES)
	var elapsed := 0.0
	for t in moments:
		await get_tree().create_timer(maxf(t - elapsed, 0.05)).timeout
		elapsed = t
		await ui.drawn()
		await ui.shot("%s_t%d" % [label, int(t * 10)])


## The houses with between `lo` and `hi` floors, as the point on their roof the blow lands on.
func _houses(lo: int, hi: int) -> Array:
	var all := []
	for id in CityStore.query(&"list_buildings"):
		all.append(CityStore.query(&"probe_building", [id]))
	var tall := all.filter(func(b): return b.floors > hi)
	var out := []
	for info in all:
		if info.floors < lo or info.floors > hi or info.zone <= 0:
			continue
		var open := true
		for other in tall:
			if info.position.distance_to(other.position) < OPEN_M:
				open = false
				break
		if open:
			out.append(info.position + Vector3(0, 3.0 * info.floors, 0))
	return out


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
