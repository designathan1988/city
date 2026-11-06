## Core test of the Build tools through the store: every edit of a building's body changes what the body holds, is undone
## to the very body made from its seed and redone to the same bytes; what cannot stand is refused and leaves the body alone;
## a building nobody touched keeps its body; and a saved city opens with its edited bodies.
## Run: godot --headless --path project res://tests/core/building_edit_test.tscn -- --no-flow
extends Node3D

const PlanCases := preload("res://tests/core/building_plan_cases.gd")
const InsideCases := preload("res://tests/core/building_inside_cases.gd")
const ModelCases := preload("res://tests/core/building_model_cases.gd")
const PickCases := preload("res://tests/core/building_pick_cases.gd")
const PartCases := preload("res://tests/core/building_part_cases.gd")
const TOWN_MAP := 0
const SEED := 1
const OTHER_SEED := 7
const SLOT := "teste_corpo_editado"
const MIN_FLOORS := 3
const ADD_FLOORS := 2
const WING_SIDE := 2
const WING_DEPTH := 6.0
const WING_LENGTH := 9.0
const SETBACK_INSET := 1.0
const SETBACK_STOREYS := 2
const ROOF_TERRACE := 1
const RESIZE_SIDE := 1
const RESIZE_M := 2.0
const GROUND_H := 4.5
const STOREY_H := 3.2
const FAR_TOO_MANY := 100000
const FACE := 0
const BAY_WIDE := 6
const BAY_BALCONY := 7
const SCOPE_BAY := 0
const SCOPE_ROW := 1
const SCOPE_COLUMN := 2
const PATTERN_OFFICE := 2
const RELIEF_M := 0.5
const FACE_BAYS := 7
const BAY_WIDTH_M := 3.0
const ROOF_GABLE := 2
const ROOF_HIP := 3
const ROOF_PITCH := 30.0
const PAINT_SCOPES := [0, 1, 2, 3, 4, 5, 6]
const FINISH_BRICK := 3
const PAINT_RGB := "#a04030"
const SHOWN_CASES := 6

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	_every_body_stands()
	var id := _a_tall_building()
	var other := _another_building(id)
	var other_digest: int = _info(other).digest
	var made: int = _info(id).digest
	_check(not _info(id).edited, "a generated building is not edited")
	_mass_edits(id, made)
	_facade_edits(id, made)
	_finish_edits(id, made)
	_bays_stay_put(id, made)
	PickCases.run(id, _check)
	PartCases.run(id, made, _check)
	PlanCases.run(id, made, _check)
	InsideCases.run(id, made, _check)
	ModelCases.run(id, made, _check)
	_refusals(_one_block_on_the_ground())
	_check(_info(other).digest == other_digest and not _info(other).edited, "the neighbour did not change")
	await _save_round_trip(id)
	print("FAILURES: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## A body made from a seed must pass the rules an edit is held to, or its first edit would be refused for what nobody did.
func _every_body_stands() -> void:
	var bad := {}
	var total := 0
	for id: int in CityStore.query(&"list_buildings"):
		total += 1
		var i := _info(id)
		var why: String = i.problem
		if not why.is_empty():
			bad[why] = bad.get(why, 0) + 1
			if bad[why] <= SHOWN_CASES:
				print("body %d: %s ground %.2f storey %.2f %s" % [id, why, i.ground_h, i.storey_h, i.volumes])
	_check(total > 0 and bad.is_empty(), "the %d generated bodies pass the rules: %s" % [total, bad])


func _a_tall_building() -> int:
	for id: int in CityStore.query(&"list_buildings"):
		var volumes: Array = _info(id).volumes
		if not volumes.is_empty() and volumes[0].base == 0 and volumes[0].storeys >= MIN_FLOORS:
			return id
	_check(false, "found a tall building")
	return -1


func _another_building(id: int) -> int:
	for other: int in CityStore.query(&"list_buildings"):
		if other != id and not _info(other).edited: # a square has a body of its own from the start
			return other
	return -1


func _info(id: int) -> Dictionary:
	return CityStore.query(&"body_info", [id])


func _mass_edits(id: int, made: int) -> void:
	var v: Dictionary = _info(id).volumes[0]
	var blocks: int = _info(id).volumes.size()
	_round(id, made, "andares", &"body_storeys", [id, 0, v.storeys + ADD_FLOORS], func(i: Dictionary) -> bool:
		return i.volumes[0].storeys == v.storeys + ADD_FLOORS)
	_round(id, made, "storey inserted", &"body_insert_storey", [id, 0, 1, true], func(i: Dictionary) -> bool:
		return i.volumes[0].storeys == v.storeys + 1)
	_round(id, made, "lado", &"body_resize", [id, 0, RESIZE_SIDE, RESIZE_M, true], func(i: Dictionary) -> bool:
		return is_equal_approx(i.volumes[0].w, v.w + RESIZE_M))
	_round(id, made, "ala", &"body_add_wing", [id, 0, WING_SIDE, WING_DEPTH, WING_LENGTH], func(i: Dictionary) -> bool:
		return i.volumes.size() == blocks + 1)
	_round(id, made, "recuo", &"body_add_setback", [id, 0, SETBACK_INSET, SETBACK_STOREYS], func(i: Dictionary) -> bool:
		return i.volumes.size() == blocks + 1 and i.volumes[0].roof == ROOF_TERRACE and i.volumes[blocks].base == v.base + v.storeys)
	_round(id, made, "dividir", &"body_split", [id, 0, 1], func(i: Dictionary) -> bool:
		return i.volumes.size() == blocks + 1 and i.volumes[0].storeys == 1)
	_round(id, made, "alturas", &"body_heights", [id, GROUND_H, STOREY_H], func(i: Dictionary) -> bool:
		return is_equal_approx(i.ground_h, GROUND_H) and is_equal_approx(i.storey_h, STOREY_H))


func _facade_edits(id: int, made: int) -> void:
	var at := func(storey: int, bay: int) -> int: return CityStore.query(&"body_bay", [id, 0, storey, FACE, bay])
	_round(id, made, "bay", &"body_set_bay", [id, 0, 1, FACE, 0, BAY_BALCONY, SCOPE_BAY], func(_i: Dictionary) -> bool:
		return at.call(1, 0) == BAY_BALCONY)
	_round(id, made, "linha", &"body_set_bay", [id, 0, 1, FACE, 0, BAY_BALCONY, SCOPE_ROW], func(_i: Dictionary) -> bool:
		return at.call(1, 0) == BAY_BALCONY and at.call(1, 1) == BAY_BALCONY)
	_round(id, made, "coluna", &"body_set_bay", [id, 0, 1, FACE, 0, BAY_BALCONY, SCOPE_COLUMN], func(_i: Dictionary) -> bool:
		return at.call(0, 0) == BAY_BALCONY and at.call(2, 0) == BAY_BALCONY)
	_round(id, made, "zona", &"body_set_zone", [id, 0, FACE, 1, 2, 0, 1, BAY_BALCONY], func(_i: Dictionary) -> bool:
		return at.call(2, 1) == BAY_BALCONY)
	_round(id, made, "pattern", &"body_set_pattern", [id, 0, -1, -1, PATTERN_OFFICE], func(_i: Dictionary) -> bool:
		return at.call(1, 0) == BAY_WIDE)
	_round(id, made, "relevo", &"body_set_relief", [id, 0, {"side": FACE, "bay1": 1, "storey1": 1, "depth": RELIEF_M}, true], func(i: Dictionary) -> bool:
		return i.edited)
	_round(id, made, "ritmo", &"body_set_face", [id, 0, FACE, {"bays": FACE_BAYS}], func(i: Dictionary) -> bool:
		return i.edited)


func _finish_edits(id: int, made: int) -> void:
	var was: int = _info(id).volumes[0].roof
	var other := ROOF_GABLE if was != ROOF_GABLE else ROOF_HIP
	_round(id, made, "telhado", &"body_set_roof", [id, 0, other], func(i: Dictionary) -> bool: return i.volumes[0].roof == other)
	_round(id, made, "forma do telhado", &"body_roof_shape", [id, 0, ROOF_PITCH, 1, -1], func(i: Dictionary) -> bool: return i.edited)
	for scope: int in PAINT_SCOPES:
		_round(id, made, "tinta %d" % scope, &"body_paint", [id, scope, 0, FACE, 1, FINISH_BRICK, PAINT_RGB], func(i: Dictionary) -> bool:
			return i.edited)


## A wing counts its own bays, and a bay set on it keeps its place on the wall when the side before it is pulled: the wall grows at
## that side's start. A wing because its bay count follows its length; a composed face keeps its count and widens its bays.
func _bays_stay_put(id: int, made: int) -> void:
	var back := 2
	var pulled := 1
	var wing_side := 1
	var parent: Dictionary = _info(id).volumes[0]
	CityStore.dispatch(&"body_add_wing", [id, 0, wing_side, WING_DEPTH, WING_LENGTH])
	var wing: int = _info(id).volumes.size() - 1
	var count: int = _info(id).volumes[wing].bays[back]
	_check(count == int(round(WING_DEPTH / BAY_WIDTH_M)), "the wing counts its own bays: %d in %.0f m, the wall it came from has %s" % [count, WING_DEPTH, parent.bays])
	CityStore.dispatch(&"body_set_bay", [id, wing, 1, back, 0, BAY_BALCONY, SCOPE_BAY])
	CityStore.dispatch(&"body_resize", [id, wing, pulled, BAY_WIDTH_M * 2.0, true])
	var grown: int = _info(id).volumes[wing].bays[back] - count
	_check(grown == 2, "the wing gained two bays: %d" % grown)
	_check(CityStore.query(&"body_bay", [id, wing, 1, back, grown]) == BAY_BALCONY, "the balcony followed the wall")
	_check(CityStore.query(&"body_bay", [id, wing, 1, back, 0]) != BAY_BALCONY, "the new bay is no balcony")
	for i in 3:
		CityStore.dispatch(&"undo")
	_check(_info(id).digest == made, "three undos return to the generated body")


## One edit: it does what it says, undo gives back the body from the seed, redo gives back the same bytes, and undo again
## leaves the building as it was for the next case.
func _round(id: int, made: int, what: String, command: StringName, args: Array, did: Callable) -> void:
	var r: Dictionary = CityStore.dispatch(command, args)
	var after := _info(id)
	_check(r.get("ok", false) and did.call(after) and after.edited, "%s: %s" % [what, r])
	_check(after.digest != made, "%s changed the body" % what)
	_check(CityStore.dispatch(&"undo") and _info(id).digest == made and not _info(id).edited, "%s desfeito" % what)
	_check(CityStore.dispatch(&"redo") and _info(id).digest == after.digest, "%s refeito" % what)
	CityStore.dispatch(&"undo")


func _refusals(id: int) -> void:
	var before: int = _info(id).digest
	var r: Dictionary = CityStore.dispatch(&"body_remove_volume", [id, _ground_block(id)])
	_check(not r.get("ok", true) and r.get("error", "") == "building.problem.lastVolume", "the last block on the ground stays: %s" % r)
	_check(_info(id).digest == before, "a refusal leaves the body alone")
	var bad: Dictionary = CityStore.dispatch(&"body_storeys", [id, FAR_TOO_MANY, 1])
	_check(not bad.get("ok", true), "a block that does not exist is refused")


func _one_block_on_the_ground() -> int:
	for id: int in CityStore.query(&"list_buildings"):
		var ground: Array = _info(id).volumes.filter(func(v: Dictionary) -> bool: return v.base == 0)
		if ground.size() == 1:
			return id
	_check(false, "found a building with one block on the ground")
	return -1


func _ground_block(id: int) -> int:
	var volumes: Array = _info(id).volumes
	for i in volumes.size():
		if volumes[i].base == 0:
			return i
	return -1


func _save_round_trip(id: int) -> void:
	var v: Dictionary = _info(id).volumes[0]
	CityStore.dispatch(&"body_storeys", [id, 0, v.storeys + ADD_FLOORS])
	PartCases.fly_flag(id, _check)
	var edited: int = _info(id).digest
	var outcome := {}
	CityStore.saved.connect(func(_s: String, r: Dictionary) -> void: outcome.merge(r), CONNECT_ONE_SHOT)
	CityStore.saves.save_game(SLOT)
	while CityStore.saves.save_busy():
		await get_tree().process_frame
	_check(outcome.get("ok", false), "salvou %s" % outcome)
	CityStore.new_map(TOWN_MAP, OTHER_SEED, false)
	var r: Dictionary = CityStore.saves.load_game(SLOT)
	_check(r.ok, "abriu %s" % r)
	_check(_info(id).edited and _info(id).digest == edited, "the edited body came back the same")
	DirAccess.remove_absolute(CityStore.saves.save_path(SLOT))


func _check(ok: bool, what: String) -> void:
	print("ok   " if ok else "FAIL ", what)
	if not ok:
		failures += 1
