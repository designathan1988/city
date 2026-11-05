## Muros (Roadcraft editor/barriers.ts): a wall, fence, hedge, guardrail or railing traced point by point. A double click or
## Enter builds the run in one undo step, Backspace takes the last point, Esc drops the run, Shift-click takes a run away.
## Points snap to the back of the footway, or to the kerb for a guardrail and a railing.
extends ToolBase

var _points := PackedVector3Array()
var _last_click_ms := -1
var _last_click := Vector3.INF


func enter() -> void:
	ctl.hint.emit(tr("help.tool.barrier"), true)


func exit() -> void:
	ctl.preview.clear()


func _kind() -> int:
	return params.get("kind", 0)


func _snapped(p: Vector3) -> Dictionary:
	return CityStore.query(&"preview_barrier", [_points, p, _kind()])


func click(p: Vector3) -> void:
	if Input.is_key_pressed(KEY_SHIFT):
		var r: Dictionary = CityStore.dispatch(&"remove_street", [p, params.get("reach_m", 0.0)])
		ctl.hint.emit(tr("hint.barrier.removed" if r.get("ok", false) else "hint.barrier.nothing"), r.get("ok", false))
		return
	var now := Time.get_ticks_msec()
	var twice: bool = _last_click_ms >= 0 and now - _last_click_ms < int(params.get("double_click_ms", 0)) 		and p.distance_to(_last_click) < params.get("double_click_m", 0.0)
	_last_click_ms = now
	_last_click = p
	if twice:
		_last_click_ms = -1
		_finish()
		return
	_points.append(_snapped(p).position)
	_draw(p)


func hover(p: Vector3) -> void:
	if not _points.is_empty():
		_draw(p)


func _draw(p: Vector3) -> void:
	var at := _snapped(p)
	var line := _points.duplicate()
	line.append(at.position)
	ctl.preview.show_ribbon(line, params.get("half_width_m", 0.0), at.ok)


func key(keycode: Key) -> void:
	if keycode == KEY_ENTER or keycode == KEY_KP_ENTER:
		_finish()
	elif keycode == KEY_BACKSPACE and not _points.is_empty():
		_points.resize(_points.size() - 1)
		ctl.preview.clear()


func _finish() -> void:
	var run := _points
	_points = PackedVector3Array()
	ctl.preview.clear()
	if run.size() < 2:
		return
	var r: Dictionary = CityStore.dispatch(&"build_barrier", [run, _kind()])
	var ok: bool = r.get("ok", false)
	ctl.hint.emit(tr("hint.barrier.built") if ok else refusal(r), ok)


## A run being traced: Escape drops its points before the tool (Roadcraft barriers.ts).
func in_progress() -> bool:
	return not _points.is_empty()


func cancel() -> void:
	_points = PackedVector3Array()
	ctl.preview.clear()
