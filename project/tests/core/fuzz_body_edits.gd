## What the Build tab does to a building, in the sorting of edit_fuzz_test.gd: a block dragged, a core, or a piece on its ground floor.

const BLOCK_FROM := 0.3
const BLOCK_TO := 0.7
## The body edits the fuzz draws from (PIECE: anything past the others), the Selection panel's and the whole building's among them.
enum BodyEdit { MASS, CORE, RUN, TURN, BEVEL, COPY, PRIMITIVE, DETACH, DROP, LIFT, MIRROR, UNION, LEVEL_HEIGHT, BLOCK_SIZE, BLOCK_OFFSET,
	BLOCK_BASE, DUPLICATE, TURN_WHOLE, FUNCTION, RING, SKETCH, PIECE }
const TURN_DEG := 15.0
const OUT_M := 1.0
const DROP_OUT := 4.0
const LEVEL_H_M := 3.6 # a level's own height inside the storey limits of tuning.json (build_min_storey_m .. build_max_storey_m)
const BLOCK_W_M := 9.0 # a block's typed width inside build_min_side_m .. build_max_side_m
const FUNCTIONS := 4 # the functions drawn from: the first of BuildingFunction (sim/buildings/b40/types.h)


static func run(rng: RandomNumberGenerator, id: int, box: PackedVector3Array) -> void:
	var far: Vector3 = box[box.size() / 4]
	var mid: Vector3 = box[0].lerp(far, 0.5)
	var enums: Dictionary = CityStore.constants.enums
	match rng.randi_range(0, BodyEdit.PIECE):
		BodyEdit.MASS:
			CityStore.dispatch(&"body_add_mass", [id, 0, box[0].lerp(far, BLOCK_FROM), box[0].lerp(far, BLOCK_TO), rng.randi_range(0, enums.MassAction.size() - 1)])
		BodyEdit.CORE:
			CityStore.dispatch(&"body_add_core", [id, mid, rng.randi_range(0, enums.CoreKind.size() - 1)])
		BodyEdit.TURN:
			CityStore.dispatch(&"body_turn_block", [id, 0, TURN_DEG])
		BodyEdit.BEVEL:
			CityStore.dispatch(&"body_bevel", [id, 0, OUT_M, CityStore.constants.every_corner])
		BodyEdit.COPY:
			CityStore.dispatch(&"body_copy_block", [id, 0])
		BodyEdit.PRIMITIVE:
			CityStore.dispatch(&"body_add_primitive", [id, 0, -1, 0, mid, _card("draw", "primCross")])
		BodyEdit.DETACH:
			CityStore.dispatch(&"body_detach", [id, int(CityStore.query(&"body_info", [id]).volumes.size()) - 1])
		BodyEdit.LIFT:
			CityStore.dispatch(&"body_apply", [id, {"op": "lift", "vol": 0, "along": OUT_M, "snap": false}])
		BodyEdit.DROP:
			CityStore.dispatch(&"drop_primitive", [mid + (mid - box[0]) * DROP_OUT, _card("draw", "primCross")])
		BodyEdit.RUN:
			CityStore.dispatch(&"body_add_run", [id, 0, 0, PackedVector3Array([box[0], far]), _card("parts", "wallRun")])
		BodyEdit.MIRROR:
			CityStore.dispatch(&"body_mirror", [id])
		BodyEdit.UNION:
			CityStore.dispatch(&"body_union", [id, 0])
		BodyEdit.LEVEL_HEIGHT:
			CityStore.dispatch(&"body_level_height", [id, rng.randi_range(0, 2), LEVEL_H_M])
		BodyEdit.BLOCK_SIZE:
			CityStore.dispatch(&"body_block_size", [id, 0, rng.randi_range(0, 1), BLOCK_W_M])
		BodyEdit.BLOCK_OFFSET:
			CityStore.dispatch(&"body_block_offset", [id, int(CityStore.query(&"body_info", [id]).volumes.size()) - 1, rng.randi_range(0, 1), OUT_M])
		BodyEdit.BLOCK_BASE:
			CityStore.dispatch(&"body_block_base", [id, int(CityStore.query(&"body_info", [id]).volumes.size()) - 1, 1])
		BodyEdit.DUPLICATE:
			CityStore.dispatch(&"building_duplicate", [id])
		BodyEdit.TURN_WHOLE:
			CityStore.dispatch(&"building_turn", [id, deg_to_rad(TURN_DEG)])
		BodyEdit.FUNCTION:
			CityStore.dispatch(&"building_function", [id, rng.randi_range(0, FUNCTIONS - 1)])
		BodyEdit.RING:
			CityStore.dispatch(&"body_add_mass_ring", [id, 0, PackedVector3Array([box[0], box[0].lerp(far, BLOCK_TO), mid]), rng.randi_range(0, enums.MassAction.size() - 1)])
		BodyEdit.SKETCH:
			var out := (mid - box[0]) * DROP_OUT
			CityStore.dispatch(&"place_sketch", [PackedVector3Array([box[0] + out, far + out, mid + out * 2.0])])
		_:
			CityStore.dispatch(&"body_put_piece", [id, 0, "sofa", mid, 0.0])


## A card of the Build strip (data/ui/builder.json) by its tab and id.
static func _card(tab: String, id: String) -> Dictionary:
	for s: Dictionary in DataTables.ui("builder").tabs[tab].sections:
		for t: Dictionary in s.get("tools", []):
			if t.get("id", "") == id:
				return t
	return {}
