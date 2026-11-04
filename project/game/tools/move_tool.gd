## Move (tab Mover): a press on a node takes it, its roads follow the pointer as a preview, and letting go lays them there
## (Roadcraft editor/nodeMover.ts). Esc or a right click puts it back; the whole move is one undo step.
extends ToolBase

var _node := -1
var _from := Vector3.ZERO
var _moved := false


func enter() -> void:
	ctl.hint.emit(tr("hint.move"), true)


func exit() -> void:
	ctl.preview.clear()


func hover(p: Vector3) -> void:
	var hit: Dictionary = CityStore.query(&"node_at", [p, ctl.px_at(p)])
	if hit.node < 0:
		ctl.preview.clear()
	else:
		ctl.preview.show_mark(hit.at, ctl.px_at(p))


func click(p: Vector3) -> void:
	_node = CityStore.query(&"node_at", [p, ctl.px_at(p)]).node
	_from = p
	_moved = false


func drag(p: Vector3) -> void:
	if _node < 0:
		return
	_moved = _moved or p.distance_to(_from) > ctl.px_at(p) * DataTables.tuning("road_click_px")
	ctl.preview.show_ribbons(CityStore.query(&"moved_roads", [_node, p]))


func release(p: Vector3) -> void:
	if _node < 0:
		return
	var node := _node
	_node = -1
	ctl.preview.clear()
	if p == Vector3.INF or not _moved:
		return
	var r: Dictionary = CityStore.dispatch(&"move_node", [node, p])
	ctl.hint.emit(tr("hint.move") if r.get("ok", false) else refusal(r), r.get("ok", false))


func cancel() -> void:
	_node = -1
	ctl.preview.clear()
