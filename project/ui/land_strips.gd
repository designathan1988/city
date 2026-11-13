## The Landscape drawer as the panel of each tool (data/ui/land_panels.json): groups of cards in the strip, a group apart by a
## gap; on the left, in the options panel, the brush row, its sliders or the map's weather, and what acts on the whole map (the
## biome, the actions). The weather's numbers are read from the core, never kept here.
class_name LandStrips
extends RefCounted

const CARD_ROWS := 2 # a group of cards stands in two rows unless the panel says otherwise
const MAP_HEAD := "v2.map"
const CLEAR_ROW := "v2.row.clear"
const METRES := " m"
const LONG_NAME := " (" # a card or a row keeps the name before this


## Fills `strip` and the options panel with the panel of `tool_name` (drawer: hud_drawer.gd).
static func build(drawer: HudDrawer, strip: Container, tool_name: String) -> void:
	var s: Dictionary = DataTables.ui("land_panels").get(tool_name, {})
	if s.has("switch"):
		_switch(drawer, strip, s.switch, tool_name)
	for g: Dictionary in s.get("groups", []):
		if g.get("options", false):
			_brush(drawer, g, tool_name)
		else:
			_group(drawer, strip, g, tool_name)
	var mode: int = drawer.param(tool_name, "mode", 0)
	for sl: Dictionary in s.get("sliders", []):
		if not sl.has("when") or sl.when == mode:
			_slider(drawer, sl, tool_name)
	_whole_map(drawer, s, mode)


static func _cards(strip: Container, count: int, rows: int) -> GridContainer:
	if strip.get_child_count() > 0:
		var gap := Control.new()
		gap.custom_minimum_size.x = HudWidgets.px("gap")
		strip.add_child(gap)
	var grid := GridContainer.new()
	grid.columns = ceili(float(count) / rows)
	strip.add_child(grid)
	return grid


static func _short(text: String) -> String:
	return text.get_slice(LONG_NAME, 0)


static func _switch(drawer: HudDrawer, strip: Container, cards: Array, tool_name: String) -> void:
	var grid := _cards(strip, cards.size(), 1)
	for c: Dictionary in cards:
		grid.add_child(DrawerStrips.card(c, "tool", tool_name, LandHands.switch_to.bind(drawer)))


static func _group(drawer: HudDrawer, strip: Container, g: Dictionary, tool_name: String) -> void:
	var rows := int(g.get("rows", CARD_ROWS))
	var grid := _cards(strip, g.cards.size(), rows)
	var picked: Variant = drawer.param(tool_name, g.key, g.default)
	for at in grid.columns * rows: # the cards run down each column, the grid fills along each row
		var i: int = at % grid.columns * rows + at / grid.columns
		if i >= g.cards.size():
			grid.add_child(Control.new())
			continue
		var c: Dictionary = g.cards[i]
		var b := DrawerStrips.card(c, "value", picked, LandHands.pick.bind(tool_name, g.key, drawer))
		b.text = _short(b.text)
		if c.has("swatch"):
			b.theme_type_variation = &"SwatchCard"
			b.icon = ThemeBuilder.swatch(c.swatch, int(HudWidgets.px("card_glyph")))
		grid.add_child(b)


## What the brush does, in the options panel: an icon each.
static func _brush(drawer: HudDrawer, g: Dictionary, tool_name: String) -> void:
	var items := []
	for c: Dictionary in g.cards:
		items.append([c.value, "", c.icon, TranslationServer.translate(c.label_key)])
	var row := HudWidgets.option_row(drawer.options, TranslationServer.translate("v2.row.brush"))
	HudWidgets.choices(row, items, drawer.param(tool_name, g.key, g.default), LandHands.pick.bind(tool_name, g.key, drawer))


static func _head(drawer: HudDrawer, key: String) -> void:
	drawer.options.add_child(HSeparator.new())
	HudWidgets.label(drawer.options, &"TitleLabel", TranslationServer.translate(key).to_upper())


## How a slider writes its value: a length as the style writes it, anything else with the unit of the table.
static func _shown(sl: Dictionary) -> Callable:
	var unit: String = sl.get("unit", "")
	if unit == METRES:
		return Formatters.format_length
	var whole: bool = sl.step >= 1.0
	return func(v: float) -> String: return (str(int(v)) if whole else Formatters.format_tenths(v)) + unit


static func _slider(drawer: HudDrawer, sl: Dictionary, tool_name: String) -> void:
	if sl.has("head"):
		_head(drawer, sl.head)
	var label := _short(TranslationServer.translate(sl.label_key))
	if sl.has("weather"):
		var scale: float = sl.get("scale", 1.0)
		var now: float = CityStore.query(&"landscape").weather[sl.weather] * scale
		HudWidgets.range_row(drawer.options, label, sl.min, sl.max, sl.step, now, _shown(sl), func(v: float) -> void:
			CityStore.dispatch(&"set_weather", [{sl.weather: v / scale}]))
	else:
		var now: float = drawer.param(tool_name, sl.key, sl.default)
		HudWidgets.range_row(drawer.options, label, sl.min, sl.max, sl.step, now, _shown(sl), drawer.retool.bind(tool_name, sl.key))


## The map's biome and the actions on the whole map, under their heading.
static func _whole_map(drawer: HudDrawer, s: Dictionary, mode: int) -> void:
	var actions: Array = s.get("actions", []).filter(func(a: Dictionary) -> bool: return not a.has("when") or a.when == mode)
	var biomes: bool = s.get("biomes", false)
	if s.get("map_head", true) and (biomes or not actions.is_empty()):
		_head(drawer, MAP_HEAD)
	if biomes:
		_biomes(drawer.options)
	if not actions.is_empty():
		var row := HudWidgets.option_row(drawer.options, TranslationServer.translate(s.get("actions_row", CLEAR_ROW)))
		for a: Dictionary in actions:
			var b := HudWidgets.button(&"IconButton", a.get("icon", ""), "", a.label_key, "icon_btn", "tab_h")
			b.pressed.connect(LandHands.action.bind(a.action, drawer))
			row.add_child(b)


static func _biomes(col: Container) -> void:
	var row := HudWidgets.hbox(HudWidgets.option_row(col, TranslationServer.translate("v2.row.biome")))
	var now: int = CityStore.query(&"landscape").biome
	var nature: Dictionary = DataTables.catalog_table("nature")
	var biomes := [{"id": "none", "slot": -1, "swatch": "paint_none"}]
	for k in nature.paint.kinds:
		if k["class"] == "biome":
			biomes.append({"id": k.id, "slot": int(k.slot), "swatch": "paint_" + k.id})
	for b in biomes:
		var button := HudWidgets.button(&"SwatchCard", "", "", "", "biome_w", "biome_w")
		button.toggle_mode = true
		button.set_meta(&"pick", b.slot)
		button.set_pressed_no_signal(b.slot == now)
		button.icon = ThemeBuilder.swatch(b.swatch, int(HudWidgets.px("biome_swatch")))
		button.tooltip_text = "%s: %s" % [TranslationServer.translate("terrain.mapBiome"),
				TranslationServer.translate("terrain.mapBiome.none" if b.slot < 0 else "paint.kind." + b.id)]
		button.pressed.connect(func() -> void: # lit once the core took it; refused, the map's own biome stays lit
			var answer: Dictionary = CityStore.dispatch(&"set_biome", [b.slot])
			DrawerStrips.light(row, b.slot if answer.get("ok", false) else int(CityStore.query(&"landscape").biome)))
		row.add_child(button)
