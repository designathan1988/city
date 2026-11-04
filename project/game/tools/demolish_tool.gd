## Bulldoze: a click removes the building under it, else the rubble, else the nearest road; a drag removes everything
## inside the rectangle between where the button went down and where it came up. The core refunds the roads' share and
## the result tells how much.
extends ToolBase

## A drag shorter than this is a click.
const DRAG_MIN_M := 2.0

var _corner := Vector3.INF


func enter() -> void:
	ctl.hint.emit("", true)


func click(p: Vector3) -> void:
	_corner = p


func release(p: Vector3) -> void:
	var from := _corner
	_corner = Vector3.INF
	if from == Vector3.INF:
		return
	var box := p != Vector3.INF and from.distance_to(p) > DRAG_MIN_M
	var r: Dictionary = CityStore.dispatch(&"demolish_box", [from, p]) if box else CityStore.dispatch(&"demolish", [from])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	var text := tr("game.toast.demolished.one") if not box else tr("game.toast.demolished.box")
	if r.money < 0:
		text += tr("game.toast.refunded").format({"money": Formatters.format_money(-r.money)})
	ctl.hint.emit(text, true)
