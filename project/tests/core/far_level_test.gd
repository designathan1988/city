## The buildings' far level holds what its nearest distance shows and no more (probe far_form: the far level of the whole town, built
## on the spot as its jobs build it): no face narrower than the error allowed where that level begins (a lot's cover is held piece by
## piece, whatever a piece's own width), no face seen only from below, what stands on the roofs and the lots as a box each and only
## those wide enough to be still drawn there, and its arrays counted for the frame's upload budget as they are sent.
extends TestCase

const TOWN_MAP := 0
const SEED := 1


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	await get_tree().process_frame # the levels know their map once the first frame drew it
	await get_tree().process_frame
	var far: Dictionary = CityStore.query(&"far_form")
	print("far: ", far)
	check(int(far.get("buildings", 0)) > 0 and int(far.get("triangles", 0)) > 0,
		"the town has a far level: %d buildings, %d triangles" % [far.get("buildings", 0), far.get("triangles", 0)])
	check(int(far.get("thin_quads", -1)) == 0,
		"no face narrower than the %.2f m the far level may be off where it begins: %d found" % [far.get("error_m", 0.0), far.get("thin_quads", -1)])
	check(int(far.get("down_faces", -1)) == 0, "no face seen only from below: %d found" % far.get("down_faces", -1))
	check(int(far.get("roof_boxes", 0)) > 0 and int(far.get("boxes_left", 0)) > 0,
		"what stands on the roofs and the lots is a box each, held when a part %.2f m wide or more is still drawn there: %d held, %d left out"
		% [far.get("part_least_m", 0.0), far.get("roof_boxes", 0), far.get("boxes_left", 0)])
	check(int(far.get("bytes", 0)) == int(far.get("sent_bytes", -1)),
		"its arrays are counted as they are sent: %d bytes counted, %d sent" % [far.get("bytes", 0), far.get("sent_bytes", -1)])
	var cast := int(far.get("cast_triangles", 0))
	check(cast > 0 and cast < int(far.get("triangles", 0)),
		"what it throws in the sun is its blocks' sides and roofs alone, fewer triangles than it draws: %d of %d" % [cast, far.get("triangles", 0)])
	finish()
