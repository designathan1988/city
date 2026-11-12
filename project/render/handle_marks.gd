## The handles of the block picked, drawn over the picture where they stand in the world (the query `body_handles`): an arrow out of each face, a dot on each corner of the plan,
## a chevron over the roof, and the corners that move, turn and scale.
class_name HandleMarks
extends Control

const ARROW_BACK := 0.6
const CHEVRON := 0.7
const FAR_ANGLE := 1.5 * PI
const ARC_POINTS := 16

var handles: Array = []:
	set(value):
		if handles == value:
			return
		handles = value
		_revision += 1
		queue_redraw()
var _revision := 0
var _projected_revision := -1
var _projection: Array = []
var _projected: Array[Dictionary] = []


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_anchors_preset(Control.PRESET_FULL_RECT)


func _process(_delta: float) -> void:
	if not handles.is_empty() and _prepare_projection():
		queue_redraw()


## Drawing and hit testing share exactly the same projected handles for the current body/view.
func _prepare_projection() -> bool:
	var camera := get_viewport().get_camera_3d()
	var key := CityCamera.projection_key(camera, get_viewport_rect()) if camera != null else []
	if key == _projection and _projected_revision == _revision:
		return false
	_projection = key
	_projected_revision = _revision
	_projected.clear()
	if camera != null:
		for handle: Dictionary in handles:
			if camera.is_position_behind(handle.at):
				continue
			var at := camera.unproject_position(handle.at)
			var way := Vector2.ZERO
			if handle.kind in ["side", "relief", "scale"]:
				way = camera.unproject_position(handle.at + handle.dir) - at
			_projected.push_back({"handle": handle, "at": at, "way": way})
	return true


## The handle under a point of the screen, the nearest in reach.
func at(screen: Vector2) -> Dictionary:
	_prepare_projection()
	var best := {}
	var nearest := HudWidgets.px("handle_reach")
	for projected: Dictionary in _projected:
		var d: float = projected.at.distance_to(screen)
		if d < nearest:
			nearest = d
			best = projected.handle
	return best


func _draw() -> void:
	_prepare_projection()
	var r := HudWidgets.px("handle_r")
	var line := HudWidgets.px("handle_line")
	var ink := ThemeBuilder.color("accent")
	var dark := ThemeBuilder.color("bg_solid")
	for projected: Dictionary in _projected:
		var h: Dictionary = projected.handle
		var p: Vector2 = projected.at
		if h.kind == "vertex":
			draw_circle(p, r * ARROW_BACK, dark)
			draw_circle(p, r * ARROW_BACK - line, ink)
			continue
		draw_circle(p, r, dark)
		draw_arc(p, r, 0.0, TAU, ARC_POINTS * 2, ink, line, true)
		match String(h.kind):
			"side", "relief":
				_arrow(p, projected.way, r, ink, line)
			"scale":
				_arrow(p, projected.way, r, ink, line)
				_arrow(p, -projected.way, r, ink, line)
			"storeys", "lift":
				_arrow(p, Vector2.UP, r, ink, line)
				_arrow(p, Vector2.DOWN, r, ink, line)
			"move":
				for d: Vector2 in [Vector2.UP, Vector2.DOWN, Vector2.LEFT, Vector2.RIGHT]:
					_arrow(p, d, r, ink, line)
			"rotate":
				draw_arc(p, r * ARROW_BACK, 0.0, FAR_ANGLE, ARC_POINTS, ink, line, true)


func _arrow(p: Vector2, way: Vector2, r: float, ink: Color, line: float) -> void:
	var d := way.normalized() if way.length_squared() > 0.0 else Vector2.UP
	var tip := p + d * r * CHEVRON
	var side := d.orthogonal() * r * ARROW_BACK * CHEVRON
	var back := tip - d * r * ARROW_BACK * CHEVRON
	draw_line(back + side, tip, ink, line, true)
	draw_line(back - side, tip, ink, line, true)
