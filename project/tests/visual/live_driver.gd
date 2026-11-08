## The game open on the player's screen, driven from outside while he watches (play.gd loads this only with -- --live=<dir>; the export leaves
## tests/ out). A file <dir>/steps.json is taken up, its steps run in the window, and what each gave goes to <dir>/result.txt. Steps:
## {"new": [map, seed, with_city]}, {"view": [x, z, distance, yaw, tilt]}, {"dispatch": name, "args": [...]}, {"query": name, "args": [...]},
## {"frames": n}, {"hour": h}, {"shot": name}. In args, {"at": [x, z]} is the ground there and {"v3": [x, y, z]} a vector, as in look.gd.
extends Node

const STEPS_FILE := "steps.json"
const RESULT_FILE := "result.txt"
const LOOK_EVERY_S := 0.25

var dir := ""
var rig: CityCamera
var _left := 0.0
var _busy := false


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--live="):
			dir = arg.trim_prefix("--live=")
	rig = get_tree().get_first_node_in_group(&"camera_rig")


func _process(dt: float) -> void:
	_left -= dt
	if _busy or _left > 0.0:
		return
	_left = LOOK_EVERY_S
	var path := dir.path_join(STEPS_FILE)
	if not FileAccess.file_exists(path):
		return
	var steps: Variant = JSON.parse_string(FileAccess.get_file_as_string(path))
	DirAccess.remove_absolute(path)
	if steps is Array:
		_run(steps)


func _run(steps: Array) -> void:
	_busy = true
	var out := PackedStringArray()
	for step: Dictionary in steps:
		var said: String = await _step(step)
		if not said.is_empty():
			out.append(said)
	var file := FileAccess.open(dir.path_join(RESULT_FILE), FileAccess.WRITE)
	file.store_string("\n".join(out) + "\ndone\n")
	file.close()
	_busy = false


func _step(step: Dictionary) -> String:
	if step.has("new"):
		var hud: RoadcraftHud = get_parent().find_children("*", "RoadcraftHud", false, false)[0]
		hud.actions.open_new(int(step.new[0]), int(step.new[1]), bool(step.new[2]))
		CityStore.dispatch(&"set_speed", [1])
	elif step.has("view"):
		var v: Array = step.view
		rig.jump(_ground(v[0], v[1]), v[3], v[4], v[2])
	elif step.has("dispatch"):
		return "%s: %s" % [step.dispatch, CityStore.dispatch(StringName(step.dispatch), _args(step.get("args", [])))]
	elif step.has("query"):
		return "%s %s: %s" % [step.query, step.get("args", []), CityStore.query(StringName(step.query), _args(step.get("args", [])))]
	elif step.has("frames"):
		for i in int(step.frames):
			await get_tree().process_frame
	elif step.has("hour"):
		CityStore.dispatch(&"set_day_hour", [float(step.hour)])
	elif step.has("shot"):
		await RenderingServer.frame_post_draw
		get_viewport().get_texture().get_image().save_png(dir.path_join("%s.png" % step.shot))
	return ""


func _ground(x: float, z: float) -> Vector3:
	return Vector3(x, CityStore.query(&"height_at", [x, z]), z)


func _args(raw: Array) -> Array:
	var out := []
	for a: Variant in raw:
		if a is Dictionary and a.has("at"):
			out.push_back(_ground(a.at[0], a.at[1]))
		elif a is Dictionary and a.has("v3"):
			out.push_back(Vector3(a.v3[0], a.v3[1], a.v3[2]))
		else:
			out.push_back(a)
	return out
