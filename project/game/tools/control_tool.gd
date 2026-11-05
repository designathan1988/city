## Control (tab Controle): a click on a junction takes it to its next control, Shift to the one before
## (Roadcraft main.ts cycleNodeControl). The marks of every junction are the HUD's (ui/junction_marks.gd).
extends ToolBase


func enter() -> void:
	ctl.hint.emit(tr("hint.control"), true)


func click(p: Vector3) -> void:
	var hit: Dictionary = CityStore.query(&"node_at", [p, ctl.px_at(p)])
	if hit.node < 0:
		ctl.hint.emit(tr("hint.control.miss"), false)
		return
	var marks: Array = DataTables.ui("hud").control_marks
	var step := -1 if Input.is_key_pressed(KEY_SHIFT) else 1
	var next := posmod(int(hit.ctl) + step, marks.size())
	var r: Dictionary = CityStore.dispatch(&"set_node_control", [hit.node, next])
	ctl.hint.emit(tr(marks[next].label_key) if r.get("ok", false) else refusal(r), r.get("ok", false))
