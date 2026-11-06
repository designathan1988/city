## What the Build tab does to a building, in the sorting of edit_fuzz_test.gd: a block dragged, a core, or a piece on its ground floor.

const BLOCK_FROM := 0.3
const BLOCK_TO := 0.7
const KINDS := 2
const LAST_KIND := 9
const PRIMITIVE := {"value": 7, "top_kind": 0, "top_deg": 0}
const TURN_DEG := 15.0
const OUT_M := 1.0
const DROP_OUT := 4.0
const RUN := {"value": 5, "fit_kind": 1, "size": [4, 0.25, 1.8]}


static func run(rng: RandomNumberGenerator, id: int, box: PackedVector3Array) -> void:
	var far: Vector3 = box[box.size() / 4]
	var mid: Vector3 = box[0].lerp(far, 0.5)
	match rng.randi_range(0, LAST_KIND):
		0:
			CityStore.dispatch(&"body_add_mass", [id, 0, box[0].lerp(far, BLOCK_FROM), box[0].lerp(far, BLOCK_TO), rng.randi_range(0, KINDS)])
		1:
			CityStore.dispatch(&"body_add_core", [id, mid, rng.randi_range(0, KINDS)])
		3:
			CityStore.dispatch(&"body_turn_block", [id, 0, TURN_DEG])
		4:
			CityStore.dispatch(&"body_bevel", [id, 0, OUT_M, -1])
		5:
			CityStore.dispatch(&"body_copy_block", [id, 0])
		6:
			CityStore.dispatch(&"body_add_primitive", [id, 0, -1, 0, mid, PRIMITIVE])
		7:
			CityStore.dispatch(&"body_detach", [id, int(CityStore.query(&"body_info", [id]).volumes.size()) - 1])
		8:
			CityStore.dispatch(&"drop_primitive", [mid + (mid - box[0]) * DROP_OUT, PRIMITIVE])
		2:
			CityStore.dispatch(&"body_add_run", [id, 0, 0, PackedVector3Array([box[0], far]), RUN])
		_:
			CityStore.dispatch(&"body_put_piece", [id, 0, "sofa", mid, 0.0])
