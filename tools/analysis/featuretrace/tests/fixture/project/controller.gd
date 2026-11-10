## Fixture controller: input, signals, a narrowed and an untyped receiver.
class_name Controller
extends Node

signal picked

var active: ToolBase


func _ready() -> void:
	picked.connect(_on_picked)
	$Button.pressed.connect(_on_pressed)


func _unhandled_input(_e: InputEvent) -> void:
	if active:
		active.click(1.0)


func _on_pressed() -> void:
	picked.emit()


func _on_picked() -> void:
	pass


func _on_scene_pressed() -> void:
	pass


func narrowed(x) -> void:
	if x is RoadTool:
		x._lay(2.0)


func untyped(o) -> void:
	o.click(3.0)


func flows() -> void:
	untyped(RoadTool.new())
	mixed(RoadTool.new())
	mixed(ToolBase.new())
	var held = []
	held.append(RoadTool.new())
	for t in held:
		t._lay(4.0)
	made()._lay(1.0)
	var n = name
	n.length()


func mixed(x) -> void:
	x.click(6.0)


func never_called(o) -> void:
	o.click(5.0)


func made():
	return RoadTool.new()
