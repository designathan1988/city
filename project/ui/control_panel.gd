## A junction's control in the inspector: the player's choice as one set of buttons (automatic, signal, signs, stop on every leg,
## give way on every leg, mini-roundabout, none); on automatic, what the junction chose for itself; under any other, that it is
## locked on the player's choice, with the way back to automatic. It keeps nothing: every press goes to the store and the panel is
## drawn again from what the core reports (`inspect`: ctl, ruled_by, all_stop).
class_name ControlPanel
extends RefCounted

## The controls in the order shown: [the core's NodeControl (road_network.h), its text].
const CONTROLS := [[0, "junction.control.auto"], [1, "junction.control.signal"], [2, "junction.control.priority"],
	[3, "junction.control.stop"], [4, "junction.control.yield"], [6, "junction.control.mini"], [5, "junction.control.none"]]
const AUTO := 0
const A_ROW := 4 # buttons a row: the panel's width holds no more
const LEAST_ARMS := 3 # fewer roads make no junction to control
## What rules the junction now (the core's Control: none, by rank, a signal), named by the control it amounts to.
const RULED_KEYS := ["junction.control.none", "junction.control.priority", "junction.control.signal"]


## `info` is what `inspect` reports of the junction; `redraw` draws the panel again after a change.
static func fill(col: Control, info: Dictionary, redraw: Callable) -> void:
	if info.arms < LEAST_ARMS:
		return
	HudWidgets.label(col, &"TitleLabel", col.tr("junction.control"))
	var items := []
	for c: Array in CONTROLS:
		items.push_back([c[0], col.tr(c[1])])
	for from in range(0, items.size(), A_ROW):
		HudWidgets.choices(col, items.slice(from, from + A_ROW), info.ctl, func(ctl: int) -> Dictionary: # refused: the old one stays lit
			var answer: Dictionary = CityStore.dispatch(&"set_node_control", [info.id, ctl])
			redraw.call()
			return answer)
	if info.ctl == AUTO:
		var chosen: String = "junction.control.stop" if info.all_stop else RULED_KEYS[info.ruled_by]
		HudWidgets.label(col, &"BodyLabel", "%s: %s" % [col.tr("junction.control.auto"), col.tr(chosen)])
		return
	HudWidgets.label(col, &"BodyLabel", col.tr("junction.locked"))
	var unlock := HudWidgets.button(&"SegButton", "", col.tr("junction.unlock"), "junction.unlock", "", "")
	unlock.pressed.connect(func() -> void:
		CityStore.dispatch(&"set_node_control", [info.id, AUTO])
		redraw.call())
	col.add_child(unlock)
