## The road's cross-section in the profile editor (Roadcraft ui/roads/crossSection.ts): its elements side by side at scale. A click picks one, the
## line between two drags their width, an element dragged goes to another place.
class_name RoadSectionView
extends Control

signal picked(index: int)
signal widened(index: int, metres: float)
signal moved(from: int, to: int)

const LABEL_LEAST_PX := 34.0
const WAY_MARK_PX := 7.0
const HALF := 0.5

@export var state := RoadSectionState.new()
var width_limits: Dictionary = {}
var width_step := 0.0
var _pixels_per_metre := 0.0
var _edges := PackedFloat64Array()


func _ready() -> void:
	custom_minimum_size.y = HudWidgets.px("rp_stage_h")
	mouse_filter = Control.MOUSE_FILTER_STOP
	resized.connect(_layout)


func show_spec(next: Dictionary, pick: int, issues: Array) -> void:
	state.spec = next
	state.selected = pick
	state.flagged = issues
	_layout()


## Shared by drawing and picking; prefixes keep the original left-to-right sum of scaled element widths.
func _layout() -> void:
	_edges = PackedFloat64Array([0.0])
	_pixels_per_metre = size.x / maxf(RoadSpecOps.total(state.spec), 1.0) if not state.spec.is_empty() else 0.0
	if not state.spec.is_empty():
		var x := 0.0
		for element: Dictionary in state.spec.elements:
			x += element.width * _pixels_per_metre
			_edges.push_back(x)
	queue_redraw()


func _scale() -> float:
	return _pixels_per_metre


func _left_of(index: int) -> float:
	return _edges[index]


func _colour(e: Dictionary) -> Color:
	if e.get("bus", false):
		return ThemeBuilder.color("rc_bus")
	return ThemeBuilder.color({"footway": "rc_sidewalk", "lane": "rc_asphalt", "median": "rc_median", "parking": "rc_parking", "cycle": "rc_bike"}[e.kind])


func _draw() -> void:
	if state.spec.is_empty():
		return
	var font := get_theme_default_font()
	var font_size := int(HudWidgets.px("rp_font"))
	for i in state.spec.elements.size():
		var e: Dictionary = state.spec.elements[i]
		var box := Rect2(_left_of(i), 0.0, e.width * _scale(), size.y)
		draw_rect(box, _colour(e))
		if e.kind == "lane":
			var tip := box.get_center() + Vector2(0.0, -WAY_MARK_PX * e.dir)
			var base := box.get_center() + Vector2(0.0, WAY_MARK_PX * e.dir)
			draw_colored_polygon(PackedVector2Array([tip, base + Vector2(WAY_MARK_PX, 0.0), base - Vector2(WAY_MARK_PX, 0.0)]), ThemeBuilder.color("rc_white"))
		if state.flagged.has(i):
			draw_rect(box, ThemeBuilder.color("rp_flag"), false, HudWidgets.px("rp_line"))
		if box.size.x >= LABEL_LEAST_PX:
			draw_string(font, Vector2(box.position.x, size.y - font_size * HALF), Formatters.format_measure(e.width), HORIZONTAL_ALIGNMENT_CENTER, box.size.x, font_size, ThemeBuilder.color("rc_white"))
		if i > 0:
			draw_line(Vector2(box.position.x, 0.0), Vector2(box.position.x, size.y), ThemeBuilder.color("rc_white"), 1.0)
	if state.selected >= 0 and state.selected < state.spec.elements.size():
		draw_rect(Rect2(_left_of(state.selected), 0.0, state.spec.elements[state.selected].width * _scale(), size.y), ThemeBuilder.color("rp_pick"), false, HudWidgets.px("rp_line"))


func _edge_at(x: float) -> int:
	for i in range(1, state.spec.elements.size()):
		if absf(_left_of(i) - x) < HudWidgets.px("rp_edge"):
			return i - 1
	return -1


func _element_at(x: float) -> int:
	for i in state.spec.elements.size():
		if x < _left_of(i) + state.spec.elements[i].width * _scale():
			return i
	return state.spec.elements.size() - 1


func _gui_input(event: InputEvent) -> void:
	if state.spec.is_empty():
		return
	var b := event as InputEventMouseButton
	if b != null and b.button_index == MOUSE_BUTTON_LEFT:
		if b.pressed:
			state.drag_edge = _edge_at(b.position.x)
			state.drag_from = _element_at(b.position.x) if state.drag_edge < 0 else -1
			state.grab_x = b.position.x
			if state.drag_from >= 0:
				picked.emit(state.drag_from)
		else:
			if state.drag_from >= 0 and _element_at(b.position.x) != state.drag_from:
				moved.emit(state.drag_from, _element_at(b.position.x))
			state.drag_edge = -1
			state.drag_from = -1
		accept_event()
	var m := event as InputEventMouseMotion
	if m != null and state.drag_edge >= 0:
		var metres := (m.position.x - _left_of(state.drag_edge)) / _scale()
		var kind: String = state.spec.elements[state.drag_edge].kind
		if width_step > 0 and width_limits.has(kind):
			metres = RoadSpecOps.canonical_width(metres, width_step, width_limits[kind])
		if RoadSpecOps.width_changes(state.spec, state.drag_edge, metres):
			widened.emit(state.drag_edge, metres)
		accept_event()
