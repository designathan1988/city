## Hands the ground's materials what the ecosystem leaves on it (CityStore query "ecology_field": the dry herbs, the waterlogged
## ground and the bare soil, render/cover/ecology_raster.h) whenever a new raster is in: the revision is asked each frame, the
## field only when it moved (as ShoreView asks the shore's).
class_name EcologyView
extends Node

var _world: CityWorld
var _seen := 0


func _init(world: CityWorld) -> void:
	_world = world


func _process(_delta: float) -> void:
	if not CityStore.ready_to_read():
		return
	var revision: int = CityStore.query(&"ecology_revision")
	if revision == _seen:
		return
	_seen = revision
	_world.set_ecology(CityStore.query(&"ecology_field"))
