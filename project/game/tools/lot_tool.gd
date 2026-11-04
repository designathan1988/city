## Draw a building (Build › Desenhar): press at one corner, drag to the other, release to
## build. The lot lines up with the nearest road and faces it; the card picked in the strip says what it is (school,
## park, square, … or a building of the land use). The ribbon previews the rectangle green or red.
extends ToolBase

var fn := 0
var zone := 1
var _a := Vector3.INF


func enter() -> void:
	fn = params.get("fn", 0)
	zone = params.get("zone", 1)
	ctl.hint.emit(tr("hint.lot"), true)


func exit() -> void:
	ctl.preview.clear()


func click(p: Vector3) -> void:
	_a = p


func drag(p: Vector3) -> void:
	if _a == Vector3.INF:
		return
	var pv: Dictionary = CityStore.query(&"preview_lot", [_a, p])
	if pv.is_empty():
		return
	ctl.preview.show_ribbon(pv.points, pv.half_width, pv.ok)
	ctl.hint.emit(tr("hint.lot") if pv.ok else refusal(pv), pv.ok)


func release(p: Vector3) -> void:
	if _a == Vector3.INF or p == Vector3.INF:
		return
	var r: Dictionary = CityStore.dispatch(&"draw_building", [_a, p, fn, zone])
	_a = Vector3.INF
	ctl.preview.clear()
	ctl.hint.emit(tr("ui.build.placed") if r.get("ok", false) else refusal(r), r.get("ok", false))


func cancel() -> void:
	_a = Vector3.INF
	ctl.preview.clear()
