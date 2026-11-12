## The street's pieces as bodies standing in the world (probe `bodies_audit`): in the generated town and in the 24 km city, whichever
## segment or junction placed a piece, no two stand in one another nor nearer than the hand's gap two pieces of the street keep (no
## hydrant against a signal post, no bench against a pole), and in volume no part of one passes through a part of another (a signal's
## arm through a pole). Each pair of kinds found is printed with its first case (kind, segment, arc, place across, where).
## Run: godot --path project res://tests/core/bodies_audit_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEEDS := [7, 1] # the town of the walkers' corner (node 112) and the town of the T joint's photos
const BIG_MAP := 4 # maps.json km24: the 24 km city
const BIG_SEED := 1
const THROUGH := 0.0 # m between the parts of two pieces: none, one through the other
const WIDE_M := 1.0 # a clearance no street keeps between every two of its pieces: the audit must find pairs under it
const ARM_M := 3.0 # a signal's arm reaches further than this from its post (data/render/streets.json props.signal)
## FurnitureKind (sim/catalog/furniture_types.h), for the cases' print.
const KINDS := ["signal", "lamp", "hydrant", "bin", "bench", "tree", "column", "postbox", "phone", "drain", "shrub", "pole", "sign"]


func _ready() -> void:
	if not boot():
		finish()
		return
	for town: int in SEEDS:
		_audit(TOWN_MAP, town, true)
	_audit(BIG_MAP, BIG_SEED, false)
	finish()


## One city's pieces audited on the ground and in volume; `wide`: the audit asked again with a clearance no street keeps.
func _audit(map: int, town: int, wide: bool) -> void:
	var city := "map %d seed %d" % [map, town]
	new_map(map, town, true)
	var audit: Dictionary = CityStore.query(&"bodies_audit", [THROUGH])
	print("%s: %d bodies, %d pairs nearer than %.2f m (%d of them in one another); %d parts, reaching %.2f m from a piece's foot" % [
			city, audit.bodies, audit.near, audit.gap_m, audit.inside, audit.boxes, audit.reach_m])
	for kinds: Vector2i in audit.by_kinds:
		print("  %s and %s: %d pairs, the first %s" % [KINDS[kinds.x], KINDS[kinds.y], audit.by_kinds[kinds], audit.cases[kinds]])
	check(int(audit.bodies) > 0, "%s: the streets have pieces (%d)" % [city, audit.bodies])
	check(int(audit.inside) == 0, "%s: no two pieces of the street stand in one another: %d pairs" % [city, audit.inside])
	check(int(audit.near) == 0, "%s: no two pieces of the street stand nearer than the gap they keep (%.2f m): %d pairs, by kinds %s" % [
			city, audit.gap_m, audit.near, audit.by_kinds])
	# what a piece carries over its foot counts too: a signal's arm and heads, a pole's arm, lamp and can, each part a box
	for kinds: Vector2i in audit.volume_by_kinds:
		print("  in volume, %s and %s: %d pairs, the first %s" % [KINDS[kinds.x], KINDS[kinds.y], audit.volume_by_kinds[kinds], audit.volume_cases[kinds]])
	check(int(audit.volume) == 0, "%s: no part of a piece of the street passes through a part of another: %d pairs, by kinds %s" % [
			city, audit.volume, audit.volume_by_kinds])
	check(int(audit.boxes) > int(audit.bodies) and float(audit.reach_m) > ARM_M, "%s: the pieces stand in volume, a signal's arm among their parts (%d parts, %.2f m)" % [
			city, audit.boxes, audit.reach_m])
	if not wide:
		return
	# the audit sees what stands near in volume: asked a clearance no street keeps, it names pairs
	var far: Dictionary = CityStore.query(&"bodies_audit", [WIDE_M])
	check(int(far.volume) > 0, "%s: asked %.1f m between the parts of two pieces, the audit finds pairs: %d, by kinds %s" % [
			city, WIDE_M, far.volume, far.volume_by_kinds])
