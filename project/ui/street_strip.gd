## The Landscape drawer's Street tab (Roadcraft shell.ts renderLandscape): the pieces in two rows, the barriers, the poles' two
## verbs, and under them the options of what is in hand: a sign's type and words, a street's name, the lights of a new line.
## Everything it lists comes from data/ui/drawers.json "street".
class_name StreetStrip
extends RefCounted


## What the tab puts in hand when it opens for the first time: its first piece.
static func first_piece(table: Dictionary) -> Dictionary:
	return _item_params(table, table.items[0], func(_t: String, _k: String, fallback: Variant) -> Variant: return fallback)


## The tab in `strip`: the tool held read from `tools`, a card putting its tool in hand through `requested`, and the strip again
## when what it offers follows what is in hand.
static func show(strip: Control, tools: ToolController, requested: Signal) -> void:
	HudWidgets.clear(strip)
	var in_hand: String = ToolController.Tool.find_key(tools.tool) if tools else ""
	var last := func(tool: String, key: String, fallback: Variant) -> Variant:
		var held: Dictionary = tools.remembered.get(ToolController.Tool[tool], {}) if tools else {}
		return held.duplicate() if key.is_empty() else held.get(key, fallback)
	var use := func(tool: String, params: Dictionary, rebuild: bool) -> void:
		requested.emit(tool, params)
		if rebuild:
			show(strip, tools, requested)
	build(strip, DataTables.ui("drawers").street, in_hand, use, last)


## Fills `strip`. `in_hand` names the tool held; `use(tool, params, rebuild)` puts one in hand; `last(tool, key, fallback)`
## reads what a tool was last used with (key "": all of it).
static func build(strip: Control, table: Dictionary, in_hand: String, use: Callable, last: Callable) -> void:
	var body := HudWidgets.vbox(strip)
	var cards := HudWidgets.hbox(body)
	_cards(cards, table.items, table.item_columns, last.call("STREETSCAPE", "kind", null) if in_hand == "STREETSCAPE" else null,
		func(c: Dictionary) -> void: use.call("STREETSCAPE", _item_params(table, c, last), true))
	cards.add_child(VSeparator.new())
	_cards(cards, table.barriers, table.barrier_columns, last.call("BARRIER", "kind", null) if in_hand == "BARRIER" else null,
		func(c: Dictionary) -> void: use.call("BARRIER", _barrier_params(table, c), true))
	cards.add_child(VSeparator.new())
	var lamps: Variant = last.call("POLE", "lamps", table.default_lamps)
	var kind: Variant = last.call("POLE", "kind", table.default_pole_kind)
	_cards(cards, table.poles, 1, last.call("POLE", "mode", null) if in_hand == "POLE" else null,
		func(c: Dictionary) -> void: use.call("POLE", _pole_params(table, c.kind, lamps, kind), true))
	if in_hand == "POLE" and last.call("POLE", "mode", "") == "build":
		# Roadcraft: Poste (urban or rural) and Luzes (three choices).
		_choice_row(HudWidgets.hbox(body), "v2.row.poleKind", table.pole_kinds, kind, "pole.kind",
			func(c: Dictionary) -> void: use.call("POLE", _pole_params(table, "build", lamps, c.kind), true))
		_choice_row(HudWidgets.hbox(body), "v2.row.lights", table.lamps, lamps, "pole.lamps",
			func(c: Dictionary) -> void: use.call("POLE", _pole_params(table, "build", c.kind, kind), true))
	elif in_hand == "STREETSCAPE":
		_item_options(body, table, use, last)


## A choice for the next line: `label` and a button per entry of `choices`, the one whose kind is `current` pressed.
static func _choice_row(row: Control, label: String, choices: Array, current: Variant, tip: String, pick: Callable) -> void:
	HudWidgets.label(row, &"MutedLabel", TranslationServer.translate(label))
	for c: Dictionary in choices:
		var b := HudWidgets.button(&"DrawerTab", c.icon, TranslationServer.translate(c.label_key), tip, "", "tab_h")
		b.toggle_mode = true
		b.set_pressed_no_signal(c.kind == current)
		b.pressed.connect(pick.bind(c))
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
			use.call("STREETSCAPE", held, true))
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


## A grid of cards, each by its short name (label_key) with its whole name as the tooltip (tip_key, when it has one).
static func _cards(parent: Control, list: Array, columns: int, held: Variant, pick: Callable) -> void:
	var grid := GridContainer.new()
	grid.columns = columns
	parent.add_child(grid)
	for c: Dictionary in list:
		var b := DrawerStrips.card(c, "kind", held, func(_v: Variant) -> void: pick.call(c))
		b.tooltip_text = TranslationServer.translate(c.get("tip_key", c.label_key))
		b.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		grid.add_child(b)


static func _item_params(table: Dictionary, c: Dictionary, last: Callable) -> Dictionary:
	var text: String = last.call("STREETSCAPE", "text", "")
	var street_name: String = last.call("STREETSCAPE", "street_name", "")
	return {
		"kind": c.kind, "reach_m": table.reach_m, "pick_px": table.pick_px, "placing_ring_m": table.placing_ring_m, "row_m": c.get("row_m", 0.0),
		"sign": last.call("STREETSCAPE", "sign", 0), "text": text, "street_name": street_name,
		"words": street_name if c.kind == table.name_kind else text,
	}


static func _barrier_params(table: Dictionary, c: Dictionary) -> Dictionary:
	return {"kind": c.kind, "reach_m": table.reach_m, "half_width_m": c.half_width_m, "double_click_ms": table.double_click_ms, "double_click_m": table.double_click_m}


static func _pole_params(table: Dictionary, mode: Variant, lamps: Variant, kind: Variant) -> Dictionary:
	return {"mode": mode, "lamps": lamps, "kind": kind, "reach_m": table.pole_reach_m}
