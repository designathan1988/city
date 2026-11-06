## Build (Massa, Fachada, Telhado, Cores): a click picks a block, its face and its bay; the strip's tools then act on the pick. With a
## component in hand a click puts it in the wall, at the scope chosen. Holds what is picked, never a number of the game: each edit
## goes to the core, which answers, and undo is the game's own. A model in hand (Models shelf) follows the pointer as a ghost: R turns it,
## a click sets it down and picks it.
class_name BuildingTool
extends ToolBase

const NO_BAY := -1
## What a click on a roof does with the card in hand: a thing set on it, or the flag of the mast clicked.
const ROOF_TOOLS := ["gear", "flag", "flag_colour"]

## What the last click hit (CityStore query "body_pick"), {} for nothing.
var picked := {}
## The component a click puts in the wall, NO_BAY for none.
var armed := NO_BAY
## The part a click sets (a spec of data/ui/builder.json), {} for none.
var part := {}
var scope := 0
var paint_scope := 0
## Which of a flag's three colours the next colour card sets.
var flag_slot := 0

## The model in hand: {bytes, function, zone}, {} for none.
var hand := {}
var turn := 0.0

var _box: SelectionBox
var _ghost := 0
var _extent := Vector3.ZERO
var _fit := {}
var _last := Vector3.INF


func enter() -> void:
	_box = SelectionBox.new()
	ctl.add_child(_box)
	ctl.hint.emit(tr("hint.builder.select"), true)


func exit() -> void:
	_put_down()
	_box.queue_free()


## A body in hand from the shelf: its ghost shows from the next pointer move.
func take(entry: Dictionary) -> void:
	_put_down()
	hand = entry
	_extent = CityStore.query(&"body_size", [entry.bytes])
	_ghost = CityStore.query(&"stage_show", [entry.bytes, ctl.get_viewport().find_world_3d().scenario, true])
	ctl.hint.emit(tr("hint.builder.place"), true)


func _put_down() -> void:
	if _ghost != 0:
		CityStore.query(&"stage_free", [_ghost])
	_ghost = 0
	hand = {}
	_fit = {}


func hover(p: Vector3) -> void:
	if hand.is_empty():
		return
	_last = p
	_fit = CityStore.query(&"place_fit", [p, Vector2(_extent.x, _extent.y), turn])
	var y: float = CityStore.query(&"height_at", [_fit.x, _fit.z])
	CityStore.query(&"stage_move", [_ghost, Transform3D(Basis(Vector3.UP, _fit.rot), Vector3(_fit.x, y, _fit.z))])
	var problem: String = CityStore.query(&"place_check", [hand.bytes, _fit.x, _fit.z, _fit.rot])
	ctl.hint.emit(tr(problem) if problem != "" else tr("hint.builder.place"), problem == "")


func key(code: Key) -> void:
	if hand.is_empty() or code != KEY_R:
		return
	turn += deg_to_rad(DataTables.tuning("build_gear_turn_deg")) if Input.is_key_pressed(KEY_SHIFT) else PI / 2.0
	if _last != Vector3.INF:
		hover(_last)


func _set_down(p: Vector3) -> void:
	hover(p)
	var r: Dictionary = CityStore.dispatch(&"place_model", [hand.bytes, _fit.x, _fit.z, _fit.rot, hand.function, hand.zone])
	if not r.get("ok", false):
		_report(r)
		return
	_put_down()
	picked = {"id": r.placed, "volume": 0, "side": -1, "storey": 0, "level": 0, "bay": NO_BAY}
	_show()
	ctl.hint.emit(tr("building.placed"), true)


func click(p: Vector3) -> void:
	if not hand.is_empty():
		_set_down(p)
		return
	var cam: Camera3D = ctl.rig.camera
	var screen := ctl.get_viewport().get_mouse_position()
	var had := picked
	pick(cam.project_ray_origin(screen), cam.project_ray_normal(screen))
	if not part.is_empty() and part.do == "part" and picked.is_empty() and not had.is_empty():
		picked = had
		_report(CityStore.dispatch(&"body_add_part_at", [had.id, p, part]))
		_show()


## What the ray hits becomes the pick; with a component in hand and a wall hit, the component goes there.
func pick(origin: Vector3, dir: Vector3) -> void:
	var hit: Dictionary = CityStore.query(&"body_pick", [origin, dir])
	picked = hit
	if not hit.is_empty() and hit.side < 0 and not part.is_empty() and ROOF_TOOLS.has(part.do):
		_report(_roof(hit))
	elif hit.is_empty() or hit.side < 0:
		pass
	elif armed != NO_BAY:
		_report(CityStore.dispatch(&"body_set_bay", [hit.id, hit.volume, hit.storey, hit.side, hit.bay, armed, scope]))
	elif not part.is_empty() and part.do == "part":
		_report(CityStore.dispatch(&"body_add_part", [hit.id, hit.volume, hit.storey, hit.side, hit.bay, part]))
	_show()


## The roof clicked, with a card in hand: the thing lands at the point, or the mast there gets the flag or the colour.
func _roof(hit: Dictionary) -> Dictionary:
	match part.do:
		"gear":
			return CityStore.dispatch(&"body_add_gear", [hit.id, hit.volume, hit.point, part])
		"flag":
			return CityStore.dispatch(&"body_set_flag", [hit.id, hit.volume, hit.point, part.value])
		"flag_colour":
			return CityStore.dispatch(&"body_flag_colour", [hit.id, hit.volume, hit.point, flag_slot, part.rgb])
	return {"ok": false, "error": "tool.failed"}


## What Escape steps back first: the body in hand, the bay armed or the part held (the pick itself goes with the tool).
func in_progress() -> bool:
	return not hand.is_empty() or armed != NO_BAY or not part.is_empty()


func cancel() -> void:
	if not hand.is_empty():
		_put_down()
	elif armed != NO_BAY or not part.is_empty():
		armed = NO_BAY
		part = {}
	else:
		picked = {}
	_show()


## A tool of the strip (data/ui/builder.json): `spec.do` names what it does, `spec.value` its choice.
## Construir > Interior: the building chosen cut open, a floor up or down (Roadcraft openInside, seeInside).
func _inside(what: String, by: int) -> void:
	var view: InteriorController = ctl.get_tree().get_first_node_in_group(&"interior_controller")
	if view == null:
		return
	if what == "floor":
		view.step_floor(by)
	elif picked.is_empty():
		ctl.hint.emit(tr("hint.builder.select"), false)
	else:
		view.see_inside(int(picked.get("building", picked.get("id", -1))))


func act(spec: Dictionary) -> void:
	var value: int = spec.get("value", 0)
	match spec.do:
		"arm":
			armed = NO_BAY if armed == value else value
			part = {}
		"part", "gear":
			part = {} if part == spec else spec
			armed = NO_BAY
		"flag", "flag_colour":
			part = {} if part == spec else spec
			armed = NO_BAY
			if not part.is_empty():
				ctl.hint.emit(tr("hint.builder.flag"), true)
				return
		"flag_slot":
			flag_slot = value
		"scope":
			scope = value
		"paint_scope":
			paint_scope = value
		"inside", "floor":
			_inside(spec.do, value)
			return
		_:
			if picked.is_empty():
				ctl.hint.emit(tr("hint.builder.select"), false)
				return
			_report(_edit(spec, value))
	_show()


func _edit(spec: Dictionary, value: int) -> Dictionary:
	var id: int = picked.id
	var vol: int = picked.volume
	match spec.do:
		"storeys":
			var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[vol]
			return CityStore.dispatch(&"body_storeys", [id, vol, v.storeys + value])
		"split":
			return CityStore.dispatch(&"body_split", [id, vol, picked.level])
		"setback":
			return CityStore.dispatch(&"body_add_setback", [id, vol, -1.0, 2])
		"wing":
			return CityStore.dispatch(&"body_add_wing", [id, vol, maxi(picked.side, 0), 0.0, 0.0])
		"remove":
			var r: Dictionary = CityStore.dispatch(&"body_remove_volume", [id, vol])
			if r.get("ok", false):
				picked = {}
			return r
		"pattern":
			return CityStore.dispatch(&"body_set_pattern", [id, vol, -1, -1, value])
		"roof":
			return CityStore.dispatch(&"body_set_roof", [id, vol, value])
		"finish":
			return CityStore.dispatch(&"body_paint", [id, paint_scope, vol, maxi(picked.side, 0), picked.storey, value, spec.rgb])
	return {"ok": false, "error": "tool.failed"}


func _report(r: Dictionary) -> void:
	if not r.get("ok", false):
		ctl.hint.emit(refusal(r), false)


func _show() -> void:
	if picked.is_empty():
		_box.clear()
		ctl.hint.emit(tr("hint.builder.select"), true)
		return
	_box.show_block(CityStore.query(&"body_outline", [picked.id, picked.volume]))
