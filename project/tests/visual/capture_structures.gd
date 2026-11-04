## Captures of raised roads on open ground, in the pose of the 4.0's photos: a viaduct over a street, its ramp, a bridge.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_structures.tscn -- --no-flow --capture-dir=/tmp/x
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const PX := 0.25
const OPEN_GROUND_M := 420.0
const ELEVATED := 1
const BRIDGE := 2
const SPAN_M := 360.0
const CROSS_M := 120.0
const BRIDGE_OFF_M := 200.0
const SETTLE_FRAMES := 4

var out_dir := "user://captures"
var rig: CityCamera
var ui: UiDriver
var profiles := {}


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var world := CityWorld.new()
	add_child(world)
	PairCamera.quality(world, get_viewport())
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
	add_child(RoadcraftHud.new())
	for p in CityStore.query(&"road_profiles"):
		profiles[p.id] = p.index
	await _run()


func _run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var o := ui.ground(b.max.x + OPEN_GROUND_M, (b.min.z + b.max.z) * 0.5)
	CityStore.dispatch(&"clear_trees") # the 4.0's free ground is a bare field
	CityStore.dispatch(&"set_day_hour", [16.0])
	var mid := o + Vector3(0, 0, SPAN_M / 2)
	var far := o + Vector3(BRIDGE_OFF_M, 0, 0)
	_road(o, o + Vector3(0, 0, SPAN_M), "urbanStreet", ELEVATED)
	_road(mid - Vector3(CROSS_M / 2, 0, 0), mid + Vector3(CROSS_M / 2, 0, 0), "localStreet", 0)
	_road(far, far + Vector3(0, 0, SPAN_M), "avenue", BRIDGE)
	print("malhas: ", CityStore.query(&"road_mesh_counts"))
	await _shot("structure_low", mid, 30.0, 18.0, 16.0)
	await _shot("structure_top", mid, 20.0, 62.0, 10.0)
	await _shot("structure_ramp", o + Vector3(0, 0, 30), 300.0, 20.0, 18.0)
	await _shot("structure_side", mid, 0.0, 6.0, 12.0)
	await _shot("structure_bridge", far + Vector3(0, 0, SPAN_M / 2), 30.0, 18.0, 16.0)
	get_tree().quit(0)


func _road(a: Vector3, e: Vector3, profile: String, structure: int) -> void:
	var r: Dictionary = CityStore.dispatch(&"build_road_path", [PackedVector3Array([a, (a + e) / 2, e]), profiles[profile], PX, structure])
	if not r.get("ok", false):
		push_error("via %s: %s" % [profile, r])


func _shot(name: String, p: Vector3, azimuth: float, tilt: float, zoom: float) -> void:
	await PairCamera.stand(rig, ui, p, azimuth, tilt, zoom)
	await CaptureWait.until_drawn(self, SETTLE_FRAMES, name)
	await ui.shot(name)
	rig.set_process(true)
