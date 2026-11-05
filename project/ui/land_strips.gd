## The strips of the Landscape drawer (Relief, Ground, Nature, Sky), built from data/ui/drawers.json `land`: groups of
## cards that pick a tool parameter, sliders of the brush in hand or of the map's weather, the map's biome and the
## whole-map actions. The weather's numbers are read from the core, never kept here.
class_name LandStrips
extends RefCounted

const GRID_COLS := 8
const SLIDER_COLS := 2


## Fills `strip` with the panel of `tool_name` (drawer: hud_drawer.gd).
static func build(drawer: HudDrawer, strip: Container, tool_name: String) -> void:
	var s: Dictionary = DataTables.ui("land_panels").get(tool_name, {})
	if s.has("switch"):
		_switch(drawer, strip, s.switch, tool_name)
	for g in s.get("groups", []):
		_group(drawer, strip, g, tool_name)
	var mode: int = drawer.land_param(tool_name, "mode", 0)
	var col := HudWidgets.vbox(strip)
	var grid := GridContainer.new()
	grid.columns = SLIDER_COLS
	col.add_child(grid)
	for sl in s.get("sliders", []):
		if not sl.has("when") or sl.when == mode:
			_slider(drawer, grid, sl, tool_name)
	var row := HudWidgets.hbox(col)
	for a in s.get("actions", []):
		if not a.has("when") or a.when == mode:
			_action(drawer, row, a)
	if s.get("biomes", false):
		_biomes(drawer, col)


static func _switch(drawer: HudDrawer, strip: Container, cards: Array, tool_name: String) -> void:
	var grid := GridContainer.new()
	grid.columns = cards.size()
	strip.add_child(grid)
	for c in cards:
		var b := DrawerStrips.card(c, "tool", tool_name, drawer.land_switch)
		grid.add_child(b)


static func _group(drawer: HudDrawer, strip: Container, g: Dictionary, tool_name: String) -> void:
	var grid := GridContainer.new()
	grid.columns = mini(GRID_COLS, g.cards.size())
	strip.add_child(grid)
	var picked: Variant = drawer.land_param(tool_name, g.key, g.default)
	for c in g.cards:
		var b := DrawerStrips.card(c, "value", picked, drawer.land_pick.bind(tool_name, g.key))
		if c.has("swatch"):
			b.icon = ThemeBuilder.swatch(c.swatch, int(HudWidgets.px("card_glyph")))
		grid.add_child(b)


static func _slider(drawer: HudDrawer, grid: Container, sl: Dictionary, tool_name: String) -> void:
	var unit: String = sl.get("unit", "")
	if sl.has("weather"):
		var scale: float = sl.get("scale", 1.0)
		var now: float = CityStore.query(&"landscape").weather[sl.weather] * scale
		DrawerStrips.slider(grid, sl.label_key, sl.min, sl.max, sl.step, now, unit, func(v: float) -> void:
			CityStore.dispatch(&"set_weather", [{sl.weather: v / scale}]))
	else:
		var now: float = drawer.land_param(tool_name, sl.key, sl.default)
		DrawerStrips.slider(grid, sl.label_key, sl.min, sl.max, sl.step, now, unit, drawer.land_slide.bind(tool_name, sl.key))


static func _action(drawer: HudDrawer, row: Container, a: Dictionary) -> void:
	var b := HudWidgets.button(&"Card", a.get("icon", ""), TranslationServer.translate(a.label_key), "", "card_w", "tab_h")
	b.pressed.connect(drawer.land_action.bind(a.action))
	row.add_child(b)


static func _biomes(_drawer: HudDrawer, col: Container) -> void:
	var row := HudWidgets.hbox(col)
	HudWidgets.label(row, &"MutedLabel", TranslationServer.translate("v2.row.biome"))
	var now: int = CityStore.query(&"landscape").biome
	var nature: Dictionary = DataTables.catalog_table("nature")
	var biomes := [{"id": "none", "slot": -1, "swatch": "paint_none"}]
	for k in nature.paint.kinds:
		if k["class"] == "biome":
			biomes.append({"id": k.id, "slot": int(k.slot), "swatch": "paint_" + k.id})
	for b in biomes:
		var button := HudWidgets.button(&"Card", "", "", "", "row_h", "row_h")
		button.toggle_mode = true
		button.set_meta(&"pick", b.slot)
		button.set_pressed_no_signal(b.slot == now)
		button.icon = ThemeBuilder.swatch(b.swatch, int(HudWidgets.px("swatch")))
		button.tooltip_text = "%s: %s" % [TranslationServer.translate("terrain.mapBiome"),
				TranslationServer.translate("terrain.mapBiome.none" if b.slot < 0 else "paint.kind." + b.id)]
		button.pressed.connect(func() -> void:
			CityStore.dispatch(&"set_biome", [b.slot])
			DrawerStrips.light(row, b.slot))
		row.add_child(button)
