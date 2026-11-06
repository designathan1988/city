## The road_edit_test cases of what the Inspector does to a road's shape, through the store: a node's height held to the grade, a road bent and straightened,
## two roads in line joined, each undone to the same city.
class_name RoadShapeCases
extends RefCounted

const OFF_M := 1100.0
const ROAD_M := 200.0
const URBAN := 0
const PX := 0.25
const LIFT_M := 3.0
const TOO_HIGH_M := 500.0
const BEND_M := 30.0
const MIDDLE := 0.5
const SAME_M := 2.0
const QUARTER := 0.3 # clear of the cut in the middle of a road this long

var _check: Callable
var _mid := Vector3.ZERO


func _init(check: Callable) -> void:
	_check = check


func run(e: Vector3) -> void:
	var a := e + Vector3(OFF_M, 0, 0)
	var b := a + Vector3(ROAD_M, 0, 0)
	var laid: Dictionary = CityStore.dispatch(&"build_road", [a, b, false, Vector3.ZERO, URBAN, PX, 0.0])
	_check.call(laid.get("ok", false), "via para as edições do Inspecionar: %s" % [laid])
	var mid := (a + b) * 0.5
	_mid = mid
	var info: Dictionary = CityStore.query(&"inspect", [mid])
	_check.call(info.get("kind", "") == "road", "o Inspecionar pega a via")
	_height(info)
	_bend(info, mid)
	_join(a + (b - a) * QUARTER)


func _height(info: Dictionary) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var up: Dictionary = CityStore.dispatch(&"set_node_height", [info.node_b, LIFT_M])
	_check.call(up.get("ok", false), "altura da ponta: %s" % [up])
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "desfazer a altura volta à mesma cidade")
	var limited: Dictionary = CityStore.dispatch(&"set_node_height", [info.node_b, TOO_HIGH_M])
	_check.call(limited.get("ok", false), "pedir demais vai até o greide máximo: %s" % [limited])
	var lift: float = CityStore.query(&"inspect", [_mid]).lift_b
	_check.call(lift > LIFT_M and lift < TOO_HIGH_M, "a altura para no greide máximo: %.1f" % lift)
	CityStore.dispatch(&"undo")


func _bend(info: Dictionary, mid: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var bent: Dictionary = CityStore.dispatch(&"bend_road", [info.id, BEND_M, MIDDLE])
	_check.call(bent.get("ok", false), "curvar a via: %s" % [bent])
	var now: Dictionary = CityStore.query(&"inspect", [bent.get("at", mid)])
	_check.call(now.get("kind", "") == "road" and absf(absf(float(now.bend_m)) - BEND_M) < SAME_M, "a via fica com a curva pedida: %s" % [now.get("bend_m", "?")])
	_check.call(float(now.length_m) > float(info.length_m), "curvada, a via fica mais longa")
	var back: Dictionary = CityStore.dispatch(&"bend_road", [now.id, 0.0, MIDDLE])
	_check.call(back.get("ok", false), "endireitar a via: %s" % [back])
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "desfazer a curva volta à mesma cidade")


func _join(mid: Vector3) -> void:
	var before: Dictionary = CityStore.query(&"state_hash")
	var cut: Dictionary = CityStore.dispatch(&"split_road", [mid])
	_check.call(cut.get("ok", false), "ponto no meio da via: %s" % [cut])
	var node: int = CityStore.query(&"node_at", [mid, PX]).get("node", -1)
	_check.call(node >= 0, "o ponto é um nó")
	var joined: Dictionary = CityStore.dispatch(&"join_roads", [node])
	_check.call(joined.get("ok", false), "unir as vias em linha: %s" % [joined])
	_check.call(CityStore.query(&"node_at", [mid, PX]).get("node", -1) < 0, "unidas, o nó some")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	_check.call(CityStore.query(&"state_hash").whole == before.whole, "desfazer a união volta à mesma cidade")
