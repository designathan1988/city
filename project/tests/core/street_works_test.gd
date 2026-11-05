## Core test of the Street tab through the store: a line of poles along a footway, its refusal off the footways, a pole and a
## barrier taken away, items placed and refused, a piece of the street's own kit cut, each undone and redone; then a save
## and an open bring the same records back.
## Run: godot --headless --path project res://tests/core/street_works_test.tscn -- --no-flow
extends Node3D

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

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	var a := _on_footway()
	_check(a != Vector3.INF, "uma calçada para começar")
	if a != Vector3.INF:
		_poles(a)
		_items(a)
		_barriers(a)
		_rule_and_kit(a)
		StreetMoveCases.new(_check).run()
		await StreetSaveCases.new(self, _check).run()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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
	_check(b != Vector3.INF, "plano de postes")
	if b == Vector3.INF:
		return
	var plan: Dictionary = CityStore.query(&"preview_pole_run", [a, b, REACH, LAMPS_ALL, URBAN])
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"build_pole_run", [a, b, REACH, LAMPS_ALL, URBAN])
	_check(r.get("ok", false), "linha construída")
	var after := _counts()
	var fresh: int = plan.poles.filter(func(p: Dictionary) -> bool: return not p.standing).size()
	_check(after.poles == before.poles + fresh and after.spans == before.spans + plan.poles.size() - 1, "postes e vãos contados")
	CityStore.dispatch(&"undo")
	_check(_counts().poles == before.poles and _counts().spans == before.spans, "desfazer a linha")
	CityStore.dispatch(&"redo")
	_check(_counts().poles == after.poles and _counts().spans == after.spans, "refazer a linha")
	var far := a + Vector3(AWAY_M, 0, AWAY_M)
	var refused: Dictionary = CityStore.query(&"preview_pole_run", [far, far + Vector3(10, 0, 0), REACH, LAMPS_ALL, URBAN])
	_check(refused.get("error", "") == "hint.pole.offFootway", "fora da calçada recusa")
	var gone: Dictionary = CityStore.dispatch(&"remove_street", [plan.poles[0].position, 1.0])
	_check(gone.get("ok", false) and _counts().poles == after.poles - 1 and _counts().spans == after.spans - 1, "poste sai com seu fio")
	CityStore.dispatch(&"undo")
	_check(_counts().poles == after.poles and _counts().spans == after.spans, "desfazer a remoção")
	_rural(b)


## A rural run strung on from the urban one's end: its new poles are rural, the urban pole it starts from stays urban.
func _rural(a: Vector3) -> void:
	var b := _run_from(a)
	_check(b != Vector3.INF, "plano rural")
	if b == Vector3.INF:
		return
	var plan: Dictionary = CityStore.query(&"preview_pole_run", [a, b, REACH, LAMPS_ALL, RURAL])
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"build_pole_run", [a, b, REACH, LAMPS_ALL, RURAL])
	_check(r.get("ok", false), "linha rural construída")
	var fresh: Array = plan.poles.filter(func(p: Dictionary) -> bool: return not p.standing)
	_check(fresh.all(func(p: Dictionary) -> bool: return p.kind == RURAL), "postes novos rurais")
	_check(plan.poles.filter(func(p: Dictionary) -> bool: return p.standing).all(func(p: Dictionary) -> bool: return p.kind == URBAN), "poste reaproveitado continua urbano")
	_check(_counts().rural_poles == before.rural_poles + fresh.size(), "contagem dos rurais %s" % _counts())


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
	_check(at != Vector3.INF, "lugar para o banco")
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"place_item", [at, ITEM_BENCH, 0, "", REACH])
	_check(r.get("ok", false), "banco colocado")
	_check(_counts().items == before.items + 1, "banco contado")
	var twice: Dictionary = CityStore.dispatch(&"place_item", [at, ITEM_BENCH, 0, "", 1.0])
	_check(twice.get("error", "") == "hint.streetscape.occupied", "lugar ocupado recusa")
	CityStore.dispatch(&"undo")
	_check(_counts().items == before.items, "desfazer o banco")
	CityStore.dispatch(&"redo")
	_check(_counts().items == before.items + 1, "refazer o banco")
	var off: Dictionary = CityStore.dispatch(&"place_item", [a + Vector3(AWAY_M, 0, AWAY_M), ITEM_SIGN, SIGN_STOP, "", 1.0])
	_check(off.get("error", "") == "hint.streetscape.offFootway", "placa fora da calçada recusa")


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
	_check(run.size() == 2, "chão aberto para a cerca")
	if run.is_empty():
		return
	var before := _counts()
	var r: Dictionary = CityStore.dispatch(&"build_barrier", [run, FENCE])
	_check(r.get("ok", false), "cerca construída")
	_check(_counts().barriers == before.barriers + 1, "barreira contada")
	CityStore.dispatch(&"undo")
	_check(_counts().barriers == before.barriers, "desfazer a barreira")
	CityStore.dispatch(&"redo")
	_check(_counts().barriers == before.barriers + 1, "refazer a barreira")
	var short: Dictionary = CityStore.dispatch(&"build_barrier", [PackedVector3Array([run[0], run[0] + Vector3(0.1, 0, 0)]), FENCE])
	_check(short.get("error", "") == "hint.barrier.short", "barreira curta recusa")
	var over_road: Dictionary = CityStore.dispatch(&"build_barrier", [PackedVector3Array([a, a + Vector3(0, 0, FENCE_M)]), FENCE])
	_check(over_road.get("error", "") == "hint.barrier.road", "cerca sobre a calçada recusa")


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
			_check(_counts().items == before.items, "a regra não é uma peça")
			CityStore.step_ticks(1) # the traffic takes the junction's control up on its next step
			var node: Dictionary = CityStore.query(&"node_at", [at.position, NODE_FIND_M / DataTables.tuning("road_snap_node_px")])
			var control: int = CityStore.query(&"node_traffic", [node.get("node", -1)]).get("control", -1)
			_check(node.get("ctl", -1) == CTL_PRIORITY and control == CONTROL_PRIORITY,
				"a placa põe o cruzamento por preferência, como o 4.0: ctl %s, tráfego %s" % [node.get("ctl", -1), control])
			CityStore.dispatch(&"undo")
			var again: Dictionary = CityStore.dispatch(&"place_item", [at.position, ITEM_SIGN, SIGN_STOP, "", 1.0])
			_check(again.get("note", "") == "hint.streetscape.rule.stop", "a regra desfeita pode voltar")
			CityStore.dispatch(&"undo")
			break
		CityStore.dispatch(&"undo")
	_check(ruled, "placa de Pare vira regra")
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
			_check(_counts().cuts == was.cuts, "desfazer o corte")
			break
		if r.get("ok", false):
			CityStore.dispatch(&"undo")
	_check(cut, "peça do kit cortada")


func _check(ok: bool, what: String) -> void:
	print("ok: " if ok else "FALHOU: ", what)
	failures += 0 if ok else 1
