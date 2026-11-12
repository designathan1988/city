## The street's pieces as bodies standing in the world (probe `bodies_audit`): in the generated town, whichever segment or junction
## placed a piece, no two stand in one another nor nearer than the hand's gap two pieces of the street keep: no hydrant against a
## signal post, no bench against a pole. Each pair of kinds found is printed with its first case (kind, segment, arc, place across,
## where). Run: godot --path project res://tests/core/bodies_audit_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEEDS := [7, 1] # the town of the walkers' corner (node 112) and the town of the T joint's photos
## FurnitureKind (sim/catalog/furniture_types.h), for the cases' print.
const THROUGH := 0.0 # m between the parts of two pieces: none, one through the other
const WIDE_M := 1.0 # a clearance no street keeps between every two of its pieces: the audit must find pairs under it
const ARM_M := 3.0 # a signal's arm reaches further than this from its post (data/render/streets.json props.signal)
const KINDS := ["signal", "lamp", "hydrant", "bin", "bench", "tree", "column", "postbox", "phone", "drain", "shrub", "pole", "sign"]


func _ready() -> void:
	if not boot():
		finish()
		return
	for town: int in SEEDS:
		new_map(TOWN_MAP, town, true)
		var audit: Dictionary = CityStore.query(&"bodies_audit", [THROUGH])
		print("town seed %d: %d bodies, %d pairs nearer than %.2f m (%d of them in one another); %d parts, reaching %.2f m from a piece's foot" % [
				town, audit.bodies, audit.near, audit.gap_m, audit.inside, audit.boxes, audit.reach_m])
		for kinds: Vector2i in audit.by_kinds:
			print("  %s and %s: %d pairs, the first %s" % [KINDS[kinds.x], KINDS[kinds.y], audit.by_kinds[kinds], audit.cases[kinds]])
		check(int(audit.bodies) > 0, "seed %d: the town's streets have pieces (%d)" % [town, audit.bodies])
		check(int(audit.inside) == 0, "seed %d: no two pieces of the street stand in one another: %d pairs" % [town, audit.inside])
		check(int(audit.near) == 0, "seed %d: no two pieces of the street stand nearer than the gap they keep (%.2f m): %d pairs, by kinds %s" % [
				town, audit.gap_m, audit.near, audit.by_kinds])
		# what a piece carries over its foot counts too: a signal's arm and heads, a pole's arm, lamp and can, each part a box
		for kinds: Vector2i in audit.volume_by_kinds:
			print("  in volume, %s and %s: %d pairs, the first %s" % [KINDS[kinds.x], KINDS[kinds.y], audit.volume_by_kinds[kinds], audit.volume_cases[kinds]])
		check(int(audit.volume) == 0, "seed %d: no part of a piece of the street passes through a part of another: %d pairs, by kinds %s" % [
				town, audit.volume, audit.volume_by_kinds])
		check(int(audit.boxes) > int(audit.bodies) and float(audit.reach_m) > ARM_M, "seed %d: the pieces stand in volume, a signal's arm among their parts (%d parts, %.2f m)" % [
				town, audit.boxes, audit.reach_m])
		# the audit sees what stands near in volume: asked a clearance no street keeps, it names pairs
		var wide: Dictionary = CityStore.query(&"bodies_audit", [WIDE_M])
		check(int(wide.volume) > 0, "seed %d: asked %.1f m between the parts of two pieces, the audit finds pairs: %d, by kinds %s" % [
				town, WIDE_M, wide.volume, wide.volume_by_kinds])
	finish()
