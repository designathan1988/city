## What the building tests read of a body through the store, owned once: its info, undo back to a digest, and where a point of
## its lot stands in the world (the core's own lot frame, body_at: a test never computes the engine's geometry again).
class_name BuildingProbe
extends RefCounted


static func info(id: int) -> Dictionary:
	return CityStore.query(&"body_info", [id])


## Undo until the body is back at `made` (its digest), then one check that it is.
static func back_to(made: int, id: int, check: Callable, what: String) -> void:
	while info(id).digest != made and CityStore.dispatch(&"undo"):
		pass
	check.call(info(id).digest == made, "%s undone" % what)


## The world point of (x, y) in the building's lot frame (across the street front, then inward), at its floor.
static func on_lot(id: int, x: float, y: float) -> Vector3:
	return CityStore.query(&"body_at", [id, x, y])


## The middle of a block, at the floor: the centre of its box in the lot frame.
static func block_middle(id: int, volume := 0) -> Vector3:
	var v: Dictionary = info(id).volumes[volume]
	return on_lot(id, v.x + v.w * 0.5, v.y + v.d * 0.5)


## The middle of a block at the height of its top, as the core lays it: the top ring of its outline (body_outline).
static func roof_middle(id: int, volume := 0) -> Vector3:
	var ring: PackedVector3Array = CityStore.query(&"body_outline", [id, volume])
	var middle := block_middle(id, volume)
	return Vector3(middle.x, ring[ring.size() - 1].y, middle.z)
