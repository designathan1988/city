## Street pieces (Roadcraft editor/streetscapeTool.ts): a tree, bench, bin, light, hydrant, post box, phone, drain, long grass,
## sign or street name put where the pointer is; a ring shows where it would stand, red where it cannot. A drag lays a row
## along the footway; Shift-click takes away what stands there. A Stop or Give Way plate at an approach is that leg's rule.
extends ToolBase

var _a := Vector3.INF


func enter() -> void:
	ctl.hint.emit(tr("help.tool.streetscape"), true)


func exit() -> void:
	ctl.preview.clear()


func _kind() -> int:
	return params.get("kind", 0)


func _reach(p: Vector3) -> float:
	return pick_m(p, params.get("reach_m", 0.0), params.get("pick_px", 0.0))


func hover(p: Vector3) -> void:
	var at: Dictionary = CityStore.query(&"preview_item", [p, _kind(), _reach(p)])
	if at.is_empty():
		return
	ctl.preview.show_disc(at.position, params.get("placing_ring_m", 0.0), at.ok)


func drag(p: Vector3) -> void:
	if _a != Vector3.INF:
		ctl.preview.show_ribbon(PackedVector3Array([_a, p]), params.get("placing_ring_m", 0.0), true)


func click(p: Vector3) -> void:
	if Input.is_key_pressed(KEY_SHIFT):
		var r: Dictionary = CityStore.dispatch(&"remove_street", [p, _reach(p)])
		if r.get("ok", false):
			ctl.hint.emit(tr("hint.streetscape.removed"), true)
		return
	_a = p


func release(p: Vector3) -> void:
	if _a == Vector3.INF:
		return
	var from := _a
	_a = Vector3.INF
	var every: float = params.get("row_m", 0.0)
	var r: Dictionary
	var row: bool = p != Vector3.INF and every > 0.0 and from.distance_to(p) >= every
	if row:
		r = CityStore.dispatch(&"place_item_row", [from, p, _kind(), every])
	else:
		r = CityStore.dispatch(&"place_item", [from, _kind(), params.get("sign", 0), params.get("words", ""), _reach(from)])
	if not r.get("ok", false): # told by the store's channel
		return
	if row:
		ctl.hint.emit(tr("hint.streetscape.row"), true)
	elif not r.get("note", "").is_empty():
		ctl.hint.emit(tr(r.note), true)


## A row being dragged.
func in_progress() -> bool:
	return _a != Vector3.INF


func cancel() -> void:
	_a = Vector3.INF
	ctl.preview.clear()
