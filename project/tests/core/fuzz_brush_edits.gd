## One pointer move of a held brush, in the sorting of edit_fuzz_test.gd: zones along a road, ground paint, trees, fog or elements,
## inside its stroke.

const PATH_DABS := 3
const PATH_BRUSHES := 4
const TREE_LIKE := {"density": 1.0, "height": 1.0, "variation": 0.0, "spacing": 4.0}
const ELEMENT_LIKE := {"density": 1.0, "size": 1.0, "variation": 0.0, "spacing": 4.0, "strength": 1.0, "intensity": 1.0}


static func run(rng: RandomNumberGenerator, a: Vector3, grid_m: float, brush_m: float, modes: int, zone_radius: float, road: Vector3) -> void:
	var dabs := PackedVector3Array()
	for i in PATH_DABS:
		dabs.append(a + Vector3(grid_m * 0.5 * i, 0, 0))
	var which := rng.randi_range(0, PATH_BRUSHES)
	CityStore.dispatch(&"begin_stroke", ["cmd.place.paint"])
	match which:
		0:
			CityStore.dispatch(&"paint_zone_path", [PackedVector3Array([road, road + Vector3(zone_radius, 0, 0)]), zone_radius, ZoneTable.code("residential")])
		1:
			CityStore.dispatch(&"paint_path", [dabs, brush_m, 1.0, 0.5, rng.randi_range(0, modes)])
		2:
			CityStore.dispatch(&"tree_path", [dabs, brush_m, 0, 0, TREE_LIKE])
		3:
			CityStore.dispatch(&"fog_path", [dabs, brush_m, 1.0, 10.0, 0.0, false])
		_:
			CityStore.dispatch(&"element_path", [dabs, brush_m, 0, ELEMENT_LIKE, false])
	CityStore.dispatch(&"end_stroke")
