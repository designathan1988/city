## The cost of each frame while the camera moves the way the player moves it, always over the city (city_bounds): the
## real world, camera rig, HUD and running simulation (as game/play.tscn), at the game's own pace (vsync and the
## Settings frame cap: the GPU is never pushed past what the game asks of it). The rig stands still over the centre,
## crosses the city, circles its middle, turns round, zooms out and back and goes round close to the walls. Per movement: frame times (hitches past
## 33 / 50 ms), the GPU and CPU render time of each frame (RenderingServer measured render times, exact under vsync),
## the triangles and draw calls of the visible and shadow passes, and the memory (static, video, buffers). The slowest
## frames are broken down (when in the movement; the core's _process by section, query render_cost; GPU; render
## CPU) to tell where a hitch comes from.
## Analysis knobs: --shadows=0 (no sun shadows), --casters=MASK (only these render layers cast them), --casters-cycle=all,0,2,-64
## (A/B in one run, turn by turn: all, no shadows, 2 splits, every layer but 64: a busy machine weighs on each alike; --uncapped), --msaa=0 (no MSAA), --verges=0 (no ground round the roads), --wear=0 (no wear
## read), to see what a feature costs; --moves=a,b runs
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
const CYCLE_FRAMES := 12
const CYCLE_SKIP := 4
const ALL_LAYERS := 0xFFFFFFFF
const US_PER_MS := 1000.0
const PERCENTILES := [0.5, 0.9, 0.95, 0.99] # p90: the budgets' (doc/specification.md), read by tools/tasks/km24.py
const BROKEN_DOWN := 3
## render_cost sections, in the order of the core's frame.
const COST_PARTS := [&"agents", &"lock", &"stream", &"levels", &"flush", &"roads", &"props", &"buildings", &"chunk_max",
	&"super_max", &"b_scan", &"b_freed", &"b_sort", &"b_admit", &"b_hand", &"b_tail", &"super_mb_max", &"trees", &"overlays"]

var rig: CityCamera
var map := 0
var centre := Vector3.ZERO
var shot_dir := ""
var cycle: PackedStringArray
var cycle_sun: DirectionalLight3D
var cycle_sum := {}
var cycle_at := 0
var cycle_frame := 0
var moves := PackedStringArray()
var frames: Array[float] = []
var gpu_ms: Array[float] = []
var cpu_ms: Array[float] = []
var process_ms: Array[float] = []
var costs: Array[Dictionary] = []


## The analysis knobs (header).
## One frame of --casters-cycle: its time goes to the setting on, the first frames of a turn left out, and every CYCLE_FRAMES the next.
func _cycle_step(gpu: float) -> void:
	if cycle.is_empty():
		return
	cycle_frame += 1
	var key := cycle[cycle_at]
	if cycle_frame > CYCLE_SKIP:
		var sum: Array = cycle_sum.get(key, [0.0, 0])
		cycle_sum[key] = [sum[0] + gpu, sum[1] + 1]
	if cycle_frame < CYCLE_FRAMES:
		return
	cycle_frame = 0
	cycle_at = (cycle_at + 1) % cycle.size()
	var next := cycle[cycle_at]
	var hide: int = next.trim_prefix("h").to_int() if next.begins_with("h") else 0 # no shadow, and these layers out of the picture
	rig.camera.cull_mask = ALL_LAYERS & ~hide
	cycle_sun.shadow_enabled = next != "0" and not next.begins_with("h")
	cycle_sun.directional_shadow_mode = DirectionalLight3D.SHADOW_PARALLEL_2_SPLITS if next == "2" else DirectionalLight3D.SHADOW_PARALLEL_4_SPLITS
	cycle_sun.shadow_caster_mask = ALL_LAYERS & ~(-next.to_int()) if next.begins_with("-") else ALL_LAYERS


func _knobs(world: CityWorld) -> void:
	for arg in OS.get_cmdline_user_args():
		if arg == "--shadows=0":
			world.sun.shadow_enabled = false
		elif arg.begins_with("--casters="):
			for sun: Node in world.find_children("*", "DirectionalLight3D", true, false):
				(sun as Light3D).shadow_caster_mask = arg.trim_prefix("--casters=").to_int()
		elif arg.begins_with("--casters-cycle="):
			cycle = arg.trim_prefix("--casters-cycle=").split(",")
			cycle_sun = world.sun
		elif arg == "--msaa=0":
			get_viewport().msaa_3d = Viewport.MSAA_DISABLED
		elif arg == "--uncapped":
			Engine.max_fps = 0
			DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)
		elif arg == "--wear=0":
			world.materials().road.set_shader_parameter("wear_shown", false)


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
	RenderingServer.viewport_set_measure_render_time(get_viewport().get_viewport_rid(), true)
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		GameFlow.quit_now()
		return
	print("city: ", CityStore.new_map(map, SEED, true))
	Settings.attach(world, get_viewport()) # the tier reaches the core: after the boot
	_knobs(world)
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
	print("city %s .. %s, drawn in %d frames" % [lo, hi, waited])
	print("meshes: ", CityStore.query(&"road_mesh_counts"))
	if "--verges=0" in OS.get_cmdline_user_args():
		CityStore.query(&"draw_verges", [false])
	await _run("still", STILL_S, func(_dt: float, _t: float) -> void: pass)
	var a := Vector3(lerpf(lo.x, hi.x, CROSS_INSET), 0.0, centre.z)
	var b := Vector3(lerpf(lo.x, hi.x, 1.0 - CROSS_INSET), 0.0, centre.z)
	rig.look_at_ground(a)
	await _run("cross", a.distance_to(b) / SLIDE_MPS, func(_dt: float, t: float) -> void:
		rig.look_at_ground(a.move_toward(b, SLIDE_MPS * t)))
	var radius := LOOP_RADIUS * minf(hi.x - lo.x, hi.z - lo.z)
	await _run("circular", LOOP_S, func(_dt: float, t: float) -> void:
		var angle := SLIDE_MPS * t / radius
		rig.look_at_ground(centre + Vector3(cos(angle), 0.0, sin(angle)) * radius))
	rig.look_at_ground(centre)
	await _run("turn", TURN_S, func(dt: float, _t: float) -> void: rig.orbit(TAU * dt / TURN_S, 0.0))
	await _run("zoom", ZOOM_S, func(_dt: float, t: float) -> void:
		var k := 1.0 - absf(2.0 * t / ZOOM_S - 1.0) # out and back
		rig.jump(rig.focus, rad_to_deg(rig.yaw), VIEW_PITCH_DEG, lerpf(ZOOM_NEAR_M, ZOOM_FAR_M, k)))
	await _run("close", CLOSE_S, func(_dt: float, t: float) -> void:
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
		_cycle_step(gpu_ms[-1])
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
	var line := "%s: %d frames" % [name, sorted.size()]
	for p in PERCENTILES:
		line += ", p%d %.1f ms" % [int(p * 100), sorted[mini(int(p * sorted.size()), sorted.size() - 1)]]
	for limit in SLOW_MS:
		line += ", >%d ms: %d" % [int(limit), sorted.filter(func(f: float) -> bool: return f > limit).size()]
	print(line)
	var worst := sorted.slice(maxi(0, sorted.size() - WORST))
	worst.reverse()
	print("  worst: ", worst.map(func(f: float) -> String: return "%.0f" % f))
	var order := range(frames.size())
	order.sort_custom(func(a: int, b: int) -> bool: return frames[a] > frames[b])
	for i in order.slice(0, BROKEN_DOWN):
		var parts := COST_PARTS.map(func(k: StringName) -> String: return "%s %.1f" % [k, costs[i][k]])
		print("    frame %d of %d: %.0f ms | core %.1f ms (%s; uploads %.1f MiB) | gpu %.1f ms, render cpu %.1f ms" % [i,
			frames.size(), frames[i], process_ms[i], ", ".join(parts), costs[i].uploads, gpu_ms[i], cpu_ms[i]])
	print("  gpu %s | render cpu %s | core %s" % [_stats(gpu_ms), _stats(cpu_ms), _stats(process_ms)])
	for key: String in cycle_sum:
		print("  gpu of %s: %.2f ms" % [key, cycle_sum[key][0] / maxf(1.0, cycle_sum[key][1])])
	cycle_sum.clear()
	var vp := get_viewport().get_viewport_rid()
	var info := func(kind: int, what: int) -> int: return RenderingServer.viewport_get_render_info(vp, kind, what)
	print("  visible: %d thousand triangles, %d draw calls | shadow: %d thousand triangles, %d draw calls" % [
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_VISIBLE, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME),
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME) / 1000,
		info.call(RenderingServer.VIEWPORT_RENDER_INFO_TYPE_SHADOW, RenderingServer.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)])
	var ps: Dictionary = CityStore.query(&"pose_stats")
	print("  poses: %d published (interval %.1f-%.1f ms; %d together, %d late); of %d frames, %d past the last and %d before the oldest" % [
		ps.publications, ps.gap_min_ms, ps.gap_max_ms, ps.early, ps.late, ps.frames, ps.held, ps.behind])
	print("  memory: static %.0f MiB, video %.0f MiB, buffers %.0f MiB | chunks to draw: %d" % [
		Performance.get_monitor(Performance.MEMORY_STATIC) / MIB, Performance.get_monitor(Performance.RENDER_VIDEO_MEM_USED) / MIB,
		Performance.get_monitor(Performance.RENDER_BUFFER_MEM_USED) / MIB, CityStore.query(&"render_pending")])
	print("  largest superchunk HLOD built: %.1f MiB" % float(CityStore.query(&"render_cost").get("super_mb_max", 0.0)))
	print("  buildings: ", JSON.stringify(CityStore.query(&"building_cost")))
	print("  vehicles: ", JSON.stringify(CityStore.query(&"agent_cost")))


## Mean, 90th and 95th percentiles and maximum of per-frame milliseconds.
func _stats(values: Array[float]) -> String:
	if values.is_empty():
		return "-"
	var sorted := values.duplicate()
	sorted.sort()
	var mean := 0.0
	for v in sorted:
		mean += v
	mean /= sorted.size()
	var at := func(share: float) -> float: return sorted[mini(int(share * sorted.size()), sorted.size() - 1)]
	return "mean %.1f ms, p90 %.1f ms, p95 %.1f ms, max %.1f ms" % [mean, at.call(PERCENTILES[1]), at.call(PERCENTILES[2]), sorted.back()]
