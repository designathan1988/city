## The game's day sun is fixed at 16:00, as the 4.0's day mode (renderer.ts): both photos stand under the same sun.
## Captures of the roads up close, on open ground where nothing stands in front of them: the same crossings, T junction,
## road end and curve the Roadcraft 4.0 photos show, then the road tool in use with real mouse events. Works on a build
## without the road paths too (no px argument, no freehand), so one script takes the "before" and the "after".
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_roads.tscn -- --no-flow --capture-dir=/tmp/x
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const PX := 0.25
const OPEN_GROUND_M := 420.0
const TOP := [20.0, 62.0]
const LOW := [30.0, 24.0]
const PAIR_LINES := 900.0 # the 4.0's zoom is pixels a metre, on the lines of its photos
const RIGHT_ANGLE := 90.0
const CLOSE_M := 90.0
const NEAR_M := 42.0
const WIDE_M := 520.0
const NODE_VIEW_M := 200.0
const DRAG_STEPS := 6
const STROKE_STEPS := 40
const STROKE_SWING_M := 34.0
const POINTER_AIMS := 40
const POINTER_VIEWS_M: Array[float] = [NEAR_M, CLOSE_M, WIDE_M]

var out_dir := "user://captures"
var world: CityWorld
var only := PackedStringArray()
var rig: CityCamera
var tools: ToolController
var hud: RoadcraftHud
var failures := 0
var ui: UiDriver
var o := Vector3.ZERO
var profiles := {}


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		if arg.begins_with("--only="):
			only = arg.trim_prefix("--only=").split(",")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport()) # as the player sees it: the viewport's anti-aliasing comes from here
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
	tools = ToolController.new(rig)
	add_child(tools)
	hud = RoadcraftHud.new()
	add_child(hud)
	for p in CityStore.query(&"road_profiles"):
		profiles[p.id] = p.index
	await _run()


func _run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	o = ui.ground(b.max.x + OPEN_GROUND_M, (b.min.z + b.max.z) * 0.5)
	_lay()
	await _pointer_error()
	if "--tool-only" in OS.get_cmdline_user_args():
		await _tool()
		print("FALHAS: ", failures)
		get_tree().quit(1 if failures > 0 else 0)
		return
	await _photo("cross_urban_top", at(0, -120), TOP, CLOSE_M)
	await _photo("cross_urban_low", at(0, -120), LOW, NEAR_M)
	await _photo("cross_avenue_top", at(-60, 100), TOP, CLOSE_M)
	await _photo("cross_avenue_low", at(-60, 100), LOW, NEAR_M)
	await _photo("t_top", at(90, 100), TOP, CLOSE_M)
	await _photo("road_end", at(0, -40), [200.0, 55.0], NEAR_M * 1.5)
	await _photo("curve_top", at(280, -80), [0.0, 70.0], WIDE_M * 0.5)
	await _photo("layout_top", at(40, 0), [0.0, 70.0], WIDE_M)
	await _pair("pair_avenue_top", at(-60, 100), 20.0, 62.0, 10.0)
	await _pair("pair_avenue_low", at(-60, 100), 30.0, 24.0, 18.0)
	await _pair("pair_urban_top", at(0, -120), 20.0, 62.0, 14.0)
	await _pair("pair_t_top", at(90, 100), 20.0, 62.0, 12.0)
	await _pair("pair_road_end", at(0, -20), 200.0, 55.0, 16.0)
	await _pair("close_kerb_low", at(-26, 93), 30.0, 30.0, 60.0)
	await _pair("close_kerb_top", at(-26, 93), 30.0, 70.0, 60.0)
	await _pair("line_far", at(-20, 100), 20.0, 62.0, 10.0)
	await _pair("line_near", at(-20, 100), 20.0, 62.0, 40.0)
	await _pair("median_top", at(480, 60), 20.0, 62.0, 14.0)
	await _pair("median_low", at(480, 60), 30.0, 30.0, 30.0)
	await _pair("median_nose_top", at(480, -60), 20.0, 62.0, 14.0)
	await _pair("median_nose_low", at(480, -60), 30.0, 30.0, 30.0)
	await _pair("median_paved_top", at(640, 0), 20.0, 62.0, 14.0)
	await _pair("median_paved_low", at(640, 0), 30.0, 30.0, 30.0)
	await _photo("bend_top", at(0, 300), [0.0, 62.0], NEAR_M * 2.0)
	await _photo("t_close", at(90, 100), [0.0, 90.0], CLOSE_M)
	await _night()
	if only.is_empty():
		await _tool()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## Where a click lands against where it aimed, in pixels, at three distances of the camera: the pixel grid and the ray to the
## ground, all a test can see of the pointer's error. The player's hand is not in this number.
func _pointer_error() -> void:
	var worst := 0.0
	var size: Vector2 = get_viewport().get_visible_rect().size
	for dist in POINTER_VIEWS_M:
		rig.jump(at(0, -120), TOP[0], TOP[1], dist)
		await ui.frames(2)
		for i in POINTER_AIMS:
			var aim: Variant = rig.ground_at(size * Vector2(fposmod(i * 0.381966, 1.0), fposmod(i * 0.618034, 1.0)))
			if aim == null:
				continue
			var seen := rig.camera.unproject_position(aim)
			var hit: Variant = rig.ground_at(seen.round())
			if hit != null:
				worst = maxf(worst, rig.camera.unproject_position(hit).distance_to(seen))
	print("erro do ponteiro: %.2f px" % worst)


## The same crossings under the game's night, then the day again for the tool.
func _night() -> void:
	var sky: Dictionary = DataTables.game("daylight")
	world.set_hour(sky.night_hour)
	await _photo("night_urban_top", at(0, -120), TOP, CLOSE_M)
	await _photo("night_urban_low", at(0, -120), LOW, NEAR_M)
	await _photo("night_avenue_low", at(-60, 100), LOW, NEAR_M)
	world.set_hour(sky.day_hour)


func at(x: float, z: float) -> Vector3:
	return ui.ground(o.x + x, o.z + z)


func _road(a: Vector3, e: Vector3, profile: String, control: Variant = null) -> void:
	var args := [a, e, control != null, control if control != null else Vector3.ZERO, profiles[profile]]
	if CityStore.COMMANDS.has(&"build_road_path"):
		args.push_back(PX)
	var r: Dictionary = CityStore.dispatch(&"build_road", args)
	_check(r.get("ok", false), "via %s: %s" % [profile, r])


func _lay() -> void:
	_road(at(-160, -120), at(160, -120), "urbanStreet")
	_road(at(0, -220), at(0, -20), "urbanStreet")
	_road(at(-200, 100), at(200, 100), "avenue")
	_road(at(-60, 30), at(-60, 170), "avenue")
	_road(at(90, 30), at(90, 100), "urbanStreet")
	_road(at(230, -190), at(350, 40), "urbanStreet", at(360, -180))
	_road(at(-200, 300), at(0, 300), "urbanStreet")
	_road(at(0, 300), at(190, 360), "urbanStreet")
	_road(at(480, -200), at(480, 200), "streetWithMedian")
	_road(at(420, -60), at(540, -60), "localStreet")
	_road(at(640, -200), at(640, 200), "highway6Lanes")


func _photo(name: String, p: Vector3, view: Array, dist: float) -> void:
	if not only.is_empty() and not only.has(name):
		return
	rig.jump(p, view[0], view[1], dist)
	await ui.drawn()
	await ui.shot(name)


## The camera of a 4.0 photo (`__roadcraft.setView`): its bearing runs from +X towards +Z, ours from +Z towards +X.
func _pair(name: String, p: Vector3, azimuth: float, tilt: float, zoom: float) -> void:
	await _photo(name, p, [RIGHT_ANGLE - azimuth, tilt], CameraProfile.view_distance(rig.cfg, PAIR_LINES / (2.0 * zoom)))


## The tool as the player uses it: the drawer, a stretch held by a click, a curve waiting for its bend, a freehand stroke.
func _tool() -> void:
	rig.jump(at(40, -320), 0.0, 70.0, WIDE_M)
	await ui.drawn()
	hud.open_category("roads")
	await ui.frames(4)
	await ui.shot("tool_drawer")
	await ui.click(at(-120, -380))
	await ui.hover(at(120, -380))
	await ui.shot("tool_chain")
	tools.active.cancel()
	var street: int = profiles["urbanStreet"]
	tools.set_tool(ToolController.Tool.ROAD, {"profile": street, "alignment": 1, "curve": true})
	await ui.click(at(-120, -300))
	await ui.click(at(120, -300))
	await ui.hover(at(0, -420))
	await ui.shot("tool_curve_bend")
	tools.active.cancel()
	if CityStore.COMMANDS.has(&"build_road_path"):
		await _freehand(street)
	if ToolController.Tool.has("MOVE"):
		await _node_tools()
	hud.drawer.close()


## The Controle and Mover tabs on the urban crossing.
func _node_tools() -> void:
	var junction := at(0, -120)
	rig.jump(junction, 0.0, 70.0, NODE_VIEW_M)
	await ui.drawn()
	tools.set_tool(ToolController.Tool.CONTROL, {})
	await ui.hover(junction)
	await ui.shot("tool_control")
	await ui.click(junction)
	await ui.hover(at(60, -60))
	await ui.shot("tool_control_signal")
	tools.set_tool(ToolController.Tool.MOVE, {})
	await ui.hover(junction)
	await ui.button(rig.camera.unproject_position(junction), true)
	for i in range(1, DRAG_STEPS + 1):
		await ui.hover(junction.lerp(at(30, -100), float(i) / DRAG_STEPS))
	await ui.shot("tool_move")
	await ui.button(rig.camera.unproject_position(at(30, -100)), false)
	await ui.drawn()
	await ui.shot("tool_moved")


func _freehand(street: int) -> void:
	tools.set_tool(ToolController.Tool.ROAD, {"profile": street, "alignment": 2})
	var s0: int = CityStore.snapshot.segments
	var start := at(-180, -470)
	await ui.hover(start)
	await ui.button(rig.camera.unproject_position(start), true)
	var end := start
	for i in range(1, STROKE_STEPS + 1):
		end = at(-180 + i * 9.0, -470 + STROKE_SWING_M * sin(i * 0.3))
		await ui.hover(end)
	await ui.shot("tool_free_stroke")
	await ui.button(rig.camera.unproject_position(end), false)
	await ui.drawn()
	_check(CityStore.snapshot.segments > s0, "traço à mão assentado (%d -> %d)" % [s0, CityStore.snapshot.segments])
	await ui.shot("tool_free_laid")


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
