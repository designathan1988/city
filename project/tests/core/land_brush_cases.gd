## The relief brush by the store, in road_commands_test: what a dab leaves on open ground past the town.
class_name LandBrushCases
extends RefCounted

const MESA_MODE := 4 # BrushMode::MESA (heightfield.h): a round landform with no noise in its outline
const SMOOTH_MODE := 2 # BrushMode::SMOOTH
const LAND_OUT_M := 400.0
const LAND_BRUSH_M := 40.0
const LAND_STRENGTH := 0.4
const LAND_DT := 0.5
const SMOOTH_DABS := 6
## Across a round mesa smoothed with a dab on its centre, the heights at mirrored points: the stored height is quantised to
## 1/HEIGHT_Q_SCALE m (heightfield.h, about 2 mm) and the cells' grid is the same on both sides, so 1 cm is several quanta.
const EVEN_M := 0.01

var t: Node


## `test`: the road_commands_test (TestCase), whose check these are.
func _init(test: Node) -> void:
	t = test


## The smooth brush works each cell from the ground as it was before the dab: a round mesa smoothed on its centre stays round, the
## same at mirrored points east and west, north and south (a smooth read in place leaned with the scan's direction).
func smooth_even(b: Dictionary) -> void:
	var c := Vector3(b.min.x - LAND_OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	CityStore.dispatch(&"terrain_brush", [c, LAND_BRUSH_M, LAND_STRENGTH, LAND_DT, MESA_MODE, 0.5])
	for i in SMOOTH_DABS:
		CityStore.dispatch(&"terrain_brush", [c, LAND_BRUSH_M, 1.0, LAND_DT, SMOOTH_MODE, 0.5])
	var worst := 0.0
	for r: float in [LAND_BRUSH_M * 0.5, LAND_BRUSH_M * 0.8]:
		var e: float = CityStore.query(&"height_at", [c.x + r, c.z]) - float(CityStore.query(&"height_at", [c.x - r, c.z]))
		var n: float = CityStore.query(&"height_at", [c.x, c.z + r]) - float(CityStore.query(&"height_at", [c.x, c.z - r]))
		worst = maxf(worst, maxf(absf(e), absf(n)))
	var top: float = CityStore.query(&"height_at", [c.x, c.z]) - float(CityStore.query(&"height_at", [c.x + LAND_BRUSH_M * 2, c.z]))
	t.check(top > 0.0, "smooth: the mesa stands (%.2f m)" % top)
	t.check(worst < EVEN_M, "smooth: the smoothed mesa is the same on mirrored sides (worst %.3f m)" % worst)
