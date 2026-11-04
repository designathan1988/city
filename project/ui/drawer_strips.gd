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
	b.pressed.connect(func() -> void: on_pick.call(c[key]))
	return b


## Lights the card whose value is `picked` among the strip's cards.
static func light(strip: Node, picked: Variant) -> void:
	for c in strip.get_children():
		if c is Button and c.has_meta(&"pick"):
			c.set_pressed_no_signal(c.get_meta(&"pick") == picked)


## The zone of this use at this density; a use with one density has only that.
static func zone_at(use: int, density: int) -> int:
	for z: Dictionary in DataTables.catalog().zones.zones:
		if int(z.base) == use and int(z.density) == density:
			return int(z.code)
	return use


## The zone's base use, whatever density it was painted with.
static func use_of(zone: int) -> int:
	for z: Dictionary in DataTables.catalog().zones.zones:
		if int(z.code) == zone:
			return int(z.base)
	return zone


## "<label>  [——o——]  <value><unit>": on_change(value) on every move.
static func slider(parent: Control, label_key: String, lo: float, hi: float, step: float, value: float, unit: String,
		on_change: Callable) -> HSlider:
	var col := HudWidgets.vbox(parent)
	var head := HudWidgets.hbox(col)
	var name := HudWidgets.label(head, &"MutedLabel", TranslationServer.translate(label_key))
	name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	var form := "%d%s" if step >= 1.0 else "%.1f%s"
	var shown := HudWidgets.label(head, &"BodyLabel", form % [value if step < 1.0 else int(value), unit])
	var s := HSlider.new()
	s.min_value = lo
	s.max_value = hi
	s.step = step
	s.value = value
	s.custom_minimum_size.x = HudWidgets.px("side_w")
	s.focus_mode = Control.FOCUS_NONE
	s.value_changed.connect(func(v: float) -> void:
		shown.text = form % [v if step < 1.0 else int(v), unit]
		on_change.call(v))
	col.add_child(s)
	return s
