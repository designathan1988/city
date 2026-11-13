## Mass › Wing, Stack and Cut out: a rectangle dragged against the block picked. A wing is drawn on the ground; a block on the roof and a cut on the block's roof, which
## is where the pointer's ray is read, because the ground under the pointer lies far behind a tall building.
class_name MassDrag
extends RefCounted

const WING := 0
const SLIDE := 3
const NONE := -1

var action := NONE

var _ctl: ToolController
var _id := -1
var _vol := -1
var _y := 0.0
var _from := Vector3.INF


func _init(ctl: ToolController) -> void:
	_ctl = ctl


func armed() -> bool:
	return action != NONE


func dragging() -> bool:
	return _from != Vector3.INF


func arm(value: int) -> void:
	cancel()
	action = NONE if action == value else value


func disarm() -> void:
	cancel()
	action = NONE


func press(ground: Vector3, picked: Dictionary) -> void:
	_id = picked.id
	_vol = picked.volume
	var outline: PackedVector3Array = CityStore.query(&"body_outline", [_id, _vol])
	_y = outline[outline.size() - 1].y if not outline.is_empty() else ground.y
	_from = _at(ground)


func move(ground: Vector3) -> void:
	if not dragging():
		return
	var rect: Dictionary = CityStore.query(&"body_mass_rect", _slid(_at(ground)) if action == SLIDE else [_id, _from, _at(ground)])
	_ctl.preview.show_ribbon(rect.points, rect.half_width, true)


func finish(ground: Vector3) -> Dictionary:
	var r: Dictionary
	if action == SLIDE:
		r = CityStore.dispatch(&"body_slide_block", [_id, _vol, _from, _at(ground), true])
	else:
		r = CityStore.dispatch(&"body_add_mass", [_id, _vol, _from, _at(ground), action])
	cancel()
	return r


## The band of the block where the drag has it: across its box, moved by the drag.
func _slid(to: Vector3) -> Array:
	var outline: PackedVector3Array = CityStore.query(&"body_outline", [_id, _vol])
	var shift := to - _from
	return [_id, outline[0] + shift, outline[outline.size() / 4] + shift]


func cancel() -> void:
	_from = Vector3.INF
	_ctl.preview.clear()


## Where the pointer is: the ground for a wing; else the point of its ray at the height of the roof.
func _at(ground: Vector3) -> Vector3:
	if action == WING:
		return ground
	var cam: Camera3D = _ctl.rig.camera
	var origin := cam.project_ray_origin(_ctl.pointer)
	var dir := cam.project_ray_normal(_ctl.pointer)
	return origin + dir * ((_y - origin.y) / dir.y) if not is_zero_approx(dir.y) else ground
