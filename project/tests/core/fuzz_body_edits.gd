## What the Build tab does to a building, in the sorting of edit_fuzz_test.gd: a block dragged, a core, or a piece on its ground floor.

const BLOCK_FROM := 0.3
const BLOCK_TO := 0.7
## The body edits the fuzz draws from (PIECE: anything past the others).
enum BodyEdit { MASS, CORE, RUN, TURN, BEVEL, COPY, PRIMITIVE, DETACH, DROP, LIFT, PIECE }
const TURN_DEG := 15.0
const OUT_M := 1.0
const DROP_OUT := 4.0


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
		_:
			CityStore.dispatch(&"body_put_piece", [id, 0, "sofa", mid, 0.0])


## A card of the Build strip (data/ui/builder.json) by its tab and id.
static func _card(tab: String, id: String) -> Dictionary:
	for s: Dictionary in DataTables.ui("builder").tabs[tab].sections:
		for t: Dictionary in s.get("tools", []):
			if t.get("id", "") == id:
				return t
	return {}
