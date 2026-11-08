# Panel.
extends Control
var _speed: int = CityStore.snapshot.speed
var _rows := []
@export var on := false
var _seen := 0
func f():
	on = true
	_seen = CityStore.query(&"x").size()
