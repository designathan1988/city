## Each road's rule at a junction, in the inspector (Roadcraft ui/roads/junctionPanel.ts): priority, give way or stop by leg, named as the player sees it. It keeps
## nothing: every press goes to the store and the panel is drawn again from what the core reports.
class_name ApproachPanel
extends RefCounted

const RULES := [["junction.rule.priority", &"RULE_NONE"], ["junction.rule.yield", &"RULE_YIELD"], ["junction.rule.stop", &"RULE_STOP"]] # the core's SegRule
const SCREEN_DIRS := ["up", "upRight", "right", "downRight", "down", "downLeft", "left", "upLeft"]
const EIGHTH := PI / 4.0


## The way a leg points on the screen: 0 up, clockwise (the camera may have the map turned).
static func _named(col: Control, leg: Dictionary) -> String:
	var cam := col.get_viewport().get_camera_3d()
	var out := Vector3(leg.dx, 0.0, leg.dz)
	var right := cam.global_basis.x
	var up := -cam.global_basis.z
	up.y = 0.0
	var angle := atan2(out.dot(right), out.dot(up.normalized()))
	return col.tr("screenDir." + SCREEN_DIRS[posmod(roundi(angle / EIGHTH), SCREEN_DIRS.size())])


static func fill(col: Control, node: int, redraw: Callable) -> void:
	var legs: Array = CityStore.query(&"node_legs", [node])
	if legs.size() < 3:
		return
	HudWidgets.label(col, &"TitleLabel", col.tr("junction.legs"))
	for leg: Dictionary in legs:
		var row := HudWidgets.option_row(col, _named(col, leg))
		var items := []
		for r: Array in RULES:
			items.push_back([CityStore.code(&"SegRule", r[1]), col.tr(r[0])])
		HudWidgets.choices(row, items, leg.rule, func(rule: int) -> void:
			CityStore.dispatch(&"set_approach_rule", [node, leg.seg, rule])
			redraw.call())
	_movements(col, node, legs, redraw)


## The movements through it, a row per road they come from: pressed, allowed.
static func _movements(col: Control, node: int, legs: Array, redraw: Callable) -> void:
	HudWidgets.label(col, &"TitleLabel", col.tr("junction.movements"))
	for from: Dictionary in legs:
		var row := HudWidgets.option_row(col, _named(col, from))
		for to: Dictionary in legs:
			if to.seg == from.seg:
				continue
			var banned: bool = from.banned.has(to.seg)
			var chip := HudWidgets.button(&"Ghost", "", _named(col, to), "junction.moveBlocked" if banned else "junction.moveAllowed", "", "opt_row_h")
			chip.toggle_mode = true
			chip.set_pressed_no_signal(not banned)
			chip.pressed.connect(func() -> void:
				CityStore.dispatch(&"set_movement_banned", [node, from.seg, to.seg, not banned])
				redraw.call())
			row.add_child(chip)
