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
	return ThemeArt.swatch(color(key), px, size("swatch_radius"), size("swatch_ring"), color("swatch_ring_colour"))


static func size(key: String) -> float:
	return float(tokens()[key])


static func theme() -> Theme:
	if _theme == null:
		_theme = Theme.new()
		_theme.default_font_size = int(size("font_body"))
		_theme.default_font = font(SystemFont.new().font_weight)
		_panels()
		_buttons()
		ThemeArt.slider(_theme)
		ThemeText.apply(_theme)
	return _theme


## The interface's face (the reference asks for Inter, then the system's sans; Godot's own font when none is there).
static func font(weight: int) -> SystemFont:
	var f := SystemFont.new()
	f.font_names = PackedStringArray(tokens().font_names)
	f.font_weight = weight
	f.antialiasing = TextServer.FONT_ANTIALIASING_LCD # as the reference's browser draws text
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
	for dot: Array in [[&"DotBroken", "danger"], [&"DotSlow", "warn"], [&"DotNote", "muted"], [&"DotGo", "accent"]]:
		_variation(dot[0], &"Panel")
		_theme.set_stylebox(&"panel", dot[0], box(dot[1], "dot"))
	_theme.set_type_variation(&"ScrollInset", &"MarginContainer")
	_theme.set_constant(&"margin_right", &"ScrollInset", int(size("pad")))
	var surface := box("bg", "radius", "line", true)
	surface.set_content_margin_all(size("pad"))
	_panel(&"Surface", surface)
	_panel(&"SidePanel", _pad(box("bg", "radius", "line", true), "gap"))
	var icons := box("bg_strong", "radius", "line", true)
	icons.set_content_margin_all(size("pad_s") + 1.0)
	_panel(&"PopoverIcons", icons)
	_theme.set_type_variation(&"Inset", &"MarginContainer")
	for side in [&"margin_left", &"margin_right"]:
		_theme.set_constant(side, &"Inset", int(size("pop_inset")))
	_theme.set_color(&"font_color", &"LinkButton", color("text"))
	_theme.set_color(&"font_hover_color", &"LinkButton", color("accent"))
	var bar := surface.duplicate()
	bar.content_margin_left = size("bar_pad")
	bar.content_margin_right = size("bar_pad")
	_panel(&"Bar", bar)
	var line := StyleBoxLine.new()
	line.color = color("line")
	_theme.set_stylebox(&"separator", &"HSeparator", line)
	_panel(&"StatCard", _pad(box("stat_bg", "card_radius", "line"), "stat_pad"))
	_theme.set_type_variation(&"StatGrid", &"GridContainer")
	_theme.set_constant(&"h_separation", &"StatGrid", int(size("stat_gap")))
	_theme.set_constant(&"v_separation", &"StatGrid", int(size("stat_gap")))
	_panel(&"Dock", box("bg", "dock_radius", "line", true))
	_panel(&"Seg", box("raise", "pill_radius", "line"))
	var strong := box("bg_strong", "radius", "line", true)
	strong.set_content_margin_all(size("gap"))
	_panel(&"Popover", strong)
	_panel(&"CardArt", box("card_art", "card_radius"))
	_panel(&"TabStrip", box("rc_strip", "card_radius"))
	_panel(&"Card2", _pad(box("bg", "radius", "line", true), "dialog_pad"))
	var start := box("bg", "start_radius", "line", true)
	start.content_margin_left = size("start_pad_x")
	start.content_margin_right = size("start_pad_x")
	start.content_margin_top = size("start_pad_top")
	start.content_margin_bottom = size("start_pad_bottom")
	_panel(&"StartCard", start)
	var scrim := StyleBoxTexture.new()
	scrim.texture = ThemeArt.vignette(color("scrim_in"), color("scrim_out"), Vector2(size("scrim_x"), size("scrim_y")), size("scrim_reach"))
	_panel(&"Scrim", scrim)
	_panel(&"Kbd", box("raise", "kbd_radius", "line"))
	_panel(&"Tip", _pad(box("bg_solid", "kbd_radius", "line"), "pad"))
	_panel(&"RowBand", box("raise", "kbd_radius"))
	_panel(&"LampChip", _pad(box("raise", "pill_radius", "line"), "pad"))
	_panel(&"Toast", box("bg_strong", "pill_radius", "line", true))
	_panel(&"DangerToast", box("bg_strong", "pill_radius", "danger", true))
	for axis in ["H", "V"]:
		var base := StringName(axis + "BoxContainer")
		_theme.set_constant(&"separation", base, int(size("pad_s")))
		for v in [["Gap", "gap"], ["Pad", "pad"], ["Dialog", "dialog_gap"], ["Tight", "tight"]]:
			_variation(StringName(axis + v[0]), base)
			_theme.set_constant(&"separation", StringName(axis + v[0]), int(size(v[1])))


static func _pad(sb: StyleBoxFlat, key: String) -> StyleBoxFlat:
	sb.set_content_margin_all(size(key))
	return sb


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


static func _icon_widths() -> void:
	for name in [&"SegButton", &"DockButton", &"DockDanger", &"IconButton", &"Row", &"DrawerTab"]:
		_theme.set_constant(&"icon_max_width", name, int(size("cat_icon")))
	_theme.set_constant(&"icon_max_width", &"IconButton", int(size("bar_icon")))
	_theme.set_constant(&"icon_max_width", &"SegButton", int(size("seg_icon")))
	_theme.set_constant(&"icon_max_width", &"Row", int(size("row_icon")))
	_theme.set_constant(&"icon_max_width", &"LayerRow", int(size("row_icon")))
	_theme.set_constant(&"icon_max_width", &"IconChoice", int(size("bar_icon")))
	_theme.set_constant(&"icon_max_width", &"Card", int(size("card_glyph")))


static func _buttons() -> void:
	var none := StyleBoxEmpty.new()
	none.set_content_margin_all(size("pad_s"))
	_button(&"SegButton", [none, none, box("accent", "pill_radius")], ["text", "text", "accent_ink"], "font_seg")
	_button(&"IconButton", [box("raise", "icon_radius", "line"), box("raise_2", "icon_radius", "accent_line"),
		box("accent_soft", "icon_radius", "accent")], ["text", "text", "accent"])
	_button(&"DockButton", [none, box("raise", "icon_radius"), box("accent_soft", "icon_radius", "accent")],
		["text", "text", "accent"], "font_cat")
	_button(&"DockDanger", [none, box("raise", "icon_radius"), box("danger_soft", "icon_radius", "danger")],
		["text", "text", "danger"], "font_cat")
	_button(&"DrawerTab", [none, box("raise_2", "card_radius"), box("accent", "card_radius", "accent")],
		["muted", "text", "accent_ink"], "font_tab")
	var chosen := box("raise_2", "card_radius", "accent")
	chosen.set_border_width_all(int(size("chosen_border")))
	_button(&"Card", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"), chosen],
		["muted", "text", "text"], "font_card")
	_button(&"SwatchCard", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"), chosen],
		["muted", "text", "text"], "font_card")
	_button(&"RcCard", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"),
		box("accent_soft", "card_radius", "accent")], ["muted", "text", "accent"], "font_card")
	for role in [&"icon_normal_color", &"icon_hover_color", &"icon_pressed_color", &"icon_hover_pressed_color"]:
		for picture in [&"SwatchCard", &"RcCard"]: # a picture keeps its colours: the icon tint multiplies them
			_theme.set_color(role, picture, color("white"))
	_button(&"RcTab", [none, box("raise", "card_radius"), box("raise_2", "card_radius")], ["muted", "text", "text"],
		"font_tab")
	_button(&"CompassButton", [box("bg", "pill_radius", "line", true), box("bg", "pill_radius", "accent_line", true),
		box("bg", "pill_radius", "accent", true)], ["text", "text", "text"])
	_button(&"Choice", [_wide(box("raise", "kbd_radius", "line")), _wide(box("raise_2", "kbd_radius", "accent_line")),
		_wide(box("accent", "kbd_radius"))], ["text", "text", "accent_ink"], "font_tab")
	_button(&"IconChoice", [none, box("raise_2", "kbd_radius"), box("accent", "kbd_radius")], ["muted", "text", "accent_ink"])
	_button(&"Primary", [box("accent", "start_btn_radius"), box("accent", "start_btn_radius", "text"), box("accent", "start_btn_radius")],
		["accent_ink", "accent_ink", "accent_ink"], "font_clock")
	_button(&"StartBtn", [box("raise", "start_btn_radius", "line"), box("raise_2", "start_btn_radius", "accent_line"),
		box("raise_2", "start_btn_radius", "accent")], ["text", "text", "text"], "font_start")
	_button(&"GhostDanger", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "danger"),
		box("raise_2", "card_radius", "danger")], ["danger_ink", "danger_ink", "danger_ink"])
	_button(&"LayerRow", [none, box("raise_2", "card_radius"), none], ["text", "text", "text"], "font_row")
	_theme.set_color(&"icon_pressed_color", &"LayerRow", color("accent"))
	_theme.set_color(&"icon_hover_pressed_color", &"LayerRow", color("accent"))
	_button(&"Ghost", [box("raise", "card_radius", "line"), box("raise_2", "card_radius", "accent_line"),
		box("raise_2", "card_radius", "accent")], ["text", "text", "text"])
	_button(&"Chip", [_wide(box("raise", "pill_radius", "line")), _wide(box("raise_2", "pill_radius", "accent_line")),
		_wide(box("accent", "pill_radius"))], ["text", "text", "accent_ink"], "font_body")
	_button(&"LinkBtn", [none, none, none], ["link", "accent", "accent"], "font_tab")
	_button(&"Row", [none, box("raise_2", "card_radius"), box("raise_2", "card_radius")], ["text", "text", "accent"], "font_row")
	for name in [&"Row", &"LayerRow"]:
		_theme.set_color(&"icon_normal_color", name, color("muted"))
		_theme.set_color(&"icon_hover_color", name, color("muted"))
	_icon_widths()
	_theme.set_constant(&"icon_max_width", &"SwatchCard", int(size("card_glyph")))
	_theme.set_constant(&"icon_max_width", &"RcCard", int(size("rc_card_w")))


	_variation(&"Glyph", &"TextureRect")

