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


func by_names() -> void:
	var m = "go_by_name"
	Callable(self, m).call()
	call(m)
	var routes = {}
	routes["k"] = route("go_routed")
	routes["k"].callv([])


func route(spec: String) -> Callable:
	return Callable(self, spec)


func go_by_name() -> void:
	pass


func go_routed() -> void:
	pass


var watched: int:
	get:
		return watched
	set(value):
		watched = value
		picked.emit()

var named_prop: int: set = _set_named


func _set_named(v: int) -> void:
	named_prop = v


func uses_props() -> void:
	watched = 1
	var w = watched
	named_prop = 2


func cyc_start() -> void:
	cyc_a(RoadTool.new())


func cyc_a(x) -> void:
	cyc_b(x)


func cyc_b(y) -> void:
	cyc_a(y)
	y.click(7.0)
