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
		_theme.default_font = _font(SystemFont.new().font_weight)
		_panels()
		_buttons()
		_slider()
		_labels()
	return _theme


## The interface's face (the reference asks for Inter, then the system's sans; Godot's own font when none is there).
static func _font(weight: int) -> SystemFont:
	var f := SystemFont.new()
	f.font_names = PackedStringArray(tokens().font_names)
	f.font_weight = weight
	f.fallbacks = [ThemeDB.fallback_font]
	return f


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


## A band from the panel's colour to nothing, `from_left` or the other way: over the end of a row with more in it.
static func fade(from_left: bool) -> GradientTexture1D:
	var solid := color("bg_solid")
	var g := Gradient.new()
	g.set_color(0, solid if from_left else Color(solid, 0.0))
	g.set_color(1, Color(solid, 0.0) if from_left else solid)
	var tex := GradientTexture1D.new()
	tex.gradient = g
	return tex


static func _panels() -> void:
	for dot: Array in [[&"DotBroken", "danger"], [&"DotSlow", "warn"], [&"DotNote", "muted"]]:
		_variation(dot[0], &"Panel")
		_theme.set_stylebox(&"panel", dot[0], box(dot[1], "dot"))
	_panel(&"Tip", box("bg_strong", "kbd_radius", "line"))
	_theme.set_type_variation(&"ScrollInset", &"MarginContainer")
	_theme.set_constant(&"margin_right", &"ScrollInset", int(size("pad")))
	var surface := box("bg", "radius", "line", true)
	surface.set_content_margin_all(size("pad"))
	_panel(&"Surface", surface)
	_panel(&"Dock", box("bg", "dock_radius", "line", true))
	_panel(&"Seg", box("raise", "pill_radius", "line"))
	var strong := box("bg_strong", "radius", "line", true)
	strong.set_content_margin_all(size("gap"))
	_panel(&"Popover", strong)
	_panel(&"CardArt", box("card_art", "card_radius"))
	_panel(&"TabStrip", box("rc_strip", "card_radius"))
	var card := box("bg_strong", "radius", "line", true)
	card.set_content_margin_all(size("gap") * 2)
	_panel(&"Card2", card)
	_panel(&"Scrim", box("scrim", "border"))
	_panel(&"Kbd", box("raise", "kbd_radius", "line"))
	_panel(&"Tip", box("bg_strong", "kbd_radius", "line"))
	_panel(&"RowBand", box("raise", "kbd_radius"))
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


## A box whose text keeps room at its sides (.v2-choice padding).
static func _wide(sb: StyleBoxFlat) -> StyleBoxFlat:
	sb.content_margin_left = size("card_radius")
	sb.content_margin_right = size("card_radius")
	return sb


## The slider of the panels: a thin track, the part behind the knob in the accent colour.
static func _slider() -> void:
	var track := box("raise_2", "pill_radius")
	track.content_margin_top = size("pad_s")
	track.content_margin_bottom = size("pad_s")
	var filled := box("accent", "pill_radius")
	_theme.set_stylebox(&"slider", &"HSlider", track)
	_theme.set_stylebox(&"grabber_area", &"HSlider", filled)
	_theme.set_stylebox(&"grabber_area_highlight", &"HSlider", filled)


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
	_button(&"Choice", [_wide(box("raise", "kbd_radius", "line")), _wide(box("raise_2", "kbd_radius", "accent_line")),
		_wide(box("accent", "kbd_radius"))], ["text", "text", "accent_ink"], "font_tab")
	_button(&"Primary", [box("accent", "card_radius"), box("accent", "card_radius", "text"), box("accent", "card_radius")],
		["accent_ink", "accent_ink", "accent_ink"], "font_clock")
	_button(&"Ghost", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"),
		box("raise_2", "card_radius", "accent")], ["text", "text", "text"])
	_button(&"Chip", [_wide(box("raise", "pill_radius", "line")), _wide(box("raise_2", "pill_radius", "accent_line")),
		_wide(box("accent", "pill_radius"))], ["text", "text", "accent_ink"], "font_tab")
	_button(&"LinkBtn", [none, none, none], ["link", "accent", "accent"], "font_tab")
	_button(&"Row", [none, box("raise_2", "card_radius"), box("raise_2", "card_radius")], ["text", "text", "accent"])
	for name in [&"SegButton", &"DockButton", &"DockDanger", &"IconButton", &"Row", &"DrawerTab"]:
		_theme.set_constant(&"icon_max_width", name, int(size("cat_icon")))
	_theme.set_constant(&"icon_max_width", &"IconButton", int(size("bar_icon")))
	_theme.set_constant(&"icon_max_width", &"SegButton", int(size("bar_icon")))
	_theme.set_constant(&"icon_max_width", &"Row", int(size("row_icon")))
	_theme.set_constant(&"icon_max_width", &"Card", int(size("card_glyph")))
	_theme.set_constant(&"icon_max_width", &"SwatchCard", int(size("card_glyph")))
	_theme.set_constant(&"icon_max_width", &"RcCard", int(size("rc_card_w")))


static func _label(name: StringName, font: String, colour: String) -> void:
	_variation(name, &"Label")
	_theme.set_font_size(&"font_size", name, int(size(font)))
	_theme.set_color(&"font_color", name, color(colour))


## The tooltip the engine draws for every `tooltip_text`: the interface's own look (shell.css .v2-tip).
static func _tooltip() -> void:
	var sb := box("bg_strong", "kbd_radius", "line")
	sb.set_content_margin_all(size("pad"))
	_theme.set_stylebox(&"panel", &"TooltipPanel", sb)
	_theme.set_color(&"font_color", &"TooltipLabel", color("text"))
	_theme.set_font_size(&"font_size", &"TooltipLabel", int(size("font_tab")))


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
	_label(&"KbdLabel", "font_kbd", "text")
	_label(&"HeadLabel", "font_h2", "text")
	_label(&"TitleBig", "font_h1", "white")
	var bold := _font(int(size("font_bold")))
	_theme.set_font(&"font", &"TitleBig", bold)
	var strong := _font(int(size("font_strong")))
	for name: StringName in [&"KbdLabel", &"TitleLabel", &"ClockLabel", &"MoneyLabel"]:
		_theme.set_font(&"font", name, strong)
	_theme.set_font(&"font", &"DockButton", strong)
	_theme.set_font(&"font", &"DockDanger", strong)
	_theme.set_font(&"font", &"HeadLabel", bold)
	_theme.set_font(&"font", &"ClockLabel", bold)
	_theme.set_font(&"font", &"MoneyLabel", bold)
	_tooltip()
	_variation(&"Glyph", &"TextureRect")

