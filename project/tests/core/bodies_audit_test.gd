## The street's pieces as bodies standing in the world (probe `bodies_audit`): in the generated town, whichever segment or junction
## placed a piece, no two stand in one another nor nearer than the hand's gap two pieces of the street keep: no hydrant against a
## signal post, no bench against a pole. Each pair of kinds found is printed with its first case (kind, segment, arc, place across,
## where). Run: godot --path project res://tests/core/bodies_audit_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEEDS := [7, 1] # the town of the walkers' corner (node 112) and the town of the T joint's photos
## FurnitureKind (sim/catalog/furniture_types.h), for the cases' print.
const KINDS := ["signal", "lamp", "hydrant", "bin", "bench", "tree", "column", "postbox", "phone", "drain", "shrub", "pole", "sign"]


func _ready() -> void:
	if not boot():
		finish()
		return
	for town: int in SEEDS:
		new_map(TOWN_MAP, town, true)
		var audit: Dictionary = CityStore.query(&"bodies_audit")
		print("town seed %d: %d bodies, %d pairs nearer than %.2f m (%d of them in one another)" % [town, audit.bodies, audit.near, audit.gap_m, audit.inside])
		for kinds: Vector2i in audit.by_kinds:
			print("  %s and %s: %d pairs, the first %s" % [KINDS[kinds.x], KINDS[kinds.y], audit.by_kinds[kinds], audit.cases[kinds]])
		check(int(audit.bodies) > 0, "seed %d: the town's streets have pieces (%d)" % [town, audit.bodies])
		check(int(audit.inside) == 0, "seed %d: no two pieces of the street stand in one another: %d pairs" % [town, audit.inside])
		check(int(audit.near) == 0, "seed %d: no two pieces of the street stand nearer than the gap they keep (%.2f m): %d pairs, by kinds %s" % [
				town, audit.gap_m, audit.near, audit.by_kinds])
	finish()
