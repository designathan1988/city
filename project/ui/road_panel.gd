## What the Inspector does to a road's shape and to a point of it (Roadcraft ui/inspector.ts): the height of each end in half metres, its curve and where
## along it the curve stands, a height point in the middle; on a point, its height and joining the two roads in line there. A refusal comes back to the
## hint bar by the store.
class_name RoadPanel
extends RefCounted

const HEIGHT_STEP_M := 0.5
const CURVE_LEAST_M := 20.0
const CURVE_MOST_M := 280.0
const CURVE_SHARE := 0.65 # of the road's length
const CURVE_STRAIGHT_M := 0.5
const CURVE_STEP_M := 1.0
const AT_LEAST := 0.15
const AT_MOST := 0.85
const AT_STEP := 0.01
const AT_MIDDLE := 0.5
const JOINT_ARMS := 2
const ZEBRA := 1 # road_network.h NodeCrossing
const SIGNAL := 2


## `changed`: the panel is read again.
static func road(col: Control, info: Dictionary, at: Vector3, changed: Callable, moved: Callable) -> void:
	_stepper(col, "inspector.heightStart", info.node_a, info.lift_a, changed)
	_stepper(col, "inspector.heightEnd", info.node_b, info.lift_b, changed)
	var most := maxf(CURVE_LEAST_M, minf(CURVE_MOST_M, float(info.length_m) * CURVE_SHARE))
	var bent := absf(float(info.bend_m)) >= CURVE_STRAIGHT_M
	var curve := HudWidgets.range_row(col, tr_key("inspector.curvature"), -most, most, CURVE_STEP_M, clampf(info.bend_m, -most, most), _metres, _noop)
	var place := HudWidgets.range_row(col, tr_key("inspector.curvePosition"), AT_LEAST, AT_MOST, AT_STEP, float(info.bend_at) if bent else AT_MIDDLE, _percent, _noop)
	place.editable = bent
	curve.drag_ended.connect(func(did: bool) -> void:
		if did:
			var h := 0.0 if absf(curve.value) < CURVE_STRAIGHT_M else curve.value
			moved.call(CityStore.dispatch(&"bend_road", [info.id, h, place.value])))
	place.drag_ended.connect(func(did: bool) -> void:
		if did:
			moved.call(CityStore.dispatch(&"bend_road", [info.id, curve.value, place.value])))


## A point on a road: how high it stands and, where two roads meet in line, joining them.
static func point(col: Control, info: Dictionary, changed: Callable, gone: Callable) -> void:
	if int(info.arms) > JOINT_ARMS:
		return
	_stepper(col, "cmd.place.nodeHeight", info.id, info.lift, changed)
	if int(info.crossing) != 0:
		var off := HudWidgets.button(&"Ghost", "", tr_key("inspector.removeCrossing"), "", "", "opt_row_h")
		off.pressed.connect(func() -> void:
			CityStore.dispatch(&"remove_crossing", [info.id])
			changed.call())
		col.add_child(off)
	if int(info.arms) == JOINT_ARMS:
		var b := HudWidgets.button(&"Ghost", "", tr_key("inspector.joinAligned"), "", "", "opt_row_h")
		b.pressed.connect(func() -> void:
			if CityStore.dispatch(&"join_roads", [info.id]).get("ok", false):
				gone.call())
		col.add_child(b)


static func _stepper(col: Control, key: String, node: int, metres: float, changed: Callable) -> void:
	var row := HudWidgets.option_row(col, tr_key(key))
	var label := "%.1f m" % metres
	for step: float in [-HEIGHT_STEP_M, HEIGHT_STEP_M]:
		var b := HudWidgets.button(&"Ghost", "", "\u2212" if step < 0.0 else "+", "junction.lower" if step < 0.0 else "junction.raise", "icon_btn", "icon_btn")
		b.pressed.connect(func() -> void:
			CityStore.dispatch(&"set_node_height", [node, metres + step])
			changed.call())
		row.add_child(b)
		if step < 0.0:
			HudWidgets.label(row, &"BodyLabel", label)


static func tr_key(key: String) -> String:
	return TranslationServer.translate(key)


static func _metres(v: float) -> String:
	return "%d m" % roundi(v)


static func _percent(v: float) -> String:
	return "%d%%" % roundi(v * 100.0)


static func _noop(_v: float) -> void:
	pass
