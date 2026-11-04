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


static func clear(node: Node) -> void:
	for c in node.get_children():
		node.remove_child(c)
		c.queue_free()
