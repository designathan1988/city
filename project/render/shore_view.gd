## Hands the terrain material the levels of the water beside the land (CityStore query "shore_field") whenever new ones are in.
class_name ShoreView
extends Node

var _world: CityWorld
var _seen := 0


func _init(world: CityWorld) -> void:
	_world = world


func _process(_delta: float) -> void:
	if not CityStore.ready_to_read():
		return
	var field: Dictionary = CityStore.query(&"shore_field")
	if field.revision == _seen:
		return
	_seen = field.revision
	_world.terrain.material.set_shader_param("shore_map", field.texture)
	_world.terrain.material.set_shader_param("shore_grid", field.grid)
