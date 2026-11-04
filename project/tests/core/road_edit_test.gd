## Core test of the road edits beyond laying one, through the store: the tool's options on a class road, the Move and
## Controle tabs and what an edit may not leave behind, on open ground past the town.
## Run: godot --headless --path project res://tests/core/road_edit_test.tscn -- --no-flow
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const OUT_M := 460.0
## Metres a screen pixel covers (the tool asks the camera; here a close view).
const PX := 0.25
const URBAN_M := 12.0
const URBAN_PRICE := 1520.0
const LANE_M := 4.0
const BAY_M := 2.0
const WIDE_M := 15.0
const VARIANT_OFF_M := 180.0
const VARIANT_LEN_M := 60.0
const MOVE_OFF_M := 300.0
const MOVE_LEN_M := 60.0
const MOVE_BY_M := 30.0
const CONTROL_OFF_M := 420.0
const RULES_OFF_M := 540.0
const SHARP_DEG := 15.0
const OPEN_DEG := 40.0
const AVENUE_GAP_M := 20.0

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot %s" % [errors])
	CityStore.new_map(TOWN_MAP, SEED, true)
	CityStore.dispatch(&"set_speed", [0])
	var b: Dictionary = CityStore.query(&"city_bounds")
	var e := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	_variants(e)
	_move_node(e)
	_control(e)
	_rules(e)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The tool's options on a class road: a profile of their own, made once, with the 4.0's widths and prices.
func _variants(e: Vector3) -> void:
	var urban: int = CityStore.first_road()
	var profiles: Array = CityStore.query(&"road_profiles")
	_check(profiles[urban].width == URBAN_M and profiles[urban].cost_per_m == URBAN_PRICE, "via urbana como no 4.0")
	var four: int = CityStore.dispatch(&"road_variant", [urban, 4, 0.0, 0])
	var bays: int = CityStore.dispatch(&"road_variant", [urban, 0, 0.0, 1])
	var wide: int = CityStore.dispatch(&"road_variant", [urban, 0, WIDE_M, 0])
	profiles = CityStore.query(&"road_profiles")
	_check(profiles[four].lanes == 4 and profiles[four].width == URBAN_M + 2 * LANE_M, "quatro faixas: %s" % profiles[four].width)
	_check(profiles[bays].width == URBAN_M + 2 * BAY_M, "vagas dos dois lados: %s" % profiles[bays].width)
	_check(profiles[wide].width == WIDE_M and profiles[wide].lanes == 2, "largura total: %s" % profiles[wide].width)
	_check(CityStore.dispatch(&"road_variant", [urban, 4, 0.0, 0]) == four, "a mesma variante é o mesmo perfil")
	_check(CityStore.dispatch(&"road_variant", [urban, 0, 0.0, 0]) == urban, "sem opção é a própria via")
	var card: int = profiles.filter(func(p: Dictionary) -> bool: return p.id == "narrowStreet")[0].index
	_check(CityStore.dispatch(&"road_variant", [card, 4, 0.0, 0]) == card, "via do catálogo não aceita opção")
	var a := e + Vector3(0, 0, VARIANT_OFF_M)
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, a + Vector3(VARIANT_LEN_M, 0, 0), false, Vector3.ZERO, four, PX])
	_check(r.get("ok", false), "via com a variante: %s" % r)


## The Move tab: a node dropped elsewhere takes its roads along, charges only the difference, undoes in one step.
func _move_node(e: Vector3) -> void:
	var a := e + Vector3(0, 0, MOVE_OFF_M)
	var mid := a + Vector3(MOVE_LEN_M, 0, 0)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, mid, false, Vector3.ZERO, urban, PX])
	CityStore.dispatch(&"build_road", [mid, mid + Vector3(MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	var node: int = CityStore.query(&"node_at", [mid, PX]).node
	_check(node >= 0, "acha o nó do meio")
	var s0: int = CityStore.snapshot.segments
	var to := mid + Vector3(0, 0, MOVE_BY_M)
	_check(CityStore.query(&"moved_roads", [node, to]).size() == 2, "prévia com as duas vias")
	var r: Dictionary = CityStore.dispatch(&"move_node", [node, to])
	_check(r.get("ok", false), "move o nó: %s" % r)
	CityStore.refresh()
	_check(CityStore.snapshot.segments == s0, "as vias continuam duas")
	_check(CityStore.query(&"node_at", [to, PX]).node >= 0 and CityStore.query(&"node_at", [mid, PX]).node < 0, "o nó está no lugar novo")
	_check(float(r.money) > 0 and float(r.money) < MOVE_LEN_M * URBAN_PRICE, "cobra só a diferença: %s" % r.money)
	CityStore.dispatch(&"undo")
	CityStore.refresh()
	_check(CityStore.query(&"node_at", [mid, PX]).node >= 0, "desfazer devolve o nó")
	var far: Dictionary = CityStore.dispatch(&"move_node", [node, a + Vector3(1, 0, 0)])
	_check(not far.get("ok", false) and far.get("error", "") == "hint.move.tooShort", "curta demais é recusada: %s" % far)


## The Controle tab: a junction takes a control and undoes it; a node of two roads is refused.
func _control(e: Vector3) -> void:
	var a := e + Vector3(0, 0, CONTROL_OFF_M)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, a + Vector3(2 * MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	var mid := a + Vector3(MOVE_LEN_M, 0, 0)
	CityStore.dispatch(&"build_road", [mid, mid + Vector3(0, 0, MOVE_LEN_M), false, Vector3.ZERO, urban, PX])
	var hit: Dictionary = CityStore.query(&"node_at", [mid, PX])
	_check(hit.node >= 0 and hit.ctl == 0, "cruzamento novo é automático")
	_check(CityStore.query(&"junctions").any(func(j: Dictionary) -> bool: return j.node == hit.node), "o cruzamento está na lista")
	_check(CityStore.dispatch(&"set_node_control", [hit.node, 1]).get("ok", false), "semáforo")
	_check(CityStore.query(&"node_at", [mid, PX]).ctl == 1, "o controle ficou")
	CityStore.dispatch(&"undo")
	_check(CityStore.query(&"node_at", [mid, PX]).ctl == 0, "desfazer volta o controle")
	var end: Dictionary = CityStore.query(&"node_at", [a, PX])
	var r: Dictionary = CityStore.dispatch(&"set_node_control", [end.node, 1])
	_check(r.get("error", "") == "hint.control.notJunction", "ponta não tem controle: %s" % r)


## What an edit may not leave: two roads meeting under 25 degrees, a stretch too short for its two junctions. A refusal
## rolls the whole edit back.
func _rules(e: Vector3) -> void:
	var a := e + Vector3(0, 0, RULES_OFF_M)
	var b := a + Vector3(MOVE_LEN_M, 0, 0)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, urban, PX])
	var tight := b + Vector3(-MOVE_LEN_M * cos(deg_to_rad(SHARP_DEG)), 0, MOVE_LEN_M * sin(deg_to_rad(SHARP_DEG)))
	var r: Dictionary = CityStore.dispatch(&"build_road", [b, tight, false, Vector3.ZERO, urban, PX])
	_check(r.get("error", "") == "refuse.road.sharpAngle", "ângulo fechado é recusado: %s" % r)
	var open := b + Vector3(-MOVE_LEN_M * cos(deg_to_rad(OPEN_DEG)), 0, MOVE_LEN_M * sin(deg_to_rad(OPEN_DEG)))
	_check(CityStore.dispatch(&"build_road", [b, open, false, Vector3.ZERO, urban, PX]).get("ok", false), "ângulo aberto passa")
	var avenue: int = CityStore.query(&"road_profiles").filter(func(p: Dictionary) -> bool: return p.id == "avenue")[0].index
	var c := e + Vector3(0, 0, RULES_OFF_M + 2 * MOVE_LEN_M)
	var d := c + Vector3(0, 0, AVENUE_GAP_M)
	CityStore.dispatch(&"build_road", [c, c + Vector3(2 * MOVE_LEN_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	CityStore.dispatch(&"build_road", [d, d + Vector3(2 * MOVE_LEN_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	CityStore.refresh()
	var s0: int = CityStore.snapshot.segments
	var m0: float = CityStore.snapshot.money
	var mid := c + Vector3(MOVE_LEN_M, 0, 0)
	r = CityStore.dispatch(&"build_road", [mid, mid + Vector3(0, 0, AVENUE_GAP_M), false, Vector3.ZERO, avenue, PX])
	_check(r.get("error", "") == "hint.rule.squeezed", "trecho apertado é recusado: %s" % r)
	CityStore.refresh()
	_check(CityStore.snapshot.segments == s0 and CityStore.snapshot.money == m0, "recusa desfaz a obra inteira")


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1
