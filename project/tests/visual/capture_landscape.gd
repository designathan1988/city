## Captures of the Landscape port, the views the 4.0 was photographed from as the player sees it (ports/landscape/
## reference-minha): the open field and its woods, a stand close up, rain, clouds and their shadows, painted fog, the
## painted ground and a river. Each scene drives the core through the store, as the tools do.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/capture_landscape.tscn -- --capture-dir=DIR
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const SETTLE_FRAMES := 240
const SHOT_FRAMES := 30
const GPU_STEP_MS := 0.01
const FIELD_AT := Vector2(3000.0, 0.0) # the 4.0 runs' setView x and -y: the same ground in both games
const PAINT_AT := Vector2(4500.0, -1500.0)
const RIVER_AT := Vector2(3000.0, 2250.0)
const BARE_AT := Vector2(3160.0, -720.0)
const VIEW_LINES := 1080.0
const SHOT_SIZE := Vector2i(1920, 1080) # a window with a title bar loses 19 lines on a 1080-line screen
const CITY_ZOOM := 0.8695652 # the view the 4.0 opens on
const CITY_BEARING := 45.0
const CITY_TILT := 48.0
const RIGHT_ANGLE := 90.0
const MINUTES_AN_HOUR := 60.0
const CLOCK_SHOTS := 4
const CLOCK_STEP_H := 5
const CLEAR_DABS := 11
const NIGHT_HOUR := 22.0
const ELEMENT_KINDS := 18
const TREE_CUT := 2
const RIVER_DABS := 60
const RIVER_STEP_M := 20.0 # the dabs of a stroke (brush_tool.gd): a share of the radius apart

var out_dir := "user://captures"
var only: PackedStringArray = [] # --only=a,b: just these views
var hour: float = DataTables.game("daylight").day_hour # --hour=h: the sun of that hour, the 4.0's "always day" by default
var preset := "" # --preset=high: the graphics of that preset, as the game applies them
var rig: CityCamera
var world: CityWorld


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		if arg.begins_with("--hour="):
			hour = arg.trim_prefix("--hour=").to_float()
		if arg.begins_with("--preset="):
			preset = arg.trim_prefix("--preset=")
		if arg.begins_with("--only="):
			only = arg.trim_prefix("--only=").split(",")
	DirAccess.make_dir_recursive_absolute(out_dir)
	get_window().borderless = true
	get_window().size = SHOT_SIZE
	world = CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	RenderingServer.viewport_set_measure_render_time(get_viewport().get_viewport_rid(), true)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	world.set_hour(hour)
	if not preset.is_empty():
		Settings.preset = preset
		Settings.apply(world, get_viewport())
	print("janela ", get_window().size, " preset ", preset)
	await _whole_city()
	await _run()
	print("capture_landscape: pronto")
	get_tree().quit(0)


## The whole town from the view the 4.0 opens on.
func _whole_city() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	await shot("cidade", _ground((b.min.x + b.max.x) * 0.5, (b.min.z + b.max.z) * 0.5), CITY_BEARING, CITY_TILT, CITY_ZOOM)


func _ground(x: float, z: float) -> Vector3:
	return Vector3(x, CityStore.query(&"height_at", [x, z]), z)


## The 4.0's view (`__roadcraft.setView`, render/isoViewport.ts): bearing and tilt in degrees and its zoom, pixels a metre at
## 1080 lines, so a pair stands in the same frame: its bearing runs from +X towards +Z, ours from +Z towards +X.
func shot(name: String, at: Vector3, azimuth: float, tilt: float, zoom: float) -> void:
	if not only.is_empty() and not only.has(name):
		return
	var half := VIEW_LINES / (2.0 * zoom)
	rig.jump(at, RIGHT_ANGLE - azimuth, tilt, CameraProfile.view_distance(rig.cfg, half))
	print("pose ", name, ": inclinacao ", rad_to_deg(rig.pitch), " lente ", rig.camera.fov, " meia altura ", half, " olho ", rig.camera.global_position)
	await _frames(SHOT_FRAMES)
	while CityStore.query(&"render_pending") > 0:
		await get_tree().process_frame
	var gpu_ms := 0.0
	for i in SHOT_FRAMES:
		await get_tree().process_frame
		gpu_ms += RenderingServer.viewport_get_measured_render_time_gpu(get_viewport().get_viewport_rid())
	print("gpu ", name, " ", snappedf(gpu_ms / SHOT_FRAMES, GPU_STEP_MS), " ms")
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png(out_dir.path_join(name + ".png"))


## The city at noon, for the light's targets (asphalt, sidewalk, lawn, wall), then the night over field and city.
func _city_light(field: Vector3) -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var mid := _ground((b.min.x + b.max.x) * 0.5, (b.min.z + b.max.z) * 0.5)
	await shot("luz_cidade", mid, 30.0, 40.0, 2.0)
	await shot("luz_rua", mid, 30.0, 40.0, 14.0)
	await shot("luz_parede", mid, 120.0, 14.0, 30.0)
	await shot("ceu_baixo", field, 45.0, -14.0, 200.0)
	await shot("ceu_alto", field, 45.0, -30.0, 200.0)
	world.set_hour(NIGHT_HOUR)
	await shot("ceu_noite", field, 45.0, -14.0, 200.0)
	await shot("noite_campo", field, 45.0, 24.0, 2.0)
	await shot("noite_cidade", mid, 30.0, 26.0, 6.0)
	await _clock(mid)


## The light through the game's own path: the sky modes, and in "cycle" the city's clock run by the simulation's ticks.
func _clock(at: Vector3) -> void:
	world.set_sky_mode("cycle")
	var ticks_an_hour := int(MINUTES_AN_HOUR / DataTables.tuning("sim_minutes_per_second") * DataTables.tuning("sim_tick_hz"))
	for i in CLOCK_SHOTS:
		CityStore.step_ticks(ticks_an_hour * CLOCK_STEP_H)
		await _frames(SHOT_FRAMES)
		print("relogio: hora ", CityStore.snapshot.hour, " sol ", world.sun.light_energy, " noite ", world.daylight.night)
		await shot("relogio_%d" % i, at, 30.0, 40.0, 2.0)
	for mode: String in ["day", "night"]:
		world.set_sky_mode(mode)
		await shot("modo_" + mode, at, 30.0, 40.0, 2.0)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The cards measured alone: nothing else is drawn with them.
func _measure_card(c: Vector3) -> bool:
	if only.has("rampa"):
		await LandscapeCards.ramp(self, c)
	elif only.has("cubo_a"):
		await LandscapeCards.cube(self, c)
	elif only.has("trama_3_10"):
		await LandscapeCards.weave(self, c)
	elif only.has("solta_palma"):
		await LandscapeCards.alone(self, _ground(BARE_AT.x, BARE_AT.y))
	elif only.has("arv_perto"):
		await LandscapeCards.trees(self, c)
	else:
		return false
	return true


func _run() -> void:
	var c := _ground(FIELD_AT.x, FIELD_AT.y)
	await _frames(SETTLE_FRAMES)
	if await _measure_card(c):
		return
	await shot("campo_perto", c, 45.0, 4.0, 30.0)
	await shot("campo_meio", c, 45.0, 12.0, 8.0)
	await shot("matas_meio", c, 45.0, 24.0, 2.0)
	await shot("matas_alto", c, 45.0, 38.0, 0.6)
	await shot("horizonte", c, 120.0, 4.0, 20.0)
	CityStore.dispatch(&"set_weather", [{"rain": 0.8, "wind": 12.0}])
	await shot("chuva", c, 45.0, 8.0, 5.0)
	CityStore.dispatch(&"set_weather", [{"rain": 0.0, "wind": 4.0}])
	for i in 8:
		CityStore.dispatch(&"add_cloud", [{"x": c.x + (i % 4 - 1.5) * 800.0, "z": c.z + (550.0 if i > 3 else -550.0), "height": 450.0, "size": 375.0, "density": 0.8, "yaw": float(i)}])
	await shot("nuvens_sombra", c, 45.0, 22.0, 1.2)
	await shot("nuvens_ceu", c, 60.0, 4.0, 2.0)
	CityStore.dispatch(&"clear_clouds")
	CityStore.dispatch(&"begin_stroke", ["cmd.fog"])
	for i in 30:
		CityStore.dispatch(&"fog_brush", [c + Vector3(i * 35.0 - 500.0, 0, sin(i / 3.0) * 100.0), 225.0, 0.5, 45.0, 10.0, false])
	CityStore.dispatch(&"end_stroke")
	await shot("neblina", c, 45.0, 16.0, 3.0)
	CityStore.dispatch(&"clear_fog")
	var nature: Dictionary = DataTables.catalog_table("nature")
	var kinds: Array = nature.paint.kinds
	var d := _ground(PAINT_AT.x, PAINT_AT.y)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.tree"])
	for i in CLEAR_DABS:
		CityStore.dispatch(&"tree_brush", [d + Vector3(i * 90.0 - 450.0, 0, 0), 240.0, TREE_CUT, 0, {"density": 0.0, "height": 1.0, "variation": 0.0, "spacing": 1.0}])
	CityStore.dispatch(&"end_stroke")
	CityStore.dispatch(&"begin_stroke", ["cmd.place.paintGround"])
	for k in kinds.size():
		for n in 6:
			CityStore.dispatch(&"paint_brush", [d + Vector3((k % 7) * 110.0 - 330.0, 0, (k / 7) * 110.0 - 110.0), 45.0, 1.0, 0.0, k])
	CityStore.dispatch(&"end_stroke")
	await shot("chao_pintado", d, 90.0, 50.0, 2.2)
	await shot("cobertura", d + Vector3(-60.0, 0, 0), 90.0, 18.0, 10.0)
	var e := d + Vector3(0, 0, -300.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.element"])
	for k in ELEMENT_KINDS:
		var at := e + Vector3((k % 6) * 22.0 - 55.0, 0, (k / 6) * 22.0 - 22.0)
		CityStore.dispatch(&"element_brush", [at, 9.0, k, {"density": 30.0, "size": 1.0, "variation": 0.4, "spacing": 0.4, "strength": 1.0, "intensity": 0.8}, false])
	CityStore.dispatch(&"end_stroke")
	await shot("elementos", e, 90.0, 22.0, 24.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.tree"])
	for i in 12:
		CityStore.dispatch(&"tree_brush", [d + Vector3(i * 30.0 - 150.0, 0, 260.0), 40.0, 0, i % 4, {"density": 200.0, "height": 14.0, "variation": 0.3, "spacing": 6.0}])
	CityStore.dispatch(&"end_stroke")
	await shot("plantadas", d + Vector3(0, 0, 260.0), 90.0, 14.0, 10.0)
	var r := _ground(RIVER_AT.x, RIVER_AT.y)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
	for i in RIVER_DABS:
		CityStore.dispatch(&"terrain_brush", [r + Vector3(i * RIVER_STEP_M - 600.0, 0, -sin(i / 12.0) * 150.0), 125.0, 1.0, 0.1, 8, 0.0])
	CityStore.dispatch(&"end_stroke")
	await shot("rio", r, 30.0, 24.0, 3.0)
	await _city_light(c)
