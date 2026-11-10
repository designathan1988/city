## LOOK AT THE GAME FROM A PLAN, as the Roadcraft 4.0's scripts/look.mjs does: a JSON plan of steps, so a view is a line of
## data and not a scene of its own, and one plan serves both games (the same setView, in metres, gives the same camera:
## PairCamera; its y is the 4.0's, whose map north is our -z). The plan: {"city": {"map": 0, "seed": 1, "generate": true}, "steps": [...]}; `generate` false opens the
## bare land of the map, with no town to make and draw.
## Steps: {"setView": [x, y, zoom, azimuth, elevation]} (the 4.0's map and degrees), {"shot": name} (after the world is drawn),
## {"frames": n}, {"hour": h} (light and clock together), {"dispatch": command, "args": [...]},
## {"query": name, "args": [...]} (its answer printed), {"key": "Escape"}, {"click": [x, z]}, {"tap": [screen x, y]}, {"control": "what it says"}, {"blastShot": name, "at": s}, {"walker": [zoom, azimuth, elevation]}, {"global": [name, value]} (a shader global: an A/B inside one run, so what else loads the GPU falls on both), {"drag": [[x, z], [x, z]]}.
## In args, {"at": [x, z]} is the point of the ground there (what the player's pointer would hit), {"profile": id} the index of
## that road class, {"v3": [x, y, z]} a vector.
## Measures (look_measure.gd): {"record": name} … {"recordEnd": true} (the frames between: intervals, hitches, vsyncs missed,
## CPU and GPU), {"usage": seconds} (peak, mean and least), {"info": true} (what the last frame drew and what is held),
## {"clock": true} (the stages' stamps on the camera), {"switch": [key, on]} (an F7 switch), {"passes": true} (the GPU's stamps), {"view": "overdraw"} (any Viewport.DEBUG_DRAW_* by its name, "disabled" back),
## {"until": query, "args": [...], "equals": value, "timeout": ms}.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/look.tscn -- --no-flow --plan=<file> --capture-dir=<dir>
extends Node3D

const SETTLE_FRAMES := 4
const UNTIL_TIMEOUT_MS := 60000.0
const TIP_SHORTCUT := " · " # a tip says the name, then the key (HudWidgets)

var out_dir := "user://captures"
var plan_path := ""
var world: CityWorld
var rig: CityCamera
var ui: UiDriver
var failures := 0
var shots := 0
var measure: LookMeasure
var recording := ""
var frozen := false


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		if arg.begins_with("--plan="):
			plan_path = arg.trim_prefix("--plan=")
	frozen = "--frozen" in OS.get_cmdline_user_args()
	var plan: Variant = JSON.parse_string(FileAccess.get_file_as_string(plan_path)) if plan_path != "" else null
	if not plan is Dictionary:
		push_error("unreadable plan: %s" % plan_path)
		get_tree().quit(2)
		return
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	if frozen: # a temporal filter's history is the frames before the photo, which the machine's speed decides: none here
		Settings.kept = {"temporal": QualityTier.TEMPORAL_NONE}
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
	CityStore.new_map(int(city.get("map", 0)), int(city.get("seed", 1)), bool(city.get("generate", true)), true)
	print("city opened in %d ms" % (Time.get_ticks_msec() - t0))
	if frozen:
		Engine.time_scale = 0.0 # the clock and every delta stand: the same plan gives the same picture, by frames alone
		world.still_dither()
	ui = UiDriver.new(rig, get_tree(), out_dir)
	add_child(ToolController.new(rig))
	add_child(RoadcraftHud.new())
	add_child(InteriorController.new())
	measure = LookMeasure.new(get_viewport())
	add_child(measure)
	for step: Dictionary in plan.get("steps", []):
		await _step(step)
	if shots == 0: # a plan that photographs nothing proves nothing
		failures += 1
		print("FAIL the plan took no shot")
	print("FAILURES: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _step(step: Dictionary) -> void:
	if step.has("setView"):
		var v: Array = step.setView
		await PairCamera.stand(rig, ui, ui.ground(v[0], -v[1]), v[3], v[4], v[2])
	elif step.has("shot"):
		await CaptureWait.until_drawn(self, SETTLE_FRAMES, step.shot)
		if frozen:
			if not await CaptureWait.until_steady(self, step.shot):
				failures += 1
			await ui.shot("%02d-%s" % [shots, step.shot], 0)
		else:
			await ui.shot("%02d-%s" % [shots, step.shot])
		shots += 1
	elif step.has("blastShot"): # at that second of the effects' own clock, with no wait for the world to be drawn
		while CityStore.query(&"effects_since_blast") < float(step.at):
			await get_tree().process_frame
		await ui.shot("%02d-%s" % [shots, step.blastShot], 0)
		shots += 1
	elif step.has("frames"):
		await ui.frames(int(step.frames))
	elif step.has("hour"):
		UiDriver.at_hour(world, float(step.hour))
	elif step.has("dispatch"):
		var r: Variant = CityStore.dispatch(StringName(step.dispatch), _args(step.get("args", [])))
		print("command %s: %s" % [step.dispatch, r])
		if r is Dictionary and r.has("ok") and not r.ok:
			failures += 1
			print("FAIL command refused: ", step.dispatch)
	elif step.has("query"):
		print("query %s %s: %s" % [step.query, step.get("args", []), CityStore.query(StringName(step.query), _args(step.get("args", [])))])
	elif step.has("key"):
		await ui.key(OS.find_keycode_from_string(step.key), false)
	elif step.has("click"):
		await ui.click(_point(step.click))
	elif step.has("global"):
		RenderingServer.global_shader_parameter_set(StringName(step.global[0]), step.global[1])
	elif step.has("reference"):
		_reference(step.reference)
	elif step.has("until"):
		await _until(step)
	elif not await _pointer_step(step) and not await _measure_step(step):
		failures += 1
		print("FAIL unknown step: ", step)



## The steps of the pointer. False: not one of them.
func _pointer_step(step: Dictionary) -> bool:
	if step.has("walker"):
		await LookAim.stand_on_walker(rig, ui, step.walker)
	elif step.has("tap"):
		var at := Vector2(step.tap[0], step.tap[1])
		await ui.move(at)
		await ui.button(at, true)
		await ui.button(at, false)
	elif step.has("press"): # the button held, for a shot in the middle of a gesture
		await ui.move(Vector2(step.press[0], step.press[1]))
		await ui.button(Vector2(step.press[0], step.press[1]), true)
	elif step.has("moveTo"):
		await ui.move(Vector2(step.moveTo[0], step.moveTo[1]))
	elif step.has("letGo"):
		await ui.button(Vector2(step.letGo[0], step.letGo[1]), false)
	elif step.has("drag"):
		await ui.drag(_point(step.drag[0]), _point(step.drag[1]))
	elif step.has("control"): # the `press` of scripts/look.mjs: a control found by what it says, pressed with the mouse where it stands
		await _press_control(str(step.control))
	else:
		return false
	return true


## The last control on screen that says `said` (its text or its tip, a tip's shortcut left out).
func _press_control(said: String) -> void:
	var found: BaseButton = null
	var seen: Array[Node] = [self]
	while not seen.is_empty():
		var n: Node = seen.pop_back()
		seen.append_array(n.get_children())
		if n is BaseButton and n.is_visible_in_tree() and (n.get("text") == said or n.tooltip_text.get_slice(TIP_SHORTCUT, 0) == said):
			found = n
	if found == null:
		failures += 1
		print("FAIL no control says: ", said)
		return
	await ui.press(found)


## The steps that read the servers. False: not one of them.
func _measure_step(step: Dictionary) -> bool:
	if step.has("record"):
		measure.record()
		recording = str(step.record)
	elif step.has("recordEnd"):
		print("frames %s: %s" % [recording, measure.record_end()])
	elif step.has("usage"):
		print("usage: %s" % await measure.usage(self, float(step.usage) if step.usage is float else LookMeasure.USAGE_SECONDS))
	elif step.has("info"):
		print("info: %s" % measure.info())
	elif step.has("clock"):
		measure.clock_on(get_viewport().get_camera_3d(), DataTables.game("render_switches").doctor.stages)
	elif step.has("switch"):
		Settings.switches.set_on(str(step.switch[0]), bool(step.switch[1]))
		Settings.switches_changed()
	elif step.has("passes"):
		print("passes: %s" % [measure.passes()])
	elif step.has("view"):
		get_viewport().debug_draw = ClassDB.class_get_integer_constant(&"Viewport", ("DEBUG_DRAW_%s" % str(step.view)).to_upper())
	else:
		return false
	return true


## A model translucent over the map: [file, x, z, scale].
func _reference(args: Array) -> void:
	var model := ReferenceModel.new()
	add_child(model)
	print("reference ", model.load_file(args[0]))
	var at := _point([args[1], args[2]])
	model.place_on(at.x, at.z, at.y, 0.0)
	model.scale = Vector3.ONE * float(args[3])


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
		elif a is Dictionary and a.has("path"):
			var path := PackedVector3Array()
			for p: Array in a.path:
				path.push_back(_point(p))
			out.push_back(path)
		else:
			out.push_back(a)
	return out


## {"until": query, "args": [...], "equals": value, "timeout": ms}: frames until the game's own answer is the one asked
## (a wait on a signal, never a guess at a time); past the timeout the step fails.
func _until(step: Dictionary) -> void:
	var t0 := Time.get_ticks_msec()
	var limit := float(step.get("timeout", UNTIL_TIMEOUT_MS))
	while CityStore.query(StringName(step.until), _args(step.get("args", []))) != step.get("equals", 0):
		if Time.get_ticks_msec() - t0 > limit:
			failures += 1
			print("FAIL until %s: not %s after %d ms" % [step.until, step.get("equals", 0), limit])
			return
		await get_tree().process_frame
	print("until %s: %d ms" % [step.until, Time.get_ticks_msec() - t0])


func _profile(id: String) -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			return p.index
	failures += 1
	print("FAIL unknown road class: ", id)
	return -1


func _point(xz: Array) -> Vector3:
	return ui.ground(xz[0], xz[1])
