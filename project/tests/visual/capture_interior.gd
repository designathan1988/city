## Captures of the Interior layer, driven as the player does it (the layer's switch through the interior controller, the floor
## buttons' own command): each shot names a building of the small city (seed 7: the ids the 4.0's photos name), the floor, the way the
## camera looks relative to the lot's front and the scale on the ground; the picture is saved when the chunks the cut asked for have landed.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_interior.tscn -- --no-flow --capture-dir=<folder>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
## Building id, floor, turn from the lot's front (degrees), pixels a metre at the focus, tilt (degrees), hour, name.
const SHOTS := [
	[146, 0, 0.0, 38.0, 75.0, 12.0, "casa-terreo"],
	[146, 1, 90.0, 38.0, 75.0, 12.0, "casa-andar1-giro90"],
	[434, 0, 0.0, 30.0, 70.0, 12.0, "loja-terreo"],
	[42, 2, 0.0, 20.0, 65.0, 12.0, "escritorio-andar2"],
	[146, 0, 0.0, 38.0, 75.0, 22.0, "casa-noite-terreo"],
]
const SETTLE_FRAMES := 60
const WAIT_LIMIT_S := 240.0
const CENTRE_PIXELS := 450.0

var out_dir := "user://captures"
var rig: CityCamera
var world: CityWorld
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	add_child(ToolController.new(rig))
	var hud := RoadcraftHud.new()
	add_child(hud)
	var layer := InteriorController.new()
	add_child(layer)
	await _frames(SETTLE_FRAMES)
	for s in SHOTS:
		await _shot(layer, s)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _shot(layer: InteriorController, s: Array) -> void:
	var info: Dictionary = CityStore.query(&"probe_building", [s[0]])
	if info.is_empty():
		failures += 1
		print("FAIL: no building ", s[0])
		return
	CityStore.dispatch(&"set_day_hour", [s[5]])
	world.set_hour(s[5]) # the sky follows the clock in the game (the HUD's hour control); here the scene does it
	var yaw: float = rad_to_deg(float(info.rot)) + float(s[2])
	rig.jump(info.focus, yaw, s[4], _distance_for(info.focus, yaw, s[4], s[3]))
	await _frames(SETTLE_FRAMES)
	layer.set_enabled(false)
	CityStore.dispatch(&"set_interior", [true, s[0], s[1], layer._direction_step(), Vector3.ZERO, 0.0])
	layer.call("_send", true, s[0], s[1])
	await _settled()
	get_viewport().get_texture().get_image().save_png("%s/%s.png" % [out_dir, s[6]])
	print("shot ", s[6], " ", CityStore.query(&"interior").level, "/", CityStore.query(&"interior").floors)


## The camera's distance that gives `px` pixels a metre at the focus: the lens depends on the distance, so it is searched.
func _distance_for(at: Vector3, yaw: float, pitch: float, px: float) -> float:
	var dist := CENTRE_PIXELS / px
	for i in 8:
		rig.jump(at, yaw, pitch, dist)
		dist *= rig.px_per_m() / px
	return dist


func _settled() -> void:
	var t0 := Time.get_ticks_msec()
	await _frames(SETTLE_FRAMES)
	while CityStore.render_pending() > 0 and (Time.get_ticks_msec() - t0) / 1000.0 < WAIT_LIMIT_S:
		await _frames(SETTLE_FRAMES)
	await _frames(SETTLE_FRAMES)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
