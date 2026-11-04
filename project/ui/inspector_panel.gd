## The inspector side panel (what the core reports): what the inspect tool picked, a name over its numbers (a building: floors,
## residents and jobs; a road: lanes, width, speed, price) and a Demolish action that sends the store command at that place.
class_name InspectorPanel
extends PanelContainer

@onready var _at := Vector3.ZERO


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


func _road(col: Control, info: Dictionary) -> void:
	HudWidgets.label(col, &"TitleLabel", tr("inspector.road").to_upper())
	HudWidgets.label(col, &"BodyLabel", tr(info.profile))
	_rows(col, [["inspector.lanes", str(info.lanes)], ["inspector.speed", "%d km/h" % int(info.speed_kmh)],
		["palette.width", "%.1f m" % info.width_m], ["ui.insp.pricePerM", Formatters.format_money(info.cost_per_m)]])


## The Delete key: what the panel shows goes; false when it shows nothing.
func demolish_shown() -> bool:
	if not visible:
		return false
	_demolish(_at)
	return true


func show_info(info: Dictionary, at: Vector3) -> void:
	_at = at
	HudWidgets.clear(self)
	if info.is_empty() or not info.has("kind"):
		hide()
		return
	var col := HudWidgets.vbox(self, &"Pad")
	if info.kind == "building":
		_building(col, info)
	else:
		_road(col, info)
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
