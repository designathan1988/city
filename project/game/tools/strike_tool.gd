## Bomb: a click lets a charge of the chosen force fall where the pointer is (on the ground under it); the core says
## what it brought down, worn and killed, and the whole blow is one Undo step. The force (1..strength_max) is the
## tool's `strength` param, which the Demolish drawer's stepper sets; it starts at the smallest the data allows.
extends ToolBase


func enter() -> void:
	ctl.hint.emit(tr("help.tool.strike"), true)


func click(p: Vector3) -> void:
	var force: int = int(params.get("strength", DataTables.catalog().blast.strength_min))
	var r: Dictionary = CityStore.dispatch(&"strike", [p, force])
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
		return
	ctl.hint.emit(tr("hint.strike.done").format({"force": force}), true)
