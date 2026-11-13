## The yards of works, wherever a player puts them: a new street out of the town in each of several directions, zoned industrial,
## grown, the camera over it. The generated town has no industry, so nothing else tells a yard without a way from one that works:
## every dock has its ways, a lorry fits its own lane at the stop before a gate, and one truck goes the whole round. Then the records
## kept true (doc/specs/003-roadcraft-features/sheets/backbone-records-true.md items 9 and 10): a plan kept while the camera is away
## still sends lorries to the right docks when the lot's bays are read again into other slots, and a lorry coming to a dock whose
## building goes gives up.
## Run: godot --path project res://tests/core/yard_test.tscn -- --no-flow [--sites=N]
extends TestCase

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
const PLAN_STEPS := 900 # the planner advances by lorry_plan_work units each tick (lorry_traffic.cpp): the steps bound the work
const PLANNED_DOCKS := 12
const LEAVE_TRIES := 12
const LEAVE_STEP_M := 12.0
const ROUND_STEPS := 120
const CAMERA_UP_M := 30.0
const NO_STOP := 1 # traffic::YardReach YARD_NO_STOP
const AWAY_M := 2500.0 # the camera this far out past the works: past park_window_out_m (traffic.json 1100) from every lot
const AWAY_HOP_M := 80.0 # and on by this much each step, past park_window_step_m (64): the parking looks round again
const AWAY_STEPS := 40
const COMING_TRIES := 1200
const LOOK_TICKS := 20 # a second of the city: a lorry is seen while it is still coming
const SETTLE_TICKS := 4 # under lorry_call_timeout_s (traffic.json 180 s): a lorry left bound is still there
const EL_DOCK := 19 # b40 Element


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
	var at := Vector3.ZERO
	for i in sites:
		at = await _site(camera, _far_in(sample, TAU * i / sites, TAU / sites))
	await _round()
	await _away_and_back(camera, at)
	await _lorry_building_gone()
	finish()


## The vehicle farthest out inside a slice of the compass.
func _far_in(sample: Array, angle: float, width: float) -> Vector3:
	var far := Vector3.ZERO
	for p: Vector3 in sample:
		var flat := Vector2(p.x, p.z)
		if absf(angle_difference(flat.angle(), angle)) <= width * 0.5 and flat.length() > Vector2(far.x, far.z).length():
			far = p
	return far


## A works site out of the town in the direction of `far`, its yards planned; returns its middle, where the camera stays.
func _site(camera: Camera3D, far: Vector3) -> Vector3:
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
	return at


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


## The camera leaves the works until their lots are let go, and comes back: the lots' bays are read again into other slots of the
## parking, and the plans kept (they cost much) name their docks by place among the lot's bays, so a lorry docks again (item 9).
func _away_and_back(camera: Camera3D, at: Vector3) -> void:
	var yards: int = CityStore.query(&"park_sample", []).yards
	var out := Vector3(at.x, 0, at.z).normalized()
	var read: int = CityStore.query(&"traffic_probe", [0]).lots_read
	for i in AWAY_STEPS:
		if read == 0:
			break
		camera.global_position = at + out * (AWAY_M + AWAY_HOP_M * i) + Vector3.UP * CAMERA_UP_M
		await get_tree().process_frame
		ticks(STEP_TICKS)
		read = CityStore.query(&"traffic_probe", [0]).lots_read
	check(read == 0, "the works' lots let go behind the camera: %d still read" % read)
	var s: Dictionary = CityStore.query(&"park_sample", [])
	check(s.yards >= yards, "the yards' plans kept while the camera is away: %d of %d" % [s.yards, yards])
	camera.global_position = at + Vector3.UP * CAMERA_UP_M
	var docked: int = s.lorries_docked
	for i in ROUND_STEPS:
		await get_tree().process_frame
		ticks(STEP_TICKS)
		s = CityStore.query(&"park_sample", [])
		if s.lorries_docked > docked:
			break
	check(s.lorries_docked > docked, "a lorry docks again after the camera came back: %d -> %d (calls %s)" % [docked, s.lorries_docked, s.lorry_calls])
	var broken := _broken("parking.yard_docks_live")
	check(broken.is_empty(), "every planned dock names a dock of its own lot: %s" % [broken])


## A lorry still coming to a dock when its building goes: it gives up and drives on, none is left bound to a building gone (item 10).
## The lorries cycle together (lorry_most), so the one watched is the one just called: within a second of its call it is still on
## its way to the gate.
func _lorry_building_gone() -> void:
	var s: Dictionary = CityStore.query(&"park_sample", [])
	var sent: int = s.lorries_sent
	for i in COMING_TRIES:
		await get_tree().process_frame
		ticks(LOOK_TICKS)
		s = CityStore.query(&"park_sample", [])
		if s.lorries_sent > sent:
			break
	if not check(s.lorries_sent > sent, "a lorry called to a dock: %d sent" % s.lorries_sent):
		return
	var before: int = s.lorries
	var gone := 0
	for id: int in CityStore.query(&"list_buildings"):
		if not _has_dock(id):
			continue
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		gone += 1 if CityStore.dispatch(&"demolish", [b.position]).get("ok", false) else 0
	check(gone > 0, "the works with docks demolished: %d" % gone)
	ticks(SETTLE_TICKS)
	s = CityStore.query(&"park_sample", [])
	check(s.lorries < before, "the lorries bound to them gave up: %d of %d left, only those driving out" % [s.lorries, before])
	var broken := _broken("parking.lorry_target_live")
	check(broken.is_empty(), "no lorry bound to a building gone: %s" % [broken])


func _has_dock(id: int) -> bool:
	for row: Array in CityStore.query(&"lot_elements", [id]):
		if row[0] == EL_DOCK:
			return true
	return false


## The records the core's invariant `name` (feature.check) finds broken now.
func _broken(name: String) -> Array:
	var out := []
	for b: Dictionary in CityStore.query(&"broken_records"):
		if "%s.%s" % [b.feature, b.check] == name:
			for r in b.records:
				out.append(r)
	return out


## Where the traffic stands, as one number: two runs of the same map and seed print the same ones.
func _mark(what: String) -> void:
	var p: Dictionary = CityStore.query(&"traffic_probe", [SAMPLE])
	print("mark ", what, " ", str(p.sample).hash(), " vehicles ", p.vehicles, " buildings ", CityStore.snapshot.get("buildings"))


func _arg(prefix: String, fallback: int) -> int:
	for a in OS.get_cmdline_user_args():
		if a.begins_with(prefix):
			return int(a.trim_prefix(prefix))
	return fallback
