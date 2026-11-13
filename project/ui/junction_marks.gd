## With the Control tab up every junction says what it runs by: a ring and a letter over it, bigger under the pointer
## (Roadcraft main.ts). Drawn only for the junctions on the screen; the list is the store's, read again only when the
## network changes (CityStore.cached), not with every refresh of the snapshot. The marks are placed on the screen again
## only when the view or the list changes, and drawn again only when they moved or the pointer came onto or off one.
class_name JunctionMarks
extends Control

const COLD := 0
const HOT := 1
const UNKNOWN := 2 # placed, the pointer not yet measured against it

@onready var tools: ToolController = get_tree().get_first_node_in_group(&"tools")

var _marks: Array = [] # data/ui/hud.json control_marks, read once
var _bg: Color
var _colours: Array[Color] = [] # each mark's token colour
var _font_px := 0
var _r := 0.0
var _hot_r := 0.0
var _w := 0.0
var _hot_w := 0.0
var _at := PackedVector2Array() # each junction on the screen: where
var _kind := PackedInt32Array() # and which mark
var _hot := PackedByteArray() # HOT under the pointer, else COLD
var _list: Array = [] # the junctions the marks were placed from (the store's cached answer)
var _eye := Transform3D()
var _lens := Projection()
var _rect := Rect2()
var _placed := false


func _ready() -> void:
	set_anchors_preset(Control.PRESET_FULL_RECT)
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	_marks = DataTables.ui("hud").control_marks
	for mark: Dictionary in _marks:
		_colours.append(ThemeBuilder.color(mark.token))
	_bg = ThemeBuilder.color("control_mark_bg")
	_font_px = int(HudWidgets.px("control_mark_font"))
	_r = HudWidgets.px("control_mark_r")
	_hot_r = HudWidgets.px("control_mark_hot_r")
	_w = HudWidgets.px("control_mark_w")
	_hot_w = HudWidgets.px("control_mark_hot_w")
	hide()
	if tools:
		tools.tool_changed.connect(func(tool: int, _params: Dictionary) -> void:
			visible = tool == ToolController.Tool.CONTROL)


func _notification(what: int) -> void:
	if what == NOTIFICATION_VISIBILITY_CHANGED:
		_placed = false # shown again: placed from the view of then


func _process(_delta: float) -> void:
	if not visible:
		return
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		if not _at.is_empty():
			_at.clear()
			_kind.clear()
			_hot.clear()
			queue_redraw()
		_placed = false
		return
	var list: Array = CityStore.cached(&"junctions")
	var rect := get_viewport_rect()
	var moved := not _placed or not is_same(list, _list) or camera.global_transform != _eye \
			or camera.get_camera_projection() != _lens or rect != _rect
	if moved:
		_place(camera, list, rect)
	if _point(get_viewport().get_mouse_position()) or moved:
		queue_redraw()


## Each junction in front of the camera and inside the screen, projected once for this view.
func _place(camera: Camera3D, list: Array, rect: Rect2) -> void:
	_placed = true
	_list = list
	_eye = camera.global_transform
	_lens = camera.get_camera_projection()
	_rect = rect
	_at.clear()
	_kind.clear()
	for j in list:
		if camera.is_position_behind(j.at):
			continue
		var at := camera.unproject_position(j.at)
		if not rect.has_point(at):
			continue
		_at.append(at)
		_kind.append(int(j.ctl))
	_hot.resize(_at.size())
	_hot.fill(UNKNOWN) # the pointer's pass below sets each


## Which marks are under the pointer; true when that changed.
func _point(mouse: Vector2) -> bool:
	var changed := false
	for i in _at.size():
		var hot := HOT if _at[i].distance_to(mouse) <= _r else COLD
		if _hot[i] != hot:
			_hot[i] = hot
			changed = true
	return changed


func _draw() -> void:
	var font := get_theme_default_font()
	for i in _at.size():
		var at := _at[i]
		var hot := _hot[i] == HOT
		var colour := _colours[_kind[i]]
		var radius := _hot_r if hot else _r
		draw_circle(at, radius, _bg)
		draw_arc(at, radius, 0, TAU, int(radius) * 3, colour, _hot_w if hot else _w, true)
		var text: String = _marks[_kind[i]].glyph
		var w := font.get_string_size(text, HORIZONTAL_ALIGNMENT_LEFT, -1, _font_px)
		draw_string(font, at + Vector2(-w.x * 0.5, font.get_ascent(_font_px) - w.y * 0.5), text, HORIZONTAL_ALIGNMENT_LEFT, -1, _font_px, colour)
