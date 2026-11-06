## The lanes of a junction by hand, in the inspector (Roadcraft ui/roads/connectorPanel.ts): its plan with each lane's end, what the traffic built as lines.
## A click on an arriving lane picks it, a click on a leaving lane connects or parts them. It keeps nothing: every press goes to the store.
class_name ConnectorPanel
extends Control

const NO_LANE := {}

var node := -1
var _lanes: Dictionary = {}
var _picked: Dictionary = NO_LANE
var _lo := Vector2.ZERO
var _scale := 1.0


static func fill(col: Control, id: int) -> void:
	HudWidgets.label(col, &"TitleLabel", col.tr("connector.title"))
	var panel := ConnectorPanel.new()
	panel.node = id
	col.add_child(panel)


func _ready() -> void:
	custom_minimum_size = Vector2.ONE * HudWidgets.px("cn_plan")
	_read()


func _read() -> void:
	_lanes = CityStore.query(&"node_lanes", [node])
	var ends: Array = _lanes.arriving + _lanes.leaving
	if ends.is_empty():
		return
	_lo = Vector2(ends[0].x, ends[0].z)
	var hi := _lo
	for e: Dictionary in ends:
		_lo = _lo.min(Vector2(e.x, e.z))
		hi = hi.max(Vector2(e.x, e.z))
	var pad := HudWidgets.px("cn_dot") * 2.0
	_scale = (custom_minimum_size.x - 2.0 * pad) / maxf(maxf(hi.x - _lo.x, hi.y - _lo.y), 1.0)
	_lo -= Vector2.ONE * pad / _scale
	queue_redraw()


func _at(e: Dictionary) -> Vector2:
	return (Vector2(e.x, e.z) - _lo) * _scale


func _find(list: Array, seg: int, lane: int) -> Dictionary:
	for e: Dictionary in list:
		if e.seg == seg and e.lane == lane:
			return e
	return NO_LANE


func _draw() -> void:
	if _lanes.is_empty():
		return
	for b: Array in _lanes.built:
		var from := _find(_lanes.arriving, b[0], b[1])
		var to := _find(_lanes.leaving, b[2], b[3])
		if from.is_empty() or to.is_empty():
			continue
		var own: bool = not _picked.is_empty() and b[0] == _picked.seg and b[1] == _picked.lane
		draw_line(_at(from), _at(to), ThemeBuilder.color("rp_pick" if own else "rc_rail"), HudWidgets.px("rp_line"))
	var dot := HudWidgets.px("cn_dot")
	for e: Dictionary in _lanes.leaving:
		draw_circle(_at(e), dot, ThemeBuilder.color("rc_bike"))
	for e: Dictionary in _lanes.arriving:
		draw_circle(_at(e), dot, ThemeBuilder.color("rp_pick" if e == _picked else "rc_white"))


func _nearest(list: Array, p: Vector2) -> Dictionary:
	var best := NO_LANE
	var reach := HudWidgets.px("cn_dot") * 2.0
	for e: Dictionary in list:
		var d := _at(e).distance_to(p)
		if d < reach:
			reach = d
			best = e
	return best


func _gui_input(event: InputEvent) -> void:
	var b := event as InputEventMouseButton
	if b == null or b.button_index != MOUSE_BUTTON_LEFT or not b.pressed:
		return
	var arriving := _nearest(_lanes.arriving, b.position)
	if not arriving.is_empty():
		_picked = arriving
		queue_redraw()
	elif not _picked.is_empty():
		var leaving := _nearest(_lanes.leaving, b.position)
		if not leaving.is_empty():
			CityStore.dispatch(&"toggle_lane_link", [node, _picked.seg, _picked.lane, leaving.seg, leaving.lane])
			get_tree().create_timer(HudWidgets.px("cn_reread_s")).timeout.connect(_read)
	accept_event()
