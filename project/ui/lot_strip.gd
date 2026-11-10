## The Lots tab of Zones: a card per gesture, and how the split cuts in the options (data/ui/drawers.json lot_cards; Roadcraft shell.ts).
class_name LotStrip
extends RefCounted


static func build(drawer: HudDrawer) -> void:
	var spec: Dictionary = DataTables.ui("drawers").lot_cards
	var mode: String = drawer.param("LOTS", "mode", spec.cards[0].mode)
	for c: Dictionary in spec.cards:
		drawer.strip.add_child(DrawerStrips.card(c, "mode", mode, drawer.retool.bind("LOTS", "mode")))
	if mode != "split":
		return
	var cut: int = drawer.param("LOTS", "cut", 0)
	var row := HudWidgets.option_row(drawer.options, TranslationServer.translate("v2.row.cut"))
	var items := []
	for k: Dictionary in spec.cuts:
		items.append([int(k.kind), "", k.icon, TranslationServer.translate(k.label_key)])
	HudWidgets.choices(row, items, cut, func(v: int) -> void:
		drawer.retool(v, "LOTS", "cut")
		drawer.rebuild())
	if cut == int(spec.line_cut):
		return
	row = HudWidgets.option_row(drawer.options, TranslationServer.translate("v2.row.parts"))
	var parts := []
	for n: int in range(int(spec.parts_least), int(spec.parts_most) + 1):
		parts.append([n, str(n), "", TranslationServer.translate("zone.split.into")])
	HudWidgets.choices(row, parts, drawer.param("LOTS", "parts", int(spec.parts_least)), func(v: int) -> void: drawer.retool(v, "LOTS", "parts"))
