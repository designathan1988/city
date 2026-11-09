## The Builder's whole-building and drawing edits (building_fields_test.gd): mirror, a level's own height, two
## blocks made one, the building turned, its use, a shape dragged beside it and an outline drawn against it, and a new building of a
## drawn outline. Each lands as asked, and undo and redo give the same city (the test's `typed`).

const LEVEL_H_M := 4.6
const FLOOR_SLACK_M := 0.01
const BOX_FROM := 0.3
const BOX_TO := 0.7
const OUT_M := 6.0 # how far out of the building a drawn block beside it reaches
const SKETCH_M := 9.0 # the side of the L outline drawn as a new building


static func run(test: TestCase, id: int, made: int) -> void:
	var t: Variant = test
	t._typed(id, made, "mirror", &"body_mirror", [id], func(f: Dictionary) -> bool: return not f.is_empty())
	t._typed(id, made, "level height", &"body_level_height", [id, 0, LEVEL_H_M], func(f: Dictionary) -> bool:
		return f.block.level == 0 and absf(f.block.level_h - LEVEL_H_M) < FLOOR_SLACK_M)
	_function(test, id)
	_union(test, id, made)
	_turn(test, id)
	_drawn(test, id, made)


## What the building is for: any use, its body kept as it is (the same bytes); undo gives the city back.
static func _function(test: TestCase, id: int) -> void:
	var school: int = CityStore.code(&"BuildingFn", &"FN_SCHOOL")
	var before: String = CityStore.query(&"state_hash").whole
	var digest: int = BuildingProbe.info(id).digest
	var r: Dictionary = CityStore.dispatch(&"building_function", [id, school])
	var f: Dictionary = CityStore.query(&"body_fields", [id, 0, -1, -1, 0, -1, -1])
	test.check(r.get("ok", false) and f.function == school and BuildingProbe.info(id).digest == digest, "a school now, its body kept: %s" % r)
	test.check(CityStore.dispatch(&"undo") and CityStore.query(&"state_hash").whole == before, "the use undone to the same city")


## A copy of block 0 set flush beside it, then the two made one: one block again, twice as wide.
static func _union(test: TestCase, id: int, made: int) -> void:
	var w: float = BuildingProbe.info(id).volumes[0].w
	var blocks: int = BuildingProbe.info(id).volumes.size()
	test.check(CityStore.dispatch(&"body_copy_block", [id, 0]).get("ok", false), "a copy of the block beside it, to join")
	var r: Dictionary = CityStore.dispatch(&"body_union", [id, 0])
	var after := BuildingProbe.info(id)
	test.check(r.get("ok", false) and after.volumes.size() == blocks and absf(after.volumes[0].w - 2 * w) < FLOOR_SLACK_M, "two blocks made one: %s" % r)
	BuildingProbe.back_to(made, id, test.check, "the union")


## The building turned a quarter about its lot's middle: it stands (or the core says why not), and undo puts it back.
static func _turn(test: TestCase, id: int) -> void:
	var before: String = CityStore.query(&"state_hash").whole
	var r: Dictionary = CityStore.dispatch(&"building_turn", [id, PI / 2])
	if not r.get("ok", false):
		test.check(r.get("error", "") != "" and CityStore.query(&"state_hash").whole == before, "a turn refused leaves the city: %s" % r)
		return
	test.check(CityStore.dispatch(&"undo") and CityStore.query(&"state_hash").whole == before, "the turn undone to the same city")


## A shape (an L) dragged out from a corner of the block, beside it, and a triangle drawn on its roof.
static func _drawn(test: TestCase, id: int, made: int) -> void:
	var box: PackedVector3Array = CityStore.query(&"body_outline", [id, 0])
	var c0: Vector3 = box[0]
	var c1: Vector3 = box[1]
	var c2: Vector3 = box[2]
	var c3: Vector3 = box[3]
	var mass: Dictionary = CityStore.constants.enums.MassAction
	var shape: Dictionary = CityStore.constants.enums.PlanShape
	var blocks: int = BuildingProbe.info(id).volumes.size()
	var r: Dictionary = CityStore.dispatch(&"body_add_mass", [id, 0, c0, c0 + (c0 - c2).normalized() * OUT_M, mass.MASS_GROUND, shape.PLAN_L])
	test.check(r.get("ok", false) and BuildingProbe.info(id).volumes.size() == blocks + 1, "an L dragged beside the block: %s" % r)
	BuildingProbe.back_to(made, id, test.check, "the L beside")
	var top := PackedVector3Array([c0.lerp(c2, BOX_FROM), c0.lerp(c2, BOX_TO), c1.lerp(c3, BOX_FROM)])
	r = CityStore.dispatch(&"body_add_mass_ring", [id, 0, top, mass.MASS_ROOF])
	test.check(r.get("ok", false) and BuildingProbe.info(id).volumes.size() == blocks + 1, "a triangle drawn on the roof: %s" % r)
	BuildingProbe.back_to(made, id, test.check, "the outline on the roof")


## A new building of an L outline, drawn where a copy stood a moment ago (ground known to be free).
static func sketch(test: TestCase, at: Vector3) -> void:
	var h := SKETCH_M / 2
	var l := PackedVector3Array([at + Vector3(-h, 0, -h), at + Vector3(h, 0, -h), at + Vector3(h, 0, 0), at + Vector3(0, 0, 0),
		at + Vector3(0, 0, h), at + Vector3(-h, 0, h)])
	var count: int = CityStore.query(&"list_buildings").size()
	var r: Dictionary = CityStore.dispatch(&"place_sketch", [l])
	test.check(r.get("ok", false) and CityStore.query(&"list_buildings").size() == count + 1, "a new building of a drawn outline: %s" % r)
	if r.get("ok", false):
		test.check(BuildingProbe.info(r.placed).volumes.size() == 1, "the drawn building is one block")
		test.check(CityStore.dispatch(&"undo") and CityStore.query(&"list_buildings").size() == count, "undo takes the drawn building away")
