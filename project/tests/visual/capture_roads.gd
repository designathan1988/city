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
const CLOSE_M := 90.0
const NEAR_M := 42.0
const WIDE_M := 520.0
const STROKE_STEPS := 40
const STROKE_SWING_M := 34.0

var out_dir := "user://captures"
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
	await _photo("cross_urban_top", at(0, -120), TOP, CLOSE_M)
	await _photo("cross_urban_low", at(0, -120), LOW, NEAR_M)
	await _photo("cross_avenue_top", at(-60, 100), TOP, CLOSE_M)
	await _photo("cross_avenue_low", at(-60, 100), LOW, NEAR_M)
	await _photo("t_top", at(90, 100), TOP, CLOSE_M)
	await _photo("road_end", at(0, -40), [200.0, 55.0], NEAR_M * 1.5)
	await _photo("curve_top", at(280, -80), [0.0, 70.0], WIDE_M * 0.5)
	await _photo("layout_top", at(40, 0), [0.0, 70.0], WIDE_M)
	await _tool()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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


func _photo(name: String, p: Vector3, view: Array, dist: float) -> void:
	rig.jump(p, view[0], view[1], dist)
	await ui.drawn()
	await ui.shot(name)


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
	hud.drawer.close()


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
