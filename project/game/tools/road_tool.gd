## Road tool (modes "Reta" and "Curva"): click the start, in curve mode click
## the control point, click the end. While aiming, the core plans the road (preview_road: the same validation the build
## runs) and the ribbon shows it green or red with the price; the click on the end builds it and the road chains on
## from its end. V toggles straight/curve, Esc or right click drops the stroke.
extends ToolBase

enum Step { START, CONTROL, END }

var step: Step = Step.START
var curve := false
var profile := 0
var start := Vector3.ZERO
var control := Vector3.ZERO
var _last_plan: Dictionary = {}


func enter() -> void:
	profile = params.get("profile", 0)
	curve = params.get("curve", false)
	ctl.hint.emit(tr("ui.do.roadPoint"), true)


func exit() -> void:
	ctl.preview.clear()


func click(p: Vector3) -> void:
	match step:
		Step.START:
			var snap: Dictionary = CityStore.query(&"snap_road", [p, profile])
			start = snap.get("position", p)
			step = Step.CONTROL if curve else Step.END
		Step.CONTROL:
			control = p
			step = Step.END
		Step.END:
			_build(p)


func hover(p: Vector3) -> void:
	if step == Step.START:
		ctl.preview.clear()
		return
	if step == Step.CONTROL:
		_preview(p, false, Vector3.ZERO)
	else:
		_preview(p, curve, control)


func cancel() -> void:
	step = Step.START
	ctl.preview.clear()
	ctl.hint.emit(tr("ui.do.roadPoint"), true)


func key(keycode: Key) -> void:
	if keycode == KEY_V:
		curve = not curve
		cancel()


func _preview(end: Vector3, has_control: bool, c: Vector3) -> void:
	_last_plan = CityStore.dispatch(&"preview_road", [start, end, has_control, c, profile])
	if _last_plan.is_empty():
		return
	var ok: bool = _last_plan.ok and _last_plan.affordable
	ctl.preview.show_ribbon(_last_plan.points, _last_plan.half_width, ok)
	if not _last_plan.ok:
		ctl.hint.emit(refusal(_last_plan), false)
	elif not _last_plan.affordable:
		ctl.hint.emit(refusal({"error": "ui.insp.noMoney"}, _last_plan.price), false)
	else:
		ctl.hint.emit(Formatters.format_money(_last_plan.price), true)


func _build(end: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"build_road", [start, end, curve, control, profile])
	if r.is_empty() or not r.ok:
		ctl.hint.emit(refusal(r, _last_plan.get("price", 0.0)), false)
		return
	var snap: Dictionary = CityStore.query(&"snap_road", [end, profile])
	start = snap.get("position", end)
	step = Step.CONTROL if curve else Step.END
	ctl.preview.clear()
	ctl.hint.emit(Formatters.format_money(r.money), true)
