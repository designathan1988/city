## The inspector side panel (what the core reports): what the
## inspect tool picked (a building: zone or function and floors; a road: its profile; a signal: JunctionPanel) and a Demolish
## action that sends the store command at that place.
class_name InspectorPanel
extends PanelContainer

func _ready() -> void:
	theme_type_variation = &"Surface"
	anchor_left = 1.0
	anchor_right = 1.0
	grow_horizontal = Control.GROW_DIRECTION_BEGIN
	offset_right = -HudWidgets.px("margin")
	offset_top = HudWidgets.px("toast_top")
	custom_minimum_size.x = HudWidgets.px("side_w")
	hide()


func show_info(info: Dictionary, at: Vector3) -> void:
	HudWidgets.clear(self)
	if info.is_empty() or not info.has("kind"):
		hide()
		return
	var col := HudWidgets.vbox(self, &"Pad")
	if info.kind == "junction":
		JunctionPanel.fill(col, info, _inspect_again.bind(at))
	elif info.kind == "building":
		var fn_keys: Array = DataTables.ui("drawers").fn_keys
		var fn: int = info.fn
		var name := tr(fn_keys[fn]) if fn > 0 and fn < fn_keys.size() else tr("zone.name.%d" % info.zone)
		HudWidgets.label(col, &"TitleLabel", name.to_upper())
		HudWidgets.label(col, &"BodyLabel", tr("ui.floors.other").format({"count": info.floors}))
	else:
		HudWidgets.label(col, &"TitleLabel", tr("inspector.road"))
		HudWidgets.label(col, &"BodyLabel", tr(info.profile))
		for e in info.get("ends", []):
			_end_row(col, e, at)
	var row := HudWidgets.hbox(col)
	if info.kind != "junction":
		var demolish := HudWidgets.button(&"IconButton", "demolish", "", "inspector.demolish", "icon_btn", "icon_btn")
		demolish.pressed.connect(_demolish.bind(at))
		row.add_child(demolish)
	var close := HudWidgets.button(&"IconButton", "close", "", "v2.close", "icon_btn", "icon_btn")
	close.pressed.connect(hide)
	row.add_child(close)
	show()


## A dead end of the road: "Ponta inicial [balão]"; the choice comes back from the core, so a refused one stays off.
func _end_row(col: Control, e: Dictionary, at: Vector3) -> void:
	var row := HudWidgets.hbox(col)
	var name := HudWidgets.label(row, &"MutedLabel", tr("inspector.endStart" if e.start else "inspector.endEnd"))
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var b := HudWidgets.button(&"IconButton", "roundabout", "", "inspector.end.bulb", "icon_btn", "icon_btn")
	b.toggle_mode = true
	b.set_pressed_no_signal(e.bulb)
	b.pressed.connect(func() -> void:
		CityStore.dispatch(&"set_road_end", [e.node, not e.bulb])
		show_info(CityStore.query(&"inspect", [at]), at))
	row.add_child(b)


func _inspect_again(at: Vector3) -> void:
	show_info(CityStore.query(&"inspect", [at]), at)


func _demolish(at: Vector3) -> void:
	CityStore.dispatch(&"demolish", [at])
	hide()
