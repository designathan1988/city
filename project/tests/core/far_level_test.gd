## The buildings' far level holds what its nearest distance shows and no more (probe far_form: the far level of the whole town, built
## on the spot as its jobs build it): no face narrower than the error allowed where that level begins, no face seen only from below,
## what stands on the roofs as a box each and only those wide enough to be still drawn there, and its arrays counted for the frame's
## upload budget as they are sent.
extends Node

const TOWN_MAP := 0
const SEED := 1


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	await get_tree().process_frame # the levels know their map once the first frame drew it
	await get_tree().process_frame
	var far: Dictionary = CityStore.query(&"far_form")
	print("far: ", far)
	var failures := 0
	failures += _check(int(far.get("buildings", 0)) > 0 and int(far.get("triangles", 0)) > 0,
		"the town has a far level: %d buildings, %d triangles" % [far.get("buildings", 0), far.get("triangles", 0)])
	failures += _check(int(far.get("thin_quads", -1)) == 0,
		"no face narrower than the %.2f m the far level may be off where it begins: %d found" % [far.get("error_m", 0.0), far.get("thin_quads", -1)])
	failures += _check(int(far.get("down_faces", -1)) == 0, "no face seen only from below: %d found" % far.get("down_faces", -1))
	failures += _check(int(far.get("roof_boxes", 0)) > 0 and int(far.get("boxes_left", 0)) > 0,
		"what stands on the roofs is a box each, held when a part %.2f m wide or more is still drawn there: %d held, %d left out"
		% [far.get("part_least_m", 0.0), far.get("roof_boxes", 0), far.get("boxes_left", 0)])
	failures += _check(int(far.get("bytes", 0)) == int(far.get("sent_bytes", -1)),
		"its arrays are counted as they are sent: %d bytes counted, %d sent" % [far.get("bytes", 0), far.get("sent_bytes", -1)])
	print("FAILURES: ", failures)
	get_tree().quit(0 if failures == 0 else 1)


func _check(good: bool, what: String) -> int:
	print(("ok   " if good else "FAIL ") + what)
	return 0 if good else 1
