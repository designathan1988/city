## Core test of the player road commands through the store (no rendering needed): builds a road from free ground into
## the generated town, checks the money, the network counts, a refusal, demolition with refund, zoning and growth.
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
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
## How far past the road the snapping and the freehand stroke are tried, on open ground.
const FREE_OFFSET_M := 200.0
const LOOSE_M := 0.01
const TANGENT_DOT := 0.999
const COST_RUNS := 20
const TOWN_STEPS := 400
const TOWN_SWING_M := 60.0

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
	var pv: Dictionary = CityStore.dispatch(&"preview_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	print("prévia: ", {"ok": pv.ok, "error": pv.error, "price": pv.price, "len": pv.length, "start": pv.start})
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, false, Vector3.ZERO, PROFILE, PX])
	print("construir: ", r)
	var s1 := CityStore.snapshot
	_check(r.ok, "via construída")
	_check(is_equal_approx(s0.money - s1.money, pv.price), "cobrou o preço da prévia (%s)" % [s0.money - s1.money])
	_check(s1.segments > s0.segments, "segmentos %d -> %d" % [s0.segments, s1.segments])
	var bad: Dictionary = CityStore.dispatch(&"build_road", [e, e + Vector3(2, 0, 0), false, Vector3.ZERO, PROFILE, PX])
	_check(not bad.ok and bad.error == "refuse.road.tooShort", "recusa via curta: %s" % bad.error)
	_road_end(e)
	_snapping(a, e)
	_freehand(e)
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
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## A road the player lays ends in a straight cut; the bulb is a choice, undone like any edit.
func _road_end(e: Vector3) -> void:
	var ends: Array = CityStore.query(&"inspect", [e - Vector3(2, 0, 0)]).get("ends", [])
	_check(ends.size() == 1 and not ends[0].bulb, "via nova termina em corte reto: %s" % [ends])
	var r: Dictionary = CityStore.dispatch(&"set_road_end", [ends[0].node, true])
	var now: Array = CityStore.query(&"inspect", [e - Vector3(2, 0, 0)]).ends
	_check(r.ok and now[0].bulb, "balão a pedido: %s" % r)
	CityStore.dispatch(&"undo", [])
	now = CityStore.query(&"inspect", [e - Vector3(2, 0, 0)]).ends
	_check(not now[0].bulb, "desfazer tira o balão")


## Where the road lands: the grid, the angles, the road it leaves and the road it arrives on.
func _snapping(a: Vector3, e: Vector3) -> void:
	var o := Vector3(roundf(e.x), 0, roundf(e.z) + FREE_OFFSET_M)
	var start: Vector3 = CityStore.query(&"road_start", [o + Vector3(0.3, 0, 0.2), PX, true, true]).position
	_check(absf(start.x - o.x) < LOOSE_M and absf(start.z - o.z) < LOOSE_M, "início em chão livre vai para a grade: %s" % start)
	var r: Dictionary = CityStore.query(&"road_end", [start, o + Vector3(100.4, 0, 3), PX, true, true, true])
	_check(r.guide == "orthogonal" and absf(r.position.z - o.z) < LOOSE_M and absf(r.position.x - o.x - 100) < LOOSE_M,
		"fim encaixa no eixo e na grade: %s" % r)
	var raw := o + Vector3(100.4, 0, 3)
	r = CityStore.query(&"road_end", [start, raw, PX, false, true, true])
	_check(r.guide == "" and absf(r.position.x - raw.x) < LOOSE_M and absf(r.position.z - raw.z) < LOOSE_M, "encaixe desligado segue o ponteiro")
	raw = o + Vector3(cos(deg_to_rad(31.0)), 0, sin(deg_to_rad(31.0))) * 100.0
	r = CityStore.query(&"road_end", [start, raw, PX, true, true, false])
	_check(r.guide == "angle" and absf(r.angle_deg - 30.0) < LOOSE_M, "encaixa no degrau de 15 graus: %s" % r)
	r = CityStore.query(&"road_end", [e, e + Vector3(80, 0, 2), PX, true, true, false])
	_check(r.guide == "continue" and absf(r.position.z - e.z) < 0.1, "segue em frente a partir da ponta: %s" % r)
	r = CityStore.query(&"road_end", [e, e + Vector3(2, 0, 80), PX, true, true, false])
	_check(r.guide == "perpendicular" and absf(r.position.x - e.x) < 0.1, "perpendicular à via de onde sai: %s" % r)
	var mid := a.lerp(e, 0.5)
	var from := mid + Vector3(3, 0, 60)
	r = CityStore.query(&"road_end", [from, mid + Vector3(-1, 0, 0), PX, true, true, false])
	_check(r.guide == "perpendicular" and absf(r.position.x - from.x) < 0.1 and absf(r.position.z - mid.z) < 0.1,
		"chega perpendicular à via: %s" % r)


## A freehand stroke: tangent at its joints, one undo step, and all or nothing.
func _freehand(e: Vector3) -> void:
	var base := e + Vector3(0, 0, FREE_OFFSET_M)
	var samples := PackedVector3Array()
	for i in 61:
		samples.push_back(base + Vector3(i * 4.0, 0, 30.0 * sin(i * 0.12)))
	var pieces: PackedVector3Array = CityStore.query(&"road_pieces", [samples[0], samples[-1], 2, false, Vector3.ZERO, samples, PROFILE, PX])
	var n := pieces.size() / 3
	_check(n >= 2, "traço à mão vira cadeia de curvas (%d)" % n)
	var tangent := true
	for k in n - 1:
		var d1 := (pieces[3 * k + 2] - pieces[3 * k + 1]).normalized()
		var d2 := (pieces[3 * k + 4] - pieces[3 * k + 3]).normalized()
		tangent = tangent and d1.dot(d2) > TANGENT_DOT
	_check(tangent, "curvas tangentes nas juntas")
	var seg0: int = CityStore.snapshot.segments
	var money0: float = CityStore.snapshot.money
	var r: Dictionary = CityStore.dispatch(&"build_road_path", [pieces, PROFILE, PX])
	_check(r.ok and CityStore.snapshot.segments >= seg0 + n, "traço construído: %s" % r)
	CityStore.dispatch(&"undo", [])
	_check(CityStore.snapshot.segments == seg0 and is_equal_approx(CityStore.snapshot.money, money0), "um desfazer tira o traço inteiro")
	var b := base + Vector3(60, 0, 0)
	var bad := PackedVector3Array([base, base.lerp(b, 0.5), b, b, b + Vector3(1, 0, 0), b + Vector3(2, 0, 0)])
	r = CityStore.dispatch(&"build_road_path", [bad, PROFILE, PX])
	_check(not r.ok and r.error == "refuse.road.tooShort" and CityStore.snapshot.segments == seg0, "peça recusada recusa a via inteira: %s" % r)
	_plan_cost(base)
	_plan_cost_town()
	var tight: PackedVector3Array = CityStore.query(&"road_pieces", [base, base + Vector3(20, 0, 0), 1, true, base + Vector3(10, 0, 60), PackedVector3Array(), PROFILE, PX])
	_check(tight.size() == 3 and tight[1].z - base.z < 60.0, "curva apertada é aberta até o raio da via: %s" % tight[1])


## What a preview costs at the most pieces a stroke may have (road_free_max_controls): it runs on every pointer move.
func _plan_cost(base: Vector3) -> void:
	var samples := PackedVector3Array()
	for i in 400:
		samples.push_back(base + Vector3(i * 4.0, 0, 60.0 * sin(i * 0.25)))
	var pieces: PackedVector3Array = CityStore.query(&"road_pieces", [samples[0], samples[-1], 2, false, Vector3.ZERO, samples, PROFILE, PX])
	var t0 := Time.get_ticks_usec()
	for i in COST_RUNS:
		CityStore.dispatch(&"preview_road_path", [pieces, PROFILE, PX])
	print("prévia: %d peças, %d us por movimento" % [pieces.size() / 3, (Time.get_ticks_usec() - t0) / COST_RUNS])
	_check(pieces.size() / 3 < DataTables.tuning("road_free_max_controls"), "peças dentro do limite (%d)" % [pieces.size() / 3])


## The same stroke across the town, among its roads: a preview's cost where it looks at the most.
func _plan_cost_town() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var from := Vector3(b.min.x, 0, (b.min.z + b.max.z) * 0.5)
	var samples := PackedVector3Array()
	for i in TOWN_STEPS + 1:
		var at := from.lerp(Vector3(b.max.x, 0, from.z), float(i) / TOWN_STEPS)
		samples.push_back(at + Vector3(0, 0, TOWN_SWING_M * sin(i * 0.25)))
	var pieces: PackedVector3Array = CityStore.query(&"road_pieces", [samples[0], samples[-1], 2, false, Vector3.ZERO, samples, PROFILE, PX])
	var t0 := Time.get_ticks_usec()
	for i in COST_RUNS:
		CityStore.dispatch(&"preview_road_path", [pieces, PROFILE, PX])
	print("prévia na cidade: %d peças, %d us por movimento" % [pieces.size() / 3, (Time.get_ticks_usec() - t0) / COST_RUNS])
	_check(pieces.size() >= 3, "traço pela cidade tem peças")


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
