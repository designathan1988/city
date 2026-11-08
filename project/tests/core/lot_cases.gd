## Zones > Lots by the store, in road_commands_test: a lot drawn, zoned, cut, joined, its front changed, a road in its way refused, and undo and
## the save holding all of it.
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
