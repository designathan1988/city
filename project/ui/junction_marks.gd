## With the Controle tab up every junction says what it runs by: a ring and a letter over it, bigger under the pointer
## (Roadcraft main.ts). Drawn only for the junctions on the screen; the list is the store's, read again only when the
## network changes (CityStore.cached), not with every refresh of the snapshot.
class_name JunctionMarks
extends Control

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
var _projection: Array = []
var _projected: Array = []
var _revision := -1
var _generation := -1
var _mouse := Vector2.INF
var _hot: Dictionary = {}


func _ready() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	hide()
	if tools:
		tools.tool_changed.connect(func(tool: int, _params: Dictionary) -> void:
			visible = tool == ToolController.Tool.CONTROL
			if visible:
				_projection.clear())


func _process(_delta: float) -> void:
	if not visible:
		return
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		if not _projected.is_empty():
			_projected.clear()
			_projection.clear()
			queue_redraw()
		return
	var key := CityCamera.projection_key(camera, get_viewport_rect())
	var revision: int = CityStore.snapshot.get("edit_revision", -1)
	var generation: int = CityStore.snapshot.get("city_generation", -1)
	var changed := false
	if key != _projection or revision != _revision or generation != _generation:
		_projected = CityStore.query(&"junction_marks", [camera, get_viewport_rect()])
		_projection = key
		_revision = revision
		_generation = generation
		changed = true
	var mouse := get_viewport().get_mouse_position()
	if changed or mouse != _mouse:
		var hot := {}
		var radius := HudWidgets.px("control_mark_r")
		for mark: Dictionary in _projected:
			if mark.screen.distance_to(mouse) <= radius:
				hot[mark.node] = true
		changed = changed or hot != _hot
		_hot = hot
		_mouse = mouse
	if changed:
		queue_redraw()


func _draw() -> void:
	var marks: Array = DataTables.ui("hud").control_marks
	var font := get_theme_default_font()
	var size_px := int(HudWidgets.px("control_mark_font"))
	var r := HudWidgets.px("control_mark_r")
	for j: Dictionary in _projected:
		var at: Vector2 = j.screen
		var hot := _hot.has(j.node)
		var mark: Dictionary = marks[int(j.ctl)]
		var colour := ThemeBuilder.color(mark.token)
		var radius := HudWidgets.px("control_mark_hot_r") if hot else r
		draw_circle(at, radius, ThemeBuilder.color("control_mark_bg"))
		draw_arc(at, radius, 0, TAU, int(radius) * 3, colour, HudWidgets.px("control_mark_hot_w") if hot else HudWidgets.px("control_mark_w"), true)
		var text: String = mark.glyph
		var w := font.get_string_size(text, HORIZONTAL_ALIGNMENT_LEFT, -1, size_px)
		draw_string(font, at + Vector2(-w.x * 0.5, font.get_ascent(size_px) - w.y * 0.5), text, HORIZONTAL_ALIGNMENT_LEFT, -1, size_px, colour)


func _notification(what: int) -> void:
	if what == NOTIFICATION_THEME_CHANGED and is_node_ready():
		_mouse = Vector2.INF
		queue_redraw()
