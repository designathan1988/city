## The Interior tab through the store (building_edit_test.gd): a core of each kind goes through the building and comes out where it is clicked; a floor's first edit keeps what
## was made for it, and a piece is put down, found, moved, turned and taken away; all of it undone to the very body.

const CORE_KINDS := [0, 1, 2]
const LEVEL := 0
const PIECE := "sofa"
const MOVE_M := 2.0
const NO_PIECE := -1


static func run(id: int, made: int, check: Callable) -> void:
	var box: PackedVector3Array = CityStore.query(&"body_outline", [id, 0])
	var at: Vector3 = (box[0] + box[box.size() / 4]) * 0.5
	for kind: int in CORE_KINDS:
		var r: Dictionary = CityStore.dispatch(&"body_add_core", [id, at, kind])
		check.call(r.get("ok", false) and BuildingProbe.info(id).digest != made, "core %d went in: %s" % [kind, r])
		r = CityStore.dispatch(&"body_remove_core", [id, at])
		check.call(r.get("ok", false), "core %d left by the click: %s" % [kind, r])
		BuildingProbe.back_to(made, id, check, "core %d" % kind)
	var far := at + (at - box[0]) * 100.0
	var miss: Dictionary = CityStore.dispatch(&"body_remove_core", [id, far])
	check.call(not miss.get("ok", false) and miss.get("error", "") == "building.problem.noCore", "taking a core where there is none: %s" % miss)
	_pieces(id, made, at, check)


static func _pieces(id: int, made: int, at: Vector3, check: Callable) -> void:
	var r: Dictionary = CityStore.dispatch(&"body_put_piece", [id, LEVEL, PIECE, at, 0.0])
	check.call(r.get("ok", false), "piece went in: %s" % r)
	var index: int = CityStore.query(&"body_piece_at", [id, LEVEL, at])
	check.call(index != NO_PIECE, "piece found where it was put")
	var turned: int = BuildingProbe.info(id).digest
	r = CityStore.dispatch(&"body_turn_piece", [id, LEVEL, index])
	check.call(r.get("ok", false) and BuildingProbe.info(id).digest != turned, "piece turned: %s" % r)
	var moved := at + Vector3(MOVE_M, 0, 0)
	r = CityStore.dispatch(&"body_move_piece", [id, LEVEL, index, moved])
	check.call(r.get("ok", false), "piece moved: %s" % r)
	r = CityStore.dispatch(&"body_remove_piece", [id, LEVEL, moved])
	check.call(r.get("ok", false), "piece left: %s" % r)
	BuildingProbe.back_to(made, id, check, "piece")
	r = CityStore.dispatch(&"body_put_piece", [id, LEVEL, "not_a_piece", at, 0.0])
	check.call(not r.get("ok", false) and BuildingProbe.info(id).digest == made, "a piece with no name is refused: %s" % r)
