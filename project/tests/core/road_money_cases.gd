## The money rules of road_commands_test, measured on the 4.0: the price paid, the share given back, the generated
## roads, undo, refusals without money and how much of a road is built each way.
class_name RoadMoneyCases
extends RefCounted

const PROFILE := 0
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const FIELD_OUT_M := 500.0
const FIELD_ROAD_M := 500.0
const WALK_M := 8.0
const WALK_STEPS := 200
const LOW_FUNDS := 1000.0
const START_FUNDS := 20000000.0
const WIDE_PROFILE := 4
const NARROW_PROFILE := 8
const RING_RADIUS := 22.0
const RING_OUT_M := 300.0
const LEN_M := 100.0
const GROUND := 0
const ELEVATED := 1
const BRIDGE := 2
const TUNNEL := 3
## Structure, height at each end, and the metres of each 100 m road (the 4.0's buildLengths; fill from 1 m, piers from 2.8 m).
const BUILD_TABLE := [
	[GROUND, 0.0, 0.0, {"ground": 100.0}],
	[GROUND, 2.0, 2.0, {"embankment": 100.0}],
	[GROUND, 3.0, 3.0, {"bridge": 100.0}],
	[GROUND, -2.0, -2.0, {"trench": 100.0}],
	[GROUND, -2.0, 2.0, {"trench": 25.0, "ground": 50.0, "embankment": 25.0}],
	[GROUND, 0.0, 4.0, {"ground": 25.0, "embankment": 45.0, "bridge": 30.0}],
	[ELEVATED, 0.0, 0.0, {"bridge": 100.0}],
	[BRIDGE, 5.0, 5.0, {"bridge": 100.0}],
	[TUNNEL, -9.0, 3.0, {"tunnel": 100.0}],
]
const KINDS := ["ground", "embankment", "trench", "bridge", "tunnel"]

var _check: Callable


func _init(check: Callable) -> void:
	_check = check


func run(b: Dictionary) -> void:
	_paying(b)
	_split_road(b)
	_generated_roads(b)
	_refusals(b)
	_build_table()


func _money() -> float:
	return CityStore.snapshot.money


func _set_money(value: float) -> void:
	CityStore.query(&"probe_set_money", [value])
	CityStore.refresh()


func _field(b: Dictionary) -> Vector3:
	return Vector3(b.max.x + FIELD_OUT_M, 0, (b.min.z + b.max.z) * 0.5)


## The preview's price is what is paid; demolishing the whole road gives a quarter of it back; undo puts it all back.
func _paying(b: Dictionary) -> void:
	var a := _field(b)
	var e := a + Vector3(FIELD_ROAD_M, 0, 0)
	var m0 := _money()
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	_check.call(r.ok and r.money == pv.price and m0 - _money() == pv.price, "pagou a prévia: %s" % [m0 - _money()])
	var m1 := _money()
	var pieces := 0
	for i in int(FIELD_ROAD_M / WALK_M) + 1:
		if CityStore.dispatch(&"demolish", [a + Vector3(i * WALK_M, 0, 0)]).get("ok", false):
			pieces += 1
	var back := _money() - m1
	var share: float = DataTables.tuning("economy_refund_share")
	_check.call(pieces > 0 and absf(back - pv.price * share) <= pieces, "devolveu %s de %s em %d pedaços" % [back, pv.price, pieces])
	for i in pieces:
		CityStore.dispatch(&"undo")
	_check.call(_money() == m1, "desfazer repôs o dinheiro: %s" % _money())
	CityStore.dispatch(&"undo")
	_check.call(_money() == m0, "desfazer a via devolveu tudo: %s" % _money())


## A road laid in pieces is one road, and Split makes it two: a click on one half leaves the other.
func _split_road(b: Dictionary) -> void:
	var a := _field(b)
	var e := a + Vector3(FIELD_ROAD_M, 0, 0)
	var mid := a + Vector3(FIELD_ROAD_M * 0.3, 0, 0)
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	_check.call(r.ok and r.segments > 1, "a via longa tem %d pedaços" % r.get("segments", 0))
	_check.call(CityStore.dispatch(&"split_road", [mid]).get("ok", false), "dividiu a via")
	var d: Dictionary = CityStore.dispatch(&"demolish", [a + Vector3(WALK_M * 2.0, 0, 0)])
	_check.call(d.ok, "demoliu um lado")
	var near: Dictionary = CityStore.query(&"segment_at", [a + Vector3(WALK_M * 2.0, 0, 0)])
	var far: Dictionary = CityStore.query(&"segment_at", [e - Vector3(WALK_M * 2.0, 0, 0)])
	_check.call(near.get("seg", -1) < 0 and far.get("seg", -1) >= 0, "o outro lado do Dividir ficou (%s, %s)" % [near.get("seg", -1), far.get("seg", -1)])
	var all: Dictionary = CityStore.dispatch(&"demolish", [e - Vector3(WALK_M * 2.0, 0, 0)])
	_check.call(all.ok and all.segments > 1, "o que sobrou caiu de uma vez: %d pedaços" % all.get("segments", 0))
	for i in 4:
		CityStore.dispatch(&"undo")


## A point on the axis of the first road found walking east from the middle of town: the pick finds a road from road_pick_m
## off its axis, where a lot's building may stand, and a click there takes the building.
func _town_road(b: Dictionary) -> Vector3:
	var from := Vector3((b.min.x + b.max.x) * 0.5, 0, (b.min.z + b.max.z) * 0.5)
	for i in WALK_STEPS:
		var seg: Dictionary = CityStore.query(&"segment_at", [from + Vector3(i * WALK_M, 0, 0)])
		if seg.get("seg", -1) >= 0:
			return seg.at
	return from


## A road of the generated town is worth its price, as any other.
func _generated_roads(b: Dictionary) -> void:
	var p := _town_road(b)
	var m0 := _money()
	var d: Dictionary = CityStore.dispatch(&"demolish", [p])
	_check.call(d.ok and d.money < 0 and _money() == m0 - d.money, "via gerada devolveu %s: %s em %s" % [_money() - m0, d, p])
	CityStore.dispatch(&"undo")
	_check.call(_money() == m0, "desfazer tirou a devolução")
	var cheaper: Dictionary = CityStore.dispatch(&"upgrade_road", [p, NARROW_PROFILE, false])
	_check.call(cheaper.ok and cheaper.money < 0 and _money() == m0 - cheaper.money, "perfil mais barato devolveu %s" % [_money() - m0])
	CityStore.dispatch(&"undo")
	var dearer: Dictionary = CityStore.dispatch(&"upgrade_road", [p, WIDE_PROFILE, false])
	_check.call(dearer.ok and dearer.money > 0 and _money() == m0 - dearer.money, "perfil mais caro cobrou %s" % [m0 - _money()])
	CityStore.dispatch(&"undo")
	_check.call(_money() == m0, "desfazer Melhorar repôs o dinheiro")


## Without the money no edit goes through and nothing moves.
func _refusals(b: Dictionary) -> void:
	var a := _field(b)
	var e := a + Vector3(FIELD_ROAD_M, 0, 0)
	var ring := a + Vector3(0, 0, RING_OUT_M)
	var p := _town_road(b)
	_set_money(LOW_FUNDS)
	var s0: int = CityStore.snapshot.segments
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	_check.call(pv.ok and not pv.affordable, "prévia sem dinheiro")
	var edits := {
		"via": CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, PX]),
		"melhorar": CityStore.dispatch(&"upgrade_road", [p, WIDE_PROFILE, false]),
		"rotatória": CityStore.dispatch(&"roundabout", [ring, RING_RADIUS, false]),
	}
	for what in edits:
		_check.call(not edits[what].ok and edits[what].error == "hint.rule.funds", "%s recusada: %s" % [what, edits[what].error])
	_check.call(_money() == LOW_FUNDS and CityStore.snapshot.segments == s0, "nada mudou: %s %d" % [_money(), CityStore.snapshot.segments])
	_set_money(START_FUNDS)
	var built: Dictionary = CityStore.dispatch(&"roundabout", [ring, RING_RADIUS, false])
	_check.call(built.ok and built.money > 0 and _money() == START_FUNDS - built.money, "rotatória cobrou %s" % built.money)
	CityStore.dispatch(&"undo")


## How much of a road is built each way and what that costs.
func _build_table() -> void:
	var structure: Dictionary = DataTables.catalog().road_profiles.styles[DataTables.REGIONAL_STYLE].structure
	var cost := 0.0
	for p in CityStore.query(&"road_profiles"):
		if p.index == PROFILE:
			cost = p.cost_per_m
	for row in BUILD_TABLE:
		var l: Dictionary = CityStore.query(&"probe_road_build", [PROFILE, row[0], row[1], row[2], LEN_M])
		var value := 0.0
		var same := true
		for k in KINDS:
			same = same and is_equal_approx(l[k] + 1.0, float(row[3].get(k, 0.0)) + 1.0)
			value += float(row[3].get(k, 0.0)) * structure[k] * cost
		_check.call(same and is_equal_approx(l.value, value), "modos %s: %s" % [row, l])
