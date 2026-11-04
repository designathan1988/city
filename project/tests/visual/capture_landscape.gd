## Captures of the Landscape port, the views the 4.0 was photographed from as the player sees it (ports/landscape/
## reference-minha): the open field and its woods, a stand close up, rain, clouds and their shadows, painted fog, the
## painted ground and a river. Each scene drives the core through the store, as the tools do.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_landscape.tscn -- --capture-dir=DIR
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const SETTLE_FRAMES := 240
const SHOT_FRAMES := 30
const CITY_FREE_X := 2600.0

var out_dir := "user://captures"
var rig: CityCamera
var world: CityWorld


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
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
	world.set_hour(12.0)
	await _run()
	print("capture_landscape: pronto")
	get_tree().quit(0)


func _ground(x: float, z: float) -> Vector3:
	return Vector3(x, CityStore.query(&"height_at", [x, z]), z)


func _shot(name: String, at: Vector3, yaw: float, pitch: float, dist: float) -> void:
	rig.jump(at, yaw, pitch, dist)
	await _frames(SHOT_FRAMES)
	while CityStore.query(&"render_pending") > 0:
		await get_tree().process_frame
	await _frames(SHOT_FRAMES)
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png(out_dir.path_join(name + ".png"))


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _run() -> void:
	var c := _ground(CITY_FREE_X, 0.0)
	await _frames(SETTLE_FRAMES)
	await _shot("campo_perto", c, 45.0, 4.0, 60.0)
	await _shot("campo_meio", c, 45.0, 12.0, 220.0)
	await _shot("matas_meio", c, 45.0, 24.0, 900.0)
	await _shot("matas_alto", c, 45.0, 38.0, 3200.0)
	await _shot("horizonte", c, 120.0, 3.0, 300.0)
	CityStore.dispatch(&"set_weather", [{"rain": 0.8, "wind": 12.0}])
	await _shot("chuva", c, 45.0, 8.0, 320.0)
	CityStore.dispatch(&"set_weather", [{"rain": 0.0, "wind": 4.0}])
	for i in 8:
		CityStore.dispatch(&"add_cloud", [{"x": c.x + (i % 4 - 1.5) * 320.0, "z": c.z + (220.0 if i > 3 else -220.0), "height": 260.0, "size": 260.0, "density": 0.85, "yaw": float(i)}])
	await _shot("nuvens_sombra", c, 45.0, 22.0, 1400.0)
	await _shot("nuvens_ceu", c, 60.0, 4.0, 900.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.fog"])
	for i in 30:
		CityStore.dispatch(&"fog_brush", [c + Vector3(i * 14.0 - 200.0, 0, sin(i / 3.0) * 40.0), 90.0, 0.5, 18.0, 4.0, false])
	CityStore.dispatch(&"end_stroke")
	await _shot("neblina", c, 45.0, 16.0, 500.0)
	CityStore.dispatch(&"clear_fog")
	CityStore.dispatch(&"clear_clouds")
	var nature: Dictionary = DataTables.catalog_table("nature")
	var kinds: Array = nature.paint.kinds
	var d := _ground(CITY_FREE_X + 600.0, 600.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.paintGround"])
	for k in kinds.size():
		for n in 6:
			CityStore.dispatch(&"paint_brush", [d + Vector3((k % 7) * 110.0 - 330.0, 0, (k / 7) * 110.0 - 110.0), 45.0, 1.0, 0.0, k])
	CityStore.dispatch(&"end_stroke")
	await _shot("chao_pintado", d, 0.0, 50.0, 700.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.tree"])
	for i in 12:
		CityStore.dispatch(&"tree_brush", [d + Vector3(i * 30.0 - 150.0, 0, 260.0), 40.0, 0, i % 4, {"density": 200.0, "height": 14.0, "variation": 0.3, "spacing": 6.0}])
	CityStore.dispatch(&"end_stroke")
	await _shot("plantadas", d + Vector3(0, 0, 260.0), 0.0, 14.0, 160.0)
	var r := _ground(CITY_FREE_X, -900.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
	for i in 20:
		CityStore.dispatch(&"terrain_brush", [r + Vector3(i * 24.0 - 240.0, 0, sin(i / 4.0) * 60.0), 50.0, 1.0, 0.1, 8, 0.0])
	CityStore.dispatch(&"end_stroke")
	await _shot("rio", r, 30.0, 24.0, 420.0)
