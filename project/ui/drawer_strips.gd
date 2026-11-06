## Strip parts of the drawers that are not the road catalogue (Draw, Relief): icon cards that pick a tool parameter. Texts are
## translated keys, sizes are tokens; the values go back to the tool through the callbacks.
class_name DrawerStrips
extends RefCounted


## A card that, when pressed, calls on_pick(value); `key` names the parameter it stands for (kept as meta).
static func card(c: Dictionary, key: String, picked: Variant, on_pick: Callable) -> Button:
	var b := HudWidgets.button(&"Card", c.get("icon", ""), TranslationServer.translate(c.label_key), "", "card_w", "card_h")
	b.toggle_mode = true
	b.set_meta(&"pick", c[key])
	b.set_pressed_no_signal(c[key] == picked)
	b.vertical_icon_alignment = VERTICAL_ALIGNMENT_TOP
	b.icon_alignment = HORIZONTAL_ALIGNMENT_CENTER
	b.expand_icon = false
	b.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	b.pressed.connect(func() -> void: on_pick.call(c[key]))
	return b


## Lights the card whose value is `picked` among the strip's cards.
static func light(strip: Node, picked: Variant) -> void:
	for c in strip.get_children():
		if c is Button and c.has_meta(&"pick"):
			c.set_pressed_no_signal(c.get_meta(&"pick") == picked)


## The zone of this use at this density; a use with one density has only that.
static func zone_at(use: int, density: int) -> int:
	for z: Dictionary in DataTables.catalog_table("zones").zones:
		if int(z.base) == use and int(z.density) == density:
			return int(z.code)
	return use


## The zone's base use, whatever density it was painted with.
static func use_of(zone: int) -> int:
	for z: Dictionary in DataTables.catalog_table("zones").zones:
		if int(z.code) == zone:
			return int(z.base)
	return zone


## The room left by the options; the catalogue takes all it needs of it, the panel stands centred there, on its own width.
static func fit(drawer: HudDrawer) -> void:
	if not drawer.visible:
		return
	var margin := HudWidgets.px("margin")
	var vw := drawer.get_viewport_rect().size.x
	var left := margin + (HudWidgets.px("opts_w") + margin if drawer.options_panel.visible else 0.0)
	var room := vw - margin - left
	if drawer.catalog.is_inside_tree(): # built when it first shows (road_catalog.gd _ready)
		drawer.catalog.fit(room - HudWidgets.px("gap"))
	drawer.get_tree().call_group(HudDrawer.FITS, &"fit", room - HudWidgets.px("gap"))
	var need := drawer.get_combined_minimum_size()
	var width := minf(need.x, room)
	drawer.offset_left = left + (room - width) * 0.5 - vw * 0.5
	drawer.offset_right = drawer.offset_left + width
	drawer.offset_top = drawer.offset_bottom - need.y
