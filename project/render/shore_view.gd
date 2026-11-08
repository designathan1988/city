## Hands the terrain material the levels of the water beside the land (CityStore query "shore_field") whenever new ones are in:
## the revision is asked each frame, the field only when it moved (as the sky asks the fog and the canyon).
class_name ShoreView
extends Node

var _world: CityWorld
var _seen := 0


func _init(world: CityWorld) -> void:
	_world = world


func _process(_delta: float) -> void:
	if not CityStore.ready_to_read():
		return
	var revision: int = CityStore.query(&"shore_revision")
	if revision == _seen:
		return
	_seen = revision
	var field: Dictionary = CityStore.query(&"shore_field")
	_world.terrain.material.set_shader_param("shore_map", field.texture)
	_world.terrain.material.set_shader_param("shore_grid", field.grid)
