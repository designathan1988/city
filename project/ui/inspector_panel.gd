## The inspector side panel: what the inspect tool picked, under a name. A building: floors, residents and jobs. A road: its live numbers
## (InspectorRows) and what can be done to it: its class, reversed when one-way, split at the middle, duplicated, demolished. A junction:
## control, movements, traffic and each signal stage's lamp. The numbers follow the traffic while it is open; what the road's height,
## curve, parking, crossings and the junction's plan offer in the reference are the sessions road-elevation, roads and traffic's.
class_name InspectorPanel
extends PanelContainer

signal closed
signal edit_profile(profile: Dictionary, spec: Dictionary, at: Vector3)
signal laid_again(info: Dictionary) # the road shown stands somewhere else now

const LAMP_DOTS := [&"DotGo", &"DotSlow", &"DotBroken"]

@onready var _at := Vector3.ZERO
@onready var _info := {}
@onready var _stats: Control
@onready var _since := 0.0
var _stat_grid: GridContainer
var _stat_names: Array[Label] = []
var _stat_values: Array[Label] = []
var _lamp_chips: HFlowContainer
var _lamp_names: Array[Label] = []
var _lamp_dots: Array[Panel] = []
var _info_key: Array = []


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
	_set_stats(InspectorRows.segment_rows(info), [])
	for e in info.get("ends", []):
		RoadPanel.end_row(col, e, _redraw)
	var picks := []
	for p: Dictionary in CityStore.query(&"road_profiles"):
		picks.append([p.index, tr(p.name_key)])
	HudWidgets.dropdown(col, tr("inspector.roadClass"), picks, info.profile_id, _set_class)
	RoadPanel.road(col, info, _at, _redraw, _bent)
	var row := _actions(col)
	if info.one_way:
		_action(row, "inspector.reverse", &"reverse_road")
	_action(row, "inspector.duplicate", &"duplicate_road")
	_action(row, "inspector.splitMiddle", &"split_road")
	var profile := HudWidgets.button(&"Ghost", "", tr("profileEditor.open"), "", "", "opt_row_h")
	profile.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	profile.pressed.connect(func() -> void:
		edit_profile.emit({"index": info.profile_id, "id": ""}, CityStore.query(&"road_spec", [info.profile_id]), _at))
	row.add_child(profile)
	RoadPanel.works(row, info, _at, _redraw)
	_demolish_button(row)


## The road was laid again: the panel goes on with it, where it stands now.
func _bent(result: Dictionary) -> void:
	if result.get("ok", false) and result.has("at"):
		_at = result.at
	_redraw()
	laid_again.emit(_info)


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
	_fill_node()
	RoadPanel.point(col, _info, _redraw, close)
	_impossible(col)
	ApproachPanel.fill(col, _info.id, _redraw)
	ConnectorPanel.fill(col, _info.id)
	if _info.has("greens"): # a signal: what the player sets on it (JunctionPanel), drawn again from what the core reports
		JunctionPanel.fill(col, _info, _redraw)


## A junction that cannot close (Network.impossible): told, and it can be taken away with its roads.
func _impossible(col: Control) -> void:
	for n: Dictionary in CityStore.query(&"impossible_nodes"):
		if n.id != _info.id:
			continue
		HudWidgets.label(col, &"BodyLabel", "%s · %s" % [tr("inspector.impossible"), tr("inspector.impossibleGap").format({"angle": roundi(n.gap_deg)})])
		var off := HudWidgets.button(&"GhostDanger", "", tr("inspector.demolish"), "", "", "opt_row_h")
		off.pressed.connect(func() -> void:
			CityStore.dispatch(&"remove_node", [_info.id])
			close())
		col.add_child(off)


func _fill_node() -> void:
	var traffic: Dictionary = CityStore.query(&"node_traffic", [_info.id])
	_set_stats(InspectorRows.node_rows(traffic), InspectorRows.lamps(traffic))


## Layout changes create controls; numeric/lamp changes update the existing controls only when their shown value changes.
func _set_stats(rows: Array, lamps: Array) -> void:
	if _stat_grid == null or _stat_values.size() != rows.size():
		if _stat_grid != null:
			_stats.remove_child(_stat_grid)
			_stat_grid.queue_free()
		_stat_names.clear()
		_stat_values.clear()
		_stat_grid = GridContainer.new()
		_stat_grid.columns = 2
		_stat_grid.theme_type_variation = &"StatGrid"
		_stats.add_child(_stat_grid)
		_stats.move_child(_stat_grid, 0)
		for _row_data: Array in rows:
			var card := HudWidgets.panel(_stat_grid, &"StatCard")
			card.size_flags_horizontal = Control.SIZE_EXPAND_FILL
			var values := HudWidgets.vbox(card)
			_stat_names.push_back(HudWidgets.label(values, &"StatName"))
			_stat_values.push_back(HudWidgets.label(values, &"StatValue"))
	for i in rows.size():
		var name := tr(rows[i][0])
		if _stat_names[i].text != name:
			_stat_names[i].text = name
		if _stat_values[i].text != rows[i][1]:
			_stat_values[i].text = rows[i][1]
	if _info.kind != "node":
		return
	if _lamp_chips == null:
		_lamp_chips = HFlowContainer.new()
		_stats.add_child(_lamp_chips)
	if _lamp_names.size() != lamps.size():
		HudWidgets.clear(_lamp_chips)
		_lamp_names.clear()
		_lamp_dots.clear()
		for _lamp_data: Array in lamps:
			var row := HudWidgets.hbox(HudWidgets.panel(_lamp_chips, &"LampChip"))
			var dot := Panel.new()
			dot.custom_minimum_size = Vector2.ONE * HudWidgets.px("dot")
			dot.size_flags_vertical = Control.SIZE_SHRINK_CENTER
			row.add_child(dot)
			_lamp_dots.push_back(dot)
			_lamp_names.push_back(HudWidgets.label(row, &"MetaLabel"))
	for i in lamps.size():
		var variation: StringName = LAMP_DOTS[lamps[i][1]]
		if _lamp_dots[i].theme_type_variation != variation:
			_lamp_dots[i].theme_type_variation = variation
		if _lamp_names[i].text != lamps[i][0]:
			_lamp_names[i].text = lamps[i][0]


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
	_info_key = _descriptor_key()
	_stats = null
	_stat_grid = null
	_stat_names.clear()
	_stat_values.clear()
	_lamp_chips = null
	_lamp_names.clear()
	_lamp_dots.clear()
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
func _descriptor_key() -> Array:
	var snapshot := CityStore.snapshot
	return [snapshot.get("city_generation", -1), snapshot.get("edit_revision", -1),
		snapshot.get("traffic_graph_revision", -1) if _info.get("kind", "") == "node" else snapshot.get("buildings_revision", -1)]


func refresh() -> void:
	if not visible:
		return
	var now: Dictionary = _info
	var key := _descriptor_key()
	if key != _info_key:
		now = CityStore.query(&"inspect", [_at])
		_info_key = key
	if now != _info:
		show_info(now, _at)
	elif _stats != null:
		if _info.kind == "node":
			_fill_node()
		else:
			_set_stats(InspectorRows.segment_rows(_info), [])


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
