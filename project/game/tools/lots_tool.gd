## Zones > Lots (Roadcraft editor/lotTool.ts): the lots the player draws. `mode` says what a gesture does: add (a rectangle dragged square to the
## street), polygon (point by point, closed on its first), edit (a corner dragged), curve (a side bent through the pointer), front (the side
## clicked becomes the front), split (across, along or by a drawn line), join (one lot, then its neighbour), delete. The zone brush paints them.
extends ToolBase

## The kinds of cut, as the core takes them (LotCutKind).
const CUT_ACROSS := 0
const CUT_ALONG := 1
const CUT_LINE := 2
## What of a corner or a side the pointer takes, in screen pixels.
const PICK_PX := 14.0
const LINE_PX := 2.0

var mode := "add"
var cut_kind := CUT_ACROSS
var parts := 2
var _lots: Array = []
var _from := Vector3.INF # where the gesture began
var _side: Array = [] # the side picked: [a, b]
var _points := PackedVector3Array() # the polygon so far
var _first_lot := 0 # join: the lot picked first


func enter() -> void:
	mode = params.get("mode", "add")
	cut_kind = params.get("cut", CUT_ACROSS)
	parts = params.get("parts", 2)
	_read()
	ctl.hint.emit("zone.lots.help", true)


func exit() -> void:
	ctl.preview.clear()


func cancel() -> void:
	_from = Vector3.INF
	_side = []
	_points = PackedVector3Array()
	_first_lot = 0
	_show()


func _read() -> void:
	_lots = CityStore.query(&"lots")


func _done(r: Dictionary) -> void:
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
	_read()
	cancel()


func _width(at: Vector3) -> float:
	return ctl.px_at(at) * LINE_PX


func _closed(points: PackedVector3Array) -> PackedVector3Array:
	var ring := points.duplicate()
	ring.push_back(points[0])
	return ring


## The lots on the ground, and `extra` over them.
func _show(extra: Array = []) -> void:
	var roads := extra.duplicate()
	var at: Vector3 = _from if _from != Vector3.INF else Vector3.ZERO
	for l: Dictionary in _lots:
		roads.push_back({"points": _closed(l.points), "half_width": _width(l.points[0])})
	if _points.size() >= 2:
		roads.push_back({"points": _points, "half_width": _width(at) * 2.0})
	ctl.preview.show_ribbons(roads)


## The corner under `p`, or INF.
func _corner(p: Vector3) -> Vector3:
	var reach := ctl.px_at(p) * PICK_PX
	var best := Vector3.INF
	for l: Dictionary in _lots:
		for q: Vector3 in l.points:
			var d := Vector2(q.x - p.x, q.z - p.z).length()
			if d < reach:
				reach = d
				best = q
	return best


## The side of lot `lot` under `p`, by its index; -1 when none.
func _side_at(lot: Dictionary, p: Vector3, reach: float) -> int:
	var best := -1
	var pts: PackedVector3Array = lot.points
	for i in pts.size():
		var a := Vector2(pts[i].x, pts[i].z)
		var b := Vector2(pts[(i + 1) % pts.size()].x, pts[(i + 1) % pts.size()].z)
		var d := Geometry2D.get_closest_point_to_segment(Vector2(p.x, p.z), a, b).distance_to(Vector2(p.x, p.z))
		if d < reach:
			reach = d
			best = i
	return best


func _lot_of(id: int) -> Dictionary:
	for l: Dictionary in _lots:
		if l.id == id:
			return l
	return {}


func click(p: Vector3) -> void:
	var id: int = CityStore.query(&"lot_at", [p])
	match mode:
		"add":
			_from = p
		"polygon":
			if _points.size() >= 3 and Vector2(p.x - _points[0].x, p.z - _points[0].z).length() < ctl.px_at(p) * PICK_PX:
				_done(CityStore.dispatch(&"lot_add_polygon", [_points, 0]))
			else:
				_points.push_back(p)
				_show()
		"edit":
			_from = _corner(p)
		"curve":
			for l: Dictionary in _lots:
				var k := _side_at(l, p, ctl.px_at(p) * PICK_PX)
				if k >= 0:
					_side = [l.points[k], l.points[(k + 1) % l.points.size()]]
					_from = p
		"front":
			var lot := _lot_of(id)
			if not lot.is_empty():
				var k := _side_at(lot, p, INF)
				_done(CityStore.dispatch(&"lot_set_front", [id, k]))
		"split":
			if cut_kind == CUT_LINE:
				_from = p
			elif id != 0:
				_done(CityStore.dispatch(&"lot_split", [id, cut_kind, parts, p, p]))
		"join":
			if id != 0 and _first_lot == 0:
				_first_lot = id
			elif id != 0:
				_done(CityStore.dispatch(&"lot_join", [_first_lot, id]))
		"delete":
			if id != 0:
				_done(CityStore.dispatch(&"lot_delete", [id]))


func drag(p: Vector3) -> void:
	if _from == Vector3.INF:
		return
	if mode == "curve" and not _side.is_empty():
		_show([{"points": PackedVector3Array([_side[0], p, _side[1]]), "half_width": _width(p) * 2.0}])
	else:
		_show([{"points": PackedVector3Array([_from, p]), "half_width": _width(p) * 2.0}])


func release(p: Vector3) -> void:
	if _from == Vector3.INF or p == Vector3.INF:
		_from = Vector3.INF
		return
	match mode:
		"add":
			_done(CityStore.dispatch(&"lot_add_rect", [_from, p, CityStore.query(&"lot_angle_at", [_from])]))
		"edit":
			_done(CityStore.dispatch(&"lot_move_corner", [_from, p]))
		"curve":
			if not _side.is_empty():
				_done(CityStore.dispatch(&"lot_curve_side", [_side[0], _side[1], p]))
		"split":
			var id: int = CityStore.query(&"lot_at", [_from.lerp(p, 0.5)])
			if id != 0:
				_done(CityStore.dispatch(&"lot_split", [id, CUT_LINE, parts, _from, p]))
	_from = Vector3.INF


func hover(p: Vector3) -> void:
	var extra := []
	if mode == "split" and cut_kind != CUT_LINE:
		var id: int = CityStore.query(&"lot_at", [p])
		var lines: PackedVector3Array = CityStore.query(&"lot_cut_lines", [id, cut_kind, parts, p, p]) if id != 0 else PackedVector3Array()
		for i in range(0, lines.size() - 1, 2):
			extra.push_back({"points": PackedVector3Array([lines[i], lines[i + 1]]), "half_width": _width(p) * 2.0})
	elif _points.size() >= 1:
		extra.push_back({"points": PackedVector3Array([_points[-1], p]), "half_width": _width(p) * 2.0})
	_show(extra)
