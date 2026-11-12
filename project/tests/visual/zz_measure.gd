## One measurement scene for every version (copied into each measuring worktree, never into the repository): the town map, seed 7,
## generated through the store; fixed cameras; at each stage what the engine holds and what the frame drew, the frame times, and a
## photo. The external sampler reads the process's working set; this prints the engine's own counts.
extends Node3D

const STAGES := [
	# name, camera position, look-at, frames to wait, sim speed
	["a_overview", Vector3(-600, 520, 600), Vector3(0, 0, 0), 900, 0],
	["b_overview_running", Vector3(-600, 520, 600), Vector3(0, 0, 0), 1800, 1],
	["c_centre", Vector3(-120, 90, 120), Vector3(0, 0, 0), 600, 1],
	["d_street", Vector3(-58, 14, 30), Vector3(-58, 4, -60), 600, 1],
	["e_wide", Vector3(-2500, 1400, 2500), Vector3(0, 0, 0), 600, 1],
]
var camera: Camera3D
var out_dir := "user://measure"


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	var world := CityWorld.new()
	add_child(world)
	camera = Camera3D.new()
	camera.far = 30000.0
	camera.fov = 50.0
	add_child(camera)
	camera.global_position = STAGES[0][1]
	camera.look_at(STAGES[0][2])
	var errors = CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		print("MEASURE boot errors: ", errors)
		get_tree().quit(1)
		return
	var t0 := Time.get_ticks_msec()
	var report = CityStore.new_map(0, 7, true)
	print("MEASURE opened_ms ", Time.get_ticks_msec() - t0)
	_line("opened")
	if world.has_method("set_hour"):
		world.set_hour(16.0)
	for s: Array in STAGES:
		CityStore.dispatch(&"set_speed", [s[4]])
		var base := _ground(s[2].x, s[2].z)
		camera.global_position = s[1] + Vector3(0, base.y, 0)
		camera.look_at(s[2] + Vector3(0, base.y, 0))
		var times := PackedFloat32Array()
		var last := Time.get_ticks_usec()
		for i in int(s[3]):
			await get_tree().process_frame
			var now := Time.get_ticks_usec()
			times.push_back((now - last) / 1000.0)
			last = now
		_frames(s[0], times)
		_line(s[0])
		get_viewport().get_texture().get_image().save_png(out_dir.path_join(s[0] + ".png"))
	print("MEASURE snapshot ", CityStore.snapshot)
	get_tree().quit(0)


func _frames(stage: String, t: PackedFloat32Array) -> void:
	var tail := t.slice(t.size() / 2) # the second half: after the stage's own building
	var sorted := tail.duplicate()
	sorted.sort()
	var sum := 0.0
	for x in tail:
		sum += x
	print("MEASURE frames %s mean_ms %.2f p50 %.2f p95 %.2f p99 %.2f max %.2f" % [stage, sum / tail.size(), sorted[sorted.size() / 2],
		sorted[int(sorted.size() * 0.95)], sorted[int(sorted.size() * 0.99)], sorted[sorted.size() - 1]])


func _line(stage: String) -> void:
	var rs := RenderingServer
	var vp := get_viewport().get_viewport_rid()
	var mib := 1048576.0
	print("MEASURE engine %s static %.0f video %.0f textures %.0f buffers %.0f objects %d nodes %d | drawn objects %d primitives %d draws %d | shadow primitives %d draws %d | gpu_ms %.2f cpu_ms %.2f | pipelines %d %d %d %d | pending %s vehicles %s people %s" % [
		stage, OS.get_static_memory_usage() / mib, rs.get_rendering_info(rs.RENDERING_INFO_VIDEO_MEM_USED) / mib,
		rs.get_rendering_info(rs.RENDERING_INFO_TEXTURE_MEM_USED) / mib, rs.get_rendering_info(rs.RENDERING_INFO_BUFFER_MEM_USED) / mib,
		Performance.get_monitor(Performance.OBJECT_COUNT), Performance.get_monitor(Performance.OBJECT_NODE_COUNT),
		rs.viewport_get_render_info(vp, rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, rs.VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME),
		rs.viewport_get_render_info(vp, rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME),
		rs.viewport_get_render_info(vp, rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		rs.viewport_get_render_info(vp, rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW, rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME),
		rs.viewport_get_render_info(vp, rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW, rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		rs.viewport_get_measured_render_time_gpu(vp), rs.viewport_get_measured_render_time_cpu(vp),
		rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_CANVAS), rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_MESH),
		rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_SURFACE), rs.get_rendering_info(rs.RENDERING_INFO_PIPELINE_COMPILATIONS_SPECIALIZATION),
		CityStore.query(&"render_pending"), CityStore.snapshot.get("vehicles"), CityStore.snapshot.get("people")])


func _ground(x: float, z: float) -> Vector3:
	return Vector3(x, float(CityStore.query(&"height_at", [x, z])) if _has_height_at() else 0.0, z)


func _has_height_at() -> bool:
	return true
