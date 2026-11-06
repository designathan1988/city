## Captures of the Interior layer, driven as the player does it (the layer's switch through the interior controller, the floor
## buttons' own command): each shot names a building of the game's town map (map 0, seed 7: its own ids; the 4.0's city is another one), the floor, the way the
## camera looks relative to the lot's front and the scale on the ground; the picture is saved when the chunks the cut asked for have landed.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_interior.tscn -- --no-flow --capture-dir=<folder>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
## Building id, floor, turn from the lot's front (degrees), pixels a metre at the focus, tilt (degrees), hour, name, and "toggle" for
## the layer's switch (the building in the middle of the view) in place of the floor buttons' command.
const SHOTS := [
	[146, 0, 0.0, 38.0, 75.0, 12.0, "casa146-terreo"],
	[146, 1, 90.0, 38.0, 75.0, 12.0, "casa146-andar1-giro90"],
	[695, 10, 0.0, 20.0, 65.0, 12.0, "torre695-andar10"],
	[1090, 1, 0.0, 30.0, 70.0, 12.0, "comercial1090-andar1"],
	[146, 0, 0.0, 38.0, 75.0, 22.0, "casa146-noite-terreo"],
	[146, 0, 0.0, 8.0, 60.0, 12.0, "cidade-camada-146", "toggle"],
]
const SETTLE_FRAMES := 60
const WAIT_LIMIT_S := 60.0
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
	print("FAILURES: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _shot(layer: InteriorController, s: Array) -> void:
	var info: Dictionary = CityStore.query(&"probe_building", [s[0]])
	if info.is_empty():
		failures += 1
		print("FAIL: no building ", s[0])
		return
	UiDriver.at_hour(world, s[5])
	var yaw: float = rad_to_deg(float(info.rot)) + float(s[2])
	rig.jump(info.focus, yaw, s[4], _distance_for(info.focus, yaw, s[4], s[3]))
	await _frames(SETTLE_FRAMES)
	layer.set_enabled(false)
	if s.size() > 7 and s[7] == "toggle":
		if not layer.set_enabled(true):
			failures += 1
			print("FAIL: the switch found no building in the middle of the view for ", s[6])
		rig.jump(info.focus, yaw, s[4], _distance_for(info.focus, yaw, s[4], s[3])) # the switch frames the building: this shot is the city
	else:
		layer.call("_send", true, s[0], s[1])
		_aim_at_floor(info.focus, yaw, s[4], s[3])
	await _settled()
	get_viewport().get_texture().get_image().save_png("%s/%s.png" % [out_dir, s[6]])
	print("shot ", s[6], " ", CityStore.query(&"interior").level, "/", CityStore.query(&"interior").floors)


## The rig stands on the ground: the floor open high in a tower is framed by the point of the ground behind it on the line of sight.
func _aim_at_floor(at: Vector3, yaw: float, pitch: float, px: float) -> void:
	var lamps: Array = CityStore.query(&"interior").lamps
	if lamps.is_empty():
		return
	var ahead := -rig.camera.global_transform.basis.z
	ahead.y = 0.0
	var ground: Vector3 = at + ahead.normalized() * ((lamps[0].y - at.y) / tan(deg_to_rad(pitch)))
	rig.jump(ground, yaw, pitch, _distance_for(ground, yaw, pitch, px))


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
	while int(CityStore.query(&"render_pending")) > 0 and (Time.get_ticks_msec() - t0) / 1000.0 < WAIT_LIMIT_S:
		await _frames(SETTLE_FRAMES)
	await _frames(SETTLE_FRAMES)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
