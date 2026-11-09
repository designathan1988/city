## The game open on the player's screen, driven from outside while he watches (play.gd loads this only with -- --live=<dir>; the export leaves
## tests/ out). A file <dir>/steps.json is taken up, its steps run in the window, and what each gave goes to <dir>/result.txt. Steps:
## {"new": [map, seed, with_city]}, {"view": [x, z, distance, yaw, tilt]}, {"dispatch": name, "args": [...]}, {"query": name, "args": [...]},
## {"frames": n}, {"frame_times": n} (the wall time between n frames), {"memory": 0}, {"census": top}, {"hour": h}, {"shot": name}. In args, {"at": [x, z]} is the ground there and {"v3": [x, y, z]} a vector, as in look.gd.
extends Node

const STEPS_FILE := "steps.json"
const RESULT_FILE := "result.txt"
const LOOK_EVERY_S := 0.25
const LONG_MS := 100.0 # a frame this long is told one by one by frame_times

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
	elif step.has("frame_times"):
		return await _frame_times(int(step.frame_times))
	elif step.has("memory"):
		return _memory()
	elif step.has("census"):
		return _census(int(step.census))
	elif step.has("hour"):
		CityStore.dispatch(&"set_day_hour", [float(step.hour)])
	elif step.has("shot"):
		await RenderingServer.frame_post_draw
		get_viewport().get_texture().get_image().save_png(dir.path_join("%s.png" % step.shot))
	return ""


## Over n frames as the player sees them: the wall time between frames (the median, the 99th percentile, how many past twice the median)
## and each frame past LONG_MS with what could have taken it: the core's own _process (render_cost), the pipelines the engine compiled
## in it, and the graphics tier then. A frame that long moves every agent forward at once.
func _frame_times(n: int) -> String:
	var gaps := PackedFloat32Array()
	var long_frames := []
	var last := Time.get_ticks_usec()
	var compiled := _pipelines()
	var tier: String = Settings.level
	for i in n:
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		var gap := float(now - last) / 1000.0
		last = now
		gaps.append(gap)
		var compiled_now := _pipelines()
		if gap > LONG_MS or Settings.level != tier:
			var cost: Dictionary = CityStore.query(&"render_cost")
			long_frames.append({"frame": i, "gap_ms": snappedf(gap, 0.1), "core_ms": snappedf(float(cost.get("process", 0.0)), 0.1),
				"pipelines": _minus(compiled_now, compiled), "tier": "%s>%s" % [tier, Settings.level]})
		compiled = compiled_now
		tier = Settings.level
	var sorted := gaps.duplicate()
	sorted.sort()
	var median := sorted[sorted.size() / 2]
	var past := 0
	for g in gaps:
		if g > 2.0 * median:
			past += 1
	return "frame_times %d (preset %s): median %.1f ms, p99 %.1f ms, past twice the median %d, long %s" % [n, Settings.preset, median,
			sorted[int(sorted.size() * 0.99)], past, long_frames]


## The pipelines the rendering device compiled so far, by kind (Godot docs, Pipeline compilations: canvas, mesh, surface and draw stall the
## frame they are made on; specialization runs in the background).
func _pipelines() -> Dictionary:
	return {"canvas": Performance.get_monitor(Performance.PIPELINE_COMPILATIONS_CANVAS), "mesh": Performance.get_monitor(Performance.PIPELINE_COMPILATIONS_MESH),
		"surface": Performance.get_monitor(Performance.PIPELINE_COMPILATIONS_SURFACE), "draw": Performance.get_monitor(Performance.PIPELINE_COMPILATIONS_DRAW),
		"specialization": Performance.get_monitor(Performance.PIPELINE_COMPILATIONS_SPECIALIZATION)}


static func _minus(a: Dictionary, b: Dictionary) -> Dictionary:
	var out := {}
	for k: String in a:
		if a[k] != b[k]:
			out[k] = int(a[k] - b[k])
	return out


## What the engine counts of its own memory: its allocator (static), and the video memory by kind, MiB.
func _memory() -> String:
	return "memory: static %.0f MiB, video %.0f MiB (textures %.0f, buffers %.0f), objects %d, resources %d" % [OS.get_static_memory_usage() / 1048576.0,
		Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / 1048576.0, Performance.get_monitor(Performance.RENDER_TEXTURE_MEM_USED) / 1048576.0,
		Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / 1048576.0, Performance.get_monitor(Performance.OBJECT_COUNT),
		Performance.get_monitor(Performance.OBJECT_RESOURCE_COUNT)]


## Every owner the engine can name, once (log/owner-review.md §8): the core's census (memory_census), the textures by size
## and the device's (RenderingDevice memory and its driver report, `top` x 200 characters of it), the Terrain3D regions and their maps, what the frame and the sun's
## shadow drew, the pipelines compiled by kind, and the graphics tier.
func _census(top: int) -> String:
	var out := PackedStringArray()
	out.append("core: %s" % [CityStore.query(&"memory_census")])
	out.append(_memory())
	var rd := RenderingServer.get_rendering_device()
	if rd:
		out.append("device: textures %.0f MiB, buffers %.0f MiB, total %.0f MiB; driver total %.0f MiB in %d allocations, device total %.0f MiB in %d" % [
			rd.get_memory_usage(RenderingDevice.MEMORY_TEXTURES) / 1048576.0, rd.get_memory_usage(RenderingDevice.MEMORY_BUFFERS) / 1048576.0,
			rd.get_memory_usage(RenderingDevice.MEMORY_TOTAL) / 1048576.0, rd.get_driver_total_memory() / 1048576.0, rd.get_driver_allocation_count(),
			rd.get_device_total_memory() / 1048576.0, rd.get_device_allocation_count()])
		out.append("device report: %s" % rd.get_driver_and_device_memory_report().left(top * 200))
	var terrain: Node = get_tree().root.find_child("Terrain3D", true, false)
	if terrain:
		var data: Object = terrain.call("get_data")
		out.append("terrain3d: region_size %s, vertex_spacing %s, regions %s, mesh_size %s, lods %s" % [terrain.get("region_size"), terrain.get("vertex_spacing"),
			data.call("get_region_count") if data else -1, terrain.get("mesh_size"), terrain.get("mesh_lods")])
	var vp := get_viewport().get_viewport_rid()
	for kind: Array in [["visible", RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE], ["shadow", RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW]]:
		out.append("frame %s: objects %d, primitives %d, draw calls %d" % [kind[0], RenderingServer.viewport_get_render_info(vp, kind[1], RenderingServer.VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME),
			RenderingServer.viewport_get_render_info(vp, kind[1], RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME),
			RenderingServer.viewport_get_render_info(vp, kind[1], RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)])
	out.append("gpu %.2f ms, cpu render %.2f ms, pipelines %s" % [RenderingServer.viewport_get_measured_render_time_gpu(vp),
		RenderingServer.viewport_get_measured_render_time_cpu(vp), _pipelines()])
	out.append("tier %s (preset %s), render cost %s" % [Settings.level, Settings.preset, CityStore.query(&"render_cost")])
	return "
".join(out)


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
