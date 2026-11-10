## Every part of the Parts tab set against a wall of a building (building_edit_test.gd): it lands or is refused with a text, undo takes
## it away, and the free placement, the turn and the row work on a part that landed. The things of the Roof tab land on the roof, and
## the flag of a mast is set by its pattern and colours.

const SIDE := 0
const FREE_OUT_M := 30.0
const FLAG_EDITS := 4
const SECOND_COLOUR := 1


static func run(id: int, made: int, check: Callable) -> void:
	_gear(id, made, check)
	_flags(id, made, check)
	var landed := 0
	var spec_of_last := {}
	for section: Dictionary in DataTables.ui("builder").tabs.parts.sections:
		for spec: Dictionary in section.get("tools", []):
			var before: int = _count(id)
			var r: Dictionary = CityStore.dispatch(&"body_add_part", [id, 0, 1, SIDE, 0, spec])
			if r.get("ok", false):
				landed += 1
				spec_of_last = spec
				check.call(_count(id) == before + 1, "%s went into the body" % spec.id)
				if spec.get("standoff_m", 0.0) > 0.0 and spec.fit_kind == 0:
					check.call(_clear_of_wall(id, spec), "%s stands %.1f m from the wall" % [spec.id, spec.standoff_m])
				check.call(CityStore.dispatch(&"undo") and _digest(id) == made, "%s desfeito" % spec.id)
			else:
				var why: String = r.get("error", "")
				check.call(why != "" and TranslationServer.translate(why) != why, "%s refused with a text: %s" % [spec.id, r])
	check.call(landed > 0, "%d pieces went in" % landed)
	var free: Dictionary = CityStore.dispatch(&"body_add_part_at", [id, _away(id), spec_of_last])
	check.call(free.get("ok", false), "a loose piece on the ground: %s" % free)
	var last: int = _count(id) - 1
	check.call(CityStore.dispatch(&"body_turn_part", [id, last]).get("ok", false), "piece turned")
	check.call(CityStore.dispatch(&"body_remove_part", [id, last]).get("ok", false), "piece removed")
	check.call(_count(id) == last, "the piece left the body")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	check.call(_digest(id) == made, "three undos return to the generated body")


## A lantern flying a flag of its own colours, left on the body (the save round trip carries it).
static func fly_flag(id: int, check: Callable) -> void:
	var tools := _roof_tools()
	var middle := _middle(id)
	check.call(CityStore.dispatch(&"body_add_gear", [id, 0, middle, tools.lantern]).get("ok", false), "a lantern to save")
	check.call(CityStore.dispatch(&"body_flag_colour", [id, 0, middle, SECOND_COLOUR, tools.flagColour6.rgb]).get("ok", false), "a coloured flag to save")
	check.call(_flags_of(id) == 1, "the saved lantern flies the flag")


## Each thing of the roof lands on it or says why not, and undo takes it away; off the roof it is refused.
static func _gear(id: int, made: int, check: Callable) -> void:
	var middle := _middle(id)
	var landed := 0
	for section: Dictionary in DataTables.ui("builder").tabs.roof.sections:
		for spec: Dictionary in section.get("tools", []):
			if spec.do != "gear":
				continue
			var r: Dictionary = CityStore.dispatch(&"body_add_gear", [id, 0, middle, spec])
			if r.get("ok", false):
				landed += 1
				check.call(CityStore.query(&"body_info", [id]).volumes[0].gear == 1, "%s on the roof" % spec.id)
				check.call(CityStore.dispatch(&"undo") and _digest(id) == made, "%s desfeito" % spec.id)
			else:
				check.call(r.get("error", "") == "building.problem.roofPart", "%s does not fit: %s" % [spec.id, r])
	check.call(landed > 0, "%d pieces of gear went on the roof" % landed)
	var off: Dictionary = CityStore.dispatch(&"body_add_gear", [id, 0, _away(id), _roof_tools().solar])
	check.call(off.get("error", "") == "building.problem.roofPart", "off the roof is refused: %s" % off)


## The flag of a mast: a lantern comes flying the default one, a spire none; the pattern and a colour change the body, none
## takes it down, colouring a bare spire raises it, a point with no mast is refused, and undo brings the generated body back.
static func _flags(id: int, made: int, check: Callable) -> void:
	var tools := _roof_tools()
	var middle := _middle(id)
	check.call(CityStore.dispatch(&"body_add_gear", [id, 0, middle, tools.lantern]).get("ok", false), "lantern on the roof")
	check.call(_flags_of(id) == 1, "the lantern already flies the flag")
	var before: int = _digest(id)
	check.call(CityStore.dispatch(&"body_set_flag", [id, 0, middle, tools.flagCross.value]).get("ok", false) and _digest(id) != before, "the flag's design changed")
	before = _digest(id)
	check.call(CityStore.dispatch(&"body_flag_colour", [id, 0, middle, SECOND_COLOUR, tools.flagColour6.rgb]).get("ok", false) and _digest(id) != before, "the flag's colour changed")
	check.call(CityStore.dispatch(&"body_set_flag", [id, 0, middle, tools.flagNone.value]).get("ok", false) and _flags_of(id) == 0, "bandeira arriada")
	check.call(CityStore.dispatch(&"body_set_flag", [id, 0, _away(id), tools.flagCross.value]).get("error", "") == "building.problem.noMast", "with no mast it is refused")
	for i in FLAG_EDITS:
		CityStore.dispatch(&"undo")
	check.call(_digest(id) == made, "undo gives the generated body back")
	check.call(CityStore.dispatch(&"body_add_gear", [id, 0, middle, tools.spire]).get("ok", false) and _flags_of(id) == 0, "the spire starts with no flag")
	check.call(CityStore.dispatch(&"body_flag_colour", [id, 0, middle, 0, tools.flagColour2.rgb]).get("ok", false) and _flags_of(id) == 1, "colouring the spire flies the flag")
	CityStore.dispatch(&"undo")
	CityStore.dispatch(&"undo")
	check.call(_digest(id) == made, "the spire undone gives the generated body back")


## The tools of the Roof tab by id.
static func _roof_tools() -> Dictionary:
	var tools := {}
	for section: Dictionary in DataTables.ui("builder").tabs.roof.sections:
		for spec: Dictionary in section.get("tools", []):
			tools[spec.id] = spec
	return tools


## The middle of the first block's roof, in the world.
static func _middle(id: int) -> Vector3:
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var v: Dictionary = CityStore.query(&"body_info", [id]).volumes[0]
	var s: float = sin(p.rot)
	var c: float = cos(p.rot)
	return p.position + Vector3(s, 0, c) * (p.d * 0.5) + Vector3(c, 0, -s) * (v.x + v.w * 0.5) + Vector3(-s, 0, -c) * (v.y + v.d * 0.5)


static func _flags_of(id: int) -> int:
	return CityStore.query(&"body_info", [id]).volumes[0].flags


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
