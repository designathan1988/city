## With the Controle tab up every junction says what it runs by: a ring and a letter over it, bigger under the pointer
## (Roadcraft main.ts). Drawn only for the junctions on the screen; the list comes again when the city changes.
class_name JunctionMarks
extends Control

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
@onready var _junctions: Array = []


func _ready() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	hide()
	CityStore.changed.connect(func(_snapshot: Dictionary) -> void:
		if visible:
			_junctions = CityStore.query(&"junctions"))
	if tools:
		tools.tool_changed.connect(func(tool: int, _params: Dictionary) -> void:
			visible = tool == ToolController.Tool.CONTROL
			if visible:
				_junctions = CityStore.query(&"junctions"))


func _process(_delta: float) -> void:
	if visible:
		queue_redraw()


func _draw() -> void:
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		return
	var marks: Array = DataTables.ui("hud").control_marks
	var view := get_viewport_rect()
	var mouse := get_viewport().get_mouse_position()
	var font := get_theme_default_font()
	var size_px := int(HudWidgets.px("control_mark_font"))
	var r := HudWidgets.px("control_mark_r")
	for j in _junctions:
		if camera.is_position_behind(j.at):
			continue
		var at := camera.unproject_position(j.at)
		if not view.has_point(at):
			continue
		var hot := at.distance_to(mouse) <= r
		var mark: Dictionary = marks[int(j.ctl)]
		var colour := ThemeBuilder.color(mark.token)
		var radius := HudWidgets.px("control_mark_hot_r") if hot else r
		draw_circle(at, radius, ThemeBuilder.color("control_mark_bg"))
		draw_arc(at, radius, 0, TAU, int(radius) * 3, colour, HudWidgets.px("control_mark_hot_w") if hot else HudWidgets.px("control_mark_w"), true)
		var text: String = mark.glyph
		var w := font.get_string_size(text, HORIZONTAL_ALIGNMENT_LEFT, -1, size_px)
		draw_string(font, at + Vector2(-w.x * 0.5, font.get_ascent(size_px) - w.y * 0.5), text, HORIZONTAL_ALIGNMENT_LEFT, -1, size_px, colour)
