## Bomb: a click lets a charge of the chosen force fall where the pointer is (on the ground under it); the core says
## what it brought down, worn and killed, and the whole blow is one Undo step. The force (1..strength_max) is the
## tool's `strength` param, which the bomb tab's slider sets (hud_drawer.gd); it starts at the smallest the data allows.
extends ToolBase


func click(p: Vector3) -> void:
	var force: int = int(params.get("strength", DataTables.catalog().blast.strength_min))
	var r: Dictionary = CityStore.dispatch(&"strike", [p, force])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	ctl.hint.emit(tr("hint.strike.done").format({"force": force}), true)
