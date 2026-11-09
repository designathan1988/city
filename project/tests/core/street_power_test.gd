## Core test of how a new road is lit (Roadcraft roadTool.ts layPart, powerLine.ts): by default the power line alone, its poles
## carrying the lamps and no lighting column beside them; the line keeps one face of its street through a node where the street
## goes on, drawn either way (the run of wires goes on pole to pole there) and round a corner; with the road tool's choice off,
## lighting columns on both faces and no power line. The choice is the core's for new roads and comes back as it was set.
## Run: godot --headless --path project res://tests/core/street_power_test.tscn -- --no-flow
extends TestCase

const BARE_MAP := 0
const SEED := 1
const PX := 0.25 # metres a pixel, as the road tool lays a road at play zoom
const STREET := "urbanStreet" # a street with a sidewalk on both faces
const WEST := Vector3(-200, 0, 0)
const EAST := Vector3(0, 0, 0)
const FAR_WEST := Vector3(-380, 0, 0)
const SOUTH := Vector3(0, 0, 160)
const COLUMNS_FROM := Vector3(600, 0, 0)
const COLUMNS_TO := Vector3(800, 0, 0)


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(BARE_MAP, SEED, false)
	check(CityStore.query(&"road_power"), "new roads are lit by the power line by default")
	var street := _profile(STREET)
	check(street >= 0, "the street class is known")
	if street >= 0:
		_power_line(street)
		_columns(street)
	finish()


func _profile(id: String) -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			return p.index
	return -1


func _road(a: Vector3, b: Vector3, profile: int) -> int:
	var built: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, profile, PX])
	check(built.get("ok", false), "a road from %s to %s: %s" % [a, b, built])
	return CityStore.query(&"segment_at", [a.lerp(b, 0.5)]).get("seg", -1)


func _lamps(seg: int) -> Array:
	var out := []
	for s: Dictionary in CityStore.query(&"furniture_spots", [seg]):
		if s.kind == CityStore.code(&"FurnitureKind", &"LAMP") or s.kind == CityStore.code(&"FurnitureKind", &"COLUMN"):
			out.push_back(s)
	return out


## The face of the segment's main row (0 right, 1 left of its a -> b), or -1 when it has no row or the row is not one face.
func _face(lamps: Array) -> int:
	var face := -1
	for s: Dictionary in lamps:
		if s.row != 0:
			continue
		if face >= 0 and s.side != face:
			return -1
		face = s.side
	return face


## On which side of the line z = 0 the main row stands: +1 south, -1 north, 0 mixed or none.
func _z_side(lamps: Array) -> int:
	var sign := 0
	for s: Dictionary in lamps:
		if s.row != 0:
			continue
		var here := 1 if s.position.z > 0.0 else -1
		if sign != 0 and here != sign:
			return 0
		sign = here
	return sign


func _power_line(street: int) -> void:
	var a := _road(WEST, EAST, street)
	var reversed := _road(WEST, FAR_WEST, street) # drawn away from the shared node: the other way round
	var corner := _road(EAST, SOUTH, street)
	for seg: int in [a, reversed, corner]:
		var lamps := _lamps(seg)
		check(not lamps.is_empty(), "segment %d has its line's poles" % seg)
		var alone := true
		for s: Dictionary in lamps:
			alone = alone and s.row == 0 and not s.column and s.kind == CityStore.code(&"FurnitureKind", &"LAMP")
		check(alone, "segment %d: the power line's poles alone, no lighting column beside them" % seg)
	var census: Dictionary = CityStore.query(&"furniture_census")
	check(census.lit == census.kinds[CityStore.code(&"FurnitureKind", &"LAMP")], "every pole of the line carries its lamp: %s" % [census])
	var west := _z_side(_lamps(a))
	check(west != 0 and west == _z_side(_lamps(reversed)), "the line keeps its face through the node a reversed segment meets")
	check(_face(_lamps(reversed)) == 1 - _face(_lamps(a)), "drawn the other way, the reversed segment takes its other face")
	check(_face(_lamps(corner)) == _face(_lamps(a)) and _face(_lamps(a)) >= 0, "round the corner the line goes on along the same face")


func _columns(street: int) -> void:
	CityStore.dispatch(&"set_road_power", [false])
	check(not CityStore.query(&"road_power"), "the choice of lighting columns is kept for new roads")
	var seg := _road(COLUMNS_FROM, COLUMNS_TO, street)
	var lamps := _lamps(seg)
	var columns := not lamps.is_empty()
	var faces := {}
	for s: Dictionary in lamps:
		columns = columns and s.column
		faces[s.side] = true
	check(columns, "lit by columns: every lamp is a lighting column, no power line")
	check(faces.size() == 2, "lit by columns: both faces have them")
	CityStore.dispatch(&"set_road_power", [true])
	check(CityStore.query(&"road_power"), "the power line is chosen again")
	check(_lamps(seg).all(func(s: Dictionary) -> bool: return s.column), "a road laid with columns keeps them")
