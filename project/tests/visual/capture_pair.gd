## Photos in the frame of the Roadcraft 4.0's: one list of views feeds this and the 4.0's look plan, so each pair shares position, zoom,
## bearing and tilt, at the 4.0's fixed day sun (16:00). The list: [{"name", "x", "y", "zoom", "azimuth", "elevation"}], the 4.0's setView.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_pair.tscn -- --no-flow --views=FILE --capture-dir=DIR
extends Node3D

const TOWN_MAP := 0
const SEED := 7
const DAY_HOUR := 16.0
const RIGHT_ANGLE := 90.0
const PAIR_LINES := 900.0 # the 4.0's zoom is pixels a metre, on the lines of its photos

var out_dir := "user://captures"
var views_file := ""
var world: CityWorld
var rig: CityCamera
var ui: UiDriver


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		if arg.begins_with("--views="):
			views_file = arg.trim_prefix("--views=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	Settings.apply(world, get_viewport()) # what the player's preset shows: its antialiasing and shadows
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	CityStore.dispatch(&"set_day_hour", [DAY_HOUR])
	ui = UiDriver.new(rig, get_tree(), out_dir)
	var views: Array = JSON.parse_string(FileAccess.get_file_as_string(views_file))
	for v: Dictionary in views:
		var at := ui.ground(v.x, v.y)
		rig.jump(at, RIGHT_ANGLE - v.azimuth, v.elevation, CameraProfile.view_distance(rig.cfg, PAIR_LINES / (2.0 * v.zoom)))
		await ui.drawn()
		await ui.shot(v.name)
	get_tree().quit(0)
