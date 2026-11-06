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


static func _back_to(made: int, id: int, check: Callable, what: String) -> void:
	while _info(id).digest != made and CityStore.dispatch(&"undo"):
		pass
	check.call(_info(id).digest == made, "%s desfeito" % what)


static func _info(id: int) -> Dictionary:
	return CityStore.query(&"body_info", [id])
