## The handles of the block picked, as the player drags them (the 4.0's beginHandleDrag and pointerMove): the arrows of the faces, the points of the plan, the floors, the corners
## that move, turn and scale. While the button is down a ghost shows what the release will do; nothing is edited before it.
class_name HandleDrag
extends RefCounted

const TURN_STEP := PI / 12.0
const ROOF_REACH := 1.0

const CM := 0.01
const NO_POINT := -1

var marks: HandleMarks
## Every corner of the block is a handle, boxes included.
var points := false
## The corner last taken hold of.
var vertex := NO_POINT

var _ctl: ToolController
var _id := -1
var _vol := -1
var _held := {}
var _start := Vector3.ZERO
var _pointer_at := Vector2.ZERO
var _spec := {}
var _shown := ""
var _ghost := 0
var _place := {}
var _base := {}
var _px_storey := 1.0
var _count := 1
var _lift := 0.0
var _storey_h := 1.0
var _width := 1.0


func _init(ctl: ToolController) -> void:
	_ctl = ctl
	marks = HandleMarks.new()
	var layer := CanvasLayer.new()
	layer.add_child(marks)
	ctl.add_child(layer)


func refresh(picked: Dictionary) -> void:
	marks.handles = [] if picked.is_empty() else CityStore.query(&"body_handles", [picked.id, picked.volume, points, _ctl.rig.camera.global_position, picked])
	_id = picked.get("id", -1)
	_vol = picked.get("volume", -1)


func dragging() -> bool:
	return not _held.is_empty()


## A card of the Modelling section that works on the points of the plan.
func act(spec: Dictionary, value: int, picked: Dictionary) -> Dictionary:
	if spec.do == "points":
		points = not points
		return {"ok": true}
	if picked.is_empty():
		return {"ok": false, "error": "hint.builder.select"}
	if spec.do == "point_add":
		if picked.side < 0:
			return {"ok": false, "error": "builder.pickFace"}
		return CityStore.dispatch(&"body_change_point", [picked.id, picked.volume, picked.side, false])
	if vertex == NO_POINT:
		return {"ok": false, "error": "builder.pickCorner"}
	var at := vertex
	vertex = NO_POINT
	if spec.do == "point_remove":
		return CityStore.dispatch(&"body_change_point", [picked.id, picked.volume, at, true])
	return CityStore.dispatch(&"body_bevel", [picked.id, picked.volume, value * CM, at])


## A press on a handle takes hold of it.
func press() -> bool:
	_held = marks.at(_ctl.pointer)
	if _held.is_empty():
		return false
	_start = _on_plane(_held.at.y)
	_pointer_at = _ctl.pointer
	vertex = _held.vertex
	var info: Dictionary = CityStore.query(&"body_info", [_id])
	_count = info.volumes[_vol].storeys
	_lift = info.volumes[_vol].lift
	var cam: Camera3D = _ctl.rig.camera
	_px_storey = maxf(absf(cam.unproject_position(_held.at).y - cam.unproject_position(_held.at - Vector3.UP * info.storey_h).y), ROOF_REACH)
	_width = info.volumes[_vol].w
	_storey_h = info.storey_h
	_base = CityStore.query(&"body_draft", [_id, {}])
	return true


func move(shift: bool) -> void:
	if not dragging():
		return
	match String(_held.kind):
		"move":
			_show_place(_moved())
			return
		"rotate":
			_show_place(_turned(shift))
			return
	_spec = _edit(shift)
	var key := str(_spec)
	if key == _shown:
		return
	_shown = key
	_free_ghost()
	var d: Dictionary = CityStore.query(&"body_draft", [_id, _spec])
	_show_draft(d)
	if not d.is_empty():
		_ctl.tagged.emit(_measure(), _held.at, d.problem == "")


func finish() -> Dictionary:
	var r := {"ok": true}
	if not _place.is_empty():
		r = CityStore.dispatch(&"building_place", [_id, _place.x, _place.z, _place.rot])
		if r.get("ok", false):
			_id = r.placed
	elif not _spec.is_empty():
		r = CityStore.dispatch(&"body_apply", [_id, _spec])
	cancel()
	return r


## The ghost of an edit about to be made, under the pointer: a part or a basic shape in hand. Asked again only when the edit changes.
func preview(id: int, spec: Dictionary) -> void:
	var key := "%d %s" % [id, spec]
	if key == _shown:
		return
	_shown = key
	_free_ghost()
	if spec.is_empty():
		return
	_show_draft(CityStore.query(&"body_draft", [id, spec]))


## What a click would add: the part on the wall under the pointer, the basic shape on the roof or the wall.
func ghost_in_hand(card: Dictionary) -> void:
	if card.is_empty():
		return
	var cam: Camera3D = _ctl.rig.camera
	var hit: Dictionary = CityStore.query(&"body_pick", [cam.project_ray_origin(_ctl.pointer), cam.project_ray_normal(_ctl.pointer)])
	if hit.is_empty() or (card.do == "part" and hit.side < 0):
		preview(-1, {})
	elif card.do == "part":
		preview(hit.id, {"op": "part", "vol": hit.volume, "side": hit.side, "index": hit.bay, "count": hit.storey, "card": card})
	else:
		preview(hit.id, {"op": "primitive", "vol": hit.volume, "side": hit.side, "count": hit.storey, "at": hit.point, "shape": card})


func _show_draft(d: Dictionary) -> void:
	if d.is_empty():
		return
	_ghost = CityStore.query(&"stage_show", [d.bytes, _ctl.get_viewport().find_world_3d().scenario, true])
	CityStore.query(&"stage_move", [_ghost, Transform3D(Basis(Vector3.UP, d.rot), Vector3(d.x, d.y, d.z))])
	if d.problem != "":
		_ctl.hint.emit(_ctl.tr(d.problem), false)


func cancel() -> void:
	_free_ghost()
	_held = {}
	_spec = {}
	_shown = ""
	_place = {}
	_ctl.tagged.emit("", Vector3.ZERO, true)


## The building the drag left: moved, it is a new record.
func building() -> int:
	return _id


func _edit(shift: bool) -> Dictionary:
	var p := _on_plane(_held.at.y)
	var snap := not Input.is_key_pressed(KEY_ALT)
	match String(_held.kind):
		"storeys":
			return {"op": "storeys", "vol": _vol, "count": _count + int(round((_pointer_at.y - _ctl.pointer.y) / _px_storey))}
		"lift":
			return {"op": "lift", "vol": _vol, "along": _lift + (_pointer_at.y - _ctl.pointer.y) / _px_storey * _storey_h, "snap": snap}
		"relief":
			return {"op": "relief", "vol": _vol, "side": _held.side, "index": _held.bay, "count": _held.storey, "along": _held.depth + (p - _start).dot(_held.dir), "snap": snap}
		"side":
			return {"op": "side", "vol": _vol, "side": _held.side, "along": (p - _start).dot(_held.dir), "wing": shift, "snap": snap}
		"vertex":
			return {"op": "vertex", "vol": _vol, "index": _held.vertex, "at": p, "snap": snap}
		"scale":
			return {"op": "scale", "factor": 1.0 + (p - _start).dot(_held.dir) / _width, "snap": snap}
	return {}


func _measure() -> String:
	match String(_held.kind):
		"storeys":
			return str(_spec.count)
		"side", "lift", "relief":
			return Formatters.format_length_tenths(absf(_spec.along))
	return ""


## The ghost of the building where the drag has it: the body drawn once, then only moved.
func _show_place(place: Dictionary) -> void:
	_place = place
	if _ghost == 0:
		_ghost = CityStore.query(&"stage_show", [_base.bytes, _ctl.get_viewport().find_world_3d().scenario, true])
	CityStore.query(&"stage_move", [_ghost, Transform3D(Basis(Vector3.UP, place.rot), Vector3(place.x, _base.y, place.z))])


func _moved() -> Dictionary:
	var shift := _on_plane(_held.at.y) - _start
	var step: float = DataTables.tuning("build_grid_m")
	return {"x": _base.x + snappedf(shift.x, step), "z": _base.z + snappedf(shift.z, step), "rot": _base.rot}


## About the middle of the lot, as the front's middle goes round it.
func _turned(free: bool) -> Dictionary:
	var centre := Vector3(_base.cx, _held.at.y, _base.cz)
	var from := _start - centre
	var to := _on_plane(_held.at.y) - centre
	var angle := atan2(to.x, to.z) - atan2(from.x, from.z)
	if not free:
		angle = snappedf(angle, TURN_STEP)
	var front := (Vector3(_base.x, centre.y, _base.z) - centre).rotated(Vector3.UP, angle) + centre
	return {"x": front.x, "z": front.z, "rot": _base.rot + angle}


func _free_ghost() -> void:
	if _ghost != 0:
		CityStore.query(&"stage_free", [_ghost])
	_ghost = 0


func _on_plane(y: float) -> Vector3:
	var cam: Camera3D = _ctl.rig.camera
	var origin := cam.project_ray_origin(_ctl.pointer)
	var dir := cam.project_ray_normal(_ctl.pointer)
	return origin + dir * ((y - origin.y) / dir.y) if not is_zero_approx(dir.y) else origin
