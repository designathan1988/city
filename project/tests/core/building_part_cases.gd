## Every part of the Parts tab set against a wall of a building (building_edit_test.gd): it lands or is refused with a text, undo takes
## it away, and the free placement, the turn and the row work on a part that landed.

const SIDE := 0
const FREE_OUT_M := 30.0


static func run(id: int, made: int, check: Callable) -> void:
	_gear(id, made, check)
	var landed := 0
	var spec_of_last := {}
	for section: Dictionary in DataTables.ui("builder").tabs.parts.sections:
		for spec: Dictionary in section.tools:
			var before: int = _count(id)
			var r: Dictionary = CityStore.dispatch(&"body_add_part", [id, 0, 1, SIDE, 0, spec])
			if r.get("ok", false):
				landed += 1
				spec_of_last = spec
				check.call(_count(id) == before + 1, "%s entrou no corpo" % spec.id)
				if spec.get("standoff_m", 0.0) > 0.0 and spec.fit_kind == 0:
					check.call(_clear_of_wall(id, spec), "%s fica a %.1f m da parede" % [spec.id, spec.standoff_m])
				check.call(CityStore.dispatch(&"undo") and _digest(id) == made, "%s desfeito" % spec.id)
			else:
				var why: String = r.get("error", "")
				check.call(why != "" and TranslationServer.translate(why) != why, "%s recusada com texto: %s" % [spec.id, r])
	check.call(landed > 0, "%d peças entraram" % landed)
	var free: Dictionary = CityStore.dispatch(&"body_add_part_at", [id, _away(id), spec_of_last])
	check.call(free.get("ok", false), "peça solta no chão: %s" % free)
	var last: int = _count(id) - 1
	check.call(CityStore.dispatch(&"body_turn_part", [id, last]).get("ok", false), "peça girada")
	check.call(CityStore.dispatch(&"body_remove_part", [id, last]).get("ok", false), "peça removida")
	check.call(_count(id) == last, "a peça saiu do corpo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	check.call(_digest(id) == made, "três desfazer voltam ao corpo gerado")


## Each thing of the roof lands on it or says why not, and undo takes it away; off the roof it is refused.
static func _gear(id: int, made: int, check: Callable) -> void:
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[0]
	var s: float = sin(p.rot)
	var c: float = cos(p.rot)
	var middle: Vector3 = p.position + Vector3(s, 0, c) * (p.d * 0.5) + Vector3(c, 0, -s) * (v.x + v.w * 0.5) + Vector3(-s, 0, -c) * (v.y + v.d * 0.5)
	var landed := 0
	for section: Dictionary in DataTables.ui("builder").tabs.roof.sections:
		for spec: Dictionary in section.tools:
			if spec.do != "gear":
				continue
			var r: Dictionary = CityStore.dispatch(&"body_add_gear", [id, 0, middle, spec])
			if r.get("ok", false):
				landed += 1
				check.call(CityStore.query(&"body_info", [id]).volumes[0].gear == 1, "%s no telhado" % spec.id)
				check.call(CityStore.dispatch(&"undo") and _digest(id) == made, "%s desfeito" % spec.id)
			else:
				check.call(r.get("error", "") == "building.problem.roofPart", "%s não cabe: %s" % [spec.id, r])
	check.call(landed > 0, "%d equipamentos entraram no telhado" % landed)
	var off: Dictionary = CityStore.dispatch(&"body_add_gear", [id, 0, _away(id), DataTables.ui("builder").tabs.roof.sections[1].tools[0]])
	check.call(off.get("error", "") == "building.problem.roofPart", "fora do telhado é recusado: %s" % off)


## The last part stands in front of the street wall by its standoff (lot frame: y grows away from the street).
static func _clear_of_wall(id: int, spec: Dictionary) -> bool:
	var info: Dictionary = CityStore.query(&"body_info", [id])
	return is_equal_approx(info.volumes[0].y - info.last_part_y, spec.standoff_m)


static func _count(id: int) -> int:
	return CityStore.query(&"body_info", [id]).parts


static func _digest(id: int) -> int:
	return CityStore.query(&"body_info", [id]).digest


## A point on the ground beyond the street front.
static func _away(id: int) -> Vector3:
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	return p.position + Vector3(sin(p.rot), 0, cos(p.rot)) * (p.d * 0.5 + FREE_OUT_M)
