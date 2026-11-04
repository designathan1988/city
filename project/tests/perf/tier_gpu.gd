## What each graphics tier costs the GPU on this machine, at the three views of the game: the renderer's own measure of the frame on the
## GPU in samples, and the counts of what was drawn, which do not depend on what else the machine is doing. The default tier is chosen from
## this table. Quiet machine: godot_console --path project res://tests/perf/tier_gpu.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const LEVELS := ["low", "medium", "high", "ultra"]
const VIEWS := [["rua", 90.0, 25.0], ["bairro", 500.0, 40.0], ["cidade", 2200.0, 55.0]]
const SETTLE_FRAMES := 90
const SETTLE_LIMIT := 900
const SAMPLES := 120
const BUDGET_MS := 16.7
const MILLION := 1000000.0

var _rig: CityCamera


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_rig = CityCamera.new()
	add_child(_rig)
	CityStore.boot(world.terrain, world.materials())
	CityStore.new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	print("nível vista | gpu p50 p90 max (ms) | objetos chamadas Mtri | %s" % RenderingServer.get_video_adapter_name())
	for level: String in LEVELS:
		Settings.set_preset(level)
		for v: Array in VIEWS:
			await _measure(level, v)
	Settings.switches.set_on("occlusion", false)
	Settings.switches_changed()
	for v: Array in VIEWS:
		await _measure("ultra-sem-oclusão", v)
	get_tree().quit(0)


func _measure(level: String, view: Array) -> void:
	_rig.jump(_rig.focus, 0.0, view[2], view[1])
	var waited := 0
	while waited < SETTLE_LIMIT and (waited < SETTLE_FRAMES or CityStore.query(&"render_pending") > 0):
		await get_tree().process_frame
		waited += 1
	var rid := get_viewport().get_viewport_rid()
	var gpu := PackedFloat32Array()
	var cpu := 0.0
	for i in SAMPLES:
		await get_tree().process_frame
		gpu.append(RenderingServer.viewport_get_measured_render_time_gpu(rid))
		cpu += RenderingServer.viewport_get_measured_render_time_cpu(rid) + RenderingServer.get_frame_setup_time_cpu()
	gpu.sort()
	var objects := RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TOTAL_OBJECTS_IN_FRAME)
	var calls := RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TOTAL_DRAW_CALLS_IN_FRAME)
	var tris := RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TOTAL_PRIMITIVES_IN_FRAME)
	var p90 := gpu[int(SAMPLES * 0.9)]
	print("%s %s | %.2f %.2f %.2f | %d %d %.2f | cpu %.2f | %s" % [level, view[0], gpu[SAMPLES / 2], p90, gpu[SAMPLES - 1], objects, calls, tris / MILLION,
		cpu / SAMPLES, "cabe" if p90 < BUDGET_MS else "estoura"])
