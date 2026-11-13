## The save_undo_test case of the road variants' room (doc/specs/003-roadcraft-features/sheets/backbone-records-true.md item 17):
## the catalogue keeps room for every variant through opening a file. A city saved with fewer variants than the open one, then one
## with two more; both open, and a variant can still be made after.
class_name VariantRoomCases
extends RefCounted

const TOWN_MAP := 0
const SEED := 7
## Lane widths of the roads composed: each a variant of its own, none a catalogue road's own.
const LANES_M := [3.15, 3.35, 3.55]
const PLAIN_SLOT := "variants_plain_test"
const VARIANTS_SLOT := "variants_two_test"


## `t`: the save_undo_test (TestCase), whose map, save and check these are.
static func run(t: Node) -> void:
	t.new_map(TOWN_MAP, SEED, false)
	var plain: Dictionary = await t.save(PLAIN_SLOT)
	t.check(plain.get("ok", false), "variants: the city saved as it is (%s)" % [plain])
	var base: int = CityStore.first_road()
	var made := []
	for k in 2:
		var r: Dictionary = CityStore.dispatch(&"road_spec_make", [base, _composed(base, LANES_M[k])])
		made.append(r.get("index", -1))
		t.check(r.get("ok", false) and r.get("index", -1) >= 0, "variants: road %d composed (%s)" % [k, r])
	t.check(made[0] != made[1], "variants: two variants of their own %s" % [made])
	var two: Dictionary = await t.save(VARIANTS_SLOT)
	t.check(two.get("ok", false), "variants: the city saved with two more (%s)" % [two])
	var opened_plain: Dictionary = CityStore.saves.load_game(PLAIN_SLOT, true)
	t.check(opened_plain.get("ok", false), "variants: the city with fewer opens (%s)" % [opened_plain])
	t.check(_catalog_broken().is_empty(), "variants: the catalogue keeps room after opening the city with fewer %s" % [_catalog_broken()])
	var opened_two: Dictionary = CityStore.saves.load_game(VARIANTS_SLOT, true)
	t.check(opened_two.get("ok", false), "variants: the city with more opens after it, not refused as damaged (%s)" % [opened_two])
	t.check(_catalog_broken().is_empty(), "variants: the catalogue keeps room after opening the city with more %s" % [_catalog_broken()])
	var third: Dictionary = CityStore.dispatch(&"road_spec_make", [base, _composed(base, LANES_M[2])])
	t.check(third.get("ok", false) and third.get("index", -1) >= 0, "variants: a new one can be made after the opens (%s)" % [third])
	for slot: String in [PLAIN_SLOT, VARIANTS_SLOT]: # what a slot leaves on disk, the save and its session, goes
		DirAccess.remove_absolute(CityStore.saves.save_path(slot))
		DirAccess.remove_absolute(Session.path(slot))


## The catalogue road `base` read as elements, its first lane `lane_m` wide.
static func _composed(base: int, lane_m: float) -> Dictionary:
	var spec: Dictionary = (CityStore.query(&"road_spec", [base]) as Dictionary).duplicate(true)
	RoadSpecOps.with_width(spec, 1, lane_m)
	return spec


## The core's catalogue invariants broken now (probes/broken_records).
static func _catalog_broken() -> Array:
	return (CityStore.query(&"broken_records") as Array).filter(func(b: Dictionary) -> bool: return String(b.feature) == "catalog")
