## Visual check of the buses at their stops and of the run of track being laid: a bus line along the busiest street, its first bus
## photographed through its dwell at a stop (a sequence, from the kerb, from the street and from above: riders off by the middle door,
## walkers called up to the front door, who sits inside), and a run of train track and of metro laid out as the transport tool
## shows it (see-through over the ground). The sim stands still between shots and moves only by ticks.
## Run (hidden, through tools/tasks): godot --path project --resolution 1600x900 res://tests/visual/bus_stop_look.tscn -- --no-flow --capture-dir=<dir>
extends Node3D

const SETTLE_FRAMES := 6
const JUMP_FRAMES := 30
const NEAR_PER_M := 0.004
const MIN_NEAR_M := 0.05
const MAX_NEAR_M := 8.0
const HOUR := 16.0
const STEP_M := 60.0
const STOPS_TRIED := 12
const WAIT_STEP_TICKS := 5
const WAIT_TICKS := 12000
const SEQUENCE := 8 # shots through a dwell
const SEQUENCE_TICKS := 30 # 1.5 s between them
# Views of the bus: [along its heading, out to its kerb side, up, height looked at], metres from its middle.
const KERB_VIEW := [-9.0, 7.5, 2.2, 1.2]
const KERB_LOW := [3.0, 5.0, 1.6, 1.3]
const STREET_VIEW := [-6.0, -7.0, 3.2, 1.4]
const ABOVE_VIEW := [-1.0, 1.5, 9.0, 1.0]
const RUN_FROM := Vector3(-60.0, 0, 70.0) # off the bus street, among the blocks
const RUN_LONG_M := 120.0
const RUN_BEND_M := 40.0
const RUN_VIEW := [40.0, 35.0, 30.0] # back along the run, out to its side, up
const RUN_CLOSE := [12.0, 9.0, 4.0]

var world: CityWorld
var camera: Camera3D
var out_dir := "user://captures"
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	camera = Camera3D.new()
	camera.far = 30000.0
	camera.fov = 50.0
	add_child(camera)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	add_child(RoadcraftHud.new())
	Settings.apply(world, get_viewport())
	CityStore.new_map(0, 1, true, true)
	camera.global_position = Vector3(0, 300, 300)
	camera.look_at(Vector3.ZERO)
	await CaptureWait.until_drawn(self, 2, "city")
	UiDriver.at_hour(world, HOUR)
	await _bus_stop()
	await _track_preview()
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


func _shot(name: String, from: Vector3, at: Vector3) -> void:
	var settle := JUMP_FRAMES if camera.global_position.distance_to(from) > STEP_M else SETTLE_FRAMES
	camera.global_position = from
	camera.look_at(at)
	camera.near = clampf(from.distance_to(at) * NEAR_PER_M, MIN_NEAR_M, MAX_NEAR_M)
	await CaptureWait.until_drawn(self, settle, name)
	var path := "%s/%s.png" % [out_dir, name]
	get_viewport().get_texture().get_image().save_png(path)
	print("shot: ", path)


func _bus_view(name: String, run: Dictionary, view: Array) -> void:
	var ahead: Vector3 = run.bus_heading
	var kerb := Vector3(-ahead.z, 0, ahead.x)
	var mid: Vector3 = run.bus_at
	await _shot(name, mid + ahead * view[0] + kerb * view[1] + Vector3.UP * view[2], mid + Vector3.UP * view[3])


func _bus_stop() -> void:
	var probe: Dictionary = CityStore.query(&"traffic_probe", [1])
	CityStore.step_ticks(10)
	var ground: Vector3 = probe.busy_crossing
	for i in STOPS_TRIED:
		CityStore.dispatch(&"transit_stop", [ground + Vector3(STEP_M * (i - STOPS_TRIED * 0.5), 0, 0)])
	var ids := PackedInt32Array()
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		ids.append(s.id)
	if not CityStore.dispatch(&"transit_line", [ids]).get("ok", false):
		failures += 1
		print("FAIL no bus line")
		return
	var run: Dictionary = {}
	for tick in range(0, WAIT_TICKS, WAIT_STEP_TICKS):
		run = CityStore.query(&"transit_run")
		if run.has("first_bus") and float(run.first_bus[1]) > 0 and int(run.first_bus[0]) > 0:
			break
		CityStore.step_ticks(WAIT_STEP_TICKS)
	if not run.has("bus_at") or float(run.first_bus[1]) <= 0:
		failures += 1
		print("FAIL no bus at a stop")
		return
	print("at the stop: ", run)
	for k in SEQUENCE:
		run = CityStore.query(&"transit_run")
		print("dwell %d: riders %s, off %s, on %s" % [k, run.riders, run.alighted, run.boarded])
		await _bus_view("bus_kerb_%02d" % k, run, KERB_VIEW)
		if k % 2 == 0:
			await _bus_view("bus_door_%02d" % k, run, KERB_LOW)
		CityStore.step_ticks(SEQUENCE_TICKS)
	run = CityStore.query(&"transit_run")
	await _bus_view("bus_street", run, STREET_VIEW)
	await _bus_view("bus_above", run, ABOVE_VIEW)


func _track_preview() -> void:
	var ground: Vector3 = CityStore.query(&"traffic_probe", [0]).busy_crossing
	var train := CityStore.code(&"TransitMode", &"TRAIN")
	var a: Vector3 = CityStore.query(&"transit_snap", [ground + RUN_FROM, train]) # on the ground there: no track yet to snap to
	var b: Vector3 = CityStore.query(&"transit_snap", [a + Vector3(RUN_LONG_M, 0, 0), train])
	var c: Vector3 = CityStore.query(&"transit_snap", [b + Vector3(RUN_BEND_M, 0, RUN_BEND_M), train])
	var run := PackedVector3Array([a, b, c])
	CityStore.query(&"transit_xray", [true])
	for mode: StringName in [&"TRAIN", &"METRO"]:
		var label := String(mode).to_lower()
		CityStore.query(&"transit_preview", [run, CityStore.code(&"TransitMode", mode)])
		await _shot("preview_%s" % label, b + Vector3(-RUN_VIEW[0], RUN_VIEW[2], RUN_VIEW[1]), b)
		await _shot("preview_%s_close" % label, b + Vector3(-RUN_CLOSE[0], RUN_CLOSE[2], RUN_CLOSE[1]), b)
	CityStore.query(&"transit_preview", [PackedVector3Array(), train])
	CityStore.query(&"transit_xray", [false])
