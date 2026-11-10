## Public transport tool: `params.kind` says what a click lays. stop, terminal and station act at once; track and line collect points
## or stops and Enter or a double click ends them, Backspace takes the last back, Esc drops them. Shift and a click removes the stop or
## track there. While it is on the metro shows in its cut and the plan over the map. The core checks everything: refusals come back as
## hint keys.
extends ToolBase

const MODE_BUS := 0
const MODE_TRAIN := 1

var kind := "stop"
var mode := MODE_TRAIN
var points := PackedVector3Array()
var stops := PackedInt32Array()
var cursor := Vector3.INF
var _overlay: TransitOverlay


func enter() -> void:
	kind = params.get("kind", "stop")
	mode = params.get("mode", MODE_TRAIN)
	ctl.hint.emit(tr("game.hint.transit"), true)
	CityStore.query(&"transit_xray", [true])
	_overlay = TransitOverlay.new()
	_overlay.tool = self
	ctl.add_child(_overlay)


func exit() -> void:
	CityStore.query(&"transit_xray", [false])
	CityStore.query(&"transit_preview", [PackedVector3Array(), mode])
	_overlay.queue_free()


func click(p: Vector3) -> void:
	if Input.is_key_pressed(KEY_SHIFT):
		if _say(CityStore.dispatch(&"transit_remove", [p])):
			ctl.hint.emit(tr("hint.transit.removed"), true)
			ctl.plan_changed.emit()
		return
	match kind:
		"stop":
			_say(CityStore.dispatch(&"transit_stop", [p]))
		"terminal":
			_say(CityStore.dispatch(&"transit_terminal", [p]))
		"station":
			_say(CityStore.dispatch(&"transit_station", [p, mode]))
		"track":
			points.append(CityStore.query(&"transit_snap", [p, mode]))
			if ctl.double_press:
				finish()
			else:
				_show_run(p)
		"line":
			_pick(p)


## The stop under a click joins the line being made: of the first one's mode, once it has one.
func _pick(p: Vector3) -> void:
	var first := _first_mode()
	var stop: int = CityStore.query(&"transit_pick", [p, maxi(first, MODE_BUS), first < 0])
	if stop < 0:
		ctl.hint.emit(tr("hint.transit.lineShort" if ctl.double_press else "hint.transit.noStop"), false)
	elif stops.is_empty() or stops[stops.size() - 1] != stop:
		stops.append(stop)
	if ctl.double_press:
		finish()


func _first_mode() -> int:
	if stops.is_empty():
		return -1
	for s: Dictionary in CityStore.query(&"transit_plan").stops:
		if s.id == stops[0]:
			return int(s.mode)
	return -1


func hover(p: Vector3) -> void:
	cursor = p
	if kind == "track" and not points.is_empty():
		_show_run(p)


func _show_run(to: Vector3) -> void:
	var run := points.duplicate()
	run.append(CityStore.query(&"transit_snap", [to, mode]))
	CityStore.query(&"transit_preview", [run, mode])


func in_progress() -> bool:
	return not points.is_empty() or not stops.is_empty()


func cancel() -> void:
	points.clear()
	stops.clear()
	CityStore.query(&"transit_preview", [PackedVector3Array(), mode])


func key(code: int) -> void:
	if points.is_empty() and stops.is_empty():
		return
	if code == KEY_ENTER or code == KEY_KP_ENTER:
		finish()
	elif code == KEY_BACKSPACE:
		if not points.is_empty():
			points.resize(points.size() - 1)
		if not stops.is_empty():
			stops.resize(stops.size() - 1)


## Lays the run or makes the line collected so far.
func finish() -> void:
	if kind == "track":
		_say(CityStore.dispatch(&"transit_track", [points, mode]))
	elif kind == "line" and _say(CityStore.dispatch(&"transit_line", [stops])):
		ctl.hint.emit(tr("hint.transit.lineMade"), true)
		ctl.plan_changed.emit()
	cancel()


func _say(result: Dictionary) -> bool:
	var ok: bool = result.get("ok", false)
	if not ok:
		ctl.hint.emit(tr(result.get("error", "")), false)
	return ok
