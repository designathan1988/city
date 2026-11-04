## Captures of the Landscape port, the views the 4.0 was photographed from as the player sees it (ports/landscape/
## reference-minha): the open field and its woods, a stand close up, rain, clouds and their shadows, painted fog, the
## painted ground and a river. Each scene drives the core through the store, as the tools do.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/capture_landscape.tscn -- --capture-dir=DIR
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const SETTLE_FRAMES := 240
const SHOT_FRAMES := 30
const CITY_FREE_X := 2600.0
const VIEW_LINES := 1080.0
const SHOT_SIZE := Vector2i(1920, 1080) # a window with a title bar loses 19 lines on a 1080-line screen
const CITY_ZOOM := 0.8695652 # the view the 4.0 opens on
const CITY_BEARING := 45.0
const CITY_TILT := 48.0
const RIGHT_ANGLE := 90.0
const MINUTES_AN_HOUR := 60.0
const RAMP_STEPS := 17
const RAMP_LEVELS := 16.0
const BYTE_TOP := 255.0
const RAMP_CARD_M := 6.0
const RAMP_HEIGHT_M := 80.0
const CUBE_SIZE := Vector3(40.0, 120.0, 40.0)
const CUBE_ALBEDO := 0.216
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
	await _shot("cidade", _ground((b.min.x + b.max.x) * 0.5, (b.min.z + b.max.z) * 0.5), CITY_BEARING, CITY_TILT, CITY_ZOOM)


func _ground(x: float, z: float) -> Vector3:
	return Vector3(x, CityStore.query(&"height_at", [x, z]), z)


## The 4.0's view (`__roadcraft.setView`, render/isoViewport.ts): bearing and tilt in degrees and its zoom, pixels a metre at
## 1080 lines, so a pair stands in the same frame: its bearing runs from +X towards +Z, ours from +Z towards +X.
func _shot(name: String, at: Vector3, azimuth: float, tilt: float, zoom: float) -> void:
	if not only.is_empty() and not only.has(name):
		return
	var half := VIEW_LINES / (2.0 * zoom)
	rig.jump(at, RIGHT_ANGLE - azimuth, tilt, CameraProfile.view_distance(rig.cfg, half))
	print("pose ", name, ": inclinacao ", rad_to_deg(rig.pitch), " lente ", rig.camera.fov, " meia altura ", half, " olho ", rig.camera.global_position)
	await _frames(SHOT_FRAMES)
	while CityStore.query(&"render_pending") > 0:
		await get_tree().process_frame
	await _frames(SHOT_FRAMES)
	await RenderingServer.frame_post_draw
	get_viewport().get_texture().get_image().save_png(out_dir.path_join(name + ".png"))


## The city at noon, for the light's targets (asphalt, sidewalk, lawn, wall), then the night over field and city.
func _city_light(field: Vector3) -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var mid := _ground((b.min.x + b.max.x) * 0.5, (b.min.z + b.max.z) * 0.5)
	await _shot("luz_cidade", mid, 30.0, 40.0, 2.0)
	await _shot("luz_rua", mid, 30.0, 40.0, 14.0)
	await _shot("luz_parede", mid, 120.0, 14.0, 30.0)
	await _shot("ceu_baixo", field, 45.0, -14.0, 200.0)
	await _shot("ceu_alto", field, 45.0, -30.0, 200.0)
	world.set_hour(NIGHT_HOUR)
	await _shot("ceu_noite", field, 45.0, -14.0, 200.0)
	await _shot("noite_campo", field, 45.0, 24.0, 2.0)
	await _shot("noite_cidade", mid, 30.0, 26.0, 6.0)
	await _clock(mid)


## A ramp of unlit grey cards over the field, the 4.0's radiance per step in our exposure: proves the curve and the grade
## against the same cards in the 4.0 (ports/landscape/plano-rampa.json).
func _ramp(at: Vector3) -> void:
	var day: Dictionary = DataTables.game("daylight")
	for k in RAMP_STEPS:
		var v := minf(1.0, k * RAMP_LEVELS / BYTE_TOP)
		var m := StandardMaterial3D.new()
		m.shading_mode = BaseMaterial3D.SHADING_MODE_UNSHADED
		m.no_depth_test = true
		var l: float = Color(v, v, v).srgb_to_linear().r * day.sky_gain
		m.albedo_color = Color(l, l, l).linear_to_srgb()
		var card := MeshInstance3D.new()
		var quad := PlaneMesh.new()
		quad.size = Vector2(RAMP_CARD_M, RAMP_CARD_M)
		card.mesh = quad
		card.material_override = m
		card.position = Vector3(at.x + (k - RAMP_STEPS / 2) * RAMP_CARD_M, RAMP_HEIGHT_M, at.z)
		add_child(card)
	await _shot("rampa", Vector3(at.x, 0.0, at.z), 90.0, 50.0, 6.0)


## A grey box in both games, the same albedo on every face: how much light each side gets, in the sun and out of it.
func _cube(at: Vector3) -> void:
	var box := BoxMesh.new()
	box.size = CUBE_SIZE
	var m := StandardMaterial3D.new()
	m.albedo_color = Color(CUBE_ALBEDO, CUBE_ALBEDO, CUBE_ALBEDO).linear_to_srgb()
	m.roughness = 1.0
	box.material = m
	var cube := MeshInstance3D.new()
	cube.mesh = box
	cube.position = Vector3(at.x, 0.0, at.z)
	add_child(cube)
	await _shot("cubo_a", Vector3(at.x, 0.0, at.z), 45.0, 35.0, 3.0)
	await _shot("cubo_b", Vector3(at.x, 0.0, at.z), 225.0, 35.0, 3.0)


## The light through the game's own path: the sky modes, and in "cycle" the city's clock run by the simulation's ticks.
func _clock(at: Vector3) -> void:
	world.set_sky_mode("cycle")
	var ticks_an_hour := int(MINUTES_AN_HOUR / DataTables.tuning("sim_minutes_per_second") * DataTables.tuning("sim_tick_hz"))
	for i in CLOCK_SHOTS:
		CityStore.step_ticks(ticks_an_hour * CLOCK_STEP_H)
		await _frames(SHOT_FRAMES)
		print("relogio: hora ", CityStore.snapshot.hour, " sol ", world.sun.light_energy, " noite ", world.daylight.night)
		await _shot("relogio_%d" % i, at, 30.0, 40.0, 2.0)
	for mode: String in ["day", "night"]:
		world.set_sky_mode(mode)
		await _shot("modo_" + mode, at, 30.0, 40.0, 2.0)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The cards measured alone: nothing else is drawn with them.
func _measure_card(c: Vector3) -> bool:
	if only.has("rampa"):
		await _ramp(c)
	elif only.has("cubo_a"):
		await _cube(c)
	else:
		return false
	return true


func _run() -> void:
	var c := _ground(CITY_FREE_X, 0.0)
	await _frames(SETTLE_FRAMES)
	if await _measure_card(c):
		return
	await _shot("campo_perto", c, 45.0, 4.0, 30.0)
	await _shot("campo_meio", c, 45.0, 12.0, 8.0)
	await _shot("matas_meio", c, 45.0, 24.0, 2.0)
	await _shot("matas_alto", c, 45.0, 38.0, 0.6)
	await _shot("horizonte", c, 120.0, 4.0, 20.0)
	CityStore.dispatch(&"set_weather", [{"rain": 0.8, "wind": 12.0}])
	await _shot("chuva", c, 45.0, 8.0, 5.0)
	CityStore.dispatch(&"set_weather", [{"rain": 0.0, "wind": 4.0}])
	for i in 8:
		CityStore.dispatch(&"add_cloud", [{"x": c.x + (i % 4 - 1.5) * 800.0, "z": c.z + (550.0 if i > 3 else -550.0), "height": 450.0, "size": 375.0, "density": 0.8, "yaw": float(i)}])
	await _shot("nuvens_sombra", c, 45.0, 22.0, 1.2)
	await _shot("nuvens_ceu", c, 60.0, 4.0, 2.0)
	CityStore.dispatch(&"clear_clouds")
	CityStore.dispatch(&"begin_stroke", ["cmd.fog"])
	for i in 30:
		CityStore.dispatch(&"fog_brush", [c + Vector3(i * 35.0 - 500.0, 0, sin(i / 3.0) * 100.0), 225.0, 0.5, 45.0, 10.0, false])
	CityStore.dispatch(&"end_stroke")
	await _shot("neblina", c, 45.0, 16.0, 3.0)
	CityStore.dispatch(&"clear_fog")
	var nature: Dictionary = DataTables.catalog_table("nature")
	var kinds: Array = nature.paint.kinds
	var d := _ground(CITY_FREE_X + 600.0, 600.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.tree"])
	for i in CLEAR_DABS:
		CityStore.dispatch(&"tree_brush", [d + Vector3(i * 90.0 - 450.0, 0, 0), 240.0, TREE_CUT, 0, {"density": 0.0, "height": 1.0, "variation": 0.0, "spacing": 1.0}])
	CityStore.dispatch(&"end_stroke")
	CityStore.dispatch(&"begin_stroke", ["cmd.place.paintGround"])
	for k in kinds.size():
		for n in 6:
			CityStore.dispatch(&"paint_brush", [d + Vector3((k % 7) * 110.0 - 330.0, 0, (k / 7) * 110.0 - 110.0), 45.0, 1.0, 0.0, k])
	CityStore.dispatch(&"end_stroke")
	await _shot("chao_pintado", d, 90.0, 50.0, 2.2)
	await _shot("cobertura", d + Vector3(-60.0, 0, 0), 90.0, 18.0, 10.0)
	var e := d + Vector3(0, 0, -300.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.element"])
	for k in ELEMENT_KINDS:
		var at := e + Vector3((k % 6) * 22.0 - 55.0, 0, (k / 6) * 22.0 - 22.0)
		CityStore.dispatch(&"element_brush", [at, 9.0, k, {"density": 30.0, "size": 1.0, "variation": 0.4, "spacing": 0.4, "strength": 1.0, "intensity": 0.8}, false])
	CityStore.dispatch(&"end_stroke")
	await _shot("elementos", e, 90.0, 22.0, 24.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.tree"])
	for i in 12:
		CityStore.dispatch(&"tree_brush", [d + Vector3(i * 30.0 - 150.0, 0, 260.0), 40.0, 0, i % 4, {"density": 200.0, "height": 14.0, "variation": 0.3, "spacing": 6.0}])
	CityStore.dispatch(&"end_stroke")
	await _shot("plantadas", d + Vector3(0, 0, 260.0), 90.0, 14.0, 10.0)
	var r := _ground(CITY_FREE_X, -900.0)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.terrainBrush"])
	for i in RIVER_DABS:
		CityStore.dispatch(&"terrain_brush", [r + Vector3(i * RIVER_STEP_M - 600.0, 0, sin(i / 12.0) * 150.0), 125.0, 1.0, 0.1, 8, 0.0])
	CityStore.dispatch(&"end_stroke")
	await _shot("rio", r, 30.0, 24.0, 3.0)
	await _city_light(c)
