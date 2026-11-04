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
var buildings_file := ""
var world: CityWorld
var rig: CityCamera
var ui: UiDriver


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		if arg.begins_with("--views="):
			views_file = arg.trim_prefix("--views=")
		if arg.begins_with("--buildings="):
			buildings_file = arg.trim_prefix("--buildings=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	Settings.apply(world, get_viewport()) # what the player's preset shows: its antialiasing and shadows
	CityStore.new_map(TOWN_MAP, SEED, buildings_file.is_empty())
	if not buildings_file.is_empty():
		_lay_buildings()
	CityStore.dispatch(&"set_speed", [0])
	CityStore.dispatch(&"set_day_hour", [DAY_HOUR])
	ui = UiDriver.new(rig, get_tree(), out_dir)
	var views: Array = JSON.parse_string(FileAccess.get_file_as_string(views_file))
	for v: Dictionary in views:
		if v.has("pos"):
			_stand(v)
		else:
			var at := ui.ground(v.x, v.y)
			rig.jump(at, RIGHT_ANGLE - v.azimuth, v.elevation, CameraProfile.view_distance(rig.cfg, PAIR_LINES / (2.0 * v.zoom)))
		await ui.drawn()
		await ui.shot(v.name)
	get_tree().quit(0)


## The Roadcraft buildings of a file, each where it stands there: its plan's y is the world's -z.
func _lay_buildings() -> void:
	var placed := 0
	var rows: Array = JSON.parse_string(FileAccess.get_file_as_string(buildings_file))
	for doc: Dictionary in rows:
		var made: Dictionary = CityStore.query(&"doc_body", [doc])
		if not made.has("bytes"):
			print("documento recusado: ", made.errors)
			continue
		var turn: float = doc.get("rotation", 0.0)
		var front: Vector2 = Vector2(made.front).rotated(turn) + Vector2(doc.x, doc.y)
		var done: Dictionary = CityStore.dispatch(&"place_model", [made.bytes, front.x, -front.y, turn, made.function, made.zone])
		if done.get("ok", false):
			placed += 1
		else:
			print("prédio %d recusado: %s" % [doc.id, done.get("error", "")])
	print("prédios postos: %d de %d" % [placed, rows.size()])


## The camera exactly where the other game's stood: position, look, up and lens read from it.
func _stand(v: Dictionary) -> void:
	rig.set_process(false)
	rig.set_process_unhandled_input(false)
	var pos := Vector3(v.pos[0], v.pos[1] + ui.ground(v.x, -v.y).y, v.pos[2]) # the 4.0's ground stands at 0
	var look := Vector3(v.fwd[0], v.fwd[1], v.fwd[2])
	var up := Vector3(v.up[0], v.up[1], v.up[2])
	rig.camera.fov = v.fov
	rig.camera.global_transform = Transform3D(Basis.looking_at(look, up), pos)
	print("%s: câmera %s para %s, lente %.2f" % [v.name, pos, look, v.fov])
