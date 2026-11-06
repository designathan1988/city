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

var _check: Callable


func _init(check: Callable) -> void:
	_check = check


func run(at: Vector3) -> void:
	var before: int = CityStore.query(&"lots").size()
	var r: Dictionary = CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M), 0.0])
	_check.call(r.get("ok", false), "lote: desenhado (%s)" % r.get("error", ""))
	var mid := at + Vector3(SIDE_M * 0.5, 0, SIDE_M * 0.5)
	var id: int = CityStore.query(&"lot_at", [mid])
	_check.call(id != 0, "lote: o ponto cai nele")
	_check.call(not CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M), 0.0]).get("ok", false), "lote: recusa outro em cima")
	_check.call(CityStore.dispatch(&"lot_zone", [id, ZONE]).get("ok", false), "lote: recebe zona")
	_check.call(CityStore.dispatch(&"lot_split", [id, CUT_ACROSS, 2, mid, mid]).get("ok", false), "lote: dividido")
	var lots: Array = CityStore.query(&"lots")
	_check.call(lots.size() == before + 2, "lote: dois depois do corte (%d)" % (lots.size() - before))
	_check.call(int(lots[-1].zone) == ZONE, "lote: o corte guarda a zona")
	var r2: Dictionary = CityStore.dispatch(&"lot_join", [lots[-2].id, lots[-1].id])
	_check.call(r2.get("ok", false), "lote: juntados (%s)" % r2.get("error", ""))
	_check.call(CityStore.query(&"lots").size() == before + 1, "lote: um depois de juntar")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"lots").size() == before + 2, "lote: desfazer volta o corte")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"lots").size() == before, "lote: desfazer leva tudo")
	_against_a_road(at, before)


## The street: a lot drawn over one is cut back to the land, a road laid across one cuts it in two in its own step of undo, and a point
## near a footway lands on it.
func _against_a_road(at: Vector3, before: int) -> void:
	var a := at + Vector3(-ROAD_OFF_M, 0, SIDE_M * 0.5)
	var b := at + Vector3(SIDE_M + ROAD_OFF_M, 0, SIDE_M * 0.5)
	CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M), 0.0])
	var whole: Array = CityStore.query(&"lots")
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, ROAD_PROFILE, ROAD_PX])
	_check.call(r.get("ok", false), "lote: via sobre ele (%s)" % r.get("error", ""))
	var cut: Array = CityStore.query(&"lots")
	_check.call(cut.size() == before + 2, "lote: a via corta em dois (%d)" % (cut.size() - before))
	var mid := at + Vector3(SIDE_M * 0.5, 0, SIDE_M * 0.5)
	_check.call(int(CityStore.query(&"lot_at", [mid])) == 0, "lote: nenhum sobre a via")
	var near: Vector3 = at + Vector3(SIDE_M * 0.5, 0, SIDE_M * 0.5 - SNAP_FROM_M)
	var snapped: Vector3 = CityStore.query(&"lot_snap_point", [near, SNAP_REACH_M, Vector3.INF])
	_check.call(not snapped.is_equal_approx(near), "lote: o ponto perto da via vai à calçada (%s)" % snapped)
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"lots").size() == whole.size(), "lote: desfazer a via volta o lote inteiro")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"lots").size() == before, "lote: desfazer leva o lote")
	CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, ROAD_PROFILE, ROAD_PX])
	var over: Dictionary = CityStore.dispatch(&"lot_add_rect", [at, at + Vector3(SIDE_M, 0, SIDE_M * 0.45), 0.0])
	_check.call(over.get("ok", false), "lote: desenhado até a via (%s)" % over.get("error", ""))
	var drawn: Array = CityStore.query(&"lots")
	var deepest := -INF
	for q: Vector3 in drawn[-1].points:
		deepest = maxf(deepest, q.z)
	_check.call(deepest < at.z + SIDE_M * 0.45, "lote: cortado de volta à calçada (%.1f)" % deepest)
	var offered: Array = CityStore.query(&"lot_proposals", [(a + b) * 0.5])
	_check.call(not offered.is_empty(), "lote: a via propõe lotes (%d)" % offered.size())
	var taken: Dictionary = CityStore.dispatch(&"lot_add_polygon", [offered[0]])
	_check.call(taken.get("ok", false), "lote: a proposta vira lote (%s)" % taken.get("error", ""))
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
