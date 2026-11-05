## The road tool's options (Roadcraft ui/v2/shell.ts renderRoads): a row per choice, each a label and its toggles, standing in the
## drawer's options panel while the road tool is in hand (hud_drawer.gd). Alignment is the tool's own parameter; snapping is the
## player's setting (Settings.road_snap). The rows and their icons come from data/ui/drawers.json "road_options".
class_name RoadOptions
extends VBoxContainer

signal alignment_picked(alignment: int)
## A choice of the class road's: "lanes", "width" or "parking".
signal option_picked(key: String, value: Variant)
## The card the median choice takes the tool to.
signal road_picked(id: String)

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")


func _ready() -> void:
	theme_type_variation = &"VPad"
	if tools:
		tools.tool_changed.connect(func(_tool: int, _params: Dictionary) -> void:
			if is_inside_tree():
				refresh())
		tools.road_height_changed.connect(func() -> void:
			if is_inside_tree():
				refresh())


## The rows again from the tool's parameters and the player's settings.
func refresh() -> void:
	HudWidgets.clear(self)
	var spec: Dictionary = DataTables.ui("drawers").road_options
	var picked: int = tools.last_param(ToolController.Tool.ROAD, "alignment", 0) if tools else 0
	var row := _row(spec.trace.label_key)
	for i in spec.trace.choices.size():
		var c: Dictionary = spec.trace.choices[i]
		_toggle(row, c, i == picked, true, func() -> void: alignment_picked.emit(i))
	row = _row(spec.snap.label_key)
	var snap: Dictionary = Settings.road_snap
	for c in spec.snap.choices:
		var key: String = c.key
		var live: bool = key == "on" or snap.on
		_toggle(row, c, snap[key] and live, live, func() -> void:
			Settings.set_road_snap(key, not snap[key])
			refresh())
	var class_card: bool = _base().class_card
	if class_card:
		_class_rows(spec)
	_height_rows()
	_furniture_row(spec.furniture)
	if class_card:
		_parking_row(spec)


## The base road in hand, as the core describes it.
func _base() -> Dictionary:
	return CityStore.query(&"road_profiles")[_param("profile", CityStore.first_road())]


func _param(key: String, fallback: Variant) -> Variant:
	return tools.last_param(ToolController.Tool.ROAD, key, fallback) if tools else fallback


## Lanes and width tune a class's own road, and parking (_parking_row); a road of the catalogue carries its own (Roadcraft ui/v2/shell.ts).
func _class_rows(spec: Dictionary) -> void:
	var base := _base()
	var chosen: int = _param("lanes", 0)
	var row := _row(spec.lanes.label_key)
	for c in spec.lanes.choices:
		var count: int = c.count
		var on: bool = chosen == 0 and base.id == spec.lanes.median_road if count == 0 else (chosen if chosen > 0 else base.lanes) == count
		var pick := func() -> void:
			if count == 0:
				road_picked.emit(spec.lanes.median_road)
			option_picked.emit("lanes", count)
		_toggle(row, c, on, true, pick)
		var b: Button = row.get_child(-1)
		b.tooltip_text = tr("palette.lanes.median") if count == 0 else tr("palette.lanes.count.other").format({"count": count})
	row = _row(spec.width.label_key)
	var width: float = _param("width", 0.0)
	_step(row, "minus", width, -spec.width.step_m, base.width)
	HudWidgets.label(row, &"BodyLabel", tr("palette.width.auto") if width <= 0 else "%d m" % roundi(width))
	_step(row, "plus", width, spec.width.step_m, base.width)
	if width > 0:
		var back := HudWidgets.button(&"IconButton", "undo", "", "palette.width.auto", "icon_btn", "icon_btn")
		back.pressed.connect(func() -> void: option_picked.emit("width", 0.0))
		row.add_child(back)


## The height the road is drawn at, a bridge above and a cutting or tunnel below, and the sides of a cutting (shell.ts).
func _height_rows() -> void:
	var metres: float = CityStore.query(&"road_height")
	var row := _row("v2.row.height")
	var where := "palette.height.above" if metres > 0 else "palette.height.below" if metres < 0 else "palette.height.ground"
	row.tooltip_text = "%s: %s" % [tr("palette.height"), tr(where)]
	_height_step(row, "minus", -1)
	var whole := is_equal_approx(metres, roundf(metres))
	HudWidgets.label(row, &"BodyLabel", Formatters.format_length(metres) if whole else Formatters.format_length_tenths(metres))
	_height_step(row, "plus", 1)
	row = _row("cutSides.row")
	HudWidgets.choices(row, [[false, tr("cutSides.batter")], [true, tr("cutSides.walls")]], CityStore.query(&"road_walls"), func(on: bool) -> void:
		CityStore.dispatch(&"set_road_walls", [on]))


func _height_step(row: Control, icon: String, metres: int) -> void:
	var b := HudWidgets.button(&"IconButton", icon, "", "palette.height", "icon_btn", "icon_btn")
	b.pressed.connect(func() -> void: tools.step_road_height(metres))
	row.add_child(b)


## What new roads are laid with (Roadcraft shell.ts, roadFurniture): every road's choice, after the class's own size and before its
## parking, as the reference orders the rows.
func _furniture_row(spec: Dictionary) -> void:
	var row := _row(spec.label_key)
	row.tooltip_text = tr(spec.tip_key)
	var items := []
	for c: Dictionary in spec.choices:
		items.push_back([c.level, tr(c.label_key)])
	HudWidgets.choices(row, items, CityStore.query(&"road_furniture"), func(level: int) -> void:
		CityStore.dispatch(&"set_road_furniture", [level]))


func _parking_row(spec: Dictionary) -> void:
	var row := _row(spec.parking.label_key)
	var parking: int = _param("parking", 0)
	for i in spec.parking.choices.size():
		_toggle(row, spec.parking.choices[i], i == parking, true, func() -> void: option_picked.emit("parking", i))


## A step of the total width, from the class's own while none is chosen.
func _step(row: Control, icon: String, width: float, step: float, own: float) -> void:
	var b := HudWidgets.button(&"IconButton", icon, "", "palette.width", "icon_btn", "icon_btn")
	b.pressed.connect(func() -> void: option_picked.emit("width", (width if width > 0 else own) + step))
	row.add_child(b)


func _row(label_key: String) -> HBoxContainer:
	var row := HudWidgets.hbox(self, &"Pad")
	var name := HudWidgets.label(row, &"MutedLabel", tr(label_key))
	name.custom_minimum_size.x = HudWidgets.px("option_label_w")
	return row


func _toggle(row: Control, c: Dictionary, on: bool, enabled: bool, pick: Callable) -> void:
	var b := HudWidgets.button(&"IconButton", c.icon, "", c.get("label_key", ""), "icon_btn", "icon_btn")
	b.toggle_mode = true
	b.set_pressed_no_signal(on)
	b.disabled = not enabled
	b.pressed.connect(pick)
	row.add_child(b)
