# Panel.
extends Control
@onready var _label: Label = $Label
@onready var _box := $Box
@onready var _tw := create_tween()
var _rows: Array[Label] = []
@export var width_token := "w"
var _store = CityStore
func f():
	var local := 3.0
	return local
