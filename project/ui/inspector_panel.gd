## The inspector side panel: what the inspect tool picked, under a name. A building: floors, residents and jobs. A road: its live numbers
## (InspectorRows) and what can be done to it: its class, reversed when one-way, split at the middle, duplicated, demolished. A junction:
## control, movements, traffic and each signal stage's lamp. The numbers follow the traffic while it is open; what the road's height,
## curve, parking, crossings and the junction's plan offer in the reference are the sessions road-elevation, roads and traffic's.
class_name InspectorPanel
extends PanelContainer

signal closed

@onready var _at := Vector3.ZERO
@onready var _info := {}
@onready var _stats: Control
@onready var _since := 0.0


func _ready() -> void:
	theme_type_variation = &"Surface"
	anchor_left = 1.0
	anchor_right = 1.0
	grow_horizontal = Control.GROW_DIRECTION_BEGIN
	offset_right = -HudWidgets.px("margin")
	offset_top = HudWidgets.px("toast_top")
	custom_minimum_size.x = HudWidgets.px("side_w")
	hide()


func _rows(col: Control, rows: Array) -> void:
	var g := PopBodies.grid(col)
	for r: Array in rows:
		PopBodies.metric(g, tr(r[0])).text = r[1]


func _building(col: Control, info: Dictionary) -> void:
	var fn_keys: Array = DataTables.ui("drawers").fn_keys
	var fn: int = info.fn
	var name := tr(fn_keys[fn]) if fn > 0 and fn < fn_keys.size() else tr("zone.name.%d" % info.zone)
	HudWidgets.label(col, &"TitleLabel", name.to_upper())
	HudWidgets.label(col, &"BodyLabel", tr("ui.floors.other").format({"count": info.floors}))
	_rows(col, [["ui.metric.residents", Formatters.format_int(info.residents)], ["ui.metric.jobs", Formatters.format_int(info.jobs)]])
	_demolish_row(col)


func _road(col: Control, info: Dictionary) -> void:
	HudWidgets.label(col, &"TitleLabel", ("%s · %s" % [tr("inspector.road"), tr(info.profile)]).to_upper())
	_stats = HudWidgets.vbox(col)
	_rows(_stats, InspectorRows.segment_rows(info))
	var picks := []
	for p: Dictionary in CityStore.query(&"road_profiles"):
		picks.append([p.index, tr(p.name_key)])
	HudWidgets.dropdown(col, tr("inspector.roadClass"), picks, info.profile_id, _set_class)
	var row := HFlowContainer.new()
	col.add_child(row)
	if info.one_way:
		_action(row, "inspector.reverse", &"reverse_road")
	_action(row, "inspector.splitMiddle", &"split_road")
	_action(row, "inspector.duplicate", &"duplicate_road")
	_demolish_row(col)


func _action(row: Control, key: String, command: StringName) -> void:
	var b := HudWidgets.button(&"Ghost", "", tr(key), "", "", "opt_row_h")
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
	HudWidgets.label(col, &"TitleLabel", tr("inspector.junction").to_upper())
	_stats = HudWidgets.vbox(col)
	_fill_node(_stats)


func _fill_node(col: Control) -> void:
	_rows(col, InspectorRows.node_rows(_info.id))
	for lamp: Array in InspectorRows.lamps(_info.id):
		var row := HudWidgets.hbox(col)
		HudWidgets.label(row, &"BodyLabel", lamp[0])


func _demolish_row(col: Control) -> void:
	var row := HudWidgets.hbox(col)
	var demolish := HudWidgets.button(&"IconButton", "demolish", "", "inspector.demolish", "icon_btn", "icon_btn")
	demolish.pressed.connect(_demolish.bind(_at))
	row.add_child(demolish)
	var x := HudWidgets.button(&"IconButton", "close", "", "v2.close", "icon_btn", "icon_btn")
	x.pressed.connect(close)
	row.add_child(x)


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
		"node":
			_node(col)
			_close_row(col)
		_: _road(col, info)
	show()


func _close_row(col: Control) -> void:
	var x := HudWidgets.button(&"IconButton", "close", "", "v2.close", "icon_btn", "icon_btn")
	x.pressed.connect(close)
	col.add_child(x)


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
