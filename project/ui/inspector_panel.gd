## The inspector side panel: what the inspect tool picked, under a name. A building: floors, residents and jobs. A road: its live numbers
## (InspectorRows) and what can be done to it: its class, reversed when one-way, split at the middle, duplicated, demolished. A junction:
## control, movements, traffic and each signal stage's lamp. The numbers follow the traffic while it is open; what the road's height,
## curve, parking, crossings and the junction's plan offer in the reference are the sessions road-elevation, roads and traffic's.
class_name InspectorPanel
extends PanelContainer

signal closed

const LAMP_DOTS := [&"DotGo", &"DotSlow", &"DotBroken"]

@onready var _at := Vector3.ZERO
@onready var _info := {}
@onready var _stats: Control
@onready var _since := 0.0


func _ready() -> void:
	theme_type_variation = &"SidePanel"
	anchor_left = 1.0
	anchor_right = 1.0
	grow_horizontal = Control.GROW_DIRECTION_BEGIN
	offset_right = -HudWidgets.px("margin")
	offset_top = HudWidgets.px("toast_top")
	custom_minimum_size.x = HudWidgets.px("side_w")
	hide()


## The numbers as cards, two to a row (.inspect-grid).
func _rows(col: Control, rows: Array) -> void:
	var g := GridContainer.new()
	g.columns = 2
	g.theme_type_variation = &"StatGrid"
	col.add_child(g)
	for r: Array in rows:
		var card := HudWidgets.panel(g, &"StatCard")
		card.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		var v := HudWidgets.vbox(card)
		HudWidgets.label(v, &"StatName", tr(r[0]))
		HudWidgets.label(v, &"StatValue", r[1])


## The panel's name with the close button at its end.
func _head(col: Control, text: String) -> void:
	var row := HudWidgets.hbox(col)
	var name := HudWidgets.label(row, &"SideTitle", text)
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var x := HudWidgets.button(&"IconButton", "close", "", "v2.close", "side_close", "side_close")
	x.pressed.connect(close)
	row.add_child(x)


func _actions(col: Control) -> GridContainer:
	var g := GridContainer.new()
	g.columns = 2
	g.theme_type_variation = &"StatGrid"
	col.add_child(g)
	return g


func _building(col: Control, info: Dictionary) -> void:
	var fn_keys: Array = DataTables.ui("drawers").fn_keys
	var fn: int = info.fn
	var name := tr(fn_keys[fn]) if fn > 0 and fn < fn_keys.size() else tr("zone.name.%d" % info.zone)
	_head(col, name)
	HudWidgets.label(col, &"BodyLabel", tr("ui.floors.other").format({"count": info.floors}))
	_rows(col, [["ui.metric.residents", Formatters.format_int(info.residents)], ["ui.metric.jobs", Formatters.format_int(info.jobs)]])
	_demolish_button(_actions(col))


func _road(col: Control, info: Dictionary) -> void:
	_head(col, "%s · %s" % [tr("inspector.road"), tr(info.profile)])
	_stats = HudWidgets.vbox(col)
	_rows(_stats, InspectorRows.segment_rows(info))
	for e in info.get("ends", []):
		_end_row(col, e)
	var picks := []
	for p: Dictionary in CityStore.query(&"road_profiles"):
		picks.append([p.index, tr(p.name_key)])
	HudWidgets.dropdown(col, tr("inspector.roadClass"), picks, info.profile_id, _set_class)
	var row := _actions(col)
	if info.one_way:
		_action(row, "inspector.reverse", &"reverse_road")
	_action(row, "inspector.duplicate", &"duplicate_road")
	_action(row, "inspector.splitMiddle", &"split_road")
	_demolish_button(row)


## A dead end of the road: "Ponta inicial [balão]"; the choice comes back from the core, so a refused one stays off.
func _end_row(col: Control, e: Dictionary) -> void:
	var row := HudWidgets.hbox(col)
	var name := HudWidgets.label(row, &"MutedLabel", tr("inspector.endStart" if e.start else "inspector.endEnd"))
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var b := HudWidgets.button(&"IconButton", "roundabout", "", "inspector.end.bulb", "icon_btn", "icon_btn")
	b.toggle_mode = true
	b.set_pressed_no_signal(e.bulb)
	b.pressed.connect(func() -> void:
		CityStore.dispatch(&"set_road_end", [e.node, not e.bulb])
		_redraw())
	row.add_child(b)


func _redraw() -> void:
	show_info(CityStore.query(&"inspect", [_at]), _at)


func _action(row: Control, key: String, command: StringName) -> void:
	var b := HudWidgets.button(&"Ghost", "", tr(key), "", "", "opt_row_h")
	b.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	b.pressed.connect(_edit.bind(command))
	row.add_child(b)


## A road edit: what the panel showed is gone or changed, so it closes, as after the reference's edits that replace the road.
func _edit(command: StringName) -> void:
	var at: Variant = _at if command == &"split_road" else _info.id
	CityStore.dispatch(command, [at])
	close()


func duplicate_shown() -> bool:
	if not visible or _info.get("kind", "") != "road":
		return false
	_edit(&"duplicate_road")
	return true


func _set_class(profile: int) -> void:
	CityStore.dispatch(&"upgrade_road", [_at, profile, false])
	refresh()


func _node(col: Control) -> void:
	_head(col, tr("inspector.junction"))
	_stats = HudWidgets.vbox(col)
	_fill_node(_stats)
	if _info.has("greens"): # a signal: what the player sets on it (JunctionPanel), drawn again from what the core reports
		JunctionPanel.fill(col, _info, _redraw)


func _fill_node(col: Control) -> void:
	_rows(col, InspectorRows.node_rows(_info.id))
	var chips := HFlowContainer.new()
	col.add_child(chips)
	for lamp: Array in InspectorRows.lamps(_info.id):
		var row := HudWidgets.hbox(HudWidgets.panel(chips, &"LampChip"))
		var dot := Panel.new()
		dot.theme_type_variation = LAMP_DOTS[lamp[1]]
		dot.custom_minimum_size = Vector2.ONE * HudWidgets.px("dot")
		dot.size_flags_vertical = Control.SIZE_SHRINK_CENTER
		row.add_child(dot)
		HudWidgets.label(row, &"MetaLabel", lamp[0])


func _demolish_button(row: Control) -> void:
	var demolish := HudWidgets.button(&"GhostDanger", "", tr("inspector.demolish"), "", "", "opt_row_h")
	demolish.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	demolish.pressed.connect(_demolish.bind(_at))
	row.add_child(demolish)


## The Delete key: what the panel shows goes; false when it shows nothing.
func demolish_shown() -> bool:
	if not visible or _info.get("kind", "") == "node":
		return false
	_demolish(_at)
	return true


func show_info(info: Dictionary, at: Vector3) -> void:
	_at = at
	_info = info
	_stats = null
	HudWidgets.clear(self)
	if info.is_empty() or not info.has("kind"):
		close()
		return
	var col := HudWidgets.vbox(self, &"Pad")
	match info.kind:
		"building": _building(col, info)
		"node": _node(col)
		_: _road(col, info)
	show()


## The numbers again, or everything when what was shown changed under the panel.
func refresh() -> void:
	if not visible:
		return
	var now: Dictionary = CityStore.query(&"inspect", [_at])
	if now.get("kind", "") != _info.get("kind", "") or now.get("id", -1) != _info.get("id", -1) or now.get("profile_id", -1) != _info.get("profile_id", -1):
		show_info(now, _at)
	elif _stats != null:
		HudWidgets.clear(_stats)
		if _info.kind == "node":
			_fill_node(_stats)
		else:
			_rows(_stats, InspectorRows.segment_rows(_info))


func close() -> void:
	if visible:
		closed.emit()
	hide()
	_info = {}


func _process(delta: float) -> void:
	if not visible:
		return
	_since += delta
	if _since >= DataTables.game("health").refresh_s:
		_since = 0.0
		refresh()


func _notification(what: int) -> void:
	if what == NOTIFICATION_TRANSLATION_CHANGED and is_node_ready() and visible:
		show_info(_info, _at)


func _demolish(at: Vector3) -> void:
	CityStore.dispatch(&"demolish", [at])
	close()
