## Core test of the player road commands through the store (no rendering needed): builds a road from free ground into
## the generated town, checks the money, the network counts, a refusal, demolition with refund, zoning and growth; then
## the money rules measured on the 4.0: price paid, share given back, generated roads, undo, refusals and how roads are built.
## Run: godot --headless --path project res://tests/core/road_commands_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const PROFILE := 0
## A road from the edge of the town out to free ground (metres from the city's bounding box).
const OUT_M := 260.0
const GROW_TICKS := 1200
const ZONE_RADIUS := 60.0
const ZONE_RES := 1
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

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	var s0 := CityStore.snapshot.duplicate()
	var b: Dictionary = CityStore.query(&"city_bounds")
	var a := Vector3(b.max.x - 1.0, 0, (b.min.z + b.max.z) * 0.5)
	var e := a + Vector3(OUT_M, 0, 0)
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE])
	print("prévia: ", {"ok": pv.ok, "error": pv.error, "price": pv.price, "len": pv.length, "start": pv.start})
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE])
	print("construir: ", r)
	var s1 := CityStore.snapshot
	_check(r.ok, "via construída")
	_check(is_equal_approx(s0.money - s1.money, pv.price), "cobrou o preço da prévia (%s)" % [s0.money - s1.money])
	_check(s1.segments > s0.segments, "segmentos %d -> %d" % [s0.segments, s1.segments])
	var bad: Dictionary = CityStore.dispatch(&"build_road", [e, e + Vector3(2, 0, 0), false, Vector3.ZERO, PROFILE])
	_check(not bad.ok and bad.error == "refuse.road.tooShort", "recusa via curta: %s" % bad.error)
	var z: Dictionary = CityStore.dispatch(&"paint_zone", [(a + e) * 0.5, ZONE_RADIUS, ZONE_RES])
	print("zona: ", z, " células em crescimento: ", CityStore.snapshot.growth_cells)
	_check(z.ok and z.zoned_sides > 0, "zoneou lados")
	var before: int = CityStore.snapshot.buildings
	CityStore.step_ticks(GROW_TICKS)
	print("cresceram: ", CityStore.snapshot.grown, " prédios ", before, " -> ", CityStore.snapshot.buildings)
	_check(CityStore.snapshot.grown > 0, "prédios cresceram na via nova")
	var m0: float = CityStore.snapshot.money
	var d: Dictionary = CityStore.dispatch(&"demolish", [e - Vector3(2, 0, 0)])
	print("demolir: ", d, " dinheiro ", m0, " -> ", CityStore.snapshot.money)
	_check(d.ok, "demoliu")
	_paying(b)
	_generated_roads(b)
	_refusals(b)
	_build_table()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE])
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE])
	_check(r.ok and r.money == pv.price and m0 - _money() == pv.price, "pagou a prévia: %s" % [m0 - _money()])
	var m1 := _money()
	var pieces := 0
	for i in int(FIELD_ROAD_M / WALK_M) + 1:
		if CityStore.dispatch(&"demolish", [a + Vector3(i * WALK_M, 0, 0)]).get("ok", false):
			pieces += 1
	var back := _money() - m1
	var share: float = DataTables.tuning("economy_refund_share")
	_check(pieces > 0 and absf(back - pv.price * share) <= pieces, "devolveu %s de %s em %d pedaços" % [back, pv.price, pieces])
	for i in pieces:
		CityStore.dispatch(&"undo")
	_check(_money() == m1, "desfazer repôs o dinheiro: %s" % _money())
	CityStore.dispatch(&"undo")
	_check(_money() == m0, "desfazer a via devolveu tudo: %s" % _money())


## The first road found walking east from the middle of town.
func _town_road(b: Dictionary) -> Vector3:
	var from := Vector3((b.min.x + b.max.x) * 0.5, 0, (b.min.z + b.max.z) * 0.5)
	for i in WALK_STEPS:
		var p := from + Vector3(i * WALK_M, 0, 0)
		if CityStore.query(&"segment_at", [p]).get("seg", -1) >= 0:
			return p
	return from


## A road of the generated town is worth its price, as any other.
func _generated_roads(b: Dictionary) -> void:
	var p := _town_road(b)
	var m0 := _money()
	var d: Dictionary = CityStore.dispatch(&"demolish", [p])
	_check(d.ok and d.money < 0 and _money() == m0 - d.money, "via gerada devolveu %s" % [_money() - m0])
	CityStore.dispatch(&"undo")
	_check(_money() == m0, "desfazer tirou a devolução")
	var cheaper: Dictionary = CityStore.dispatch(&"upgrade_road", [p, NARROW_PROFILE, false])
	_check(cheaper.ok and cheaper.money < 0 and _money() == m0 - cheaper.money, "perfil mais barato devolveu %s" % [_money() - m0])
	CityStore.dispatch(&"undo")
	var dearer: Dictionary = CityStore.dispatch(&"upgrade_road", [p, WIDE_PROFILE, false])
	_check(dearer.ok and dearer.money > 0 and _money() == m0 - dearer.money, "perfil mais caro cobrou %s" % [m0 - _money()])
	CityStore.dispatch(&"undo")
	_check(_money() == m0, "desfazer Melhorar repôs o dinheiro")


## Without the money no edit goes through and nothing moves.
func _refusals(b: Dictionary) -> void:
	var a := _field(b)
	var e := a + Vector3(FIELD_ROAD_M, 0, 0)
	var ring := a + Vector3(0, 0, RING_OUT_M)
	var p := _town_road(b)
	_set_money(LOW_FUNDS)
	var s0: int = CityStore.snapshot.segments
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE])
	_check(pv.ok and not pv.affordable, "prévia sem dinheiro")
	var edits := {
		"via": CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE]),
		"melhorar": CityStore.dispatch(&"upgrade_road", [p, WIDE_PROFILE, false]),
		"rotatória": CityStore.dispatch(&"roundabout", [ring, RING_RADIUS, false]),
	}
	for what in edits:
		_check(not edits[what].ok and edits[what].error == "hint.rule.funds", "%s recusada: %s" % [what, edits[what].error])
	_check(_money() == LOW_FUNDS and CityStore.snapshot.segments == s0, "nada mudou: %s %d" % [_money(), CityStore.snapshot.segments])
	_set_money(START_FUNDS)
	var built: Dictionary = CityStore.dispatch(&"roundabout", [ring, RING_RADIUS, false])
	_check(built.ok and built.money > 0 and _money() == START_FUNDS - built.money, "rotatória cobrou %s" % built.money)
	CityStore.dispatch(&"undo")


## How much of a road is built each way and what that costs.
func _build_table() -> void:
	var structure: Dictionary = DataTables.catalog().road_profiles.styles[DataTables.CITY_STYLE].structure
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
		_check(same and is_equal_approx(l.value, value), "modos %s: %s" % [row, l])


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
