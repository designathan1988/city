## What the Build tab does to a building, in the sorting of edit_fuzz_test.gd: a block dragged, a core, or a piece on its ground floor.

const BLOCK_FROM := 0.3
const BLOCK_TO := 0.7
const KINDS := 2
const LAST_KIND := 2


static func run(rng: RandomNumberGenerator, id: int, box: PackedVector3Array) -> void:
	var far: Vector3 = box[box.size() / 4]
	var mid: Vector3 = box[0].lerp(far, 0.5)
	match rng.randi_range(0, LAST_KIND):
		0:
			CityStore.dispatch(&"body_add_mass", [id, 0, box[0].lerp(far, BLOCK_FROM), box[0].lerp(far, BLOCK_TO), rng.randi_range(0, KINDS)])
		1:
			CityStore.dispatch(&"body_add_core", [id, mid, rng.randi_range(0, KINDS)])
		_:
			CityStore.dispatch(&"body_put_piece", [id, 0, "sofa", mid, 0.0])
