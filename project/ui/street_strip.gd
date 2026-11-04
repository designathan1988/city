## The Landscape drawer's Street tab (Roadcraft shell.ts renderLandscape): the pieces in two rows, the barriers, the poles' two
## verbs, and the options of what is in hand: a sign's type and words, a street's name, the lights of a new line. Everything
## it lists comes from data/ui/drawers.json "street".
class_name StreetStrip
extends RefCounted


## What the tab puts in hand when it opens for the first time: its first piece.
static func first_piece(table: Dictionary) -> Dictionary:
	return _item_params(table, table.items[0], func(_t: String, _k: String, fallback: Variant) -> Variant: return fallback)


## Fills `strip`. `in_hand` names the tool held; `use(tool, params, rebuild)` puts one in hand; `last(tool, key, fallback)`
## reads what a tool was last used with (key "": all of it).
static func build(strip: Control, table: Dictionary, in_hand: String, use: Callable, last: Callable) -> void:
	_cards(strip, table.items, table.item_columns, "kind", last.call("STREETSCAPE", "kind", null) if in_hand == "STREETSCAPE" else null,
		func(c: Dictionary) -> void: use.call("STREETSCAPE", _item_params(table, c, last)))
	_cards(strip, table.barriers, table.barrier_columns, "kind", last.call("BARRIER", "kind", null) if in_hand == "BARRIER" else null,
		func(c: Dictionary) -> void: use.call("BARRIER", _barrier_params(table, c)))
	var lamps: Variant = last.call("POLE", "lamps", table.default_lamps)
	_cards(strip, table.poles, 1, "kind", last.call("POLE", "mode", null) if in_hand == "POLE" else null,
		func(c: Dictionary) -> void: use.call("POLE", _pole_params(table, c.kind, lamps)))
	if in_hand == "POLE" and last.call("POLE", "mode", "") == "build":
		var row := HudWidgets.hbox(strip)
		for c: Dictionary in table.lamps:
			row.add_child(DrawerStrips.card(c, "kind", lamps, func(v: Variant) -> void: use.call("POLE", _pole_params(table, "build", v))))
	elif in_hand == "STREETSCAPE":
		_item_options(strip, table, use, last)


## The words on a sign or a street name, and the type of a sign. Typing keeps the field: the strip is not built again.
static func _item_options(strip: Control, table: Dictionary, use: Callable, last: Callable) -> void:
	var side := HudWidgets.vbox(strip)
	if last.call("STREETSCAPE", "kind", -1) == table.sign_kind:
		var kinds := OptionButton.new()
		for c: Dictionary in table.signs:
			kinds.add_item(TranslationServer.translate(c.label_key), c.sign)
		kinds.select(kinds.get_item_index(last.call("STREETSCAPE", "sign", 0)))
		side.add_child(kinds)
		kinds.item_selected.connect(func(i: int) -> void: _set_item_param("sign", kinds.get_item_id(i), use, last))
	if last.call("STREETSCAPE", "text_field", false):
		var field := LineEdit.new()
		field.placeholder_text = TranslationServer.translate(last.call("STREETSCAPE", "text_hint", ""))
		field.text = last.call("STREETSCAPE", "text", "")
		field.custom_minimum_size.x = HudWidgets.px("side_w")
		field.max_length = int(table.text_max)
		side.add_child(field)
		field.text_changed.connect(func(t: String) -> void: _set_item_param("text", t, use, last))


static func _set_item_param(key: String, value: Variant, use: Callable, last: Callable) -> void:
	var held: Dictionary = last.call("STREETSCAPE", "", {})
	held[key] = value
	use.call("STREETSCAPE", held, false)


static func _cards(strip: Control, list: Array, columns: int, key: String, held: Variant, pick: Callable) -> void:
	var grid := GridContainer.new()
	grid.columns = columns
	strip.add_child(grid)
	for c: Dictionary in list:
		grid.add_child(DrawerStrips.card(c, key, held, func(_v: Variant) -> void: pick.call(c)))


static func _item_params(table: Dictionary, c: Dictionary, last: Callable) -> Dictionary:
	return {
		"kind": c.kind, "reach_m": table.reach_m, "placing_ring_m": table.placing_ring_m, "row_m": c.get("row_m", 0.0),
		"sign": last.call("STREETSCAPE", "sign", 0), "text": last.call("STREETSCAPE", "text", ""),
		"text_field": c.has("text_hint"), "text_hint": c.get("text_hint", ""),
	}


static func _barrier_params(table: Dictionary, c: Dictionary) -> Dictionary:
	return {"kind": c.kind, "reach_m": table.reach_m, "half_width_m": c.half_width_m, "double_click_ms": table.double_click_ms}


static func _pole_params(table: Dictionary, mode: Variant, lamps: Variant) -> Dictionary:
	return {"mode": mode, "lamps": lamps, "reach_m": table.pole_reach_m}
