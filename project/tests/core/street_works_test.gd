## Core test of the Street tab through the store: a line of poles along a footway, its refusal off the footways, a pole and a
## barrier taken away, items placed and refused, a piece of the street's own kit cut, each undone and redone; then a save
## and an open bring the same records back.
## Run: godot --headless --path project res://tests/core/street_works_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const REACH := 6.0
const LAMPS_ALL := 2
const URBAN := 0 # drawers.json pole_kinds
const RURAL := 1
const ITEM_BENCH := 2
const ITEM_BIN := 3
const ITEM_SIGN := 10
const FENCE := 0
const SIGN_STOP := 0
const RUN_M := 120.0
const RUN_TRIES := 24
const MIN_RUN_POLES := 3
const SCAN_STEPS := 200
const SCAN_ROWS := 12
const SCAN_M := 3.0
## The junction a plate governs is the node nearest it within this; its control as the player sets it (NodeControl CTL_PRIORITY)
## and as the traffic runs it (Control::PRIORITY).
const NODE_FIND_M := 60.0
const CTL_PRIORITY := 2
const CONTROL_PRIORITY := 1
const FENCE_M := 12.0
const AWAY_M := 3000.0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var a := _on_footway()
	check(a != Vector3.INF, "a pavement to start on")
	if a != Vector3.INF:
		_poles(a)
		_items(a)
		_barriers(a)
		_rule_and_kit(a)
		StreetMoveCases.new(self).run()
		await StreetSaveCases.new(self).run()
	finish()


## A point on a footway: asked of the core, walking out from the middle of the town.
func _on_footway() -> Vector3:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var mid: Vector3 = (b.min + b.max) * 0.5
	for i in 400:
		for j in 8:
			var p := mid + Vector3(i * 2.0, 0, j * 12.0)
			if CityStore.query(&"preview_item", [p, ITEM_BIN, 1.0]).get("ok", false):
				return CityStore.query(&"preview_item", [p, ITEM_BIN, 1.0]).position
	return Vector3.INF


func _counts() -> Dictionary:
	return CityStore.query(&"street_counts")


## The first stretch of footway the planner can string from `a`: asked around a circle of RUN_M.
func _run_from(a: Vector3) -> Vector3:
	for i in RUN_TRIES:
		var dir := Vector3.RIGHT.rotated(Vector3.UP, TAU * i / RUN_TRIES)
		var plan: Dictionary = CityStore.query(&"preview_pole_run", [a, a + dir * RUN_M, REACH, LAMPS_ALL, URBAN])
		if plan.get("ok", false) and plan.poles.size() >= MIN_RUN_POLES:
			return a + dir * RUN_M
	return Vector3.INF


func _poles(a: Vector3) -> void:
	var b := _run_from(a)
	check(b != Vector3.INF, "a plan of poles")
	if b == Vector3.INF:
		return
	var plan: Dictionary = CityStore.query(&"preview_pole_run", [a, b, REACH, LAMPS_ALL, URBAN])
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"build_pole_run", [a, b, REACH, LAMPS_ALL, URBAN])
	check(r.get("ok", false), "line built")
	var after := _counts()
	var fresh: int = plan.poles.filter(func(p: Dictionary) -> bool: return not p.standing).size()
	check(after.poles == before.poles + fresh and after.spans == before.spans + plan.poles.size() - 1, "poles and spans counted")
	CityStore.dispatch(&"undo")
	check(_counts().poles == before.poles and _counts().spans == before.spans, "undoing the line")
	CityStore.dispatch(&"redo")
	check(_counts().poles == after.poles and _counts().spans == after.spans, "redoing the line")
	var far := a + Vector3(AWAY_M, 0, AWAY_M)
	var refused: Dictionary = CityStore.query(&"preview_pole_run", [far, far + Vector3(10, 0, 0), REACH, LAMPS_ALL, URBAN])
	check(refused.get("error", "") == "hint.pole.offFootway", "off the pavement refuses")
	var gone: Dictionary = CityStore.dispatch(&"remove_street", [plan.poles[0].position, 1.0])
	check(gone.get("ok", false) and _counts().poles == after.poles - 1 and _counts().spans == after.spans - 1, "a pole leaves with its wire")
	CityStore.dispatch(&"undo")
	check(_counts().poles == after.poles and _counts().spans == after.spans, "undoing the removal")
	_rural(b)


## A rural run strung on from the urban one's end: its new poles are rural, the urban pole it starts from stays urban.
func _rural(a: Vector3) -> void:
	var b := _run_from(a)
	check(b != Vector3.INF, "plano rural")
	if b == Vector3.INF:
		return
	var plan: Dictionary = CityStore.query(&"preview_pole_run", [a, b, REACH, LAMPS_ALL, RURAL])
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"build_pole_run", [a, b, REACH, LAMPS_ALL, RURAL])
	check(r.get("ok", false), "rural line built")
	var fresh: Array = plan.poles.filter(func(p: Dictionary) -> bool: return not p.standing)
	check(fresh.all(func(p: Dictionary) -> bool: return p.kind == RURAL), "new rural poles")
	check(plan.poles.filter(func(p: Dictionary) -> bool: return p.standing).all(func(p: Dictionary) -> bool: return p.kind == URBAN), "a pole reused stays urban")
	check(_counts().rural_poles == before.rural_poles + fresh.size(), "count of the rural %s" % _counts())


## A place on the footway where `kind` can stand, walking along from `a`.
func _free_for(kind: int, a: Vector3) -> Vector3:
	for i in SCAN_STEPS:
		for j in SCAN_ROWS:
			var at: Dictionary = CityStore.query(&"preview_item", [a + Vector3(i * SCAN_M, 0, j * SCAN_M), kind, 1.0])
			if at.get("ok", false):
				return at.position
	return Vector3.INF


func _items(a: Vector3) -> void:
	var at := _free_for(ITEM_BENCH, a)
	check(at != Vector3.INF, "a place for the bench")
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"place_item", [at, ITEM_BENCH, 0, "", REACH])
	check(r.get("ok", false), "banco colocado")
	check(_counts().items == before.items + 1, "banco contado")
	var twice: Dictionary = CityStore.dispatch(&"place_item", [at, ITEM_BENCH, 0, "", 1.0])
	check(twice.get("error", "") == "hint.streetscape.occupied", "lugar ocupado recusa")
	CityStore.dispatch(&"undo")
	check(_counts().items == before.items, "undoing the bench")
	CityStore.dispatch(&"redo")
	check(_counts().items == before.items + 1, "redoing the bench")
	var off: Dictionary = CityStore.dispatch(&"place_item", [a + Vector3(AWAY_M, 0, AWAY_M), ITEM_SIGN, SIGN_STOP, "", 1.0])
	check(off.get("error", "") == "hint.streetscape.offFootway", "a sign off the pavement refuses")


## Open ground for a fence: a run the core does not refuse, scanned out from `a`.
func _open_run(a: Vector3) -> PackedVector3Array:
	for i in SCAN_STEPS:
		for j in SCAN_ROWS:
			var o := a + Vector3(i * FENCE_M, 0, (j + 1) * FENCE_M)
			var run := PackedVector3Array([o, o + Vector3(FENCE_M, 0, 0)])
			if CityStore.query(&"preview_barrier", [PackedVector3Array([run[0]]), run[1], FENCE]).get("ok", false):
				return run
	return PackedVector3Array()


func _barriers(a: Vector3) -> void:
	var run := _open_run(a)
	check(run.size() == 2, "open ground for the fence")
	if run.is_empty():
		return
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"build_barrier", [run, FENCE])
	check(r.get("ok", false), "fence built")
	check(_counts().barriers == before.barriers + 1, "barreira contada")
	CityStore.dispatch(&"undo")
	check(_counts().barriers == before.barriers, "undoing the barrier")
	CityStore.dispatch(&"redo")
	check(_counts().barriers == before.barriers + 1, "redoing the barrier")
	var short: Dictionary = CityStore.dispatch(&"build_barrier", [PackedVector3Array([run[0], run[0] + Vector3(0.1, 0, 0)]), FENCE])
	check(short.get("error", "") == "hint.barrier.short", "barreira curta recusa")
	var over_road: Dictionary = CityStore.dispatch(&"build_barrier", [PackedVector3Array([a, a + Vector3(0, 0, FENCE_M)]), FENCE])
	check(over_road.get("error", "") == "hint.barrier.road", "a fence on the pavement refuses")


## A Stop plate at an approach becomes the rule of that leg and undoes as one step; a piece of the street's own kit taken
## away is a cut, and comes back with undo.
func _rule_and_kit(a: Vector3) -> void:
	var before := _counts()
	var ruled := false
	for i in SCAN_STEPS * SCAN_ROWS:
		var p := a + Vector3((i % SCAN_STEPS) * SCAN_M, 0, (i / SCAN_STEPS) * SCAN_M)
		var at: Dictionary = CityStore.query(&"preview_item", [p, ITEM_SIGN, 1.0])
		if not at.get("ok", false):
			continue
		var r: Dictionary = CityStore.dispatch(&"place_item", [at.position, ITEM_SIGN, SIGN_STOP, "", 1.0])
		if r.get("note", "") == "hint.streetscape.rule.stop":
			ruled = true
			check(_counts().items == before.items, "the rule is no piece")
			ticks(1) # the traffic takes the junction's control up on its next step
			var node: Dictionary = CityStore.query(&"node_at", [at.position, NODE_FIND_M / DataTables.tuning("road_snap_node_px")])
			var control: int = CityStore.query(&"node_traffic", [node.get("node", -1)]).get("control", -1)
			check(node.get("ctl", -1) == CTL_PRIORITY and control == CONTROL_PRIORITY,
				"the sign puts the junction on priority, as the 4.0: ctl %s, traffic %s" % [node.get("ctl", -1), control])
			CityStore.dispatch(&"undo")
			var again: Dictionary = CityStore.dispatch(&"place_item", [at.position, ITEM_SIGN, SIGN_STOP, "", 1.0])
			check(again.get("note", "") == "hint.streetscape.rule.stop", "the undone rule can come back")
			CityStore.dispatch(&"undo")
			break
		CityStore.dispatch(&"undo")
	check(ruled, "a Stop sign becomes a rule")
	var cut := false
	for i in SCAN_STEPS * SCAN_ROWS:
		var p := a + Vector3((i % SCAN_STEPS) * SCAN_M, 0, (i / SCAN_STEPS) * SCAN_M)
		var at: Dictionary = CityStore.query(&"preview_item", [p, ITEM_BENCH, 1.0])
		if at.get("error", "") != "hint.streetscape.occupied":
			continue
		var was := _counts()
		var r: Dictionary = CityStore.dispatch(&"remove_street", [at.position, REACH])
		if r.get("ok", false) and _counts().cuts == was.cuts + 1:
			cut = true
			CityStore.dispatch(&"undo")
			check(_counts().cuts == was.cuts, "undoing the cut")
			break
		if r.get("ok", false):
			CityStore.dispatch(&"undo")
	check(cut, "a piece of the kit cut")
