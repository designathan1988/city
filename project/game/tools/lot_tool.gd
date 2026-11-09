## Draw a building (Build › Desenhar): press at one corner, drag to the other, release to build. The lot lines up with the nearest road
## and faces it; the card picked in the strip says what it is (school, park, square, … or a building of the land use). The ribbon
## previews the rectangle green or red. Where it draws (the Roadcraft's draw actions): a new building, or against the building picked
## in the drawer (BuildPick) a block beside it, one on its roof or a cut out of it, the plan's shape over the rectangle dragged. The
## sketch card draws a free outline instead: a click a corner, Enter (or a click on the first corner) closes it, Backspace takes the
## last corner back.
extends ToolBase

const NEW := -1 # the draw target that makes a new building; the others are the core's MassAction

static var shape := 0 # the plan picked on the strip (body_ops.h PlanShape): it stays when the card changes
static var target := NEW
static var sketching := false
var fn := 0
var zone := 1
var _a := Vector3.INF
var _mass: MassDrag
var _points := PackedVector3Array()


func enter() -> void:
	fn = params.get("fn", 0)
	zone = params.get("zone", 1)
	_mass = MassDrag.new(ctl)
	ctl.hint.emit("", true)


func exit() -> void:
	_mass.disarm()
	ctl.preview.clear()


## A card of the strip: the plan what is drawn next will have, where it goes, or the free outline.
func act(spec: Dictionary) -> void:
	match spec.do:
		"draw_target":
			target = spec.get("value", NEW)
		"sketch":
			sketching = true
			_points = PackedVector3Array()
			ctl.hint.emit(tr("hint.builder.sketch"), true)
		_:
			shape = spec.get("value", 0)
			sketching = false


func click(p: Vector3) -> void:
	if target != NEW and BuildPick.standing().is_empty():
		ctl.hint.emit(tr("building.selectFirst"), false)
		return
	if sketching:
		_corner(p)
		return
	if target == NEW:
		_a = p
		return
	_mass.arm(target)
	_mass.shape = shape
	_mass.press(p, BuildPick.current)


func drag(p: Vector3) -> void:
	if _mass.dragging():
		_mass.move(p)
		return
	if _a == Vector3.INF:
		return
	var pv: Dictionary = CityStore.query(&"preview_lot", [_a, p])
	if pv.is_empty():
		return
	ctl.preview.show_ribbon(pv.points, pv.half_width, pv.ok)
	ctl.hint.emit("" if pv.ok else refusal(pv), pv.ok)


func release(p: Vector3) -> void:
	if _mass.dragging():
		var made := _mass.finish(p)
		_mass.disarm()
		_told(made)
		return
	if _a == Vector3.INF or p == Vector3.INF:
		return
	var r: Dictionary = CityStore.dispatch(&"draw_building", [_a, p, fn, zone, shape])
	_a = Vector3.INF
	ctl.preview.clear()
	_told(r)


## A corner of the outline: on the ground for a new building or a block beside, at the roof's height against a block above it.
## A click on the first corner closes the outline.
func _corner(ground: Vector3) -> void:
	var p := ground
	if target != NEW:
		if _points.is_empty():
			_mass.arm(target)
			_mass.press(ground, BuildPick.current)
			_mass.cancel()
		p = _mass.at(ground)
	if _points.size() >= 3 and p.distance_to(_points[0]) <= DataTables.tuning("build_click_in_m"):
		_close()
		return
	_points.push_back(p)
	_show_outline()


func _show_outline() -> void:
	if _points.size() < 2:
		ctl.preview.clear()
		return
	var line := _points.duplicate()
	if _points.size() >= 3:
		line.push_back(_points[0])
	ctl.preview.show_ribbon(line, DataTables.tuning("build_thinnest_m"), true)


func _close() -> void:
	var r: Dictionary
	if target == NEW:
		r = CityStore.dispatch(&"place_sketch", [_points])
	else:
		var on: Dictionary = BuildPick.current
		r = CityStore.dispatch(&"body_add_mass_ring", [on.id, on.volume, _points, target])
	_points = PackedVector3Array()
	ctl.preview.clear()
	_told(r)


func key(code: Key) -> void:
	if not sketching:
		return
	if (code == KEY_ENTER or code == KEY_KP_ENTER) and _points.size() >= 3:
		_close()
	elif code == KEY_BACKSPACE and not _points.is_empty():
		_points.remove_at(_points.size() - 1)
		_show_outline()


func _told(r: Dictionary) -> void:
	ctl.hint.emit(tr("ui.build.placed") if r.get("ok", false) else refusal(r), r.get("ok", false))


func in_progress() -> bool:
	return _a != Vector3.INF or _mass.dragging() or not _points.is_empty()


func cancel() -> void:
	_a = Vector3.INF
	_mass.cancel()
	_points = PackedVector3Array()
	ctl.preview.clear()
