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
const FILLED_STYLES := ["roadcraft", "brasil"] # regras-sessoes.md, section 10

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	_styles()
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
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The styles filled so far hold what the loaders ask of the one the game boots with: a shape for every drawing, a crossing in order.
func _styles() -> void:
	for id: String in FILLED_STYLES:
		var signs: Dictionary = DataTables.styled(DataTables.render().streets, id).signs
		_check(signs.plates.size() == signs.drawings.size(), "estilo %s: uma figura por placa" % id)
		for d: Dictionary in signs.drawings:
			_check(signs.plates.has(d.plate), "estilo %s: placa %s" % [id, d.plate])
		var c: Dictionary = DataTables.styled(DataTables.catalog().street_furniture, id).crossing
		_check(c.zebra_from_m < c.zebra_to_m and c.zebra_to_m < c.stop_from_m and c.stop_from_m < c.stop_to_m, "estilo %s: faixa antes da retenção" % id)


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
