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
const TICKS_FOR_GRAPH := 3
const RULE_STOP := 1
const RULES_OFF_M := 540.0
const SHARP_DEG := 15.0
const OPEN_DEG := 40.0
const AVENUE_GAP_M := 20.0
const SIDE_OFF_M := 720.0
const COMPOSED_LANE_M := 4.5
const SIDE_GAP_M := 8.0
const CURVE_OFF_M := 800.0
const CHORD_M := 100.0
const BEND_M := 20.0
const TIGHT_CHORD_M := 20.0
const TIGHT_CONTROL_M := 30.0
const TIGHTEST_M := 8.7
const NEAR_M := 0.01

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
	_side_by_side(e)
	_curves(e)
	RoadShapeCases.new(_check).stubs(e)
	RoadShapeCases.new(_check).run(e)
	_composed_road()
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
	_check(profiles[four].centre == "solid", "duas faixas por sentido têm a linha do 4.0: %s" % profiles[four].centre)
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
	_lanes_by_hand(hit.node)
	var legs: Array = CityStore.query(&"node_legs", [hit.node])
	_check(legs.size() == 3, "regras: três vias no cruzamento (%d)" % legs.size())
	var rule: Dictionary = CityStore.dispatch(&"set_approach_rule", [hit.node, legs[0].seg, RULE_STOP])
	_check(rule.get("ok", false), "regras: Pare posto numa via")
	_check(int(CityStore.query(&"node_legs", [hit.node])[0].rule) == RULE_STOP, "regras: a via ficou com Pare")
	CityStore.dispatch(&"undo")
	_check(int(CityStore.query(&"node_legs", [hit.node])[0].rule) == 0, "regras: desfazer tira a regra")


## A lane's connection taken away by hand, the traffic's graph following, and undo bringing it back.
func _lanes_by_hand(node: int) -> void:
	CityStore.step_ticks(TICKS_FOR_GRAPH)
	var before: Array = CityStore.query(&"node_lanes", [node]).built
	_check(before.size() > 1, "conectores: o cruzamento tem ligações (%d)" % before.size())
	var one: Array = before[0]
	var left := before.filter(func(b: Array) -> bool: return b[0] == one[0] and b[1] == one[1])
	var r: Dictionary = CityStore.dispatch(&"toggle_lane_link", [node, one[0], one[1], one[2], one[3]])
	if left.size() < 2:
		_check(r.get("error", "") == "hint.lane.last", "conectores: a faixa não fica sem saída: %s" % r)
		return
	_check(r.get("ok", false), "conectores: ligação tirada")
	CityStore.step_ticks(TICKS_FOR_GRAPH)
	var now: Array = CityStore.query(&"node_lanes", [node]).built
	_check(now.size() == before.size() - 1, "conectores: o grafo perdeu a ligação (%d -> %d)" % [before.size(), now.size()])
	CityStore.dispatch(&"undo")
	CityStore.step_ticks(TICKS_FOR_GRAPH)
	_check(CityStore.query(&"node_lanes", [node]).built.size() == before.size(), "conectores: desfazer volta a ligação")


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


## A road on top of another is refused along its length, and only there: going on from a road's end is not lying beside it.
func _side_by_side(e: Vector3) -> void:
	var a := e + Vector3(0, 0, SIDE_OFF_M)
	var b := a + Vector3(MOVE_LEN_M, 0, 0)
	var urban: int = CityStore.first_road()
	CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, urban, PX])
	var beside := a + Vector3(0, 0, SIDE_GAP_M)
	var r: Dictionary = CityStore.dispatch(&"build_road", [beside, beside + Vector3(MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	_check(r.get("error", "") == "road.reason.overlap", "via em cima de outra é recusada: %s" % r)
	r = CityStore.dispatch(&"build_road", [b, b + Vector3(MOVE_LEN_M, 0, 0), false, Vector3.ZERO, urban, PX])
	_check(r.get("ok", false), "seguir da ponta passa: %s" % r)


## A dragged curve bends as the 4.0's does, short of the farthest point of the drag, and none turns tighter than the design car.
func _curves(e: Vector3) -> void:
	var a := e + Vector3(0, 0, CURVE_OFF_M)
	var b := a + Vector3(CHORD_M, 0, 0)
	var urban: int = CityStore.first_road()
	var far := (a + b) * 0.5 + Vector3(0, 0, BEND_M)
	var q: PackedVector3Array = CityStore.query(&"road_pieces", [a, b, 1, false, Vector3.ZERO, PackedVector3Array([a, far, b]), urban, PX])
	var apex := (q[0] + 2 * q[1] + q[2]) * 0.25
	var bend: float = BEND_M * DataTables.tuning("road_bend_gain") * 0.5
	_check(absf(apex.distance_to((a + b) * 0.5) - bend) < NEAR_M, "a curva dobra como a do 4.0: %s" % apex.distance_to((a + b) * 0.5))
	b = a + Vector3(TIGHT_CHORD_M, 0, 0)
	var control := (a + b) * 0.5 + Vector3(0, 0, TIGHT_CONTROL_M)
	q = CityStore.query(&"road_pieces", [a, b, 1, true, control, PackedVector3Array(), urban, PX])
	var rise := q[1].distance_to((q[0] + q[2]) * 0.5)
	var radius := TIGHT_CHORD_M * TIGHT_CHORD_M / (4 * rise)
	_check(radius >= TIGHTEST_M - NEAR_M, "raio mínimo do carro de projeto: %s" % radius)


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


## Personalizar: a road of the catalogue read as elements, what stops one from being built, and a road of the player's laid.
func _composed_road() -> void:
	var base: int = CityStore.first_road()
	var spec: Dictionary = CityStore.query(&"road_spec", [base])
	_check(spec.elements[0].kind == "footway" and spec.elements[-1].kind == "footway", "perfil: a via lida começa e acaba em calçada")
	_check((CityStore.query(&"road_spec_issues", [base, spec]) as Array).is_empty(), "perfil: a via do catálogo pode ser construída")
	var bad: Dictionary = spec.duplicate(true)
	bad.elements.remove_at(0)
	_check(not (CityStore.query(&"road_spec_issues", [base, bad]) as Array).is_empty(), "perfil: sem calçada na ponta há problema")
	_check(int(CityStore.query(&"road_spec_make", [base, bad])) < 0, "perfil: o que tem problema não vira via")
	var own: Dictionary = spec.duplicate(true)
	RoadSpecOps.with_width(own, 1, COMPOSED_LANE_M)
	var index: int = CityStore.query(&"road_spec_make", [base, own])
	_check(index >= 0, "perfil: a via composta vira um perfil")
	_check(int(CityStore.query(&"road_spec_make", [base, own])) == index, "perfil: a mesma via é o mesmo perfil")
	var back: Dictionary = CityStore.query(&"road_spec", [index])
	_check(is_equal_approx(RoadSpecOps.total(back), RoadSpecOps.total(own)), "perfil: a largura volta igual (%.2f)" % RoadSpecOps.total(back))
