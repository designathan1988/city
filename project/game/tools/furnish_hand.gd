## Construir › Interior, as in The Sims: what is in hand on the cut floor. A core goes where the floor is clicked, through every level; a piece goes down where it is clicked
## and R turns it; Mover picks a piece and sets it down, Tirar takes what is clicked. Opens the building picked at its ground floor when it is not open.
class_name FurnishHand
extends RefCounted

const QUARTER := PI / 2.0
const HAND_EMPTY := -2
const CORE_OUT := -1
const NONE := ""
const MOVE := "move"
const TAKE := "remove"
const NOT_FOUND := -1

## The core a click sets (body_ops CoreKind), CORE_OUT to take one away, HAND_EMPTY for none.
var core := HAND_EMPTY
## The piece a click sets (`FURNITURE_NAMES`), MOVE or TAKE, NONE for none.
var piece := NONE
var angle := 0.0

var _ctl: ToolController
var _carried := NOT_FOUND


func _init(ctl: ToolController) -> void:
	_ctl = ctl


func armed() -> bool:
	return core != HAND_EMPTY or piece != NONE


func disarm() -> void:
	core = HAND_EMPTY
	piece = NONE
	_carried = NOT_FOUND


## A card of the Interior tab: a core or a piece in hand, or a turn of what is in it.
func act(spec: Dictionary, picked: Dictionary) -> Dictionary:
	if picked.is_empty():
		return {"ok": false, "error": "hint.builder.select"}
	if spec.do == "turn":
		return turn(picked.id)
	if spec.do == "core":
		arm_core(spec.value, picked.id)
	else:
		arm_piece(spec.piece, picked.id)
	_ctl.hint.emit(_ctl.tr("hint.builder.%s" % spec.id), true)
	return {"ok": true}


## Construir › Interior: the building chosen cut open, a floor up or down (Roadcraft openInside, seeInside).
func inside(what: String, by: int, picked: Dictionary) -> void:
	var view: InteriorController = _ctl.get_tree().get_first_node_in_group(&"interior_controller")
	if view == null:
		return
	if what == "floor":
		view.step_floor(by)
	elif picked.is_empty():
		_ctl.hint.emit(_ctl.tr("hint.builder.select"), false)
	else:
		view.see_inside(int(picked.get("building", picked.get("id", -1))))


func arm_core(value: int, id: int) -> void:
	var was := core
	disarm()
	core = HAND_EMPTY if was == value else value
	_open(id)


func arm_piece(value: String, id: int) -> void:
	var was := piece
	disarm()
	piece = NONE if was == value else value
	_open(id)


func _open(id: int) -> void:
	if not armed() or id < 0 or CityStore.query(&"interior").on:
		return
	CityStore.dispatch(&"set_interior", [true, id, 0, -1])


## A quarter turn for the piece in hand, or for the one being moved.
func turn(id: int) -> Dictionary:
	angle = fmod(angle + QUARTER, TAU)
	if _carried == NOT_FOUND:
		return {"ok": true}
	return CityStore.dispatch(&"body_turn_piece", [id, _level(), _carried])


func click(id: int) -> Dictionary:
	var at := _on_floor(id)
	if core == CORE_OUT:
		return CityStore.dispatch(&"body_remove_core", [id, at])
	if core != HAND_EMPTY:
		return CityStore.dispatch(&"body_add_core", [id, at, core])
	if piece == TAKE:
		return CityStore.dispatch(&"body_remove_piece", [id, _level(), at])
	if piece != MOVE:
		return CityStore.dispatch(&"body_put_piece", [id, _level(), piece, at, angle])
	if _carried == NOT_FOUND:
		_carried = CityStore.query(&"body_piece_at", [id, _level(), at])
		return {"ok": true} if _carried != NOT_FOUND else {"ok": false, "error": "building.problem.noPiece"}
	var r: Dictionary = CityStore.dispatch(&"body_move_piece", [id, _level(), _carried, at])
	_carried = NOT_FOUND
	return r


func _level() -> int:
	return CityStore.query(&"interior").level


## Where the pointer's ray meets the floor cut open.
func _on_floor(id: int) -> Vector3:
	var y: float = CityStore.query(&"body_floor_y", [id, _level()])
	var cam: Camera3D = _ctl.rig.camera
	var origin := cam.project_ray_origin(_ctl.pointer)
	var dir := cam.project_ray_normal(_ctl.pointer)
	return origin + dir * ((y - origin.y) / dir.y)
