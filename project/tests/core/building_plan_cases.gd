## A block's plan in each shape (building_edit_test.gd): the walls it comes out with, undone to the very body; what was set on the old walls stays on the wall that
## faces the same way; a relief asks to be flattened first; and a shaped block takes a setback and a wing like a box.

const SHAPE_SIDES := {1: 6, 2: 8, 3: 24, 4: 6, 5: 8, 6: 8, 7: 12, 8: 20}
const CHAMFERED := 6
const CHAMFERED_FRONT := 0.76 # of the box's width: the front wall left between the two corners cut
const FACE_BAYS := 7
const BAY_BALCONY := 7
const SCOPE_BAY := 0
const SETBACK_STOREYS := 2
const RELIEF_M := 0.5
const ROOF_MASS := 1
const CUT_MASS := 2
const VOID_MODE := 1
const CUT_FROM := 0.35
const CUT_TO := 0.65
const OFF_BLOCK := 2.0
const RUN_OFF_M := 3.0
const RUN_LEG_M := 5.0


static func run(id: int, made: int, check: Callable) -> void:
	var vol: int = _info(id).volumes.size() - 1
	for shape: int in SHAPE_SIDES:
		var r: Dictionary = CityStore.dispatch(&"body_set_plan", [id, vol, shape])
		var sides: int = _info(id).volumes[vol].bays.size()
		check.call(r.get("ok", false) and sides == SHAPE_SIDES[shape], "forma %d com %d paredes: %s" % [shape, sides, r])
		var once: int = _info(id).digest
		CityStore.dispatch(&"body_set_plan", [id, vol, shape])
		check.call(_info(id).digest == once, "a forma %d de novo não muda o bloco" % shape)
		_back_to(made, id, check, "forma %d" % shape)
	_walls_carried(id, vol, made, check)
	_relief_first(id, vol, made, check)
	_shaped_block_grows(id, vol, made, check)
	_masses(id, vol, made, check)
	_runs(id, made, check)


static func _walls_carried(id: int, vol: int, made: int, check: Callable) -> void:
	CityStore.dispatch(&"body_set_bay", [id, vol, 0, 0, 0, BAY_BALCONY, SCOPE_BAY])
	CityStore.dispatch(&"body_set_face", [id, vol, 0, {"bays": FACE_BAYS}])
	CityStore.dispatch(&"body_set_plan", [id, vol, CHAMFERED])
	var bays: int = _info(id).volumes[vol].bays[0]
	check.call(bays == int(round(FACE_BAYS * CHAMFERED_FRONT)), "a frente chanfrada ficou com %d vãos" % bays)
	check.call(CityStore.query(&"body_bay", [id, vol, 0, 0, 0]) == BAY_BALCONY, "a varanda seguiu a frente")
	_back_to(made, id, check, "paredes levadas")


static func _relief_first(id: int, vol: int, made: int, check: Callable) -> void:
	CityStore.dispatch(&"body_set_relief", [id, vol, {"side": 0, "depth": RELIEF_M}, true])
	var before: int = _info(id).digest
	var r: Dictionary = CityStore.dispatch(&"body_set_plan", [id, vol, CHAMFERED])
	check.call(not r.get("ok", false) and r.get("error", "") == "building.reshapeRelief" and _info(id).digest == before, "relevo pede alinhar antes: %s" % r)
	_back_to(made, id, check, "relevo")


static func _shaped_block_grows(id: int, vol: int, made: int, check: Callable) -> void:
	CityStore.dispatch(&"body_set_plan", [id, vol, CHAMFERED])
	var blocks: int = _info(id).volumes.size()
	var r: Dictionary = CityStore.dispatch(&"body_add_setback", [id, vol, -1.0, SETBACK_STOREYS])
	check.call(r.get("ok", false) and _info(id).volumes.size() == blocks + 1, "recuo sobre bloco com forma: %s" % r)
	var up: Dictionary = _info(id).volumes[blocks]
	check.call(up.bays.size() == SHAPE_SIDES[CHAMFERED], "o recuo tem a forma do bloco: %d paredes" % up.bays.size())
	_back_to(made, id, check, "recuo")
	CityStore.dispatch(&"body_set_plan", [id, vol, CHAMFERED])
	r = CityStore.dispatch(&"body_add_wing", [id, vol, 1, 0.0, 0.0])
	check.call(r.get("ok", false) and _info(id).volumes.size() == blocks + 1, "ala numa parede de bloco com forma: %s" % r)
	_back_to(made, id, check, "ala")


## A rectangle dragged against the block: a wing, a block on the roof and a cut; the void leaves the solid split, and a block can change how it counts.
static func _masses(id: int, vol: int, made: int, check: Callable) -> void:
	var box: PackedVector3Array = CityStore.query(&"body_outline", [id, vol])
	var corners := box.size() / 2
	var a: Vector3 = box[0]
	var c: Vector3 = box[corners / 2]
	var mid := (a + c) * 0.5
	var blocks: int = _info(id).volumes.size()
	var r: Dictionary = CityStore.dispatch(&"body_add_mass", [id, vol, mid, c, ROOF_MASS])
	check.call(r.get("ok", false) and _info(id).volumes.size() == blocks + 1, "bloco no telhado: %s" % r)
	_back_to(made, id, check, "bloco no telhado")
	r = CityStore.dispatch(&"body_add_mass", [id, vol, a.lerp(c, CUT_FROM), a.lerp(c, CUT_TO), CUT_MASS])
	var info := _info(id)
	check.call(r.get("ok", false) and info.volumes[blocks].mode == VOID_MODE, "recorte é um bloco vazio: %s" % r)
	check.call(info.solids > blocks, "o recorte parte o bloco: %d sólidos de %d blocos" % [info.solids, blocks])
	var mode: Dictionary = CityStore.dispatch(&"body_set_block_mode", [id, blocks, 0])
	check.call(mode.get("ok", false) and _info(id).volumes[blocks].mode == 0, "o vazio volta a somar: %s" % mode)
	_back_to(made, id, check, "recorte")
	r = CityStore.dispatch(&"body_add_mass", [id, vol, a + (a - c) * OFF_BLOCK, a + (a - c) * (OFF_BLOCK + 1.0), CUT_MASS])
	check.call(not r.get("ok", false) and r.get("error", "") == "building.problem.cut", "recorte fora do bloco recusado: %s" % r)


## A run along a bent path: a piece a leg, one step of undo; a stair too.
static func _runs(id: int, made: int, check: Callable) -> void:
	var box: PackedVector3Array = CityStore.query(&"body_outline", [id, 0])
	var start: Vector3 = box[0] + (box[0] - box[box.size() / 4]).normalized() * RUN_OFF_M
	var path := PackedVector3Array([start, start + Vector3(RUN_LEG_M, 0, RUN_LEG_M), start + Vector3(RUN_LEG_M * 2.0, 0, 0)])
	for spec: Dictionary in DataTables.ui("builder").tabs.parts.sections[1].tools:
		if spec.do != "run":
			continue
		var parts: int = _info(id).parts
		var r: Dictionary = CityStore.dispatch(&"body_add_run", [id, 0, 0, path, spec])
		check.call(r.get("ok", false) and _info(id).parts == parts + path.size() - 1, "%s: uma peça por trecho: %s" % [spec.id, r])
		check.call(CityStore.dispatch(&"undo") and _info(id).digest == made, "%s desfeito num passo" % spec.id)


static func _back_to(made: int, id: int, check: Callable, what: String) -> void:
	while _info(id).digest != made and CityStore.dispatch(&"undo"):
		pass
	check.call(_info(id).digest == made, "%s desfeito" % what)


static func _info(id: int) -> Dictionary:
	return CityStore.query(&"body_info", [id])
