## What the main thread pays for the script layer's reads of the core while the simulation runs (camera_pan measures the
## core's _process and the GPU, not this): how long the store's snapshot refresh keeps the frame waiting (the play does it
## every SNAPSHOT_INTERVAL_S), set against a read that takes only the map lock (height_at) and one that takes only the traffic
## lock (transit_run); then what each read a view makes every frame costs. The city is a big one at the fastest speed, where
## a traffic step is longest. Last, the game's job pool: the keys it still keeps once its queue is empty.
## Run: godot --path project --resolution 1600x900 res://tests/perf/main_thread_probe.tscn -- --no-flow [--map=N] [--seconds=S]
extends Node3D

const SEED := 1
const BUILD_FRAMES := 1200
const SETTLE_FRAMES := 30
const RUN_S := 20.0
const READS := 300 # calls of each per-frame read timed one after another
const US_PER_MS := 1000.0
const PERCENTILES := [0.5, 0.95, 0.99]

var map := DataTables.map_index("metropolis")
var seconds := RUN_S


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--map="):
			map = int(arg.trim_prefix("--map="))
		elif arg.begins_with("--seconds="):
			seconds = float(arg.trim_prefix("--seconds="))
	var world := CityWorld.new()
	add_child(world)
	Settings.apply(world, get_viewport())
	var rig := CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		GameFlow.quit_now()
		return
	print("city: ", CityStore.new_map(map, SEED, true))
	add_child(RoadcraftHud.new())
	var bounds: Dictionary = CityStore.query(&"city_bounds")
	rig.look_at_ground((bounds.min + bounds.max) * 0.5)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	var waited := 0
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await get_tree().process_frame
		waited += 1
	CityStore.dispatch(&"set_speed", [CityStore.code(&"SimSpeed", &"FASTEST")])
	await _snapshot_wait()
	_reads()
	var keys: int = await _idle_keys()
	print("job pool: keys kept with the queue empty: %d" % keys)
	GameFlow.quit_now()


## The snapshot refreshed each frame for `seconds` at the fastest speed: its time is the frame's wait for the core's locks.
func _snapshot_wait() -> void:
	var ticks_before := _step_ms_total()
	var times: Array[float] = []
	var frames: Array[float] = []
	var core_ms: Array[float] = []
	var gpu: Array[float] = []
	var vp := get_viewport().get_viewport_rid()
	RenderingServer.viewport_set_measure_render_time(vp, true)
	var t := 0.0
	var last := Time.get_ticks_usec()
	while t < seconds: # the play: only the store's refresh, as the HUD gets it
		var a := Time.get_ticks_usec()
		CityStore.refresh()
		times.append(float(Time.get_ticks_usec() - a) / US_PER_MS)
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		frames.append(float(now - last) / US_PER_MS)
		core_ms.append(float(CityStore.query(&"render_cost").get("process", 0.0)))
		gpu.append(RenderingServer.viewport_get_measured_render_time_gpu(vp))
		t += float(now - last) / US_PER_MS / US_PER_MS
		last = now
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	print("traffic: %d vehicles, %d people; traffic step summed %.0f ms in %.1f s" % [
		probe.get("vehicles", 0), probe.get("people", 0), _step_ms_total() - ticks_before, t])
	print("snapshot (CityStore.refresh, the main thread's wait): %s" % _stats(times))
	print("quadros: %s" % _stats(frames))
	print("  core (its _process, render_cost): %s" % _stats(core_ms))
	print("  gpu: %s" % _stats(gpu))
	await _locks()


## Whose lock the frame would wait for: a read that takes only the map lock shared and one that takes only the traffic's.
func _locks() -> void:
	var map_only: Array[float] = []
	var traffic_only: Array[float] = []
	var t := 0.0
	var last := Time.get_ticks_usec()
	while t < seconds:
		var a := Time.get_ticks_usec()
		CityStore.query(&"height_at", [0.0, 0.0])
		var b := Time.get_ticks_usec()
		CityStore.query(&"transit_run")
		var c := Time.get_ticks_usec()
		map_only.append(float(b - a) / US_PER_MS)
		traffic_only.append(float(c - b) / US_PER_MS)
		await get_tree().process_frame
		var now := Time.get_ticks_usec()
		t += float(now - last) / US_PER_MS / US_PER_MS
		last = now
	print("lock diagnosis: map_mutex alone (height_at): %s" % _stats(map_only))
	print("  traffic_mutex alone (transit_run): %s" % _stats(traffic_only))


## Each read a view makes every frame, timed READS times in a row: as the core answers it, and as the store keeps it.
func _reads() -> void:
	var reads := [&"fog_field", &"fog_revision", &"transit_plan", &"junctions", &"sky_revision", &"landscape"]
	for name in reads:
		print("read %s: %s" % [name, _timed(func() -> void: CityStore.query(name))])
	for name: StringName in CityStore.CACHED_BY:
		print("read %s through the CityStore cache: %s" % [name, _timed(func() -> void: CityStore.cached(name))])
	print("junctions on the list: %d" % CityStore.query(&"junctions").size())


func _timed(read: Callable) -> String:
	var times: Array[float] = []
	for i in READS:
		var a := Time.get_ticks_usec()
		read.call()
		times.append(float(Time.get_ticks_usec() - a) / US_PER_MS)
	return _stats(times)


## The game's pool once its queue ran dry (the simulation paused, no chunk asked): the keys it still keeps.
func _idle_keys() -> int:
	CityStore.dispatch(&"set_speed", [0])
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	return int(CityStore.query(&"job_pool_probe").get("game_keys", -1))


func _step_ms_total() -> float:
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	var total := 0.0
	for x in probe.get("step_ms", []):
		total += float(x)
	return total


func _stats(values: Array[float]) -> String:
	if values.is_empty():
		return "-"
	var s := values.duplicate()
	s.sort()
	var parts := []
	for p in PERCENTILES:
		parts.append("p%d %.3f ms" % [int(p * 100.0), s[mini(s.size() - 1, int(p * s.size()))]])
	var sum := 0.0
	for v in s:
		sum += v
	return "n %d, mean %.3f ms, %s, max %.3f ms" % [s.size(), sum / s.size(), ", ".join(parts), s[-1]]
