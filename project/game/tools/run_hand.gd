## Build › Parts › Runs: a wall, fence, footpath or stair laid along a path. Each click sets a corner, Enter lays the run in one step of undo, Backspace takes the last
## corner back. A stair climbs the floor picked along the whole path.
class_name RunHand
extends RefCounted

const LEAST_POINTS := 2

var spec := {}

var _ctl: ToolController
var _points := PackedVector3Array()


func _init(ctl: ToolController) -> void:
	_ctl = ctl


func armed() -> bool:
	return not spec.is_empty()


func arm(card: Dictionary) -> void:
	var same := spec == card
	disarm()
	spec = {} if same else card


func disarm() -> void:
	spec = {}
	_points.clear()
	_ctl.preview.clear()


func click(p: Vector3) -> void:
	_points.push_back(p)
	_show(_points)


func hover(p: Vector3) -> void:
	if not _points.is_empty():
		var path := _points.duplicate()
		path.push_back(p)
		_show(path)


func back() -> void:
	if not _points.is_empty():
		_points.remove_at(_points.size() - 1)
		_show(_points)


func finish(picked: Dictionary) -> Dictionary:
	if _points.size() < LEAST_POINTS:
		return {"ok": false, "error": "building.problem.size"}
	var r: Dictionary = CityStore.dispatch(&"body_add_run", [picked.id, picked.volume, picked.storey, _points, spec])
	_points.clear()
	_ctl.preview.clear()
	return r


func _show(path: PackedVector3Array) -> void:
	_ctl.preview.show_ribbon(path, spec.size[1] * 0.5, true)
