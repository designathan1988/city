## What a ray from the camera hits, for the Build tools' clicks (building_edit_test.gd): rays at a roof from above and at a wall from
## a metre off it hit that building, that face and the bay under them; a ray past the city hits nothing.

const HEIGHT_M := 40.0
const SKY_M := 3000.0
const WALL_M := 2.0
const OFF_M := 1.0


static func run(id: int, check: Callable) -> void:
	var v: Dictionary = BuildingProbe.info(id).volumes[0]
	var middle := BuildingProbe.block_middle(id)
	var over := BuildingProbe.roof_middle(id) + Vector3.UP * HEIGHT_M
	var down: Dictionary = CityStore.query(&"body_pick", [over, Vector3.DOWN])
	check.call(not down.is_empty() and down.id == id and down.side == -1, "a ray down hits the roof of %d: %s" % [id, down])
	var away: Dictionary = CityStore.query(&"body_pick", [Vector3(0, SKY_M, 0), Vector3.UP])
	check.call(away.is_empty(), "a ray to the sky hits nothing")
	# aimed at the middle of the middle bay: with an even count the middle of the facade is the line between two bays
	var mid_bay: int = v.bays[0] / 2
	var along: float = v.x + v.w * (mid_bay + 0.5) / v.bays[0]
	var out := BuildingProbe.on_lot(id, along, v.y - OFF_M) + Vector3.UP * WALL_M
	var aim := BuildingProbe.on_lot(id, along, v.y + v.d * 0.5) + Vector3.UP * WALL_M
	var wall: Dictionary = CityStore.query(&"body_pick", [out, aim - out])
	check.call(not wall.is_empty() and wall.id == id and wall.side == 0, "a ray from the front hits the facade of %d: %s" % [id, wall])
	var bay: int = wall.get("bay", -1)
	check.call(bay == mid_bay, "the middle bay is hit at its middle: %d of %d" % [bay, v.bays[0]])
	_pitched(id, over, check)


## On the same block a ray stops at the eaves of a flat roof and higher, on the slope, of a gable.
static func _pitched(id: int, over: Vector3, check: Callable) -> void:
	CityStore.dispatch(&"body_set_roof", [id, 0, CityStore.code(&"Roof", &"ROOF_FLAT")])
	var flat: Dictionary = CityStore.query(&"body_pick", [over, Vector3.DOWN])
	CityStore.dispatch(&"body_set_roof", [id, 0, CityStore.code(&"Roof", &"ROOF_GABLE")])
	var slope: Dictionary = CityStore.query(&"body_pick", [over, Vector3.DOWN])
	var rise: float = slope.point.y - flat.point.y
	check.call(slope.id == id and rise > 1.0, "a ray hits the roof slope, %.2f m above the eave" % rise)
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
