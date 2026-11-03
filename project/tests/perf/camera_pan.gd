## Frame times while the camera moves the way the player moves it, always over the city (city_bounds): the real world,
## camera rig, HUD and running simulation (as game/play.tscn), vsync off. The rig crosses the city, circles its
## middle, turns round and zooms out and back over the centre.
## Prints the frame-time percentiles, the longest frames and how many passed 33 / 50 ms.
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
const SLOW_MS := [33.0, 50.0]
const WORST := 10
const US_PER_MS := 1000.0
const PERCENTILES := [0.5, 0.95, 0.99]

var rig: CityCamera
var map := 0
var centre := Vector3.ZERO
var shot_dir := ""
var frames: Array[float] = []


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--map="):
			map = int(arg.trim_prefix("--map="))
		elif arg.begins_with("--shot-dir="):
			shot_dir = arg.trim_prefix("--shot-dir=")
	DisplayServer.window_set_vsync_mode(DisplayServer.VSYNC_DISABLED)
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport())
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
	GameFlow.quit_now()


## One movement for `seconds`: step(dt, t) moves the rig every frame; the frame times are reported.
func _run(name: String, seconds: float, step: Callable) -> void:
	frames.clear()
	var t := 0.0
	var last := Time.get_ticks_usec()
	var shot := shot_dir.is_empty()
	while t < seconds:
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		var dt := float(now - last) / (US_PER_MS * US_PER_MS)
		last = now
		frames.append(dt * US_PER_MS)
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
