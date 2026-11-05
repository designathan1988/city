## Bulldoze: a click removes the building under it, else the nearest road; the core
## refunds the road's share and the result tells how much.
extends ToolBase


func enter() -> void:
	ctl.hint.emit(tr("help.tool.bulldoze"), true)


func click(p: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"demolish", [p])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	var text := tr("game.toast.demolished.one")
	if r.money < 0:
		text += tr("game.toast.refunded").format({"money": Formatters.format_money(-r.money)})
	ctl.hint.emit(text, true)
