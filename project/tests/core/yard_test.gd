## The yards of works, wherever a player puts them: a new street out of the town in each of several directions, zoned industrial,
## grown, the camera over it. The generated town has no industry, so nothing else tells a yard without a way from one that works:
## nearly every dock has its ways, a lorry fits its own lane at the stop before a gate, and one truck goes the whole round.
## Run: godot --path project res://tests/core/yard_test.tscn -- --no-flow [--sites=N]
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SITES := 2
const WARM_TICKS := 2400
const SAMPLE := 400
const PX := 0.25 # metres a screen pixel covers: how near a click snaps to a road or a node
const ZONE_INDUSTRIAL := 3 # catalog/zones.json order
const ZONE_RADIUS_M := 160.0
const STREET_M := 300.0
const PROFILE := 0
const STEP_TICKS := 200
const GROW_STEPS := 60
const PLAN_STEPS := 900 # the plans run one at a time on a worker, by the clock: the steps only bound the wait
const PLANNED_DOCKS := 12
const LEAVE_TRIES := 12
const LEAVE_STEP_M := 12.0
const ROUND_STEPS := 120
const CAMERA_UP_M := 30.0
const NO_STOP := 1 # traffic::YardReach YARD_NO_STOP
const WAY_SHARE := 0.9 # measured 0.99 (105 of 106 docks); 0.1 with a lorry too wide for its lane and one search for each way

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var camera := Camera3D.new()
	add_child(camera)
	camera.make_current()
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	CityStore.step_ticks(WARM_TICKS)
	_mark("aquecido")
	var sites := _arg("--sites=", SITES)
	var sample: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	for i in sites:
		await _site(camera, _far_in(sample, TAU * i / sites, TAU / sites))
	await _round()
	print("FAILURES: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The vehicle farthest out inside a slice of the compass.
func _far_in(sample: Array, angle: float, width: float) -> Vector3:
	var far := Vector3.ZERO
	for p: Vector3 in sample:
		var flat := Vector2(p.x, p.z)
		if absf(angle_difference(flat.angle(), angle)) <= width * 0.5 and flat.length() > Vector2(far.x, far.z).length():
			far = p
	return far


func _site(camera: Camera3D, far: Vector3) -> void:
	var before: int = CityStore.query(&"dock_count", [])
	# a vehicle stands anywhere on its street: a new street leaves it clear of the junctions either side, so it is walked along until the rule takes it
	var out := Vector3(far.x, 0, far.z).normalized()
	var along := Vector3(out.z, 0, -out.x)
	var built := {}
	var from := far
	for k in LEAVE_TRIES:
		from = far + along * (LEAVE_STEP_M * (k + 1) / 2 * (1 if k % 2 == 0 else -1))
		built = CityStore.dispatch(&"build_road", [from, from + out * STREET_M, false, Vector3.ZERO, PROFILE, PX])
		if built.ok:
			break
	var at := from + out * STREET_M * 0.5
	var zoned: Dictionary = CityStore.dispatch(&"paint_zone", [at, ZONE_RADIUS_M, ZONE_INDUSTRIAL])
	print("lugar ", at, " road ", built.ok, " ", built.error, " zona ", zoned.zoned_sides)
	camera.global_position = at + Vector3.UP * CAMERA_UP_M
	for i in GROW_STEPS:
		await get_tree().process_frame
		CityStore.step_ticks(STEP_TICKS)
		if CityStore.query(&"dock_count", []) > before:
			break
	print("  docas ", CityStore.query(&"dock_count", []) - before)
	var s: Dictionary = CityStore.query(&"park_sample", [])
	var had: int = s.docks_planned
	for i in PLAN_STEPS:
		if s.docks_planned >= had + PLANNED_DOCKS:
			break
		await get_tree().process_frame
		CityStore.step_ticks(STEP_TICKS)
		s = CityStore.query(&"park_sample", [])
	_check(s.docks_planned > 0, "no dock planned at %s" % at)
	_check(s.docks_reached[NO_STOP] == 0, "%d docks with no room for the lorry in its own lane at %s" % [s.docks_reached[NO_STOP], at])
	_check(s.docks_with_way >= s.docks_planned * WAY_SHARE, "%d of %d docks with a way at %s" % [s.docks_with_way, s.docks_planned, at])
	_mark("planejado")
	for y: Dictionary in s.get("yard_costs", []):
		print("  cost of yard ", y)
	print("  yards ", s.yards, " gates ", s.yard_gates, " planejadas ", s.docks_planned, " with a way ", s.docks_with_way, " stopped at ", s.docks_reached, " chamadas ", s.lorry_calls, " pior plano ", s.yard_slowest_ms, " ms, pior tick ", s.yard_tick_ms, " ms")


## A lorry called, backed onto its dock, unloaded and gone: the camera stays over the last works.
func _round() -> void:
	var s: Dictionary = CityStore.query(&"park_sample", [])
	for i in ROUND_STEPS:
		if s.lorries_left > 0:
			break
		await get_tree().process_frame
		CityStore.step_ticks(STEP_TICKS)
		s = CityStore.query(&"park_sample", [])
	_mark("fim")
	print("lorries (called, docked, left) ", s.lorries_sent, " ", s.lorries_docked, " ", s.lorries_left)
	_check(s.lorries_left > 0, "no lorry made the cycle")


## Where the traffic stands, as one number: two runs of the same map and seed print the same ones.
func _mark(what: String) -> void:
	var p: Dictionary = CityStore.query(&"traffic_probe", [SAMPLE])
	print("marca ", what, " ", str(p.sample).hash(), " vehicles ", p.vehicles, " buildings ", CityStore.snapshot.get("buildings"))


func _arg(prefix: String, fallback: int) -> int:
	for a in OS.get_cmdline_user_args():
		if a.begins_with(prefix):
			return int(a.trim_prefix(prefix))
	return fallback


func _check(ok: bool, what: String) -> void:
	if ok:
		return
	failures += 1
	printerr("FAIL: ", what)
