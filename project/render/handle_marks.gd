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
		handles = value
		queue_redraw()


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_anchors_preset(Control.PRESET_FULL_RECT)


func _process(_delta: float) -> void:
	if not handles.is_empty():
		queue_redraw()


## The handle under a point of the screen, the nearest in reach.
func at(screen: Vector2) -> Dictionary:
	var camera := get_viewport().get_camera_3d()
	var best := {}
	var nearest := HudWidgets.px("handle_reach")
	for h: Dictionary in handles:
		if camera.is_position_behind(h.at):
			continue
		var d := camera.unproject_position(h.at).distance_to(screen)
		if d < nearest:
			nearest = d
			best = h
	return best


func _draw() -> void:
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		return
	var r := HudWidgets.px("handle_r")
	var line := HudWidgets.px("handle_line")
	var ink := ThemeBuilder.color("accent")
	var dark := ThemeBuilder.color("bg_solid")
	for h: Dictionary in handles:
		if camera.is_position_behind(h.at):
			continue
		var p := camera.unproject_position(h.at)
		if h.kind == "vertex":
			draw_circle(p, r * ARROW_BACK, dark)
			draw_circle(p, r * ARROW_BACK - line, ink)
			continue
		draw_circle(p, r, dark)
		draw_arc(p, r, 0.0, TAU, ARC_POINTS * 2, ink, line, true)
		match String(h.kind):
			"side":
				_arrow(p, camera.unproject_position(h.at + h.dir) - p, r, ink, line)
			"scale":
				_arrow(p, camera.unproject_position(h.at + h.dir) - p, r, ink, line)
				_arrow(p, p - camera.unproject_position(h.at + h.dir), r, ink, line)
			"storeys":
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
