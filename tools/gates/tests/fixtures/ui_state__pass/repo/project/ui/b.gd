# Panel.
extends Control
@onready var _label: Label = $Label
var _press := Vector2.ZERO
var _rows: Array[Label] = []
@export var width_token := "w"
@export var rig: Node3D
var _store = CityStore
func f():
	var local: int = CityStore.snapshot.speed
	_press = Vector2.ONE
	rig = null
	return local
