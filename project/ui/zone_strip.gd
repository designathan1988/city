## The Zones tab of the drawer: a card per use in the strip and the density row in the options panel (data/ui/drawers.json
## zone_cards). A card picks the use; the density picks which zone of that use the brush paints (data/catalog/zones.json).
class_name ZoneStrip
extends RefCounted

const DENSITIES := [[0, "zone.low", "low"], [1, "zone.medium", "medium"], [2, "zone.high", "high"]]


static func build(drawer: HudDrawer) -> void:
	for c: Dictionary in drawer.category.cards:
		drawer.strip.add_child(_card(drawer, c))
	var items := []
	for d: Array in DENSITIES:
		items.append([d[0], "", d[2], "%s: %s" % [TranslationServer.translate("v2.zone.density"), TranslationServer.translate(d[1])]])
	var row := HudWidgets.option_row(drawer.options, TranslationServer.translate("v2.zone.density"))
	HudWidgets.choices(row, items, drawer.density, func(v: int) -> void:
		drawer.density = v
		pick(drawer, DrawerStrips.use_of(drawer.zone())))


## The card of this use lit and the tool asked for the zone of that use at the density chosen.
static func pick(drawer: HudDrawer, zone: int) -> void:
	for c in drawer.strip.get_children():
		if c is Button:
			c.set_pressed_no_signal(c.get_meta(&"zone", -1) == DrawerStrips.use_of(zone))
	drawer.tool_requested.emit("ZONE", {"zone": DrawerStrips.zone_at(DrawerStrips.use_of(zone), drawer.density)})


## The token a zone's card is painted with (its use's swatch), what the brush lights that zone's lots with; the eraser's for zone 0.
static func swatch_of(zone: int) -> String:
	for cat: Dictionary in DataTables.ui("drawers").categories:
		if cat.get("strip", "") != "zone_cards":
			continue
		for c: Dictionary in cat.cards:
			if c.zone == DrawerStrips.use_of(zone) and c.has("swatch"):
				return c.swatch
	return "preview_zone_erase"


static func _card(drawer: HudDrawer, c: Dictionary) -> Button:
	var variation := &"SwatchCard" if c.has("swatch") else &"Card"
	var b := HudWidgets.button(variation, c.get("icon", ""), TranslationServer.translate(c.label_key), "", "card_w", "card_h")
	b.toggle_mode = true
	b.set_meta(&"zone", c.zone)
	b.set_pressed_no_signal(c.zone == DrawerStrips.use_of(drawer.zone()))
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	if c.has("swatch"):
		b.icon = ThemeBuilder.swatch(c.swatch, int(HudWidgets.px("card_glyph")))
	b.pressed.connect(pick.bind(drawer, c.zone))
	return b
