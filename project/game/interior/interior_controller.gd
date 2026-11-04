## The Interior layer on the script side (the core holds what is open: its set_interior command and interior query). Here: the switch (the layers
## popover), the building in the middle of the view or double-clicked, the floor, the camera's direction in eight steps and the room
## lights. As in the 4.0 (buildingsWiring.ts seeInside, insideClick, openInside) the camera stays where the player put it: opening,
## changing floor and clicking only name the building and the floor. It keeps no number of the game: the floor and the building live
## in the core; `_state` is only the last reading of them, read again when the core's revision changes (the query is heavy: never
## every frame).
class_name InteriorController
extends Node

const STEPS := 8
const STEP_RAD := TAU / float(STEPS)
## Looking straight down the horizontal "forward" of the lens is nothing: the screen's up on the ground stands in for it.
const FLAT_LOOK := 0.2

@onready var rig: CityCamera = get_tree().get_first_node_in_group(&"camera_rig")
@onready var hud: RoadcraftHud = get_tree().get_first_node_in_group(&"hud")
var _world: CityWorld
var _bar: InteriorBar
var _lights: InteriorLights
var _cfg: Dictionary
var _state: Dictionary = {}
var _revision := -1
var _step := -1


func _ready() -> void:
	add_to_group(&"interior_controller")
	_cfg = DataTables.game("interior")
	for sibling in get_parent().get_children():
		if sibling is CityWorld:
			_world = sibling
	_lights = InteriorLights.new(_cfg.lights)
	add_child(_lights)
	_bar = InteriorBar.new()
	_bar.floor_step.connect(_move_floor)
	_bar.closed.connect(func() -> void: set_enabled(false))
	var root := hud.root if hud else null
	if root:
		root.add_child(_bar)
	else:
		push_warning("InteriorController: sem HUD, a barra do andar não aparece")


## The layers popover's switch. True when the layer is on after the call (false: nothing in the middle of the view).
func set_enabled(on: bool) -> bool:
	if not on:
		_send(false, -1, 0)
		return false
	var id := _building_at(get_viewport().get_visible_rect().size / 2.0)
	if id < 0:
		_say(tr("game.toast.insideNone"))
		return false
	_open(id, 0)
	return true


func is_on() -> bool:
	return _state.get("on", false)


func _unhandled_input(event: InputEvent) -> void:
	if not (event is InputEventMouseButton) or not event.pressed or event.button_index != MOUSE_BUTTON_LEFT:
		return
	var tools: ToolController = get_tree().get_first_node_in_group(&"tools")
	if tools and tools.tool != ToolController.Tool.NONE and tools.tool != ToolController.Tool.INSPECT:
		return
	if not event.double_click and not is_on():
		return
	var id := _building_at(event.position)
	if event.double_click:
		if id < 0:
			_send(false, -1, 0)
		else:
			_open(id, 0)
	elif id >= 0 and id != _state.target:
		_open(id, _state.level)


func _process(_delta: float) -> void:
	if int(CityStore.query(&"interior_revision")) != _revision:
		_refresh() # a new city or a load closed the layer under us
	if not is_on():
		return
	var step := _direction_step()
	if step != _step:
		_send(true, _state.target, _state.level, step)
	_lights.set_night(_world.daylight.night if _world else 0.0)


## Opens building `id` at `level`; the camera stays.
func _open(id: int, level: int) -> void:
	_send(true, id, level)


func _move_floor(delta: int) -> void:
	if is_on():
		_send(true, _state.target, clampi(_state.level + delta, 0, maxi(_state.floors - 1, 0)))


func _send(on: bool, target: int, level: int, step := -1) -> void:
	_step = _direction_step() if step < 0 else step
	CityStore.dispatch(&"set_interior", [on, target, level, _step])
	_refresh()
	if not on:
		_step = -1


## Reads the core's view again: the bar and the lights follow it.
func _refresh() -> void:
	_state = CityStore.query(&"interior")
	_revision = _state.revision
	_bar.show_state(_state)
	_lights.set_lamps(_state.lamps if is_on() else PackedVector3Array())


## The way the camera looks on the ground, as one of the eight steps the core cuts for (from +x towards +z).
static func direction_of(camera: Camera3D) -> int:
	var basis := camera.global_transform.basis
	var look := Vector2(-basis.z.x, -basis.z.z)
	if look.length() < FLAT_LOOK:
		look = Vector2(basis.y.x, basis.y.z)
	return posmod(roundi(look.angle() / STEP_RAD), STEPS)


func _direction_step() -> int:
	return direction_of(rig.camera)


func _building_at(screen: Vector2) -> int:
	var p: Variant = rig.ground_at(screen)
	if p == null:
		return -1
	var hit: Dictionary = CityStore.query(&"inspect", [p])
	return hit.id if hit.get("kind", "") == "building" else -1


func _say(text: String) -> void:
	if hud:
		hud.show_hint(text, false)
