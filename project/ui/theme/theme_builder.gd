## The only owner of UI styles (gate UI_SEM_ESTILO_SOLTO): builds one Theme from data/ui/tokens.json with a type
## variation per control style. Controls only pick a
## `theme_type_variation`; colours other code needs (preview ribbon, swatches) come from `color()`.
class_name ThemeBuilder
extends RefCounted

static var _theme: Theme
static var _t: Dictionary


static func tokens() -> Dictionary:
	if _t.is_empty():
		_t = DataTables.ui("tokens")
	return _t


static func color(key: String) -> Color:
	return Color.html(tokens()[key])


## A square of one token's colour (zone cards' swatch picture).
static func swatch(key: String, px: int) -> Texture2D:
	var img := Image.create_empty(px, px, false, Image.FORMAT_RGBA8)
	img.fill(color(key))
	return ImageTexture.create_from_image(img)


static func size(key: String) -> float:
	return float(tokens()[key])


static func theme() -> Theme:
	if _theme == null:
		_theme = Theme.new()
		_theme.default_font_size = int(size("font_body"))
		_panels()
		_buttons()
		_labels()
	return _theme


static func box(fill: String, radius: String, border := "", shadow := false) -> StyleBoxFlat:
	var sb := StyleBoxFlat.new()
	sb.bg_color = color(fill)
	sb.set_corner_radius_all(int(size(radius)))
	if border != "":
		sb.border_color = color(border)
		sb.set_border_width_all(int(size("border")))
	if shadow:
		sb.shadow_color = color("shadow")
		sb.shadow_size = int(size("shadow_size"))
		sb.shadow_offset = Vector2(0, size("shadow_offset_y"))
	sb.set_content_margin_all(size("pad_s"))
	return sb


static func _variation(name: StringName, base: StringName) -> void:
	_theme.set_type_variation(name, base)


static func _panel(name: StringName, sb: StyleBox) -> void:
	_variation(name, &"PanelContainer")
	_theme.set_stylebox(&"panel", name, sb)


static func _panels() -> void:
	var surface := box("bg", "radius", "line", true)
	surface.set_content_margin_all(size("pad"))
	_panel(&"Surface", surface)
	_panel(&"Dock", box("bg", "dock_radius", "line", true))
	_panel(&"Seg", box("raise", "pill_radius", "line"))
	var strong := box("bg_strong", "radius", "line", true)
	strong.set_content_margin_all(size("pad"))
	_panel(&"Popover", strong)
	_panel(&"CardArt", box("card_art", "card_radius"))
	_panel(&"TabStrip", box("rc_strip", "card_radius"))
	_panel(&"Toast", box("bg_strong", "pill_radius", "line", true))
	_panel(&"DangerToast", box("bg_strong", "pill_radius", "danger", true))
	for axis in ["H", "V"]:
		var base := StringName(axis + "BoxContainer")
		_theme.set_constant(&"separation", base, int(size("pad_s")))
		for v in [["Gap", "gap"], ["Pad", "pad"]]:
			_variation(StringName(axis + v[0]), base)
			_theme.set_constant(&"separation", StringName(axis + v[0]), int(size(v[1])))


## One Button variation: [normal, hover, pressed] boxes and [idle, hover, on] colour tokens for text and icon.
static func _button(name: StringName, boxes: Array, colours: Array, font: String = "") -> void:
	_variation(name, &"Button")
	var states := [&"normal", &"hover", &"pressed"]
	for i in states.size():
		_theme.set_stylebox(states[i], name, boxes[i])
	_theme.set_stylebox(&"hover_pressed", name, boxes[2])
	_theme.set_stylebox(&"disabled", name, boxes[0])
	_theme.set_stylebox(&"focus", name, StyleBoxEmpty.new())
	var roles := [[&"font_color", &"icon_normal_color"], [&"font_hover_color", &"icon_hover_color"],
		[&"font_pressed_color", &"icon_pressed_color"]]
	for i in roles.size():
		for role in roles[i]:
			_theme.set_color(role, name, color(colours[i]))
	_theme.set_color(&"font_hover_pressed_color", name, color(colours[2]))
	_theme.set_color(&"icon_hover_pressed_color", name, color(colours[2]))
	_theme.set_color(&"font_disabled_color", name, color("disabled"))
	_theme.set_color(&"icon_disabled_color", name, color("disabled"))
	if font != "":
		_theme.set_font_size(&"font_size", name, int(size(font)))


static func _buttons() -> void:
	var none := StyleBoxEmpty.new()
	none.set_content_margin_all(size("pad_s"))
	_button(&"SegButton", [none, none, box("accent", "pill_radius")], ["muted", "text", "accent_ink"], "font_seg")
	_button(&"IconButton", [box("raise", "icon_radius", "line"), box("raise_2", "icon_radius", "accent_line"),
		box("accent_soft", "icon_radius", "accent")], ["text", "text", "accent"])
	_button(&"DockButton", [none, box("raise", "icon_radius"), box("accent_soft", "icon_radius", "accent")],
		["muted", "text", "accent"], "font_cat")
	_button(&"DockDanger", [none, box("raise", "icon_radius"), box("danger_soft", "icon_radius", "danger")],
		["muted", "text", "danger"], "font_cat")
	_button(&"DrawerTab", [none, box("raise_2", "card_radius"), box("accent", "card_radius", "accent")],
		["muted", "text", "accent_ink"], "font_tab")
	_button(&"Card", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"),
		box("raise_2", "card_radius", "accent")], ["muted", "text", "text"], "font_card")
	_button(&"SwatchCard", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"),
		box("raise_2", "card_radius", "accent")], ["muted", "text", "text"], "font_card")
	for role in [&"icon_normal_color", &"icon_hover_color", &"icon_pressed_color", &"icon_hover_pressed_color"]:
		_theme.set_color(role, &"SwatchCard", color("white"))
	_button(&"RcCard", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"),
		box("accent_soft", "card_radius", "accent")], ["muted", "text", "accent"], "font_card")
	_button(&"RcTab", [none, box("raise", "card_radius"), box("raise_2", "card_radius")], ["muted", "text", "text"],
		"font_tab")
	_button(&"CompassButton", [box("bg", "pill_radius", "line", true), box("bg", "pill_radius", "accent_line", true),
		box("bg", "pill_radius", "accent", true)], ["text", "text", "text"])
	_button(&"Row", [none, box("raise_2", "card_radius"), box("raise_2", "card_radius")], ["text", "text", "accent"])
	for name in [&"SegButton", &"DockButton", &"DockDanger", &"IconButton", &"Row", &"DrawerTab"]:
		_theme.set_constant(&"icon_max_width", name, int(size("cat_icon")))
	_theme.set_constant(&"icon_max_width", &"Card", int(size("card_glyph")))
	_theme.set_constant(&"icon_max_width", &"SwatchCard", int(size("card_glyph")))
	_theme.set_constant(&"icon_max_width", &"RcCard", int(size("rc_card_w")))


static func _label(name: StringName, font: String, colour: String) -> void:
	_variation(name, &"Label")
	_theme.set_font_size(&"font_size", name, int(size(font)))
	_theme.set_color(&"font_color", name, color(colour))


static func _labels() -> void:
	_label(&"ClockLabel", "font_clock", "text")
	_label(&"MoneyLabel", "font_money", "text")
	_label(&"StatsLabel", "font_stats", "muted")
	_label(&"BodyLabel", "font_body", "text")
	_label(&"MutedLabel", "font_body", "muted")
	_label(&"TitleLabel", "font_title", "muted")
	_label(&"CardLabel", "font_card", "muted")
	_label(&"MetaLabel", "font_meta", "muted")
	_label(&"DangerLabel", "font_body", "danger_ink")
	_variation(&"Glyph", &"TextureRect")

