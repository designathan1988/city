## The road's cross-section in the profile editor (Roadcraft ui/roads/crossSection.ts): its elements side by side at scale. A click picks one, the
## line between two drags their width, an element dragged goes to another place.
class_name RoadSectionView
extends Control

signal picked(index: int)
signal widened(index: int, metres: float)
signal moved(from: int, to: int)

const LABEL_LEAST_PX := 34.0
const ARROW_PX := 7.0
const HALF := 0.5

var spec: Dictionary = {}
var selected := -1
var flagged: Array = []
var _drag_edge := -1
var _drag_from := -1
var _grab_x := 0.0


func _ready() -> void:
	custom_minimum_size.y = HudWidgets.px("rp_stage_h")
	mouse_filter = Control.MOUSE_FILTER_STOP


func show_spec(next: Dictionary, pick: int, issues: Array) -> void:
	spec = next
	selected = pick
	flagged = issues
	queue_redraw()


func _scale() -> float:
	return size.x / maxf(RoadSpecOps.total(spec), 1.0)


func _left_of(index: int) -> float:
	var x := 0.0
	for i in index:
		x += spec.elements[i].width * _scale()
	return x


func _colour(e: Dictionary) -> Color:
	if e.get("bus", false):
		return ThemeBuilder.color("rc_bus")
	return ThemeBuilder.color({"footway": "rc_sidewalk", "lane": "rc_asphalt", "median": "rc_median", "parking": "rc_parking", "cycle": "rc_bike"}[e.kind])


func _draw() -> void:
	if spec.is_empty():
		return
	var font := get_theme_default_font()
	var font_size := int(HudWidgets.px("rp_font"))
	for i in spec.elements.size():
		var e: Dictionary = spec.elements[i]
		var box := Rect2(_left_of(i), 0.0, e.width * _scale(), size.y)
		draw_rect(box, _colour(e))
		if e.kind == "lane":
			var tip := box.get_center() + Vector2(0.0, -ARROW_PX * e.dir)
			var base := box.get_center() + Vector2(0.0, ARROW_PX * e.dir)
			draw_colored_polygon(PackedVector2Array([tip, base + Vector2(ARROW_PX, 0.0), base - Vector2(ARROW_PX, 0.0)]), ThemeBuilder.color("rc_white"))
		if flagged.has(i):
			draw_rect(box, ThemeBuilder.color("rp_flag"), false, HudWidgets.px("rp_line"))
		if box.size.x >= LABEL_LEAST_PX:
			draw_string(font, Vector2(box.position.x, size.y - font_size * HALF), Formatters.format_measure(e.width), HORIZONTAL_ALIGNMENT_CENTER, box.size.x, font_size, ThemeBuilder.color("rc_white"))
		if i > 0:
			draw_line(Vector2(box.position.x, 0.0), Vector2(box.position.x, size.y), ThemeBuilder.color("rc_white"), 1.0)
	if selected >= 0 and selected < spec.elements.size():
		draw_rect(Rect2(_left_of(selected), 0.0, spec.elements[selected].width * _scale(), size.y), ThemeBuilder.color("rp_pick"), false, HudWidgets.px("rp_line"))


func _edge_at(x: float) -> int:
	for i in range(1, spec.elements.size()):
		if absf(_left_of(i) - x) < HudWidgets.px("rp_edge"):
			return i - 1
	return -1


func _element_at(x: float) -> int:
	for i in spec.elements.size():
		if x < _left_of(i) + spec.elements[i].width * _scale():
			return i
	return spec.elements.size() - 1


func _gui_input(event: InputEvent) -> void:
	if spec.is_empty():
		return
	var b := event as InputEventMouseButton
	if b != null and b.button_index == MOUSE_BUTTON_LEFT:
		if b.pressed:
			_drag_edge = _edge_at(b.position.x)
			_drag_from = _element_at(b.position.x) if _drag_edge < 0 else -1
			_grab_x = b.position.x
			if _drag_from >= 0:
				picked.emit(_drag_from)
		else:
			if _drag_from >= 0 and _element_at(b.position.x) != _drag_from:
				moved.emit(_drag_from, _element_at(b.position.x))
			_drag_edge = -1
			_drag_from = -1
		accept_event()
	var m := event as InputEventMouseMotion
	if m != null and _drag_edge >= 0:
		widened.emit(_drag_edge, (m.position.x - _left_of(_drag_edge)) / _scale())
		accept_event()
