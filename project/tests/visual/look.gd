## LOOK AT THE GAME FROM A PLAN, as the Roadcraft 4.0's scripts/look.mjs does: a JSON plan of steps, so a view is a line of
## data and not a scene of its own, and one plan serves both games (the same setView, in metres, gives the same camera:
## PairCamera). The plan: {"city": {"map": 0, "seed": 1, "generate": true}, "steps": [...]}; `generate` false opens the
## bare land of the map, with no town to make and draw.
## Steps: {"setView": [x, z, zoom, azimuth, elevation]} (degrees, the 4.0's), {"shot": name} (after the world is drawn),
## {"wait": ms}, {"frames": n}, {"hour": h} (light and clock together), {"dispatch": command, "args": [...]},
## {"query": name, "args": [...]} (its answer printed), {"key": "Escape"}, {"click": [x, z]}, {"drag": [[x, z], [x, z]]}.
## In args, {"at": [x, z]} is the point of the ground there (what the player's pointer would hit), {"profile": id} the index of
## that road class, {"v3": [x, y, z]} a vector.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<file> --capture-dir=<dir>
extends Node3D

const SETTLE_FRAMES := 4
const MS_PER_S := 1000.0

var out_dir := "user://captures"
var plan_path := ""
var world: CityWorld
var rig: CityCamera
var ui: UiDriver
var failures := 0
var shots := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		if arg.begins_with("--plan="):
			plan_path = arg.trim_prefix("--plan=")
	var plan: Variant = JSON.parse_string(FileAccess.get_file_as_string(plan_path)) if plan_path != "" else null
	if not plan is Dictionary:
		push_error("plano ilegível: %s" % plan_path)
		get_tree().quit(2)
		return
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
	var city: Dictionary = plan.get("city", {})
	var t0 := Time.get_ticks_msec()
	CityStore.new_map(int(city.get("map", 0)), int(city.get("seed", 1)), bool(city.get("generate", true)))
	CityStore.dispatch(&"set_speed", [0])
	print("cidade aberta em %d ms" % (Time.get_ticks_msec() - t0))
	ui = UiDriver.new(rig, get_tree(), out_dir)
	add_child(RoadcraftHud.new())
	for step: Dictionary in plan.get("steps", []):
		await _step(step)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _step(step: Dictionary) -> void:
	if step.has("setView"):
		var v: Array = step.setView
		await PairCamera.stand(rig, ui, ui.ground(v[0], v[1]), v[3], v[4], v[2])
	elif step.has("shot"):
		await CaptureWait.until_drawn(self, SETTLE_FRAMES, step.shot)
		await ui.shot("%02d-%s" % [shots, step.shot])
		shots += 1
	elif step.has("wait"):
		await get_tree().create_timer(float(step.wait) / MS_PER_S).timeout
	elif step.has("frames"):
		await ui.frames(int(step.frames))
	elif step.has("hour"):
		UiDriver.at_hour(world, float(step.hour))
	elif step.has("dispatch"):
		var r: Variant = CityStore.dispatch(StringName(step.dispatch), _args(step.get("args", [])))
		print("comando %s: %s" % [step.dispatch, r])
		if r is Dictionary and r.has("ok") and not r.ok:
			failures += 1
			print("FAIL comando recusado: ", step.dispatch)
	elif step.has("query"):
		print("consulta %s %s: %s" % [step.query, step.get("args", []), CityStore.query(StringName(step.query), _args(step.get("args", [])))])
	elif step.has("key"):
		await ui.key(OS.find_keycode_from_string(step.key), false)
	elif step.has("click"):
		await ui.click(_point(step.click))
	elif step.has("drag"):
		await ui.drag(_point(step.drag[0]), _point(step.drag[1]))
	else:
		failures += 1
		print("FAIL passo desconhecido: ", step)


## A plan's arguments for a command: {"at": [x, z]} the ground's point there, {"profile": id} a road class's index,
## {"v3": [x, y, z]} a vector, the rest as the JSON gives it.
func _args(raw: Array) -> Array:
	var out := []
	for a: Variant in raw:
		if a is Dictionary and a.has("at"):
			out.push_back(_point(a.at))
		elif a is Dictionary and a.has("profile"):
			out.push_back(_profile(a.profile))
		elif a is Dictionary and a.has("v3"):
			out.push_back(Vector3(a.v3[0], a.v3[1], a.v3[2]))
		else:
			out.push_back(a)
	return out


func _profile(id: String) -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			return p.index
	failures += 1
	print("FAIL classe de via desconhecida: ", id)
	return -1


func _point(xz: Array) -> Vector3:
	return ui.ground(xz[0], xz[1])
