## Zones > Lots by the store, in road_commands_test: a lot drawn, zoned, cut, joined, its front changed, a road in its way refused, and undo and
## the save holding all of it.
class_name LotCases
extends RefCounted

const SIDE_M := 40.0
const ZONE := 1
const CUT_ACROSS := 0

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
