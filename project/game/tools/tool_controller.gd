## ToolController: the state machine of the tool in hand. Turns clicks on the map
## into ground points and hands them to the active tool; tools only dispatch store commands and report hints.
## Tool: NONE (camera) → ROAD | ZONE | DEMOLISH | INSPECT | UPGRADE | SPLIT | ROUNDABOUT | TERRAIN | LOT | PAINT | TREES |
## CLOUD | FOG | WEATHER | ELEMENTS; set_tool() is
## the only transition, Esc and a right click that stays a click (CityCamera.right_clicked) cancel; a right or middle
## drag is the camera's and never reaches the tool. A held left button sends drag() and, every frame, tick(); release()
## ends the stroke.
class_name ToolController
extends Node

signal tool_changed(tool: Tool, params: Dictionary)
## A line for the hint bar: text already translated, ok = false draws it as a refusal.
signal hint(text: String, ok: bool)
## Inspect result (CityStore query "inspect") or {} when the selection is cleared.
signal inspected(info: Dictionary)

enum Tool { NONE, ROAD, ZONE, DEMOLISH, INSPECT, UPGRADE, SPLIT, ROUNDABOUT, TERRAIN, LOT, BUILD, PAINT, TREES, CLOUD, FOG, WEATHER, ELEMENTS }

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
	Tool.BUILD: preload("res://game/tools/building_tool.gd"),
	Tool.PAINT: preload("res://game/tools/paint_tool.gd"),
	Tool.TREES: preload("res://game/tools/tree_tool.gd"),
	Tool.ELEMENTS: preload("res://game/tools/element_tool.gd"),
	Tool.CLOUD: preload("res://game/tools/cloud_tool.gd"),
	Tool.FOG: preload("res://game/tools/fog_tool.gd"),
	Tool.WEATHER: preload("res://game/tools/weather_tool.gd"),
}

var rig: CityCamera
var preview: PreviewLine
var tool: Tool = Tool.NONE
var params: Dictionary = {}
## The params each tool was last used with: a category reopens with the tool and choice last used.
var remembered: Dictionary = {}
var active: ToolBase
var _dragging := false


func _init(camera_rig: CityCamera) -> void:
	rig = camera_rig
	add_to_group(&"tools")
	rig.right_clicked.connect(_right_clicked)


func _right_clicked() -> void:
	if active:
		active.cancel()


func _ready() -> void:
	preview = PreviewLine.new()
	add_child(preview)


func set_tool(next: Tool, p: Dictionary = {}) -> void:
	if active:
		active.exit()
	tool = next
	params = p
	remembered[next] = p
	active = SCRIPTS[next].new(self, p) if SCRIPTS.has(next) else null
	preview.clear()
	tool_changed.emit(tool, params)


func last_param(t: Tool, key: String, fallback: Variant) -> Variant:
	return remembered.get(t, {}).get(key, fallback)


## Tests and the input handler: the ground point under a screen position.
func ground_at(screen: Vector2) -> Variant:
	return rig.ground_at(screen)


func click_world(p: Vector3) -> void:
	if active:
		active.click(p)


func hover_world(p: Vector3) -> void:
	if active:
		active.hover(p)


func _process(delta: float) -> void:
	if active and _dragging:
		active.tick(delta)


func _unhandled_input(event: InputEvent) -> void:
	if active == null:
		return
	if event is InputEventMouseButton and event.button_index == MOUSE_BUTTON_LEFT:
		_dragging = event.pressed
		var p: Variant = ground_at(event.position)
		if event.pressed and p != null:
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
