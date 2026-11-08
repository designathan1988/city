## What the Information tool has picked or stands on, drawn over the map like the reference's screen layer: a translucent band the width of
## the road, and a ring on a junction. Under the panels; asks the core for a road's line once, when it is picked.
class_name SelectionOverlay
extends Control

## Chords of a line's round cap, and of a ring.
const CAP_POINTS := 8
const RING_POINTS := 32

var _hud: RoadcraftHud
var _picked := {} # {path, half} of the road picked
var _hover := {} # the same, of the road under the pointer
var _hover_id := -1
var _node := Vector3.INF


func _init(hud: RoadcraftHud) -> void:
	_hud = hud


func _ready() -> void:
	mouse_filter = Control.MOUSE_FILTER_IGNORE
	set_anchors_and_offsets_preset(Control.PRESET_FULL_RECT)
	var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
	if tools != null:
		tools.inspected.connect(_pick)
		tools.hovered.connect(_on_hover)
	_hud.inspector.closed.connect(clear)
	_hud.inspector.laid_again.connect(_pick)


func clear() -> void:
	_picked = {}
	_hover = {}
	_hover_id = -1
	_node = Vector3.INF
	queue_redraw()


func _info_in_hand() -> bool:
	return _hud.drawer.category.get("id", "") == "info"


func _road(info: Dictionary) -> Dictionary:
	return {"id": info.id, "path": CityStore.query(&"segment_path", [info.id]), "half": info.width_m * 0.5}


func _pick(info: Dictionary) -> void:
	clear()
	match info.get("kind", ""):
		"road":
			if _info_in_hand():
				_picked = _road(info)
		"node":
			_node = info.get("at", Vector3.INF)


func _on_hover(info: Dictionary) -> void:
	var id: int = info.id if info.get("kind", "") == "road" and _info_in_hand() else -1
	if id == _hover_id:
		return
	_hover_id = id
	_hover = {} if id < 0 or id == _picked.get("id", -1) else _road(info)
	queue_redraw()


func _process(_delta: float) -> void:
	if not _picked.is_empty() or not _hover.is_empty() or _node != Vector3.INF:
		queue_redraw()


func _draw() -> void:
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		return
	if not _picked.is_empty():
		_band(camera, _picked, ThemeBuilder.color("select_band"))
	if not _hover.is_empty():
		_band(camera, _hover, ThemeBuilder.color("hover_band"))
	if _node != Vector3.INF and not camera.is_position_behind(_node):
		draw_arc(camera.unproject_position(_node), HudWidgets.px("select_px"), 0.0, TAU, RING_POINTS, ThemeBuilder.color("select_ring"),
			HudWidgets.px("select_w"), true)


## The line on the screen, as wide as the road where its middle stands, with the reference's round ends.
func _band(camera: Camera3D, road: Dictionary, colour: Color) -> void:
	var path: PackedVector3Array = road.path
	var line := PackedVector2Array()
	for p: Vector3 in path:
		if not camera.is_position_behind(p):
			line.append(camera.unproject_position(p))
	if line.size() < 2:
		return
	var i := path.size() / 2
	var along := (path[mini(i + 1, path.size() - 1)] - path[maxi(i - 1, 0)]).normalized()
	var side := path[i] + along.cross(Vector3.UP) * float(road.half)
	var width := maxf(HudWidgets.px("select_band_min"), 2.0 * camera.unproject_position(path[i]).distance_to(camera.unproject_position(side)))
	draw_polyline(line, colour, width)
	_cap(line[0], line[0] - line[1], width * 0.5, colour)
	_cap(line[line.size() - 1], line[line.size() - 1] - line[line.size() - 2], width * 0.5, colour)


func _cap(at: Vector2, out: Vector2, radius: float, colour: Color) -> void:
	var half := PackedVector2Array()
	var from := out.angle() - PI * 0.5
	for i in CAP_POINTS + 1:
		half.append(at + Vector2.from_angle(from + PI * i / CAP_POINTS) * radius)
	draw_colored_polygon(half, colour)
