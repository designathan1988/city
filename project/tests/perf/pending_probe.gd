## Investigation of the open items (goal log), on the metropolis seen as camera_pan's still view. By default, uncapped:
## the sun's shadow passes by setting (as configured, without shadows, without blended splits, two splits, one split),
## each with the GPU time, the shadow and visible passes' primitives and draws, and what the core says it draws; a click
## on a signal (inspect) while the simulation runs at its fastest; and a paused stretch, with the simulation thread's
## wake-ups (pose_stats sim_wakeups) while paused and after it goes on. --pace: the frame and GPU time under each way of
## pacing the frames. --tail: frame by frame at the fastest speed, what the slowest tenth has more of.
## Run: godot --path project --resolution 1600x900 res://tests/perf/pending_probe.tscn -- --no-flow [--map=N] [--pause-s=S]
## [--pace | --tail]
extends Node3D

const SEED := 1
const MAP_DEFAULT := 5
const BUILD_FRAMES := 1200
const SETTLE_FRAMES := 60
const MEASURE_FRAMES := 180
const CLICKS := 600
const PAUSE_S := 30.0
const SPEED_FASTEST := 3
const US_PER_MS := 1000.0

var map := MAP_DEFAULT
var pause_s := PAUSE_S
var world: CityWorld


## Reads the arguments, builds the city and runs the mode asked for.
func _ready() -> void:
	var args := OS.get_cmdline_user_args()
	for arg in args:
		if arg.begins_with("--map="):
			map = int(arg.trim_prefix("--map="))
		elif arg.begins_with("--pause-s="):
			pause_s = float(arg.trim_prefix("--pause-s="))
	var booted: bool = await _boot()
	if not booted:
		GameFlow.quit_now()
		return
	if "--pace" in args:
		await _pace()
	elif "--tail" in args:
		CityStore.dispatch(&"set_speed", [SPEED_FASTEST])
		await _tail("with the game's limit", DataTables.game("settings").frame_cap.max_fps, DisplayServer.VSYNC_ENABLED)
		await _tail("no limit", 0, DisplayServer.VSYNC_DISABLED)
	else:
		await _default_run()
	GameFlow.quit_now()


## The world, the camera over the city's middle and the HUD, uncapped; returns once the city is drawn (false: no boot).
func _boot() -> bool:
	Engine.max_fps = 0
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)
	world = CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport())
	Engine.max_fps = 0
	var rig := CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		return false
	print("city: ", CityStore.new_map(map, SEED, true))
	add_child(RoadcraftHud.new())
	var b: Dictionary = CityStore.query(&"city_bounds")
	rig.look_at_ground((b.min + b.max) * 0.5)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	var waited := 0
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await get_tree().process_frame
		waited += 1
	CityStore.dispatch(&"set_speed", [1])
	return true


## The shadow settings, the clicks on a signal and the pause.
func _default_run() -> void:
	var sun := world.sun
	print("sun: mode %d, distance %.0f m, splits %.3f %.3f %.3f, blend %s" % [sun.directional_shadow_mode,
		sun.directional_shadow_max_distance, sun.directional_shadow_split_1, sun.directional_shadow_split_2,
		sun.directional_shadow_split_3, sun.directional_shadow_blend_splits])
	await _variant("como configurado", func() -> void: pass)
	await _variant("no shadows", func() -> void: sun.shadow_enabled = false)
	sun.shadow_enabled = true
	await _variant("no blend_splits", func() -> void: sun.directional_shadow_blend_splits = false)
	sun.directional_shadow_blend_splits = true
	await _variant("2 splits", func() -> void: sun.directional_shadow_mode = DirectionalLight3D.SHADOW_PARALLEL_2_SPLITS)
	await _variant("1 split (orthogonal)", func() -> void: sun.directional_shadow_mode = DirectionalLight3D.SHADOW_ORTHOGONAL)
	sun.directional_shadow_mode = DirectionalLight3D.SHADOW_PARALLEL_4_SPLITS
	print("agentes: ", JSON.stringify(CityStore.query(&"agent_cost")))
	print("buildings: ", JSON.stringify(CityStore.query(&"building_cost")))
	await _clicks()
	await _pause()


## pause_s paused: how often the simulation thread woke meanwhile (none expected), then how often once it goes on.
func _pause() -> void:
	CityStore.dispatch(&"set_speed", [0])
	print("PAUSA_INICIO %d" % Time.get_unix_time_from_system())
	var woke: int = CityStore.query(&"pose_stats").get("sim_wakeups", -1)
	var t := 0.0
	while t < pause_s:
		await get_tree().process_frame
		t += get_process_delta_time()
	var woke_after: int = CityStore.query(&"pose_stats").get("sim_wakeups", -1)
	print("simulation thread paused: %d wakes in %.1f s" % [woke_after - woke, t])
	CityStore.dispatch(&"set_speed", [1])
	var t1: int = CityStore.query(&"pose_stats").get("sim_wakeups", -1)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	print("after resuming: %d wakes in %d frames (the simulation moved again)" % [
		int(CityStore.query(&"pose_stats").get("sim_wakeups", -1)) - t1, SETTLE_FRAMES])
	print("PAUSA_FIM %d" % Time.get_unix_time_from_system())


func _variant(name: String, setup: Callable) -> void:
	setup.call()
	var vp := get_viewport().get_viewport_rid()
	RenderingServer.viewport_set_measure_render_time(vp, true)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	var gpu := 0.0
	var frame := 0.0
	var last := Time.get_ticks_usec()
	for i in MEASURE_FRAMES:
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		frame += float(now - last) / US_PER_MS
		last = now
		gpu += RenderingServer.viewport_get_measured_render_time_gpu(vp)
	var info := func(kind: int, what: int) -> int: return RenderingServer.viewport_get_render_info(vp, kind, what)
	print("%s: gpu %.2f ms, frame %.2f ms | shadow %d thousand prim., %d draws | visible %d thousand prim., %d draws" % [name,
		gpu / MEASURE_FRAMES, frame / MEASURE_FRAMES,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)])


## The frame and GPU time under each way of pacing the frames: the game's cap with vsync, the cap alone, vsync alone, none.
func _pace() -> void:
	var cap: int = DataTables.game("settings").frame_cap.max_fps
	var ways := [["cap+vsync", cap, DisplayServer.VSYNC_ENABLED], ["cap only", cap, DisplayServer.VSYNC_DISABLED],
		["vsync only", 0, DisplayServer.VSYNC_ENABLED], ["nenhum", 0, DisplayServer.VSYNC_DISABLED],
		["cap+vsync de novo", cap, DisplayServer.VSYNC_ENABLED]]
	print("tela: %.1f Hz" % DisplayServer.screen_get_refresh_rate())
	for w in ways:
		await _variant(w[0], func() -> void:
			Engine.max_fps = w[1]
			DisplayServer.window_set_vsync_mode(w[2]))


## Frame by frame at the fastest speed: the GPU time, the visible and shadow primitives, the vehicles' triangles and the
## core's frame; the slowest tenth set against the rest tells what a slow frame has more of.
func _tail(name: String, cap: int, vsync: int) -> void:
	Engine.max_fps = cap
	DisplayServer.window_set_vsync_mode(vsync)
	var vp := get_viewport().get_viewport_rid()
	RenderingServer.viewport_set_measure_render_time(vp, true)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	var rows: Array = []
	var last := Time.get_ticks_usec()
	var t := 0.0
	while t < 20.0:
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		var cost: Dictionary = CityStore.query(&"render_cost")
		rows.append([float(now - last) / US_PER_MS, RenderingServer.viewport_get_measured_render_time_gpu(vp),
			RenderingServer.viewport_get_render_info(vp, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000.0,
			RenderingServer.viewport_get_render_info(vp, RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000.0,
			float(CityStore.query(&"agent_cost").triangles) / 1000.0, float(cost.get("process", 0.0)), float(cost.get("uploads", 0.0))])
		t += float(now - last) / US_PER_MS / US_PER_MS
		last = now
	rows.sort_custom(func(a: Array, b: Array) -> bool: return a[1] > b[1])
	var cut := maxi(1, rows.size() / 10)
	var mean := func(from: int, to: int, k: int) -> float:
		var s := 0.0
		for i in range(from, to):
			s += rows[i][k]
		return s / maxf(1.0, to - from)
	print("%s (%d frames): [frame ms, gpu ms, visible thousand, shadow thousand, vehicles thousand tri., core ms, uploads MiB]" % [name, rows.size()])
	print("  10%% mais lentos (gpu): %s" % [range(7).map(func(k: int) -> String: return "%.2f" % mean.call(0, cut, k))])
	print("  demais 90%%:            %s" % [range(7).map(func(k: int) -> String: return "%.2f" % mean.call(cut, rows.size(), k))])
	print("  piores 5: %s" % [rows.slice(0, 5).map(func(r: Array) -> String: return str(r.map(func(v: float) -> String: return "%.1f" % v)))])


## A click of the inspect tool on a signalled junction each frame while the traffic runs at its fastest.
func _clicks() -> void:
	CityStore.dispatch(&"set_speed", [SPEED_FASTEST])
	var at: Vector3 = Vector3.ZERO
	for j in CityStore.query(&"junctions"): # the first junction the inspect tool says is signalled
		if CityStore.query(&"inspect", [j.at]).get("kind", "") == "junction":
			at = j.at
			break
	var times: Array[float] = []
	for i in CLICKS:
		var a := Time.get_ticks_usec()
		var r: Dictionary = CityStore.query(&"inspect", [at])
		times.append(float(Time.get_ticks_usec() - a) / US_PER_MS)
		if i == 0:
			print("inspect: ", r.get("kind", "?"), " ", r.get("greens", []))
		await get_tree().process_frame
	times.sort()
	var sum := 0.0
	for v in times:
		sum += v
	print("inspect on a signal (top speed): mean %.3f ms, p95 %.3f ms, p99 %.3f ms, max %.3f ms" % [
		sum / times.size(), times[int(0.95 * times.size())], times[int(0.99 * times.size())], times[-1]])
