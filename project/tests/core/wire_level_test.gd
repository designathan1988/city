## The wires of the streets' lines stand wherever their poles stand, at one level, chosen by the core: for every furniture chunk
## drawn (probe wire_draws) that holds spans, exactly one of its two wire levels is told drawn, from a street, from its other side,
## from the height the game opens at and from far above; tubes in the chunk a street view stands in, the coarse level from far
## above; and its poles' shadows never at two levels at once. A chunk whose wires are left to the engine to hide (both levels told
## drawn, or neither) is a pole with no wires from some standpoint.
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 60
const SETTLE_LIMIT_S := 60.0 # real time the mesh jobs may take: StateWait
const CHUNK_M := 256.0 # a furniture chunk's side (native/src/core/grid.h CHUNK)
## A signalled T of the town with a line on every arm: where the owner's photo showed poles with no wires.
const JOINT := Vector2(540.0, -468.0)
## The standpoints: [name, yaw, pitch (degrees), distance from the point looked at (m)].
const STREET := ["a street", 30.0, 35.0, 62.0]
const STREET_BACK := ["the street's other side", 210.0, 35.0, 62.0]
const OPENING := ["the height the game opens at", 35.0, 48.0, 793.0]
const HIGH := ["far above", 0.0, 60.0, 2000.0]


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	var joint := Vector3(JOINT.x, float(CityStore.query(&"height_at", [JOINT.x, JOINT.y])), JOINT.y)
	for stand: Array in [STREET, STREET_BACK, OPENING, HIGH]:
		rig.jump(joint, stand[1], stand[2], stand[3])
		await _settled()
		_check_levels(stand[0])
	rig.jump(joint, STREET[1], STREET[2], STREET[3])
	await _settled()
	var under := _chunk_at(joint)
	check(not under.is_empty() and int(under.spans) > 0, "the chunk the joint stands in is drawn with its spans: %s" % under)
	check(not under.is_empty() and bool(under.near) and not bool(under.far), "from a street the chunk under the view draws its wires as tubes: %s" % under)
	rig.jump(joint, HIGH[1], HIGH[2], HIGH[3])
	await _settled()
	var tubes := 0
	for row: Dictionary in CityStore.query(&"wire_draws"):
		if int(row.spans) > 0 and bool(row.near):
			tubes += 1
	check(tubes == 0, "from far above no chunk draws tubes: %d do" % tubes)
	finish()


## Every chunk holding spans draws its wires at exactly one level, and no chunk its poles' shadows at two.
func _check_levels(from: String) -> void:
	var with_spans := 0
	var two := 0
	var none := 0
	var two_shadows := 0
	var first := {}
	for row: Dictionary in CityStore.query(&"wire_draws"):
		if bool(row.shadow_near) and bool(row.shadow_far):
			two_shadows += 1
		if int(row.spans) == 0:
			continue
		with_spans += 1
		var levels := int(bool(row.near)) + int(bool(row.far))
		if levels == 2:
			two += 1
		elif levels == 0:
			none += 1
		if levels != 1 and first.is_empty():
			first = row
	print("  from %s: %d chunks with spans, %d at two levels, %d at none, %d with two shadow levels" % [from, with_spans, two, none, two_shadows])
	check(with_spans > 0, "from %s the town's lines are in the furniture drawn: %d chunks with spans" % [from, with_spans])
	check(two == 0 and none == 0, "from %s every chunk with spans draws its wires at exactly one level: %d at two, %d at none (first %s)" % [from, two, none, first])
	check(two_shadows == 0, "from %s no chunk casts its poles' shadows at two levels: %d do" % [from, two_shadows])


## The drawn chunk a ground point stands in, or {}.
func _chunk_at(p: Vector3) -> Dictionary:
	var cx := int(floor(p.x / CHUNK_M))
	var cz := int(floor(p.z / CHUNK_M))
	for row: Dictionary in CityStore.query(&"wire_draws"):
		if int(row.x) == cx and int(row.z) == cz:
			return row
	return {}


func _settled() -> void:
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	await StateWait.drawn(get_tree(), SETTLE_LIMIT_S)
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
