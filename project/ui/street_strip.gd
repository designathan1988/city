## The Landscape drawer's Street tab (Roadcraft shell.ts renderLandscape): the pieces in two rows, the barriers, the poles' two
## verbs, and under them the options of what is in hand: a sign's type and words, a street's name, the lights of a new line.
## Everything it lists comes from data/ui/drawers.json "street".
class_name StreetStrip
extends RefCounted


## What the tab puts in hand when it opens for the first time: its first piece.
static func first_piece(table: Dictionary) -> Dictionary:
	return _item_params(table, table.items[0], func(_t: String, _k: String, fallback: Variant) -> Variant: return fallback)


## Fills `strip`. `in_hand` names the tool held; `use(tool, params, rebuild)` puts one in hand; `last(tool, key, fallback)`
## reads what a tool was last used with (key "": all of it).
static func build(strip: Control, table: Dictionary, in_hand: String, use: Callable, last: Callable) -> void:
	var body := HudWidgets.vbox(strip)
	var cards := HudWidgets.hbox(body)
	_cards(cards, table.items, table.item_columns, last.call("STREETSCAPE", "kind", null) if in_hand == "STREETSCAPE" else null,
		func(c: Dictionary) -> void: use.call("STREETSCAPE", _item_params(table, c, last)))
	cards.add_child(VSeparator.new())
	_cards(cards, table.barriers, table.barrier_columns, last.call("BARRIER", "kind", null) if in_hand == "BARRIER" else null,
		func(c: Dictionary) -> void: use.call("BARRIER", _barrier_params(table, c)))
	cards.add_child(VSeparator.new())
	var lamps: Variant = last.call("POLE", "lamps", table.default_lamps)
	_cards(cards, table.poles, 1, last.call("POLE", "mode", null) if in_hand == "POLE" else null,
		func(c: Dictionary) -> void: use.call("POLE", _pole_params(table, c.kind, lamps)))
	if in_hand == "POLE" and last.call("POLE", "mode", "") == "build":
		_lamp_row(HudWidgets.hbox(body), table, lamps, use)
	elif in_hand == "STREETSCAPE":
		_item_options(body, table, use, last)


## The lights of the next line (Roadcraft: Luzes, three choices).
static func _lamp_row(row: Control, table: Dictionary, lamps: Variant, use: Callable) -> void:
	HudWidgets.label(row, &"MutedLabel", TranslationServer.translate("v2.row.lights"))
	for c: Dictionary in table.lamps:
		var b := HudWidgets.button(&"DrawerTab", c.icon, TranslationServer.translate(c.label_key), "pole.lamps", "", "tab_h")
		b.toggle_mode = true
		b.set_pressed_no_signal(c.kind == lamps)
		b.pressed.connect(func() -> void: use.call("POLE", _pole_params(table, "build", c.kind)))
		row.add_child(b)


## Which sign and the words of those that carry them, or a street's name. Typing keeps the field: the strip stays as it is.
static func _item_options(body: Control, table: Dictionary, use: Callable, last: Callable) -> void:
	var kind: Variant = last.call("STREETSCAPE", "kind", -1)
	if kind == table.sign_kind:
		var row := HudWidgets.hbox(body)
		HudWidgets.label(row, &"MutedLabel", TranslationServer.translate("v2.row.sign"))
		var sign: int = last.call("STREETSCAPE", "sign", 0)
		var kinds := OptionButton.new()
		kinds.focus_mode = Control.FOCUS_NONE
		for c: Dictionary in table.signs:
			kinds.add_item(TranslationServer.translate(c.label_key), c.sign)
		kinds.select(kinds.get_item_index(sign))
		row.add_child(kinds)
		kinds.item_selected.connect(func(i: int) -> void:
			var held: Dictionary = last.call("STREETSCAPE", "", {})
			held.sign = kinds.get_item_id(i)
			use.call("STREETSCAPE", held))
		var hint: String = table.signs[kinds.get_item_index(sign)].get("text_hint", "")
		if not hint.is_empty():
			_words_row(body, table, "text", hint, use, last)
	elif kind == table.name_kind:
		_words_row(body, table, "street_name", table.name_hint, use, last)


static func _words_row(body: Control, table: Dictionary, key: String, hint: String, use: Callable, last: Callable) -> void:
	var row := HudWidgets.hbox(body)
	HudWidgets.label(row, &"MutedLabel", TranslationServer.translate("v2.row.text"))
	var field := LineEdit.new()
	field.placeholder_text = TranslationServer.translate(hint)
	field.text = last.call("STREETSCAPE", key, "")
	field.custom_minimum_size.x = HudWidgets.px("side_w")
	field.max_length = int(table.text_max)
	row.add_child(field)
	field.text_submitted.connect(func(_t: String) -> void: field.release_focus())
	field.text_changed.connect(func(t: String) -> void:
		var held: Dictionary = last.call("STREETSCAPE", "", {})
		held[key] = t
		held.words = t
		use.call("STREETSCAPE", held, false))


## A grid of cards, each named without the aside in brackets, which its tooltip keeps (Roadcraft shell.ts card).
static func _cards(parent: Control, list: Array, columns: int, held: Variant, pick: Callable) -> void:
	var grid := GridContainer.new()
	grid.columns = columns
	parent.add_child(grid)
	for c: Dictionary in list:
		var b := DrawerStrips.card(c, "kind", held, func(_v: Variant) -> void: pick.call(c))
		b.tooltip_text = b.text
		b.text = b.text.get_slice(" (", 0)
		b.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		grid.add_child(b)


static func _item_params(table: Dictionary, c: Dictionary, last: Callable) -> Dictionary:
	var text: String = last.call("STREETSCAPE", "text", "")
	var street_name: String = last.call("STREETSCAPE", "street_name", "")
	return {
		"kind": c.kind, "reach_m": table.reach_m, "placing_ring_m": table.placing_ring_m, "row_m": c.get("row_m", 0.0),
		"sign": last.call("STREETSCAPE", "sign", 0), "text": text, "street_name": street_name,
		"words": street_name if c.kind == table.name_kind else text,
	}


static func _barrier_params(table: Dictionary, c: Dictionary) -> Dictionary:
	return {"kind": c.kind, "reach_m": table.reach_m, "half_width_m": c.half_width_m, "double_click_ms": table.double_click_ms, "double_click_m": table.double_click_m}


static func _pole_params(table: Dictionary, mode: Variant, lamps: Variant) -> Dictionary:
	return {"mode": mode, "lamps": lamps, "reach_m": table.pole_reach_m}
