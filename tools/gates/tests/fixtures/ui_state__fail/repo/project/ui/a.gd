# Panel.
extends Control
@onready var _turn := 0.0
static var count := 0
var _rows = []
@onready var _model := Model.new()
@export var on := false
func f():
	on = true
