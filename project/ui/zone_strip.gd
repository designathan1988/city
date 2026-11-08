## The Zones tab of the drawer: a card per use in the strip and the density row in the options panel (data/ui/drawers.json
## zone_cards). A card picks the use; the density picks which zone of that use the brush paints (data/catalog/zones.json).
class_name ZoneStrip
extends RefCounted

static func build(drawer: HudDrawer) -> void:
	for c: Dictionary in drawer.category.cards:
		drawer.strip.add_child(_card(drawer, c))
	var row := HudWidgets.option_row(drawer.options, TranslationServer.translate("v2.zone.density"))
	var densities := HudWidgets.choices(row, drawer.density, func(v: int) -> void:
		drawer.density = v
		pick(drawer, DrawerStrips.use_of(drawer.zone())))
	for d: Dictionary in DataTables.ui("drawers").zone_densities: # zones.json's densities, as the row shows them
		var tip := "%s: %s" % [TranslationServer.translate("v2.zone.density"), TranslationServer.translate(d.label_key)]
		HudWidgets.choice(densities, int(d.density), "", d.icon, tip)


## The card of this use lit and the tool asked for the zone of that use at the density chosen.
static func pick(drawer: HudDrawer, zone: int) -> void:
	for c in drawer.strip.get_children():
		if c is Button:
			c.set_pressed_no_signal(c.get_meta(&"zone", -1) == DrawerStrips.use_of(zone))
	drawer.tool_requested.emit("ZONE", {"zone": DrawerStrips.zone_at(DrawerStrips.use_of(zone), drawer.density)})


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
