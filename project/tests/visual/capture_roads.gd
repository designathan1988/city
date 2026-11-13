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
const PAIR_HOUR := 16.0
const CLOSE_M := 90.0
const NEAR_M := 42.0
const WIDE_M := 520.0
const NODE_VIEW_M := 200.0
const DRAG_STEPS := 6
const JUMP_SETTLE_FRAMES := 4
const STROKE_STEPS := 40
const STROKE_SWING_M := 34.0
const POINTER_AIMS := 40
const POINTER_VIEWS_M: Array[float] = [NEAR_M, CLOSE_M, WIDE_M]
const WEAR_UPLOAD_S := 3.0
const FURNITURE_LAMP := 1 # furniture_types.h FurnitureKind
const FURNITURE_COLUMN := 6

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
	PairCamera.quality(world, get_viewport())
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true, true)
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
	CityStore.dispatch(&"clear_trees") # the 4.0's free ground is a bare field
	CityStore.dispatch(&"set_day_hour", [PAIR_HOUR])
	_lay()
	for probe: Vector3 in [at(-60, 60), at(-60, 140), at(-150, 100), at(150, 100)]:
		_spots(probe)
	print("pointer error: %.2f px" % await ui.pointer_error(at(0, -120), TOP, POINTER_VIEWS_M, POINTER_AIMS))
	if "--tool-only" in OS.get_cmdline_user_args():
		await CaptureRoadTools.new(self).run()
		print("FAILURES: ", failures)
		get_tree().quit(CaptureWait.exit_code(failures))
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
	await _worn(at(480, -60))
	await _photo("bend_top", at(0, 300), [0.0, 62.0], NEAR_M * 2.0)
	await _photo("t_close", at(90, 100), [0.0, 90.0], CLOSE_M)
	await _night()
	if only.is_empty():
		await CaptureRoadTools.new(self).run()
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


## The same crossings under the game's night, then the day again for the tool.
func _night() -> void:
	var sky: Dictionary = DataTables.game("daylight")
	UiDriver.at_hour(world, sky.night_hour)
	await _photo("night_urban_top", at(0, -120), TOP, CLOSE_M)
	await _photo("night_urban_low", at(0, -120), LOW, NEAR_M)
	await _photo("night_avenue_low", at(-60, 100), LOW, NEAR_M)
	UiDriver.at_hour(world, sky.day_hour)


func at(x: float, z: float) -> Vector3:
	return ui.ground(o.x + x, o.z + z)


func _road(a: Vector3, e: Vector3, profile: String, control: Variant = null) -> void:
	var args := [a, e, control != null, control if control != null else Vector3.ZERO, profiles[profile]]
	if StoreApi.COMMANDS.has(&"build_road_path"):
		args.push_back(PX)
	var r: Dictionary = CityStore.dispatch(&"build_road", args)
	_check(r.get("ok", false), "road %s: %s" % [profile, r])


## The poles of the street under `p` (lamps and columns), as the log reads them beside the photos: where the furniture
## stands on it (u across from its centre line, side, row) and in the world.
func _spots(p: Vector3) -> void:
	var seg: Dictionary = CityStore.query(&"segment_at", [p])
	if seg.get("seg", -1) < 0:
		print("poles at %s: no road" % p)
		return
	print("poles of road %d (profile %d) at %s:" % [seg.seg, seg.profile, p])
	for s: Dictionary in CityStore.query(&"furniture_spots", [seg.seg]):
		if s.kind == FURNITURE_LAMP or s.kind == FURNITURE_COLUMN:
			print("  kind %d row %d side %d station %d v %.1f u %.2f at %s" % [s.kind, s.row, s.side, s.station, s.v, s.u, s.position])


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
	# nothing pending right after a jump may only mean the view has not reached the drawing yet: a photo that lies
	if await CaptureWait.until_drawn(self, JUMP_SETTLE_FRAMES, name) == 0:
		await CaptureWait.until_drawn(self, JUMP_SETTLE_FRAMES * 2, name + " again")
	await ui.shot(name)


## The camera of a 4.0 photo (`__roadcraft.setView`): its bearing runs from +X towards +Z, ours from +Z towards +X.
func _pair(name: String, p: Vector3, azimuth: float, tilt: float, zoom: float) -> void:
	if not only.is_empty() and not only.has(name):
		return
	await PairCamera.stand(rig, ui, p, azimuth, tilt, zoom)
	if await CaptureWait.until_drawn(self, JUMP_SETTLE_FRAMES, name) == 0:
		await CaptureWait.until_drawn(self, JUMP_SETTLE_FRAMES * 2, name + " again")
	await ui.shot(name)
	rig.set_process(true)


func _worn(p: Vector3) -> void:
	print("the road's height at the median's junction: %.3f m" % p.y)
	if not only.is_empty() and not only.has("worn_low"):
		return
	await PairCamera.stand(rig, ui, p, 30.0, 30.0, 30.0) # the field's window follows the view: it must be here before the wear is
	await ui.frames(JUMP_SETTLE_FRAMES)
	PairCamera.lay_wear(p)
	await get_tree().create_timer(WEAR_UPLOAD_S).timeout
	rig.set_process(true)
	await _pair("worn_top", p, 20.0, 62.0, 14.0)
	await _pair("worn_low", p, 30.0, 30.0, 30.0)


## The tool as the player uses it: the drawer, a stretch held by a click, a curve waiting for its bend, a freehand stroke.
func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
