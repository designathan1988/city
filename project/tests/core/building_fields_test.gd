## Core test of the Build drawer's Selection panel through the store: each typed number (a block's width, depth, offset and level,
## a free part's size, a roof thing's height) lands in the body as asked and reads back from body_fields; each is one step of undo
## that gives the very body back and redo the same bytes, the city's hash with it; a pick finds the part and the roof thing a ray
## meets; a building copied beside itself stands apart, as a building of its own with the same body, and undo takes it away.
## Run: godot --headless --path project res://tests/core/building_fields_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const MIN_FLOORS := 3
const FEW_BLOCKS := 2
const NEW_WIDTH_M := 13.7
const NEW_DEPTH_M := 11.3
const OFFSET_M := 1.5
const SETBACK_INSET_M := 2.0
const SETBACK_STOREYS := 2
const PART_HEIGHT_M := 2.4
const PART_WIDTH_M := 1.8
const GEAR_HEIGHT_M := 9.5
const FREE_OUT_M := 30.0
const SLACK_M := 0.01 # a length read back within a centimetre of the one typed
const ABOVE_M := 60.0 # the eye of a pick straight down, over any roof of the test town
const PART_EYE_M := 3.0 # the eye of a pick straight down onto a part on the ground, over the part and under any neighbour's roof


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var id := _a_tall_building()
	if not check(id >= 0, "found a tall building"):
		finish()
		return
	var made: int = BuildingProbe.info(id).digest
	var axes: Dictionary = CityStore.constants.enums.PlanAxis
	_typed(id, made, "width", &"body_block_size", [id, 0, axes.AXIS_X, NEW_WIDTH_M], func(f: Dictionary) -> bool:
		return absf(f.block.w - NEW_WIDTH_M) < SLACK_M)
	_typed(id, made, "depth", &"body_block_size", [id, 0, axes.AXIS_Y, NEW_DEPTH_M], func(f: Dictionary) -> bool:
		return absf(f.block.d - NEW_DEPTH_M) < SLACK_M)
	_upper_block(id, made, axes)
	_part(id, made)
	_gear(id, made)
	_duplicate()
	finish()


func _a_tall_building() -> int:
	for id: int in CityStore.query(&"list_buildings"):
		var volumes: Array = BuildingProbe.info(id).volumes
		if not volumes.is_empty() and volumes[0].base == 0 and volumes[0].storeys >= MIN_FLOORS and volumes.size() <= FEW_BLOCKS:
			return id
	return -1


func _fields(id: int, vol := 0, part := -1, gear := -1) -> Dictionary:
	return CityStore.query(&"body_fields", [id, vol, -1, -1, 0, part, gear])


## One typed edit: it lands as asked, undo gives the body and the city's hash back, redo the same body and hash.
func _typed(id: int, made: int, what: String, command: StringName, args: Array, did: Callable, vol := 0, part := -1, gear := -1) -> void:
	var hash_before := _hash()
	var r: Dictionary = CityStore.dispatch(command, args)
	var after := BuildingProbe.info(id)
	var hash_after := _hash()
	check(r.get("ok", false) and did.call(_fields(id, vol, part, gear)) and after.digest != made, "%s typed: %s" % [what, r])
	check(CityStore.dispatch(&"undo") and _hash() == hash_before, "%s undone to the same city" % what)
	check(CityStore.dispatch(&"redo") and BuildingProbe.info(id).digest == after.digest and _hash() == hash_after, "%s redone to the same body" % what)
	CityStore.dispatch(&"undo")


## The city as the save writes it (probes/state_hash): two equal are the same city.
func _hash() -> String:
	return CityStore.query(&"state_hash").whole


## A setback on the roof: its middle typed from the block under it, and the level it starts on carried down with it.
func _upper_block(id: int, made: int, axes: Dictionary) -> void:
	check(CityStore.dispatch(&"body_add_setback", [id, 0, SETBACK_INSET_M, SETBACK_STOREYS]).get("ok", false), "a setback to place")
	var top := int(BuildingProbe.info(id).volumes.size()) - 1
	var with_setback: int = BuildingProbe.info(id).digest
	var base: int = BuildingProbe.info(id).volumes[top].base
	check(_fields(id, top).block.has("offset"), "a block on a roof is measured from the block under it")
	_typed(id, with_setback, "offset", &"body_block_offset", [id, top, axes.AXIS_X, OFFSET_M], func(f: Dictionary) -> bool:
		return absf((f.block.offset as Vector2).x - OFFSET_M) < SLACK_M, top)
	_typed(id, with_setback, "base level", &"body_block_base", [id, top, base - 1], func(f: Dictionary) -> bool:
		return f.block.base == base - 1, top)
	check(not _fields(id, 0).block.has("offset"), "the first block on the ground is what the others are measured from: it has no offset")
	BuildingProbe.back_to(made, id, check, "the setback")


## A free part away from the walls: picked by a ray straight down, sized across and up, each an undo step.
func _part(id: int, made: int) -> void:
	var bench := _card("parts", "bench")
	var at := BuildingProbe.on_lot(id, 0.0, -FREE_OUT_M)
	if not check(CityStore.dispatch(&"body_add_part_at", [id, at, bench]).get("ok", false), "a free part to size"):
		return
	var part := int(BuildingProbe.info(id).parts) - 1
	var placed: int = BuildingProbe.info(id).digest
	var hit: Dictionary = CityStore.query(&"body_pick", [at + Vector3.UP * PART_EYE_M, Vector3.DOWN]) # under any roof it may stand under
	check(hit.get("id", -1) == id and hit.get("part", -1) == part, "a ray straight down picks the part: %s" % hit)
	var parts: Dictionary = CityStore.constants.enums.PartAxis
	_typed(id, placed, "part height", &"body_size_part", [id, part, parts.PART_HEIGHT, PART_HEIGHT_M], func(f: Dictionary) -> bool:
		return absf(f.part.h - PART_HEIGHT_M) < SLACK_M, 0, part)
	_typed(id, placed, "part width", &"body_size_part", [id, part, parts.PART_ACROSS, PART_WIDTH_M], func(f: Dictionary) -> bool:
		return absf(f.part.across - PART_WIDTH_M) < SLACK_M, 0, part)
	BuildingProbe.back_to(made, id, check, "the part")


## A spire on the roof: picked by a ray down onto it, its height typed as an undo step.
func _gear(id: int, made: int) -> void:
	var middle := BuildingProbe.block_middle(id)
	if not check(CityStore.dispatch(&"body_add_gear", [id, 0, middle, _card("roof", "spire")]).get("ok", false), "a spire to size"):
		return
	var placed: int = BuildingProbe.info(id).digest
	var hit: Dictionary = CityStore.query(&"body_pick", [middle + Vector3.UP * ABOVE_M, Vector3.DOWN])
	check(hit.get("id", -1) == id and hit.get("gear", -1) == 0, "a ray down onto the spire picks it: %s" % hit)
	_typed(id, placed, "spire height", &"body_size_gear", [id, 0, 0, GEAR_HEIGHT_M], func(f: Dictionary) -> bool:
		return absf(f.gear.h - GEAR_HEIGHT_M) < SLACK_M, 0, -1, 0)
	BuildingProbe.back_to(made, id, check, "the spire")


## A copy beside a building with room beside it (a building hemmed in is refused and the city stays as it was): a new record with
## the same body, its lot apart from the source's; undo takes it away.
func _duplicate() -> void:
	var count: int = CityStore.query(&"list_buildings").size()
	var r := {}
	var id := -1
	var refused := 0
	var lot_tried := false
	for each: int in CityStore.query(&"list_buildings"):
		if BuildingProbe.info(each).volumes.is_empty(): # an empty lot is no building to copy: the first one is tried
			if not lot_tried:
				lot_tried = true
				var lot := _hash()
				var none: Dictionary = CityStore.dispatch(&"building_duplicate", [each])
				check(none.get("error", "") == "building.problem.noMass" and _hash() == lot, "an empty lot is not copied: %s" % none)
			continue
		var before := _hash()
		r = CityStore.dispatch(&"building_duplicate", [each])
		if r.get("ok", false):
			id = each
			break
		refused += 1
		if refused == 1:
			check(_hash() == before and r.get("error", "") != "", "a building with no room beside it is refused, the city as it was: %s" % r)
	if not check(id >= 0 and r.get("placed", -1) >= 0, "a building copied beside itself: %s" % r):
		return
	var copy: int = r.placed
	check(copy != id and CityStore.query(&"list_buildings").size() == count + 1, "the copy is a building of its own")
	check(BuildingProbe.info(copy).digest == BuildingProbe.info(id).digest, "the copy has the same body")
	check(BuildingProbe.block_middle(copy).distance_to(BuildingProbe.block_middle(id)) > SLACK_M, "the copy stands apart")
	check(CityStore.dispatch(&"undo") and CityStore.query(&"list_buildings").size() == count, "undo takes the copy away")


## A card of the Build strip (data/ui/builder.json) by its tab and id.
func _card(tab: String, id: String) -> Dictionary:
	for s: Dictionary in DataTables.ui("builder").tabs[tab].sections:
		for t: Dictionary in s.get("tools", []):
			if t.get("id", "") == id:
				return t
	return {}
