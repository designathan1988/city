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
	if _base().class_card:
		_class_rows(spec)


## The base road in hand, as the core describes it.
func _base() -> Dictionary:
	return CityStore.query(&"road_profiles")[_param("profile", CityStore.first_road())]


func _param(key: String, fallback: Variant) -> Variant:
	return tools.last_param(ToolController.Tool.ROAD, key, fallback) if tools else fallback


## Lanes, width and parking tune a class's own road; a road of the catalogue carries its own (Roadcraft ui/v2/shell.ts).
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
	row = _row(spec.parking.label_key)
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
