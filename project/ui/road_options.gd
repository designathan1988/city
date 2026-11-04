## The road tool's options, beside the catalogue (Roadcraft ui/v2/shell.ts renderRoads): a row per choice, each a label
## and its toggles. Alignment is the tool's own parameter; snapping is the player's setting (Settings.road_snap). The
## rows and their icons come from data/ui/drawers.json "road_options".
class_name RoadOptions
extends PanelContainer

signal alignment_picked(alignment: int)

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var _rows: VBoxContainer


func _ready() -> void:
	theme_type_variation = &"Surface"
	anchor_top = 1.0
	anchor_bottom = 1.0
	grow_vertical = Control.GROW_DIRECTION_BEGIN
	offset_left = HudWidgets.px("margin")
	offset_bottom = -HudWidgets.px("margin")
	_rows = HudWidgets.vbox(self, &"Pad")
	hide()
	if tools:
		tools.tool_changed.connect(func(_tool: int, _params: Dictionary) -> void:
			if visible:
				refresh())


func open() -> void:
	refresh()
	show()


func refresh() -> void:
	HudWidgets.clear(_rows)
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


func _row(label_key: String) -> HBoxContainer:
	var row := HudWidgets.hbox(_rows, &"Pad")
	var name := HudWidgets.label(row, &"MutedLabel", tr(label_key))
	name.custom_minimum_size.x = HudWidgets.px("option_label_w")
	return row


func _toggle(row: Control, c: Dictionary, on: bool, enabled: bool, pick: Callable) -> void:
	var b := HudWidgets.button(&"IconButton", c.icon, "", c.label_key, "icon_btn", "icon_btn")
	b.toggle_mode = true
	b.set_pressed_no_signal(on)
	b.disabled = not enabled
	b.pressed.connect(pick)
	row.add_child(b)
