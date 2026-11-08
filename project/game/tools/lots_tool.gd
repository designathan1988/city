## Zones > Lots (Roadcraft editor/lotTool.ts): the lots the player draws. `mode` says what a gesture does: add (a rectangle dragged square to the
## street), polygon (point by point, closed on its first), edit (a corner dragged), curve (a side bent through the pointer), front (the side
## clicked becomes the front), split (across, along or by a drawn line), join (one lot, then its neighbour), delete (a stroke: the lots and the
## buildings it passes over). The zone layer draws every lot, its front and the lots the streets propose while this tab is open; a gesture on
## a proposed lot, or on a lot of the generated city, takes it as the player's first (one undo step with the edit). The zone brush paints them.
extends ToolBase

## What of a corner or a side the pointer takes, how far a lot's point reaches, the marks (data/game/tools.json lots).
static var take_px: float = DataTables.game("tools").lots.take_px
static var drawn_px: float = DataTables.game("tools").drawn_px
static var catch_px: float = DataTables.game("tools").lots.catch_px
static var snap_least_m: float = DataTables.game("tools").lots.snap_least_m
static var mark_m: float = DataTables.game("tools").lots.mark_m

var mode := "add"
var cut_kind := CityStore.code(&"LotCutKind", &"ACROSS")
var parts := 2
var _from := Vector3.INF # where the gesture began
var _angle := 0.0 # the street's angle at `_from` (lot_angle_at), read once when an "add" gesture began
var _side: Array = [] # the side picked: [a, b]
var _points := PackedVector3Array() # the polygon so far
var _first_lot := 0 # join: the lot picked first
var _land := LotProposals.new()
var _erase := LotEraseStroke.new() # the delete stroke
var _stroke := false # an undo stroke is open: the land lot taken and the edit on it are one step (_lot_for, _close)


func enter() -> void:
	mode = params.get("mode", "add")
	cut_kind = params.get("cut", CityStore.code(&"LotCutKind", &"ACROSS"))
	parts = params.get("parts", 2)
	_land.forget()
	_show()
	ctl.hint.emit(tr("zone.lots.help"), true)


func exit() -> void:
	ctl.preview.clear()
	ctl.tagged.emit("", Vector3.ZERO, true)


func cancel() -> void:
	_close()
	_from = Vector3.INF
	_side = []
	_points = PackedVector3Array()
	_first_lot = 0
	_erase.clear()
	ctl.tagged.emit("", Vector3.ZERO, true)
	_show()


func in_progress() -> bool:
	return _from != Vector3.INF or not _points.is_empty() or _first_lot != 0


## An edit done: its stroke closed, its hint (`said` when it stood, the refusal otherwise), the land read again.
func _done(r: Dictionary, said := "") -> void:
	_close()
	var ok: bool = r.get("ok", false)
	if not ok:
		ctl.hint.emit(refusal(r), false)
	elif not said.is_empty():
		ctl.hint.emit(tr(said), true)
	_land.forget()
	cancel()


## The player's lot at `p`, taking the land lot there first (a proposed lot, or one of the generated city) inside the stroke that
## `name` opens: its id, 0 on none. The stroke stays open until the edit is done (_done) or dropped (_close).
func _lot_for(p: Vector3, name: String) -> int:
	CityStore.dispatch(&"begin_stroke", [name])
	_stroke = true
	var id: int = CityStore.query(&"lot_at", [p])
	if id == 0:
		id = int(CityStore.dispatch(&"lot_take", [p]).get("placed", 0))
	return maxi(id, 0)


func _close() -> void:
	if _stroke:
		CityStore.dispatch(&"end_stroke")
		_stroke = false


func _width(at: Vector3) -> float:
	return ctl.px_at(at) * drawn_px


func _closed(points: PackedVector3Array) -> PackedVector3Array:
	var ring := points.duplicate()
	ring.push_back(points[0])
	return ring


## What the gesture shows over the zone layer: `extra` and the polygon so far.
func _show(extra: Array = []) -> void:
	var roads := extra.duplicate()
	var at: Vector3 = _from if _from != Vector3.INF else Vector3.ZERO
	if _points.size() >= 2:
		roads.push_back({"points": _points, "half_width": _width(at) * 2.0})
	ctl.preview.show_ribbons(roads)


## A point of a lot by the footways, the blocks' corners and the other lots' corners; `apart` is the corner being dragged.
func _snap(p: Vector3, apart := Vector3.INF) -> Vector3:
	return CityStore.query(&"lot_snap_point", [p, maxf(snap_least_m, ctl.px_at(p) * catch_px), apart])


func _mark(p: Vector3) -> Dictionary:
	return {"points": PackedVector3Array([p + Vector3.LEFT * mark_m, p + Vector3.RIGHT * mark_m]), "half_width": mark_m}


func click(p: Vector3) -> void:
	match mode:
		"add":
			_from = _snap(p)
			_angle = CityStore.query(&"lot_angle_at", [_from])
		"polygon":
			if _points.size() >= 3 and Vector2(p.x - _points[0].x, p.z - _points[0].z).length() < ctl.px_at(p) * take_px:
				_done(CityStore.dispatch(&"lot_add_polygon", [_points]), "game.toast.lotCreated")
			else:
				_points.push_back(_snap(p))
				_show()
		"edit":
			_lot_for(p, "cmd.lot.edit")
			_from = CityStore.query(&"lot_corner_near", [p, ctl.px_at(p) * take_px])
			if _from == Vector3.INF:
				_close()
		"curve":
			_lot_for(p, "cmd.lot.edit")
			var side: Dictionary = CityStore.query(&"lot_side_near", [p, ctl.px_at(p) * take_px])
			_side = [side.a, side.b] if not side.is_empty() else []
			_from = p if not side.is_empty() else Vector3.INF
			if side.is_empty():
				_close()
		"front":
			_lot_for(p, "cmd.lot.edit")
			var side: Dictionary = CityStore.query(&"lot_side_near", [p, ctl.px_at(p) * take_px])
			if side.is_empty():
				_close()
				ctl.hint.emit(tr("hint.lot.frontPick"), false)
			else:
				_done(CityStore.dispatch(&"lot_set_front", [side.id, side.side]), "hint.lot.front")
		"split":
			if cut_kind == CityStore.code(&"LotCutKind", &"LINE"):
				_from = p
			else:
				var id := _lot_for(p, "cmd.lot.split")
				_done(CityStore.dispatch(&"lot_split", [id, cut_kind, parts, p, p]) if id != 0 else {}, "game.toast.lotSplit")
		"join":
			var id := _lot_for(p, "cmd.lot.join")
			if id != 0 and _first_lot != 0:
				_done(CityStore.dispatch(&"lot_join", [_first_lot, id]), "game.toast.lotsJoined")
				return
			_close()
			if id != 0:
				_first_lot = id
				ctl.hint.emit(tr("game.hint.lot.joinSecond"), true)
		"delete":
			_from = p # a stroke: all it passes over goes when it is let go
			_erase.start(p)
			_show_erase(p)


## The lots under the delete stroke, drawn thicker until it is let go.
func _show_erase(p: Vector3) -> void:
	var marked := []
	for ring: PackedVector3Array in _erase.rings():
		if ring.size() >= 3:
			marked.push_back({"points": _closed(ring), "half_width": _width(p) * 3.0})
	_show(marked)


func drag(p: Vector3) -> void:
	if _from == Vector3.INF:
		return
	if mode == "delete":
		_erase.reach(p)
		_show_erase(p)
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
	if mode == "delete" and not _erase.points.is_empty():
		_done(_erase.send(), "hint.lot.deleted")
		return
	if _from == Vector3.INF or p == Vector3.INF:
		_close()
		_from = Vector3.INF
		return
	match mode:
		"add":
			var ring := _land.at(p)
			if Vector2(p.x - _from.x, p.z - _from.z).length() < ctl.px_at(p) * take_px and not ring.is_empty():
				_done(CityStore.dispatch(&"lot_add_polygon", [ring]), "game.toast.lotCreated") # a click takes what the streets propose
			else:
				_done(CityStore.dispatch(&"lot_add_rect", [_from, _snap(p), _angle]), "game.toast.lotCreated")
		"edit":
			_done(CityStore.dispatch(&"lot_move_corner", [_from, _snap(p, _from)]))
		"curve":
			_done(CityStore.dispatch(&"lot_curve_side", [_side[0], _side[1], p]) if not _side.is_empty() else {})
		"split":
			var id: int = CityStore.query(&"lot_at", [_from.lerp(p, 0.5)])
			if id != 0:
				_done(CityStore.dispatch(&"lot_split", [id, CityStore.code(&"LotCutKind", &"LINE"), parts, _from, p]), "game.toast.lotSplit")
	_from = Vector3.INF


func hover(p: Vector3) -> void:
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
	elif mode == "front" or mode == "curve":
		var side: Dictionary = CityStore.query(&"lot_side_near", [p, ctl.px_at(p) * take_px])
		if not side.is_empty():
			extra.push_back({"points": PackedVector3Array([side.a, side.b]), "half_width": _width(p) * 2.0})
	var over: PackedVector3Array = CityStore.query(&"lot_ring", [id]) if id != 0 else (_land.at(p) if mode == "add" else PackedVector3Array())
	ctl.tagged.emit(LotProposals.size_of(over) if over.size() >= 3 else "", LotProposals.middle(over) if over.size() >= 3 else p, true)
	_show(extra)
