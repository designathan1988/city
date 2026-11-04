## The cost of each frame while the camera moves the way the player moves it, always over the city (city_bounds): the
## real world, camera rig, HUD and running simulation (as game/play.tscn), at the game's own pace (vsync and the
## Settings frame cap: the GPU is never pushed past what the game asks of it). The rig stands still over the centre,
## crosses the city, circles its middle, turns round, zooms out and back and goes round close to the walls. Per movement: frame times (hitches past
## 33 / 50 ms), the GPU and CPU render time of each frame (RenderingServer measured render times, exact under vsync),
## the triangles and draw calls of the visible and shadow passes, and the memory (static, video, buffers). The slowest
## frames are broken down (when in the movement; the core's _process by section, query render_cost; GPU; render
## CPU) to tell where a hitch comes from.
## Analysis knobs: --shadows=0 (no sun shadows), --msaa=0 (no MSAA), to see what a feature costs; --moves=a,b runs
## only those movements (the rig still goes where each one starts); --uncapped (no vsync, no frame cap: under the cap the GPU
## slows its clock until a frame takes about the same, so only this compares the GPU cost of two versions; not the game's pace).
## Run: godot --path project --resolution 1600x900 res://tests/perf/camera_pan.tscn -- --no-flow [--map=N]
## [--shot-dir=DIR] (a picture halfway through each movement: proof of where the camera was)
extends Node3D

const SEED := 1
const BUILD_FRAMES := 1200
const SETTLE_FRAMES := 30
const SLIDE_MPS := 150.0
const CROSS_INSET := 0.15 # of the city's extent kept clear of each edge on the crossing
const LOOP_RADIUS := 0.3 # of the city's smaller extent
const LOOP_S := 15.0
const TURN_S := 6.0
const ZOOM_S := 8.0
const ZOOM_FAR_M := 4000.0
const ZOOM_NEAR_M := 300.0
const VIEW_PITCH_DEG := 40.0
const CLOSE_S := 12.0 # the walls fill the frame: what a fragment of them costs, the panes above all
const CLOSE_M := 120.0
const CLOSE_PITCH_DEG := 15.0
const STILL_S := 10.0
const SLOW_MS := [33.0, 50.0]
const MIB := 1024.0 * 1024.0
const WORST := 10
const US_PER_MS := 1000.0
const PERCENTILES := [0.5, 0.95, 0.99]
const BROKEN_DOWN := 3
## render_cost sections, in the order of the core's frame.
const COST_PARTS := [&"agents", &"lock", &"stream", &"levels", &"flush", &"roads", &"props", &"buildings", &"chunk_max",
	&"super_max", &"b_scan", &"b_freed", &"b_sort", &"b_admit", &"b_hand", &"b_tail", &"super_mb_max", &"trees", &"overlays"]

var rig: CityCamera
var map := 0
var centre := Vector3.ZERO
var shot_dir := ""
var moves := PackedStringArray()
var frames: Array[float] = []
var gpu_ms: Array[float] = []
var cpu_ms: Array[float] = []
var process_ms: Array[float] = []
var costs: Array[Dictionary] = []


## The analysis knobs (header).
func _knobs(world: CityWorld) -> void:
	for arg in OS.get_cmdline_user_args():
		if arg == "--shadows=0":
			world.sun.shadow_enabled = false
		elif arg == "--msaa=0":
			get_viewport().msaa_3d = Viewport.MSAA_DISABLED
		elif arg == "--uncapped":
			Engine.max_fps = 0
			DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--map="):
			map = int(arg.trim_prefix("--map="))
		elif arg.begins_with("--shot-dir="):
			shot_dir = arg.trim_prefix("--shot-dir=")
		elif arg.begins_with("--moves="):
			moves = arg.trim_prefix("--moves=").split(",")
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport())
	_knobs(world)
	RenderingServer.viewport_set_measure_render_time(get_viewport().get_viewport_rid(), true)
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		GameFlow.quit_now()
		return
	print("cidade: ", CityStore.new_map(map, SEED, true))
	rig.look_at_ground(Vector3.ZERO)
	add_child(RoadcraftHud.new())
	CityStore.dispatch(&"set_speed", [1])
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	var lo: Vector3 = bounds.min
	var hi: Vector3 = bounds.max
	centre = (lo + hi) * 0.5
	rig.look_at_ground(centre)
	for i in SETTLE_FRAMES: # the core starts the full rebuild on its next frame
		await get_tree().process_frame
	var waited := 0
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await get_tree().process_frame
		waited += 1
	print("cidade %s .. %s, desenhada em %d quadros" % [lo, hi, waited])
	await _run("parada", STILL_S, func(_dt: float, _t: float) -> void: pass)
	var a := Vector3(lerpf(lo.x, hi.x, CROSS_INSET), 0.0, centre.z)
	var b := Vector3(lerpf(lo.x, hi.x, 1.0 - CROSS_INSET), 0.0, centre.z)
	rig.look_at_ground(a)
	await _run("cruzar", a.distance_to(b) / SLIDE_MPS, func(_dt: float, t: float) -> void:
		rig.look_at_ground(a.move_toward(b, SLIDE_MPS * t)))
	var radius := LOOP_RADIUS * minf(hi.x - lo.x, hi.z - lo.z)
	await _run("circular", LOOP_S, func(_dt: float, t: float) -> void:
		var angle := SLIDE_MPS * t / radius
		rig.look_at_ground(centre + Vector3(cos(angle), 0.0, sin(angle)) * radius))
	rig.look_at_ground(centre)
	await _run("girar", TURN_S, func(dt: float, _t: float) -> void: rig.orbit(TAU * dt / TURN_S, 0.0))
	await _run("zoom", ZOOM_S, func(_dt: float, t: float) -> void:
		var k := 1.0 - absf(2.0 * t / ZOOM_S - 1.0) # out and back
		rig.jump(rig.focus, rad_to_deg(rig.yaw), VIEW_PITCH_DEG, lerpf(ZOOM_NEAR_M, ZOOM_FAR_M, k)))
	await _run("perto", CLOSE_S, func(_dt: float, t: float) -> void:
		rig.jump(centre, rad_to_deg(TAU * t / CLOSE_S), CLOSE_PITCH_DEG, CLOSE_M))
	GameFlow.quit_now()


## One movement for `seconds`: step(dt, t) moves the rig every frame; the frame times are reported.
func _run(name: String, seconds: float, step: Callable) -> void:
	if not moves.is_empty() and not moves.has(name):
		return
	frames.clear()
	gpu_ms.clear()
	cpu_ms.clear()
	process_ms.clear()
	costs.clear()
	CityStore.query(&"pose_stats") # counts from here
	var vp := get_viewport().get_viewport_rid()
	var t := 0.0
	var last := Time.get_ticks_usec()
	var shot := shot_dir.is_empty()
	while t < seconds:
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		var dt := float(now - last) / (US_PER_MS * US_PER_MS)
		last = now
		frames.append(dt * US_PER_MS)
		gpu_ms.append(RenderingServer.viewport_get_measured_render_time_gpu(vp))
		cpu_ms.append(RenderingServer.viewport_get_measured_render_time_cpu(vp) + RenderingServer.get_frame_setup_time_cpu())
		var cost: Dictionary = CityStore.query(&"render_cost") # the frame that just ended (frames[-1])
		costs.append(cost)
		process_ms.append(cost.process)
		t += dt
		step.call(dt, t)
		if not shot and t >= seconds * 0.5: # taken after this frame's time was recorded
			shot = true
			get_viewport().get_texture().get_image().save_png("%s/%s.png" % [shot_dir, name])
			last = Time.get_ticks_usec()
	_report(name)


func _report(name: String) -> void:
	var sorted := frames.duplicate()
	sorted.sort()
	var line := "%s: %d quadros" % [name, sorted.size()]
	for p in PERCENTILES:
		line += ", p%d %.1f ms" % [int(p * 100), sorted[mini(int(p * sorted.size()), sorted.size() - 1)]]
	for limit in SLOW_MS:
		line += ", >%d ms: %d" % [int(limit), sorted.filter(func(f: float) -> bool: return f > limit).size()]
	print(line)
	var worst := sorted.slice(maxi(0, sorted.size() - WORST))
	worst.reverse()
	print("  piores: ", worst.map(func(f: float) -> String: return "%.0f" % f))
	var order := range(frames.size())
	order.sort_custom(func(a: int, b: int) -> bool: return frames[a] > frames[b])
	for i in order.slice(0, BROKEN_DOWN):
		var parts := COST_PARTS.map(func(k: StringName) -> String: return "%s %.1f" % [k, costs[i][k]])
		print("    quadro %d de %d: %.0f ms | núcleo %.1f ms (%s; envios %.1f MiB) | gpu %.1f ms, cpu de render %.1f ms" % [i,
			frames.size(), frames[i], process_ms[i], ", ".join(parts), costs[i].uploads, gpu_ms[i], cpu_ms[i]])
	print("  gpu %s | cpu de render %s | núcleo %s" % [_stats(gpu_ms), _stats(cpu_ms), _stats(process_ms)])
	var vp := get_viewport().get_viewport_rid()
	var info := func(kind: int, what: int) -> int: return RenderingServer.viewport_get_render_info(vp, kind, what)
	print("  visível: %d mil triângulos, %d draw calls | sombra: %d mil triângulos, %d draw calls" % [
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)])
	var ps: Dictionary = CityStore.query(&"pose_stats")
	print("  poses: %d publicações (intervalo %.1f-%.1f ms; %d coladas, %d atrasadas); de %d quadros, %d além da última e %d antes da mais velha" % [
		ps.publications, ps.gap_min_ms, ps.gap_max_ms, ps.early, ps.late, ps.frames, ps.held, ps.behind])
	print("  memória: estática %.0f MiB, vídeo %.0f MiB, buffers %.0f MiB | chunks a desenhar: %d" % [
		Performance.get_monitor(Performance.MEMORY_STATIC) / MIB, Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / MIB,
		Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / MIB, CityStore.query(&"render_pending")])
	print("  maior HLOD de superchunk construído: %.1f MiB" % float(CityStore.query(&"render_cost").get("super_mb_max", 0.0)))
	print("  prédios: ", JSON.stringify(CityStore.query(&"building_cost")))


## Mean, 95th percentile and maximum of per-frame milliseconds.
func _stats(values: Array[float]) -> String:
	if values.is_empty():
		return "-"
	var sorted := values.duplicate()
	sorted.sort()
	var mean := 0.0
	for v in sorted:
		mean += v
	mean /= sorted.size()
	return "média %.1f ms, p95 %.1f ms, máx %.1f ms" % [mean, sorted[mini(int(0.95 * sorted.size()), sorted.size() - 1)], sorted.back()]
