## The Build tool's model in hand: a body from the Models shelf or the clipboard (Ctrl+V of a building copied with Ctrl+C) follows the
## pointer as a ghost where it would land; R turns it (Shift a small step) and a click sets it down. Holds the body's bytes and where
## the core says it fits, never a number of the game.
class_name ModelHand
extends RefCounted

## R turns the model in hand by this (data/game/tools.json building.turn_steps a turn).
static var plain_turn: float = TAU / DataTables.game("tools").building.turn_steps
## The building copied with Ctrl+C: {bytes, function, zone}, {} for none. It outlives the tool, as a clipboard does.
static var clipboard := {}

## The model in hand: {bytes, function, zone}, {} for none.
var hand := {}
var turn := 0.0

var _ctl: ToolController
var _ghost := 0
var _extent := Vector3.ZERO
var _fit := {}
var _last := Vector3.INF


func _init(ctl: ToolController) -> void:
	_ctl = ctl


func holding() -> bool:
	return not hand.is_empty()


## A body in hand: its ghost shows from the next pointer move.
func take(entry: Dictionary) -> void:
	put_down()
	hand = entry
	_extent = CityStore.query(&"body_size", [entry.bytes])
	_ghost = CityStore.query(&"stage_show", [entry.bytes, _ctl.get_viewport().find_world_3d().scenario, true])
	_ctl.hint.emit(_ctl.tr("hint.builder.place"), true)


func put_down() -> void:
	if _ghost != 0:
		CityStore.query(&"stage_free", [_ghost])
	_ghost = 0
	hand = {}
	_fit = {}


func hover(p: Vector3) -> void:
	if hand.is_empty():
		return
	_last = p
	_fit = CityStore.query(&"place_hover", [hand.bytes, p, Vector2(_extent.x, _extent.y), turn]) # one lock; the check only when it moved
	CityStore.query(&"stage_move", [_ghost, Transform3D(Basis(Vector3.UP, _fit.rot), Vector3(_fit.x, _fit.y, _fit.z))])
	var problem: String = _fit.problem
	_ctl.hint.emit(_ctl.tr(problem) if problem != "" else _ctl.tr("hint.builder.place"), problem == "")


## R turns what is in hand; true when it did.
func key(code: Key) -> bool:
	if hand.is_empty() or code != KEY_R:
		return false
	turn += deg_to_rad(DataTables.tuning("build_gear_turn_deg")) if Input.is_key_pressed(KEY_SHIFT) else plain_turn
	if _last != Vector3.INF:
		hover(_last)
	return true


## The model set down where its ghost stands: the core's answer, with `placed` the new building.
func set_down(p: Vector3) -> Dictionary:
	hover(p)
	var r: Dictionary = CityStore.dispatch(&"place_model", [hand.bytes, _fit.x, _fit.z, _fit.rot, hand.function, hand.zone])
	if r.get("ok", false):
		put_down()
	return r


## Ctrl+C: the building picked kept as a model to set down again (its body, its use and its zone).
static func copy(picked: Dictionary) -> void:
	var fields: Dictionary = CityStore.query(&"body_fields", [picked.id, picked.volume, -1, -1, 0, -1, -1])
	clipboard = {"bytes": CityStore.query(&"building_bytes", [picked.id]), "function": fields.get("function", 0), "zone": fields.get("zone", 1)}
