## The lanes of a junction by hand, in the inspector (Roadcraft ui/roads/connectorPanel.ts): its plan with each lane's end, what the traffic built as lines.
## A click on an arriving lane picks it, a click on a leaving lane connects or parts them. It keeps nothing: every press goes to the store.
class_name ConnectorPanel
extends Control

const NOT_PICKED := {}

@export var edit := LaneLinksEdit.new()


static func fill(col: Control, id: int) -> void:
	HudWidgets.label(col, &"TitleLabel", col.tr("connector.title"))
	var panel := ConnectorPanel.new()
	panel.edit.node = id
	col.add_child(panel)


func _ready() -> void:
	custom_minimum_size = Vector2.ONE * HudWidgets.px("cn_plan")
	_read()


func _read() -> void:
	edit.lanes = CityStore.query(&"node_lanes", [edit.node])
	var ends: Array = edit.lanes.arriving + edit.lanes.leaving
	if ends.is_empty():
		return
	edit.lo = Vector2(ends[0].x, ends[0].z)
	var hi := edit.lo
	for e: Dictionary in ends:
		edit.lo = edit.lo.min(Vector2(e.x, e.z))
		hi = hi.max(Vector2(e.x, e.z))
	var pad := HudWidgets.px("cn_dot") * 2.0
	edit.scale = (custom_minimum_size.x - 2.0 * pad) / maxf(maxf(hi.x - edit.lo.x, hi.y - edit.lo.y), 1.0)
	edit.lo -= Vector2.ONE * pad / edit.scale
	queue_redraw()


func _at(e: Dictionary) -> Vector2:
	return (Vector2(e.x, e.z) - edit.lo) * edit.scale


func _find(list: Array, seg: int, lane: int) -> Dictionary:
	for e: Dictionary in list:
		if e.seg == seg and e.lane == lane:
			return e
	return NOT_PICKED


func _draw() -> void:
	if edit.lanes.is_empty():
		return
	for b: Array in edit.lanes.built:
		var from := _find(edit.lanes.arriving, b[0], b[1])
		var to := _find(edit.lanes.leaving, b[2], b[3])
		if from.is_empty() or to.is_empty():
			continue
		var own: bool = not edit.picked.is_empty() and b[0] == edit.picked.seg and b[1] == edit.picked.lane
		draw_line(_at(from), _at(to), ThemeBuilder.color("rp_pick" if own else "rc_rail"), HudWidgets.px("rp_line"))
	var dot := HudWidgets.px("cn_dot")
	for e: Dictionary in edit.lanes.leaving:
		draw_circle(_at(e), dot, ThemeBuilder.color("rc_bike"))
	for e: Dictionary in edit.lanes.arriving:
		draw_circle(_at(e), dot, ThemeBuilder.color("rp_pick" if e == edit.picked else "rc_white"))


func _nearest(list: Array, p: Vector2) -> Dictionary:
	var best := NOT_PICKED
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
	var arriving := _nearest(edit.lanes.arriving, b.position)
	if not arriving.is_empty():
		edit.picked = arriving
		queue_redraw()
	elif not edit.picked.is_empty():
		var leaving := _nearest(edit.lanes.leaving, b.position)
		if not leaving.is_empty():
			CityStore.dispatch(&"toggle_lane_link", [edit.node, edit.picked.seg, edit.picked.lane, leaving.seg, leaving.lane])
			get_tree().create_timer(float(DataTables.ui("hud").timings.lane_reread_s)).timeout.connect(_read)
	accept_event()
