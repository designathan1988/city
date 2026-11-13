## Poles (Roadcraft editor/poles.ts): a line of poles traced as stretches, each a drag; the next starts where the last ended
## until Esc. The run under the pointer is drawn as it will stand, and the same plan is built on
## release. Remove takes the pole under a click and its wires; so does Shift-click while building.
extends ToolBase

var _from := Vector3.INF
var _to := Vector3.INF
var _chain := Vector3.INF
var _plan: Dictionary = {}


func enter() -> void:
	ctl.hint.emit(tr("help.tool.poleRemove" if _removing() else "help.tool.pole"), true)


func exit() -> void:
	_clear()


func _removing() -> bool:
	return params.get("mode", "build") == "remove"


func _reach() -> float:
	return params.get("reach_m", 0.0)


## A run from `a` to `b` as the panel chose it: its lamps and the kind of its poles (drawers.json street).
func _run(a: Vector3, b: Vector3) -> Array:
	var street: Dictionary = DataTables.ui("drawers").street
	return [a, b, _reach(), params.get("lamps", street.default_lamps), params.get("kind", street.default_pole_kind)]


func _clear() -> void:
	_plan = {}
	CityStore.dispatch(&"preview_pole_line", [[]])


func _show(a: Vector3, b: Vector3) -> void:
	_plan = CityStore.query(&"preview_pole_run", _run(a, b))
	if _plan.is_empty():
		return
	CityStore.dispatch(&"preview_pole_line", [_plan.poles if _plan.ok else []])
	if _plan.ok:
		ctl.hint.emit(tr("help.tool.pole"), true)
	elif not _plan.error.is_empty():
		ctl.hint.emit(tr(_plan.error), false)


func click(p: Vector3) -> void:
	if _removing() or Input.is_key_pressed(KEY_SHIFT):
		var r: Dictionary = CityStore.dispatch(&"remove_street", [p, _reach()])
		if r.get("ok", false):
			ctl.hint.emit(tr("hint.pole.removed"), true)
		_chain = Vector3.INF
		return
	_from = _chain if _chain != Vector3.INF else p
	_to = p


func hover(p: Vector3) -> void:
	if not _removing() and _chain != Vector3.INF:
		_show(_chain, p)


func drag(p: Vector3) -> void:
	if _from == Vector3.INF:
		return
	_to = p
	_show(_from, _to)


func release(p: Vector3) -> void:
	if _from == Vector3.INF:
		return
	var end := p if p != Vector3.INF else _to
	var r: Dictionary = CityStore.dispatch(&"build_pole_run", _run(_from, end))
	_from = Vector3.INF
	_clear()
	_chain = r.last if r.get("ok", false) and r.has("last") else Vector3.INF


## A line being strung (Roadcraft main.ts gestureInProgress: poleTool.inProgress).
func in_progress() -> bool:
	return _from != Vector3.INF or _chain != Vector3.INF


func cancel() -> void:
	_from = Vector3.INF
	_chain = Vector3.INF
	_clear()
