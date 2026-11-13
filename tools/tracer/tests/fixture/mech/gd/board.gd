# A signal connected and emitted by its name; a method bound and connected; a node of the scene by its path.
class_name Board
extends Node

signal tapped

func _ready():
	connect("tapped", _on_tap)
	var b = Button.new()
	b.pressed.connect(_pick.bind(3))
	$Light.glow()

func fire():
	emit_signal(&"tapped")

func _on_tap():
	pass

func _pick(n):
	pass
