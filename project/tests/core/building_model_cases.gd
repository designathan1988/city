## Modelling a block through the store (building_edit_test.gd): each edit changes the body and undoes to the very body; a face is asked of what needs one; a slid block takes what
## stands on it; a corner added and taken leaves the plan it found; a block out as a building of its own leaves two records.

const OUT_M := 1.0
const BRICK_M := 4.0
const INSET_M := 0.6
const BEVEL_M := 1.0
const TURN_DEG := 15.0
const SLIDE_M := 3.0
const SIDE := 1
const CHAMFERED := 6
const CHAMFERED_SIDES := 8


static func run(id: int, made: int, check: Callable) -> void:
	var vol: int = _info(id).volumes.size() - 1
	var blocks: int = _info(id).volumes.size()
	_edit(id, made, "extrudar", &"body_extrude_face", [id, vol, SIDE, OUT_M], check)
	_edit(id, made, "tijolo", &"body_extrude_block", [id, vol, SIDE, BRICK_M], check)
	_edit(id, made, "recuar face", &"body_inset_face", [id, vol, SIDE, INSET_M], check)
	_edit(id, made, "crescer", &"body_offset", [id, vol, OUT_M], check)
	_edit(id, made, "chanfrar", &"body_bevel", [id, vol, BEVEL_M, -1], check)
	_edit(id, made, "copiar", &"body_copy_block", [id, vol], check)
	_edit(id, made, "girar", &"body_turn_block", [id, vol, TURN_DEG], check)
	var box: PackedVector3Array = CityStore.query(&"body_outline", [id, vol])
	var a: Vector3 = box[0]
	_edit(id, made, "deslizar", &"body_slide_block", [id, vol, a, a + (box[1] - a).normalized() * SLIDE_M, false], check)
	_points(id, vol, made, check)
	check.call(_info(id).volumes.size() == blocks, "os blocos voltaram")


## A point moved; a corner added on a side and taken away again is the plan it was.
static func _points(id: int, vol: int, made: int, check: Callable) -> void:
	CityStore.dispatch(&"body_set_plan", [id, vol, CHAMFERED])
	var shaped: int = _info(id).digest
	var r: Dictionary = CityStore.dispatch(&"body_change_point", [id, vol, 0, false])
	check.call(r.get("ok", false) and _info(id).volumes[vol].bays.size() == CHAMFERED_SIDES + 1, "canto a mais: %s" % r)
	r = CityStore.dispatch(&"body_change_point", [id, vol, 1, true])
	check.call(r.get("ok", false) and _info(id).volumes[vol].bays.size() == CHAMFERED_SIDES, "canto tirado: %s" % r)
	var box: PackedVector3Array = CityStore.query(&"body_outline", [id, vol])
	var out: Vector3 = (box[0] - box[box.size() / 4]).normalized() * OUT_M
	r = CityStore.dispatch(&"body_move_point", [id, vol, 0, box[0] + out, false])
	check.call(r.get("ok", false) and _info(id).digest != shaped, "ponto movido: %s" % r)
	_back_to(made, id, check, "pontos")


static func _edit(id: int, made: int, what: String, command: StringName, args: Array, check: Callable) -> void:
	var r: Dictionary = CityStore.dispatch(command, args)
	check.call(r.get("ok", false) and _info(id).digest != made, "%s: %s" % [what, r])
	_back_to(made, id, check, what)


static func _back_to(made: int, id: int, check: Callable, what: String) -> void:
	while _info(id).digest != made and CityStore.dispatch(&"undo"):
		pass
	check.call(_info(id).digest == made, "%s desfeito" % what)


static func _info(id: int) -> Dictionary:
	return CityStore.query(&"body_info", [id])
