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
