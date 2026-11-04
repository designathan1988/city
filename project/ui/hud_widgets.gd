## Small factories shared by the HUD parts: themed buttons, labels and icons (styles only by theme variation, texts
## only translated keys). Keeps every panel's code about layout, not about setting properties.
class_name HudWidgets
extends RefCounted

const ICONS := "res://ui/icons/%s.svg"


static func icon(name: String) -> Texture2D:
	return load(ICONS % name)


static func px(token: String) -> float:
	return ThemeBuilder.size(token) if token != "" else 0.0


## A themed button: icon may be "", text is shown as given (already translated), tip_key is translated.
static func button(variation: StringName, icon_name: String, text: String, tip_key: String, w: String, h: String) -> Button:
	var b := Button.new()
	b.theme_type_variation = variation
	if icon_name != "":
		b.icon = icon(icon_name)
	b.text = text
	if tip_key != "":
		b.tooltip_text = TranslationServer.translate(tip_key)
	b.custom_minimum_size = Vector2(px(w), px(h))
	b.focus_mode = Control.FOCUS_NONE
	return b


## Disabled "being built" button: tooltip "<name> · em construção".
static func soon(b: Button, name_key: String) -> void:
	b.disabled = true
	b.tooltip_text = "%s · %s" % [TranslationServer.translate(name_key), TranslationServer.translate("ui.soon")]


static func label(parent: Control, variation: StringName, text: String = "") -> Label:
	var l := Label.new()
	l.theme_type_variation = variation
	l.text = text
	l.vertical_alignment = VERTICAL_ALIGNMENT_CENTER
	parent.add_child(l)
	return l


## Rows and columns: separation from the theme (variation "Gap" = the panel gap, "Pad" = pad, default = pad_s).
static func hbox(parent: Control, variation: StringName = &"") -> HBoxContainer:
	var h := HBoxContainer.new()
	h.theme_type_variation = StringName("H" + variation) if variation != &"" else &""
	parent.add_child(h)
	return h


static func vbox(parent: Control, variation: StringName = &"") -> VBoxContainer:
	var v := VBoxContainer.new()
	v.theme_type_variation = StringName("V" + variation) if variation != &"" else &""
	parent.add_child(v)
	return v


static func panel(parent: Control, variation: StringName) -> PanelContainer:
	var p := PanelContainer.new()
	p.theme_type_variation = variation
	parent.add_child(p)
	return p


## A choice among a few, in a row: each [value, text]; the one in use lit; pick(value) on a click.
static func choices(parent: Control, items: Array, value: Variant, pick: Callable) -> HBoxContainer:
	var row := hbox(parent)
	for item: Array in items:
		var b := button(&"Choice", "", item[1], "", "", "opt_row_h")
		b.toggle_mode = true
		b.set_pressed_no_signal(item[0] == value)
		b.pressed.connect(func() -> void:
			for other: Button in row.get_children():
				other.set_pressed_no_signal(other == b)
			pick.call(item[0]))
		row.add_child(b)
	return row


## A row of a menu or a tool's options: a short name at the left, its controls at the right.
static func option_row(parent: Control, name_text: String) -> HBoxContainer:
	var row := hbox(parent, &"Gap")
	var name := label(row, &"MutedLabel", name_text)
	name.custom_minimum_size.x = px("opt_name_w")
	return row


## "<name>  [——o——]  <value>": a slider's row; on_change(value) on every move.
static func range_row(parent: Control, name_text: String, lo: float, hi: float, step: float, value: float, unit: String,
		on_change: Callable) -> HSlider:
	var row := option_row(parent, name_text)
	var s := HSlider.new()
	s.min_value = lo
	s.max_value = hi
	s.step = step
	s.value = value
	s.custom_minimum_size.x = px("slider_w")
	s.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	s.size_flags_vertical = Control.SIZE_SHRINK_CENTER
	s.focus_mode = Control.FOCUS_NONE
	row.add_child(s)
	var out := label(row, &"BodyLabel", "%d%s" % [int(value), unit])
	s.value_changed.connect(func(v: float) -> void:
		out.text = "%d%s" % [int(v), unit]
		on_change.call(v))
	return s


## A key cap: the key's name in a small rounded box.
static func kbd(parent: Control, text: String) -> PanelContainer:
	var p := panel(parent, &"Kbd")
	label(p, &"KbdLabel", text)
	p.size_flags_horizontal = Control.SIZE_SHRINK_BEGIN
	return p


static func clear(node: Node) -> void:
	for c in node.get_children():
		node.remove_child(c)
		c.queue_free()
