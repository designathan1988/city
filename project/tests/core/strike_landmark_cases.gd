## The landmark case of strike_test: a building with a design of its own (the lot lab's church) by a street on free ground.
class_name StrikeLandmarkCases
extends RefCounted

## A street past the town, off the poles case's, and the church set down along it where it fits.
const FIELD_M := 400.0
const ACROSS_M := 700.0
const ROAD_LEN_M := 300.0
const PX := 0.25
const SIDE_M := 30.0 # the first try at its place, off the street's line
const TRIES := 12
const TRY_M := 10.0
const SKY_M := 900.0
const SMALL_FORCE := 5
const HITS_MOST := 12 # blows of the strongest charge that bring it down
const PIECES_LEAST := 12
const FRAMES := 3000
const CHURCH := "lot:church"

var t: Node # the test (TestCase): its check and frames


func _init(test: Node) -> void:
	t = test


## A small bomb on its roof breaks part of it and leaves it standing, drawn in pieces of its own faces; undone it stands whole. The
## strongest charge, again and again, brings it down in pieces of its own faces, not as a plain heap.
func run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x + FIELD_M, 0, b.max.z + ACROSS_M)
	CityStore.dispatch(&"build_road", [a, a + Vector3(0, 0, ROAD_LEN_M), false, Vector3.ZERO, CityStore.first_road(), PX])
	var church: Dictionary = LotProjects.entries().filter(func(e: Dictionary) -> bool: return e.key == CHURCH)[0]
	var size: Vector3 = CityStore.query(&"body_size", [church.bytes])
	var where := a + Vector3(SIDE_M, 0, 0)
	var fit: Dictionary = {}
	for k in TRIES:
		fit = CityStore.query(&"place_fit", [where, Vector2(size.x, size.y), 0.0])
		if CityStore.query(&"place_check", [church.bytes, fit.x, fit.z, fit.rot]) == "":
			break
		where += Vector3(0, 0, TRY_M)
	var placed: Dictionary = CityStore.dispatch(&"place_model", [church.bytes, fit.x, fit.z, fit.rot, church.function, church.zone])
	var own: bool = placed.get("ok", false) and CityStore.query(&"body_info", [placed.placed]).get("signature", 0) > 0
	t.check(own, "the church stands by the street with its own design: %s" % [placed])
	if not own:
		return
	var at: Vector3 = CityStore.query(&"probe_building", [placed.placed]).position
	var hit: Dictionary = CityStore.query(&"body_pick", [Vector3(at.x, SKY_M, at.z), Vector3.DOWN])
	t.check(not hit.is_empty(), "its roof is found from the sky")
	if hit.is_empty():
		return
	await _scar(hit.point, CityStore.query(&"probe_building", [placed.placed]).seed)
	await _fall(hit.point)


func _scar(roof: Vector3, seed: int) -> void:
	await _until(func() -> bool: return not CityStore.query(&"damage_probe").effects_busy)
	var before: Dictionary = CityStore.query(&"damage_probe")
	t.check(CityStore.dispatch(&"strike", [roof, SMALL_FORCE]).get("ok", false), "a small bomb on the church's roof")
	var after: Dictionary = CityStore.query(&"damage_probe")
	t.check(after.scars > before.scars and after.buildings == before.buildings, "part of the church broke off, and it stands (%d scars)" % after.scars)
	# drawn in pieces of its own faces while it stands: the pieces of its own fall (another broken building may make way for it)
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_pieces_of", [seed]) > 0)
	var pieces: int = CityStore.query(&"effects_ruin_pieces_of", [seed])
	t.check(pieces >= PIECES_LEAST and CityStore.query(&"damage_probe").buildings == before.buildings, "the church stands drawn in pieces of its own faces (%d)" % pieces)
	t.check(CityStore.dispatch(&"undo", []), "undid the bomb on the church")
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_pieces_of", [seed]) == 0)
	t.check(CityStore.query(&"effects_ruin_pieces_of", [seed]) == 0, "undo gave the church back whole")


func _fall(roof: Vector3) -> void:
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_flying") == 0)
	var ruins: int = CityStore.query(&"damage_probe").ruins
	var force := int(DataTables.catalog_table("blast").strength_max)
	for k in HITS_MOST:
		CityStore.dispatch(&"strike", [roof, force])
		if CityStore.query(&"damage_probe").ruins > ruins:
			break
	t.check(CityStore.query(&"damage_probe").ruins > ruins, "the church came down")
	var seed: int = CityStore.query(&"ruin_seeds")[-1]
	await _until(func() -> bool: return CityStore.query(&"effects_ruin_pieces_of", [seed]) >= PIECES_LEAST)
	var pieces: int = CityStore.query(&"effects_ruin_pieces_of", [seed])
	print("pieces of the church: ", pieces)
	t.check(pieces >= PIECES_LEAST, "the church came down in pieces of its own faces (%d)" % pieces)


func _until(met: Callable) -> void:
	for f in FRAMES:
		if met.call():
			return
		await t.get_tree().process_frame
