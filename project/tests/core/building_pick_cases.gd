## What a ray from the camera hits, for the Build tools' clicks (building_edit_test.gd): rays at a roof from above and at a wall from
## a metre off it hit that building, that face and the bay under them; a ray past the city hits nothing.

const HEIGHT_M := 40.0
const SKY_M := 3000.0
const WALL_M := 2.0
const OFF_M := 1.0
const ROOF_FLAT := 0
const ROOF_GABLE := 2


static func run(id: int, check: Callable) -> void:
	var info: Dictionary = CityStore.query(&"body_info", [id])
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var v: Dictionary = info.volumes[0]
	var height: float = info.ground_h + info.storey_h * (v.storeys - 1)
	var middle := _on_lot(p, v.x + v.w * 0.5, v.y + v.d * 0.5)
	var down: Dictionary = CityStore.query(&"body_pick", [middle + Vector3.UP * (height + HEIGHT_M), Vector3.DOWN])
	check.call(not down.is_empty() and down.id == id and down.side == -1, "raio para baixo acerta o telhado de %d: %s" % [id, down])
	var away: Dictionary = CityStore.query(&"body_pick", [Vector3(0, SKY_M, 0), Vector3.UP])
	check.call(away.is_empty(), "raio para o céu não acerta nada")
	var out := _on_lot(p, v.x + v.w * 0.5, v.y - OFF_M) + Vector3.UP * WALL_M
	var wall: Dictionary = CityStore.query(&"body_pick", [out, middle + Vector3.UP * WALL_M - out])
	check.call(not wall.is_empty() and wall.id == id and wall.side == 0, "raio de frente acerta a fachada de %d: %s" % [id, wall])
	var bay: int = wall.get("bay", -1)
	check.call(bay == v.bays[0] / 2, "o meio da fachada é o vão do meio: %d de %d" % [bay, v.bays[0]])
	_pitched(id, middle + Vector3.UP * (height + HEIGHT_M), check)


## On the same block a ray stops at the eaves of a flat roof and higher, on the slope, of a gable.
static func _pitched(id: int, over: Vector3, check: Callable) -> void:
	CityStore.dispatch(&"body_set_roof", [id, 0, ROOF_FLAT])
	var flat: Dictionary = CityStore.query(&"body_pick", [over, Vector3.DOWN])
	CityStore.dispatch(&"body_set_roof", [id, 0, ROOF_GABLE])
	var slope: Dictionary = CityStore.query(&"body_pick", [over, Vector3.DOWN])
	var rise: float = slope.point.y - flat.point.y
	check.call(slope.id == id and rise > 1.0, "raio acerta a água do telhado, %.2f m sobre o beiral" % rise)
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")


## A point of the lot's frame on the map: the origin is the middle of the street front (b40::lot_frame).
static func _on_lot(p: Dictionary, x: float, y: float) -> Vector3:
	var s: float = sin(p.rot)
	var c: float = cos(p.rot)
	return p.position + Vector3(s, 0, c) * (p.d * 0.5) + Vector3(c, 0, -s) * x + Vector3(-s, 0, -c) * y
