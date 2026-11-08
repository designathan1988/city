## What the Inspector does to a road's shape and to a point of it (Roadcraft ui/inspector.ts): the height of each end in half metres, its curve and where
## along it the curve stands, a height point in the middle; on a point, its height and joining the two roads in line there. A refusal comes back to the
## hint bar by the store.
class_name RoadPanel
extends RefCounted

const RAISE_BY_M := 0.5
const CURVE_LEAST_M := 20.0
const CURVE_MOST_M := 280.0
const CURVE_SHARE := 0.65 # of the road's length
const CURVE_STRAIGHT_M := 0.5
const CURVE_STEP_M := 1.0
const CURVE_FROM := 0.15
const CURVE_TO := 0.85
const AT_STEP := 0.01
const AT_MIDDLE := 0.5
const JOINT_ARMS := 2


## `changed`: the panel is read again.
static func road(col: Control, info: Dictionary, at: Vector3, changed: Callable, moved: Callable) -> void:
	_direction(col, info, at, changed)
	_stepper(col, "inspector.heightStart", info.node_a, info.lift_a, changed)
	_stepper(col, "inspector.heightEnd", info.node_b, info.lift_b, changed)
	var most := maxf(CURVE_LEAST_M, minf(CURVE_MOST_M, float(info.length_m) * CURVE_SHARE))
	var bent := absf(float(info.bend_m)) >= CURVE_STRAIGHT_M
	var curve := HudWidgets.range_row(col, tr_key("inspector.curvature"), -most, most, CURVE_STEP_M, clampf(info.bend_m, -most, most), _metres, _noop)
	var place := HudWidgets.range_row(col, tr_key("inspector.curvePosition"), CURVE_FROM, CURVE_TO, AT_STEP, float(info.bend_at) if bent else AT_MIDDLE, _percent, _noop)
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


## Both ways, or one: the road's own section with its lanes turned, as Editar perfil would leave it.
static func _direction(col: Control, info: Dictionary, at: Vector3, changed: Callable) -> void:
	var spec: Dictionary = CityStore.query(&"road_spec", [info.profile_id])
	var now := 0
	if info.one_way:
		now = 1 if spec.elements.any(func(e: Dictionary) -> bool: return e.kind == "lane" and e.dir > 0) else -1
	var items := [[0, tr_key("direction.both")], [1, tr_key("direction.aToB")], [-1, tr_key("direction.bToA")]]
	HudWidgets.dropdown(col, tr_key("inspector.direction"), items, now, func(dir: int) -> void:
		var index: int = CityStore.query(&"road_spec_make", [info.profile_id, RoadSpecOps.directed(spec, dir)])
		if index >= 0:
			CityStore.dispatch(&"upgrade_road", [at, index, false])
		changed.call())


static func _stepper(col: Control, key: String, node: int, metres: float, changed: Callable) -> void:
	var row := HudWidgets.option_row(col, tr_key(key))
	var label := "%.1f m" % metres
	for step: float in [-RAISE_BY_M, RAISE_BY_M]:
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
	return "%d%%" % roundi(v * Formatters.PERCENT)


static func _noop(_v: float) -> void:
	pass


static func _work(row: Control, text: String, command: StringName, args: Array, redraw: Callable) -> void:
	var b := HudWidgets.button(&"Ghost", "", text, "", "", "opt_row_h")
	b.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	b.pressed.connect(func() -> void:
		if CityStore.dispatch(command, args).get("ok", false):
			redraw.call())
	row.add_child(b)


## What can be put on a road: a height point, a crossing, a way back through its median.
static func works(row: Control, info: Dictionary, at: Vector3, redraw: Callable) -> void:
	_work(row, row.tr("inspector.addHeightPoint"), &"split_road", [at], redraw)
	_work(row, row.tr("inspector.addZebra"), &"add_crossing", [at, CityStore.code(&"NodeCrossing", &"CROSSING_ZEBRA")], redraw)
	_work(row, row.tr("inspector.addSignalCrossing"), &"add_crossing", [at, CityStore.code(&"NodeCrossing", &"CROSSING_SIGNAL")], redraw)
	if info.median:
		_work(row, row.tr("inspector.addUturnToEnd"), &"add_uturn", [at, true], redraw)
		_work(row, row.tr("inspector.addUturnToStart"), &"add_uturn", [at, false], redraw)


## A dead end of the road: "Ponta inicial [balão]"; the choice comes back from the core, so a refused one stays off.
static func end_row(col: Control, e: Dictionary, redraw: Callable) -> void:
	var row := HudWidgets.hbox(col)
	var name := HudWidgets.label(row, &"MutedLabel", col.tr("inspector.endStart" if e.start else "inspector.endEnd"))
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var b := HudWidgets.button(&"IconButton", "roundabout", "", "inspector.end.bulb", "icon_btn", "icon_btn")
	b.toggle_mode = true
	b.set_pressed_no_signal(e.bulb)
	b.pressed.connect(func() -> void:
		CityStore.dispatch(&"set_road_end", [e.node, not e.bulb])
		redraw.call())
	row.add_child(b)
