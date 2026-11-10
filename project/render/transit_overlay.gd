## The plan over the map while the transport tool is on: each line through its stops in its colour, the stops and stations marked, the
## points of the run being laid and the line being made up to the pointer. Tracks are not drawn here: the scene shows them as they are built.
class_name TransitOverlay
extends Control

const MODE_BUS := 0
const MODE_TRAIN := 1
const MODE_METRO := 2
const MARK_KEYS := { MODE_BUS: "transit.mark.bus", MODE_TRAIN: "transit.mark.train", MODE_METRO: "transit.mark.metro" }
const FILL_TOKENS := { MODE_BUS: "transit_bus", MODE_TRAIN: "transit_train", MODE_METRO: "transit_metro" }
const RGB_BITS := 8
const RING_POINTS := 24
const LINE_ALPHA := 0.85

var tool: ToolBase
var _font: Font


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_anchors_preset(Control.PRESET_FULL_RECT)
	_font = ThemeDB.fallback_font


func _process(_delta: float) -> void:
	queue_redraw()


func _draw() -> void:
	var cam := get_viewport().get_camera_3d()
	if cam == null:
		return
	var plan: Dictionary = CityStore.cached(&"transit_plan") # read again only when the plan changes
	var by_id := {}
	for s: Dictionary in plan.stops:
		by_id[s.id] = s
	for line: Dictionary in plan.lines:
		var colour := Color.hex((int(line.colour) << RGB_BITS) | 0xff)
		colour.a = LINE_ALPHA
		_stroke(cam, _places(line.stops, by_id), colour, ThemeBuilder.size("transit_line_w"))
	for s: Dictionary in plan.stops:
		if not cam.is_position_behind(s.at):
			_mark(cam.unproject_position(s.at), s)
	for p: Vector3 in tool.points:
		if not cam.is_position_behind(p):
			draw_circle(cam.unproject_position(p), ThemeBuilder.size("transit_point_r"), ThemeBuilder.color("white"))
	if not tool.stops.is_empty():
		var draft := _places(tool.stops, by_id)
		if tool.cursor != Vector3.INF:
			draft.append(tool.cursor)
		_stroke(cam, draft, ThemeBuilder.color("transit_draft"), ThemeBuilder.size("transit_draft_w"))


func _places(ids: PackedInt32Array, by_id: Dictionary) -> PackedVector3Array:
	var out := PackedVector3Array()
	for id in ids:
		if by_id.has(id):
			out.append(by_id[id].at)
	return out


func _stroke(cam: Camera3D, places: PackedVector3Array, colour: Color, width: float) -> void:
	var run := PackedVector2Array()
	for p in places:
		if not cam.is_position_behind(p):
			run.append(cam.unproject_position(p))
	if run.size() >= 2:
		draw_polyline(run, colour, width, true)


func _mark(at: Vector2, s: Dictionary) -> void:
	var mode := int(s.mode)
	var fill := ThemeBuilder.color(FILL_TOKENS[mode])
	var ring := ThemeBuilder.color("white")
	var width := ThemeBuilder.size("transit_ring_w")
	if s.terminal:
		var r := ThemeBuilder.size("transit_terminal_r")
		var box := Rect2(at - Vector2(r, r), Vector2(r, r) * 2)
		draw_rect(box, fill)
		draw_rect(box, ring, false, width)
	else:
		var r := ThemeBuilder.size("transit_stop_r" if mode == MODE_BUS else "transit_station_r")
		draw_circle(at, r, fill)
		draw_arc(at, r, 0, TAU, RING_POINTS, ring, width, true)
	var size := int(ThemeBuilder.size("transit_font"))
	var text := tr(MARK_KEYS[mode])
	var ink := _font.get_string_size(text, HORIZONTAL_ALIGNMENT_CENTER, -1, size)
	draw_string(_font, at + Vector2(-ink.x, ink.y) / 2, text, HORIZONTAL_ALIGNMENT_CENTER, -1, size, ring)
