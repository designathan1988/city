## The inspector side panel (Roadcraft inspector.ts, specs ui.md §8.4, reduced to what the core reports): what the
## inspect tool picked (a building: zone or function and floors; a road: its profile) and a Demolish action that sends
## the store command at that place.
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
	if info.kind == "building":
		var fn_keys: Array = DataTables.ui("drawers").fn_keys
		var fn: int = info.fn
		var name := tr(fn_keys[fn]) if fn > 0 and fn < fn_keys.size() else tr("zone.name.%d" % info.zone)
		HudWidgets.label(col, &"TitleLabel", name.to_upper())
		HudWidgets.label(col, &"BodyLabel", tr("ui.floors.other").format({"count": info.floors}))
	else:
		HudWidgets.label(col, &"TitleLabel", tr("inspector.road"))
		HudWidgets.label(col, &"BodyLabel", tr(info.profile))
	var row := HudWidgets.hbox(col)
	var demolish := HudWidgets.button(&"IconButton", "demolish", "", "inspector.demolish", "icon_btn", "icon_btn")
	demolish.pressed.connect(_demolish.bind(at))
	row.add_child(demolish)
	var close := HudWidgets.button(&"IconButton", "close", "", "v2.close", "icon_btn", "icon_btn")
	close.pressed.connect(hide)
	row.add_child(close)
	show()


func _demolish(at: Vector3) -> void:
	CityStore.dispatch(&"demolish", [at])
	hide()
