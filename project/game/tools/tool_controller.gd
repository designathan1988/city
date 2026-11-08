## ToolController: the state machine of the tool in hand. Turns clicks on the map
## into ground points and hands them to the active tool; tools only dispatch store commands and report hints.
## Tool: NONE (camera) → ROAD | ZONE | DEMOLISH | INSPECT | UPGRADE | SPLIT | ROUNDABOUT | TERRAIN | LOT | MOVE | CONTROL | PAINT | TREES |
## CLOUD | FOG | WEATHER | ELEMENTS | STRIKE | SHOOT | BUILD | TRANSIT | POLE | BARRIER | STREETSCAPE | LOTS; set_tool() is
## the only transition, Esc and a right click that stays a click (CityCamera.right_clicked) cancel; a right or middle
## drag is the camera's and never reaches the tool. A held left button sends drag() and, every frame, tick(); release()
## ends the stroke.
class_name ToolController
extends Node

signal tool_changed(tool: Tool, params: Dictionary)
## A line for the hint bar: text already translated, ok = false draws it as a refusal.
signal hint(text: String, ok: bool)
## The text beside the road in hand (ui/preview_tag.gd): empty hides it.
signal tagged(text: String, at: Vector3, ok: bool)
signal plan_changed # the transport tool laid something the drawer lists
signal road_height_changed # the road tool draws at another height
## Inspect result (CityStore query "inspect") or {} when the selection is cleared.
signal inspected(info: Dictionary)
signal hovered(info: Dictionary)
## The tool changed what the options panel shows.
signal options_changed

enum Tool { NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT, MOVE, CONTROL, PAINT, TREES, CLOUD, FOG, WEATHER, ELEMENTS, STRIKE, SHOOT, BUILD, TRANSIT,
	POLE, BARRIER, STREETSCAPE, LOTS }

const SCRIPTS := {
	Tool.ROAD: preload("res://game/tools/road_tool.gd"),
	Tool.ZONE: preload("res://game/tools/zone_tool.gd"),
	Tool.DEMOLISH: preload("res://game/tools/demolish_tool.gd"),
	Tool.INSPECT: preload("res://game/tools/inspect_tool.gd"),
	Tool.UPGRADE: preload("res://game/tools/upgrade_tool.gd"),
	Tool.SPLIT: preload("res://game/tools/split_tool.gd"),
	Tool.ROUNDABOUT: preload("res://game/tools/roundabout_tool.gd"),
	Tool.TERRAIN: preload("res://game/tools/terrain_tool.gd"),
	Tool.LOT: preload("res://game/tools/lot_tool.gd"),
	Tool.MOVE: preload("res://game/tools/move_tool.gd"),
	Tool.CONTROL: preload("res://game/tools/control_tool.gd"),
	Tool.TRANSIT: preload("res://game/tools/transit_tool.gd"),
	Tool.PAINT: preload("res://game/tools/paint_tool.gd"),
	Tool.TREES: preload("res://game/tools/tree_tool.gd"),
	Tool.ELEMENTS: preload("res://game/tools/element_tool.gd"),
	Tool.CLOUD: preload("res://game/tools/cloud_tool.gd"),
	Tool.FOG: preload("res://game/tools/fog_tool.gd"),
	Tool.WEATHER: preload("res://game/tools/weather_tool.gd"),
	Tool.STRIKE: preload("res://game/tools/strike_tool.gd"),
	Tool.SHOOT: preload("res://game/tools/shoot_tool.gd"),
	Tool.BUILD: preload("res://game/tools/building_tool.gd"),
	Tool.POLE: preload("res://game/tools/pole_tool.gd"),
	Tool.BARRIER: preload("res://game/tools/barrier_tool.gd"),
	Tool.STREETSCAPE: preload("res://game/tools/streetscape_tool.gd"),
	Tool.LOTS: preload("res://game/tools/lots_tool.gd"),
}

var rig: CityCamera
var double_press := false # the press being handled ends a double click
var preview: PreviewLine
var flash: RoadFlash
var tool: Tool = Tool.NONE
var params: Dictionary = {}
## The params each tool was last used with: a category reopens with the tool and choice last used.
var remembered: Dictionary = {}
var active: ToolBase
var pointer := Vector2.ZERO # where the mouse event being handled happened: the OS's cursor may already be elsewhere
var _dragging := false


func _init(camera_rig: CityCamera) -> void:
	rig = camera_rig
	add_to_group(&"tools")
	rig.right_clicked.connect(_right_clicked)


func _right_clicked() -> void:
	if active:
		active.cancel()


func _ready() -> void:
	flash = RoadFlash.new()
	add_child(flash)
	preview = PreviewLine.new()
	add_child(preview)


func set_tool(next: Tool, p: Dictionary = {}) -> void:
	if active:
		active.exit()
	tool = next
	params = p
	remembered[next] = p
	preview.clear()
	active = SCRIPTS[next].new(self, p) if SCRIPTS.has(next) else null
	tool_changed.emit(tool, params)


func last_param(t: Tool, key: String, fallback: Variant) -> Variant:
	return remembered.get(t, {}).get(key, fallback)


## Tests and the input handler: the ground point under a screen position.
func ground_at(screen: Vector2) -> Variant:
	return rig.ground_at(screen)


## Metres a screen pixel covers at a point of the world: radii given in pixels mean the same at any zoom. Camera3D.fov is
## the vertical angle (the camera keeps the view's height).
func px_at(p: Vector3) -> float:
	var cam := rig.camera
	return cam.global_position.distance_to(p) * 2.0 * tan(deg_to_rad(cam.fov) / 2.0) / get_viewport().get_visible_rect().size.y


func click_world(p: Vector3) -> void:
	if active:
		active.click(p)


func hover_world(p: Vector3) -> void:
	if active:
		active.hover(p)


func _process(delta: float) -> void:
	if active and _dragging:
		active.tick(delta)


## Before any _unhandled_input: a key the tool in hand keeps from the camera (Page Up and Page Down, the road's height).
func _unhandled_key_input(event: InputEvent) -> void:
	if active != null and event.pressed and active.keeps_key(event.keycode):
		if not event.echo:
			active.key(event.keycode)
		get_viewport().set_input_as_handled()


## A step up (`dir` 1) or down (-1) of the height the road tool draws at, from its options.
func step_road_height(dir: int) -> void:
	if tool == Tool.ROAD:
		active.step_height(dir)


func _unhandled_input(event: InputEvent) -> void:
	if event is InputEventMouse:
		pointer = event.position
	if active == null:
		return
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		_dragging = event.pressed
		var p: Variant = ground_at(event.position)
		if event.pressed and p != null:
			double_press = event.double_click
			click_world(p)
			get_viewport().set_input_as_handled()
		elif not event.pressed:
			active.release(p if p != null else Vector3.INF)
	elif event is InputEventMouseMotion:
		var p: Variant = ground_at(event.position)
		if p == null:
			return
		if _dragging:
			active.drag(p)
		else:
			hover_world(p)
	elif event is InputEventKey and event.pressed and not event.echo:
		if event.keycode == KEY_ESCAPE:
			active.cancel()
		else:
			active.key(event.keycode)
