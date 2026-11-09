## Zones > Lots (Roadcraft editor/lotTool.ts): the lots the player draws. `mode` says what a gesture does: add (a rectangle dragged square to the
## street), polygon (point by point, closed on its first), edit (a corner dragged), curve (a side bent through the pointer), front (the side
## clicked becomes the front), split (across, along or by a drawn line), join (one lot, then its neighbour), delete. The zone brush paints them.
## Each lot's front, the side its building faces, is marked with an arrow out through it towards the street, and the edit mode marks
## every corner (lotTool.ts drawLot).
extends ToolBase

## What of a corner or a side the pointer takes, how far a lot's point reaches, the marks (data/game/tools.json lots).
static var take_px: float = DataTables.game("tools").lots.take_px
static var drawn_px: float = DataTables.game("tools").drawn_px
static var catch_px: float = DataTables.game("tools").lots.catch_px
static var snap_least_m: float = DataTables.game("tools").lots.snap_least_m
static var mark_m: float = DataTables.game("tools").lots.mark_m
## The colours of the lots' fronts and of the proposed lots (data/ui/tokens.json; lotTool.ts: the front cyan, a proposal white and faint).
const FRONT_TOKEN := "preview_front"
const PROPOSAL_TOKEN := "preview_proposal"
## A proposal's line half as wide as a lot's (lotTool.ts: 0.25 against a lot's 0.35 and a front's 0.8).
const PROPOSAL_WIDTH_SHARE := 0.5

var mode := "add"
var cut_kind := CityStore.code(&"LotCutKind", &"ACROSS")
var parts := 2
var _lots := LotOutlines.new()
var _from := Vector3.INF # where the gesture began
var _angle := 0.0 # the street's angle at `_from` (lot_angle_at), read once when an "add" gesture began
var _side: Array = [] # the side picked: [a, b]
var _points := PackedVector3Array() # the polygon so far
var _first_lot := 0 # join: the lot picked first
var _land := LotProposals.new()
var _erasing: Dictionary = {} # the delete stroke: the lots it passed over, by id (drawn thicker)
var _pointer := Vector3.ZERO # where the pointer was last: the proposals drawn are its cell's
var _stroke := PackedVector3Array() # the delete stroke's way: the core takes its lots and the buildings under it (lot_erase)


func enter() -> void:
	mode = params.get("mode", "add")
	cut_kind = params.get("cut", CityStore.code(&"LotCutKind", &"ACROSS"))
	parts = params.get("parts", 2)
	_read()
	_show()
	ctl.hint.emit(tr("zone.lots.help"), true)


func exit() -> void:
	ctl.preview.clear()
	ctl.tagged.emit("", Vector3.ZERO, true)


func cancel() -> void:
	_from = Vector3.INF
	_side = []
	_points = PackedVector3Array()
	_first_lot = 0
	_erasing = {}
	_stroke = PackedVector3Array()
	ctl.tagged.emit("", Vector3.ZERO, true)
	_show()


## The lots again (after an edit): their outlines are built once here, a band drawn_px pixels to each side at any zoom.
func _read() -> void:
	_lots.read()
	_land.forget()
	var rings := []
	for l: Dictionary in _lots.all:
		rings.push_back(l.points)
	ctl.preview.show_outlines(rings, drawn_px)


func _done(r: Dictionary) -> void:
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)
	_read()
	cancel()


func _width(at: Vector3) -> float:
	return ctl.px_at(at) * drawn_px


func _closed(points: PackedVector3Array) -> PackedVector3Array:
	var ring := points.duplicate()
	ring.push_back(points[0])
	return ring


## What the gesture shows over the lots' outlines (_read): the fronts, the corners in the edit mode, `extra` and the polygon so far.
func _show(extra: Array = []) -> void:
	var roads := extra.duplicate()
	var at: Vector3 = _from if _from != Vector3.INF else Vector3.ZERO
	for ring: PackedVector3Array in _land.near(_pointer): # the lots the streets propose, faint (lotTool.ts refreshProposal)
		roads.push_back({"points": _closed(ring), "half_width": _width(ring[0]) * PROPOSAL_WIDTH_SHARE, "token": PROPOSAL_TOKEN})
	for line in _lots.fronts:
		roads.push_back({"points": line, "half_width": _width(line[0]), "token": FRONT_TOKEN})
	if mode == "edit":
		for l: Dictionary in _lots.all:
			for q: Vector3 in l.points:
				roads.push_back(_mark(q))
	if _points.size() >= 2:
		roads.push_back({"points": _points, "half_width": _width(at) * 2.0})
	ctl.preview.show_ribbons(roads)


## A point of a lot by the footways, the blocks' corners and the other lots' corners; `apart` is the corner being dragged.
func _snap(p: Vector3, apart := Vector3.INF) -> Vector3:
	return CityStore.query(&"lot_snap_point", [p, maxf(snap_least_m, ctl.px_at(p) * catch_px), apart])


func _mark(p: Vector3) -> Dictionary:
	return {"points": PackedVector3Array([p + Vector3.LEFT * mark_m, p + Vector3.RIGHT * mark_m]), "half_width": mark_m}


## The corner under `p`, or INF.
func _corner(p: Vector3) -> Vector3:
	return _lots.corner_near(p, ctl.px_at(p) * take_px)


func click(p: Vector3) -> void:
	var id: int = CityStore.query(&"lot_at", [p])
	match mode:
		"add":
			_from = _snap(p)
			_angle = CityStore.query(&"lot_angle_at", [_from])
		"polygon":
			if _points.size() >= 3 and Vector2(p.x - _points[0].x, p.z - _points[0].z).length() < ctl.px_at(p) * take_px:
				_done(CityStore.dispatch(&"lot_add_polygon", [_points]))
			else:
				_points.push_back(_snap(p))
				_show()
		"edit":
			_from = _corner(p)
		"curve":
			for l: Dictionary in _lots.all:
				var k := LotOutlines.side_near(l, p, ctl.px_at(p) * take_px)
				if k >= 0:
					_side = [l.points[k], l.points[(k + 1) % l.points.size()]]
					_from = p
		"front":
			var lot := _lots.of(id)
			if not lot.is_empty():
				var k := LotOutlines.side_near(lot, p, INF)
				_done(CityStore.dispatch(&"lot_set_front", [id, k]))
		"split":
			if cut_kind == CityStore.code(&"LotCutKind", &"LINE"):
				_from = p
			elif id != 0:
				_done(CityStore.dispatch(&"lot_split", [id, cut_kind, parts, p, p]))
		"join":
			if id != 0 and _first_lot == 0:
				_first_lot = id
			elif id != 0:
				_done(CityStore.dispatch(&"lot_join", [_first_lot, id]))
		"delete":
			_from = p # a stroke: all it passes over goes when it is let go
			_erase_under(p)


## The lots under the delete stroke, drawn thicker until it is let go; its way kept for the core.
func _erase_under(p: Vector3) -> void:
	_stroke.push_back(p)
	var id: int = CityStore.query(&"lot_at", [p])
	if id != 0:
		_erasing[id] = true
	var marked := []
	for l: Dictionary in _lots.all:
		if _erasing.has(l.id):
			marked.push_back({"points": _closed(l.points), "half_width": _width(p) * 3.0})
	_show(marked)


func drag(p: Vector3) -> void:
	if _from == Vector3.INF:
		return
	if mode == "delete":
		_erase_under(p)
		return
	if mode == "curve" and not _side.is_empty():
		_show([{"points": PackedVector3Array([_side[0], p, _side[1]]), "half_width": _width(p) * 2.0}])
	elif mode == "add":
		var ring: PackedVector3Array = CityStore.query(&"lot_land", [_from, _snap(p), _angle])
		_show([{"points": _closed(ring), "half_width": _width(p) * 2.0}] if ring.size() >= 3 else [])
		ctl.tagged.emit(LotProposals.size_of(ring) if ring.size() >= 3 else "", p, true)
	elif mode == "edit":
		_show([{"points": PackedVector3Array([_from, _snap(p, _from)]), "half_width": _width(p) * 2.0}])
	else:
		_show([{"points": PackedVector3Array([_from, p]), "half_width": _width(p) * 2.0}])


func release(p: Vector3) -> void:
	if mode == "delete" and not _stroke.is_empty():
		_done(CityStore.dispatch(&"lot_erase", [_stroke])) # lots, what stands on them and the buildings under it: one undo step
	if _from == Vector3.INF or p == Vector3.INF:
		_from = Vector3.INF
		return
	match mode:
		"add":
			var ring := _land.at(p)
			if Vector2(p.x - _from.x, p.z - _from.z).length() < ctl.px_at(p) * take_px and not ring.is_empty():
				_done(CityStore.dispatch(&"lot_add_polygon", [ring])) # a click takes what the streets propose
			else:
				_done(CityStore.dispatch(&"lot_add_rect", [_from, _snap(p), _angle]))
		"edit":
			_done(CityStore.dispatch(&"lot_move_corner", [_from, _snap(p, _from)]))
		"curve":
			if not _side.is_empty():
				_done(CityStore.dispatch(&"lot_curve_side", [_side[0], _side[1], p]))
		"split":
			var id: int = CityStore.query(&"lot_at", [_from.lerp(p, 0.5)])
			if id != 0:
				_done(CityStore.dispatch(&"lot_split", [id, CityStore.code(&"LotCutKind", &"LINE"), parts, _from, p]))
	_from = Vector3.INF


func hover(p: Vector3) -> void:
	_pointer = p
	var extra := []
	var id: int = CityStore.query(&"lot_at", [p]) # the lot under the pointer, asked once
	if mode == "split" and cut_kind != CityStore.code(&"LotCutKind", &"LINE"):
		var lines: PackedVector3Array = CityStore.query(&"lot_cut_lines", [id, cut_kind, parts, p, p]) if id != 0 else PackedVector3Array()
		for i in range(0, lines.size() - 1, 2):
			extra.push_back({"points": PackedVector3Array([lines[i], lines[i + 1]]), "half_width": _width(p) * 2.0})
	elif mode == "add" or mode == "polygon":
		var at := _snap(p)
		extra.push_back(_mark(at))
		var ring := _land.at(p) if mode == "add" else PackedVector3Array()
		if not ring.is_empty():
			extra.push_back({"points": _closed(ring), "half_width": _width(p) * 2.0})
		if _points.size() >= 1:
			extra.push_back({"points": PackedVector3Array([_points[-1], at]), "half_width": _width(p) * 2.0})
	var over: PackedVector3Array = _lots.of(id).get("points", _land.at(p) if mode == "add" else PackedVector3Array())
	ctl.tagged.emit(LotProposals.size_of(over) if over.size() >= 3 else "", LotProposals.middle(over) if over.size() >= 3 else p, true)
	_show(extra)
