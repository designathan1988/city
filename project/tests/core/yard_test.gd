## The yards of works, wherever a player puts them: a new street out of the town in each of several directions, zoned industrial,
## grown, the camera over it. The generated town has no industry, so nothing else tells a yard without a way from one that works:
## every dock has its ways, a lorry fits its own lane at the stop before a gate, and one truck goes the whole round.
## Run: godot --path project res://tests/core/yard_test.tscn -- --no-flow [--sites=N]
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SITES := 2
const WARM_TICKS := 2400
const SAMPLE := 400
const PX := 0.25 # metres a screen pixel covers: how near a click snaps to a road or a node
const ZONE_RADIUS_M := 160.0
const STREET_M := 300.0
const PROFILE := 0
const STEP_TICKS := 200
const GROW_STEPS := 60
const PLAN_STEPS := 900 # the planner advances by lorry_plan_work units each tick (lorry_traffic.cpp): the steps bound the work
const PLANNED_DOCKS := 12
const LEAVE_TRIES := 12
const LEAVE_STEP_M := 12.0
const ROUND_STEPS := 120
const CAMERA_UP_M := 30.0
const NO_STOP := 1 # traffic::YardReach YARD_NO_STOP


func _ready() -> void:
	var camera := Camera3D.new()
	add_child(camera)
	camera.make_current()
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	ticks(WARM_TICKS)
	_mark("warm")
	var sites := _arg("--sites=", SITES)
	var sample: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	for i in sites:
		await _site(camera, _far_in(sample, TAU * i / sites, TAU / sites))
	await _round()
	finish()


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
	var zoned: Dictionary = CityStore.dispatch(&"paint_zone", [at, ZONE_RADIUS_M, DataTables.zone_code("industrial")])
	print("site ", at, " road ", built.ok, " ", built.error, " zoned sides ", zoned.zoned_sides)
	camera.global_position = at + Vector3.UP * CAMERA_UP_M
	for i in GROW_STEPS:
		await get_tree().process_frame
		ticks(STEP_TICKS)
		if CityStore.query(&"dock_count", []) > before:
			break
	print("  docks ", CityStore.query(&"dock_count", []) - before)
	var s: Dictionary = CityStore.query(&"park_sample", [])
	var had: int = s.docks_planned
	for i in PLAN_STEPS:
		if s.docks_planned >= had + PLANNED_DOCKS:
			break
		await get_tree().process_frame
		ticks(STEP_TICKS)
		s = CityStore.query(&"park_sample", [])
	check(s.docks_planned > 0, "docks planned at %s: %d" % [at, s.docks_planned])
	check(s.docks_reached[NO_STOP] == 0, "no dock without room for the lorry in its own lane at %s: %d" % [at, s.docks_reached[NO_STOP]])
	check(s.docks_with_way == s.docks_planned, "every dock has a way at %s: %d of %d (reach by kind %s)" % [at, s.docks_with_way, s.docks_planned, s.docks_reached])
	_mark("planned")
	for y: Dictionary in s.get("yard_costs", []):
		print("  cost of yard ", y)
	print("  yards ", s.yards, " gates ", s.yard_gates, " planned ", s.docks_planned, " with a way ", s.docks_with_way, " stopped at ", s.docks_reached, " calls ", s.lorry_calls, " slowest plan ", s.yard_slowest_ms, " ms, slowest tick ", s.yard_tick_ms, " ms")


## A lorry called, backed onto its dock, unloaded and gone: the camera stays over the last works.
func _round() -> void:
	var s: Dictionary = CityStore.query(&"park_sample", [])
	for i in ROUND_STEPS:
		if s.lorries_left > 0:
			break
		await get_tree().process_frame
		ticks(STEP_TICKS)
		s = CityStore.query(&"park_sample", [])
	_mark("end")
	print("lorries (called, docked, left) ", s.lorries_sent, " ", s.lorries_docked, " ", s.lorries_left)
	check(s.lorries_left > 0, "a lorry made the cycle: %d" % s.lorries_left)


## Where the traffic stands, as one number: two runs of the same map and seed print the same ones.
func _mark(what: String) -> void:
	var p: Dictionary = CityStore.query(&"traffic_probe", [SAMPLE])
	print("mark ", what, " ", str(p.sample).hash(), " vehicles ", p.vehicles, " buildings ", CityStore.snapshot.get("buildings"))


func _arg(prefix: String, fallback: int) -> int:
	for a in OS.get_cmdline_user_args():
		if a.begins_with(prefix):
			return int(a.trim_prefix(prefix))
	return fallback
