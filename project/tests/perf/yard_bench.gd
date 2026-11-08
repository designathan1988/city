## The yard planner's bench: the same yards for every version of the planner. The game plans a yard by the ticks, so how fast it plans
## changes the city that grows meanwhile and with it the yards a run meets; here the city is made once (yard_test's sites, zoned
## industrial, a fixed number of ticks of growth) and saved, and every run plans all its yards at once from that save
## (plan_all_yards): docks, docks with a way, how far each got, poses and time, yard by yard. A tool, not one of the tests.
## Run: godot --path project res://tests/perf/yard_bench.tscn -- --no-flow [--make] [--slot=name]
extends Node3D

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
const GROW_STEPS := 60 # yard_test's growth at each site, every step of it (yard_test stops at the first dock)
const SLOT := "yard_bench"


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		_quit("FAIL: boot")
		return
	var slot := _arg("--slot=", SLOT)
	if OS.get_cmdline_user_args().has("--make"):
		await _make(slot)
	CityStore.new_map(TOWN_MAP, SEED, false) # nothing of the city made survives the load
	var opened: Dictionary = await CityStore.saves.load_game(slot)
	if not opened.get("ok", false):
		_quit("FAIL: opening %s %s" % [slot, opened])
		return
	var yards: Array = CityStore.query(&"plan_all_yards")
	var docks := 0
	var ways := 0
	var poses := 0
	var ms := 0.0
	for y: Dictionary in yards:
		print("yard ", y)
		docks += int(y.docks)
		ways += int(y.with_way)
		ms += float(y.total_ms)
		for k: Array in y.searches:
			poses += int(k[4])
	print("bench: %d yards, %d docks, %d with a way, %d poses, %.0f ms" % [yards.size(), docks, ways, poses, ms])
	_quit("")


## The city of the bench: yard_test's sites, each a street out of the town zoned industrial and grown a fixed number of ticks.
func _make(slot: String) -> void:
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	CityStore.step_ticks(WARM_TICKS)
	var sample: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	for i in SITES:
		var far := _far_in(sample, TAU * i / SITES, TAU / SITES)
		var end := far + Vector3(far.x, 0, far.z).normalized() * STREET_M
		CityStore.dispatch(&"build_road", [far, end, false, Vector3.ZERO, PROFILE, PX])
		CityStore.dispatch(&"paint_zone", [(far + end) * 0.5, ZONE_RADIUS_M, ZoneTable.code("industrial")])
		for k in GROW_STEPS:
			await get_tree().process_frame
			CityStore.step_ticks(STEP_TICKS)
	var outcome := {}
	CityStore.saved.connect(func(_s: String, r: Dictionary) -> void: outcome.merge(r), CONNECT_ONE_SHOT)
	CityStore.saves.save_game(slot)
	while CityStore.saves.save_busy():
		await get_tree().process_frame
	print("bench saved at ", slot, " ", outcome, " docks ", CityStore.query(&"dock_count", []))


## The vehicle farthest out inside a slice of the compass (yard_test).
func _far_in(sample: Array, angle: float, width: float) -> Vector3:
	var far := Vector3.ZERO
	for p: Vector3 in sample:
		var flat := Vector2(p.x, p.z)
		if absf(angle_difference(flat.angle(), angle)) <= width * 0.5 and flat.length() > Vector2(far.x, far.z).length():
			far = p
	return far


func _arg(prefix: String, fallback: String) -> String:
	for a in OS.get_cmdline_user_args():
		if a.begins_with(prefix):
			return a.trim_prefix(prefix)
	return fallback


func _quit(failed: String) -> void:
	if not failed.is_empty():
		printerr(failed)
	get_tree().quit(1 if not failed.is_empty() else 0)
