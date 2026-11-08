## Zones > Lots by the store, in road_commands_test: a lot drawn, zoned, cut, joined, its front changed, a road in its way refused, and undo and
## the save holding all of it; a pentagon and a curved lot zoned and grown, each building's ground blocks inside its lot.
class_name LotCases
extends RefCounted

const SIDE_M := 40.0
const ZONE := 1
const CUT_ACROSS := 0
const ROAD_OFF_M := 30.0
const ROAD_PROFILE := 0
const ROAD_PX := 0.25
const SNAP_FROM_M := 9.0
const SNAP_REACH_M := 6.0
## The fitted lots: how far from the other cases, a pentagon's corners (metres from its first) and a square whose back bends in.
const FIT_OFF_M := 100.0
const PENTAGON: Array[Vector3] = [Vector3(0, 0, 0), Vector3(32, 0, 0), Vector3(32, 0, 18), Vector3(14, 0, 30), Vector3(0, 0, 26)]
const CURVED_OFF := Vector3(50, 0, 0)
const CURVED_SIDE_M := 30.0
const CURVE_IN_M := 9.0
const PENTAGON_ZONE := 6 # dense houses: a tower's block asks the most of a lot
const CURVED_ZONE := 2
## Growth is ticked in rounds until both lots stand built, at most so many.
const FIT_TICKS := 200
const FIT_ROUNDS := 30
const BLOCK_CORNERS := 4 # the probe's footprint gives a block so many corners (b40::BLOCK_CORNERS)

var t: Node # the test (TestCase): its check and ticks


func _init(test: Node) -> void:
	t = test


func run(at: Vector3) -> void:
	var before: int = CityStore.query(&"lots").size()
	var r: Dictionary = CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M), 0.0])
	t.check(r.get("ok", false), "lot: drawn (%s)" % r.get("error", ""))
	var mid := at + Vector3(SIDE_M * 0.5, 0, SIDE_M * 0.5)
	var id: int = CityStore.query(&"lot_at", [mid])
	t.check(id != 0, "lot: the point falls in it")
	t.check(not CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M), 0.0]).get("ok", false), "lot: refuses another on top")
	t.check(CityStore.dispatch(&"lot_zone", [id, ZONE]).get("ok", false), "lot: takes a zone")
	t.check(CityStore.dispatch(&"lot_split", [id, CUT_ACROSS, 2, mid, mid]).get("ok", false), "lot: divided")
	var lots: Array = CityStore.query(&"lots")
	t.check(lots.size() == before + 2, "lot: two after the cut (%d)" % (lots.size() - before))
	t.check(int(lots[-1].zone) == ZONE, "lot: the cut keeps the zone")
	var r2: Dictionary = CityStore.dispatch(&"lot_join", [lots[-2].id, lots[-1].id])
	t.check(r2.get("ok", false), "lot: joined (%s)" % r2.get("error", ""))
	t.check(CityStore.query(&"lots").size() == before + 1, "lot: one after joining")
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"lots").size() == before + 2, "lot: undo brings the cut back")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"lots").size() == before, "lot: undo takes it all")
	_against_a_road(at, before)
	_fitted(at + Vector3(0, 0, -FIT_OFF_M))


## The street: a lot drawn over one is cut back to the land, a road laid across one cuts it in two in its own step of undo, and a point
## near a footway lands on it.
func _against_a_road(at: Vector3, before: int) -> void:
	var a := at + Vector3(-ROAD_OFF_M, 0, SIDE_M * 0.5)
	var b := at + Vector3(SIDE_M + ROAD_OFF_M, 0, SIDE_M * 0.5)
	CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M), 0.0])
	var whole: Array = CityStore.query(&"lots")
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, ROAD_PROFILE, ROAD_PX])
	t.check(r.get("ok", false), "lot: a road over it (%s)" % r.get("error", ""))
	var cut: Array = CityStore.query(&"lots")
	t.check(cut.size() == before + 2, "lot: the road cuts it in two (%d)" % (cut.size() - before))
	var mid := at + Vector3(SIDE_M * 0.5, 0, SIDE_M * 0.5)
	t.check(int(CityStore.query(&"lot_at", [mid])) == 0, "lot: none on the road")
	var near: Vector3 = at + Vector3(SIDE_M * 0.5, 0, SIDE_M * 0.5 - SNAP_FROM_M)
	var snapped: Vector3 = CityStore.query(&"lot_snap_point", [near, SNAP_REACH_M, Vector3.INF])
	t.check(not snapped.is_equal_approx(near), "lot: the point near the road goes to the pavement (%s)" % snapped)
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"lots").size() == whole.size(), "lot: undoing the road brings the whole lot back")
	CityStore.dispatch(&"undo")
	t.check(CityStore.query(&"lots").size() == before, "lot: undo takes the lot")
	CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, ROAD_PROFILE, ROAD_PX])
	var over: Dictionary = CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M * 0.45), 0.0])
	t.check(over.get("ok", false), "lot: drawn up to the road (%s)" % over.get("error", ""))
	var drawn: Array = CityStore.query(&"lots")
	var deepest := -INF
	for q: Vector3 in drawn[-1].points:
		deepest = maxf(deepest, q.z)
	t.check(deepest < at.z + SIDE_M * 0.45, "lot: cut back to the pavement (%.1f)" % deepest)
	var offered: Array = CityStore.query(&"lot_proposals", [(a + b) * 0.5, 0])
	t.check(not offered.is_empty(), "lot: the road proposes lots (%d)" % offered.size())
	var taken: Dictionary = CityStore.dispatch(&"lot_add_polygon", [offered[0]])
	t.check(taken.get("ok", false), "lot: the proposal becomes a lot (%s)" % taken.get("error", ""))
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")


## A lot of any shape is built on: a pentagon and a square whose back bends in, zoned and left to grow, each gets a building whose solid
## ground blocks (the probe's footprint, drawn in by the fit's inset) all lie inside its ring.
func _fitted(o: Vector3) -> void:
	var pentagon := PackedVector3Array()
	for p: Vector3 in PENTAGON:
		pentagon.push_back(o + p)
	var made: Dictionary = CityStore.dispatch(&"lot_add_polygon", [pentagon])
	t.check(made.get("ok", false), "fit: the pentagon drawn (%s)" % made.get("error", ""))
	var c := o + CURVED_OFF
	made = CityStore.dispatch(&"lot_add_rect", [c, c + Vector3(CURVED_SIDE_M, 0, CURVED_SIDE_M), 0.0])
	t.check(made.get("ok", false), "fit: the square drawn (%s)" % made.get("error", ""))
	var square: int = CityStore.query(&"lot_at", [c + Vector3(CURVED_SIDE_M, 0, CURVED_SIDE_M) * 0.5])
	var ring: PackedVector3Array = CityStore.query(&"lot_ring", [square])
	var back_a: Vector3 = ring[2]
	var back_b: Vector3 = ring[3]
	var middle := (ring[0] + ring[1] + ring[2] + ring[3]) * 0.25
	var through := (back_a + back_b) * 0.5
	through += (middle - through).normalized() * CURVE_IN_M
	var bent: Dictionary = CityStore.dispatch(&"lot_curve_side", [back_a, back_b, through])
	t.check(bent.get("ok", false), "fit: the square's back bent in (%s)" % bent.get("error", ""))
	var pent: int = CityStore.query(&"lot_at", [o + (PENTAGON[1] + PENTAGON[3]) * 0.5])
	t.check(pent != 0 and square != 0, "fit: both lots found (%d, %d)" % [pent, square])
	t.check(CityStore.dispatch(&"lot_zone", [pent, PENTAGON_ZONE]).get("ok", false), "fit: the pentagon zoned")
	t.check(CityStore.dispatch(&"lot_zone", [square, CURVED_ZONE]).get("ok", false), "fit: the curved lot zoned")
	var rounds := 0
	while rounds < FIT_ROUNDS and (_built_on(pent).is_empty() or _built_on(square).is_empty()):
		t.ticks(FIT_TICKS)
		rounds += 1
	for id: int in [pent, square]:
		var lot := _flat(CityStore.query(&"lot_ring", [id]))
		var standing := _built_on(id)
		t.check(not standing.is_empty(), "fit: lot %d of %d corners is built on (%d rounds)" % [id, lot.size(), rounds])
		for b: int in standing:
			var foot: PackedVector3Array = CityStore.query(&"probe_building", [b]).footprint
			t.check(foot.size() >= BLOCK_CORNERS, "fit: building %d stands on blocks (%d corners)" % [b, foot.size()])
			for k in range(0, foot.size() - BLOCK_CORNERS + 1, BLOCK_CORNERS):
				var block := _flat(foot.slice(k, k + BLOCK_CORNERS))
				t.check(Geometry2D.clip_polygons(block, lot).is_empty(), "fit: building %d's block %d lies inside lot %d" % [b, k / BLOCK_CORNERS, id])


## The buildings whose middle stands in the player's lot `id`.
func _built_on(id: int) -> Array[int]:
	var out: Array[int] = []
	for b: int in CityStore.query(&"list_buildings"):
		var at: Vector3 = CityStore.query(&"probe_building", [b]).position
		if int(CityStore.query(&"lot_at", [at])) == id:
			out.push_back(b)
	return out


## Points on the ground as the plane's (x, z).
func _flat(points: PackedVector3Array) -> PackedVector2Array:
	var out := PackedVector2Array()
	for p: Vector3 in points:
		out.push_back(Vector2(p.x, p.z))
	return out
