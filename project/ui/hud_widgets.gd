## Small factories shared by the HUD parts: themed buttons, labels and icons (styles only by theme variation, texts
## only translated keys). Keeps every panel's code about layout, not about setting properties.
class_name HudWidgets
extends RefCounted

const ICONS := "res://ui/icons/%s.svg"
const MS_PER_S := 1000.0


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
		tip_for(b, TranslationServer.translate(tip_key))
	b.custom_minimum_size = Vector2(px(w), px(h))
	b.focus_mode = Control.FOCUS_NONE
	return b


## A control's tip: the name it does not already show. A name on the button needs no tip saying it again.
static func tip_for(b: Button, tip: String) -> void:
	b.tooltip_text = "" if tip == b.text else tip


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
		var drawn: bool = item.size() > 2 # [value, text] or, with an icon for a name, [value, "", icon, tip]
		var b := button(&"IconChoice" if drawn else &"Choice", item[2] if drawn else "", item[1], item[3] if drawn else "", "icon_btn" if drawn else "", "opt_row_h")
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
	var name := label(row, &"FieldLabel", name_text)
	name.custom_minimum_size.x = px("opt_name_w")
	row.custom_minimum_size.y = px("opt_row_h")
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


## "<name>  [choice v]": a list that opens; arrows, Home, End and Escape work in it. items [value, text]; pick(value) on a choice.
static func dropdown(parent: Control, name_text: String, items: Array, value: Variant, pick: Callable) -> OptionButton:
	var row := option_row(parent, name_text)
	var o := OptionButton.new()
	o.theme_type_variation = &"Ghost"
	o.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	o.fit_to_longest_item = false
	o.clip_text = true
	for i in items.size():
		o.add_item(items[i][1], i)
		if items[i][0] == value:
			o.select(i)
	o.item_selected.connect(func(i: int) -> void: pick.call(items[i][0]))
	var popup := o.get_popup()
	popup.window_input.connect(func(event: InputEvent) -> void:
		var k := event as InputEventKey
		if k != null and k.pressed and k.keycode == KEY_HOME:
			popup.set_focused_item(0)
		elif k != null and k.pressed and k.keycode == KEY_END:
			popup.set_focused_item(popup.item_count - 1))
	row.add_child(o)
	return o


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
