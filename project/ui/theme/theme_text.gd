## The interface's text: every label's size and colour, which faces are bold and the engine's own tooltip (tokens -> Theme).
class_name ThemeText
extends RefCounted

static var _theme: Theme


static func _label(name: StringName, font: String, colour: String) -> void:
	_theme.set_type_variation(name, &"Label")
	_theme.set_font_size(&"font_size", name, int(ThemeBuilder.size(font)))
	_theme.set_color(&"font_color", name, ThemeBuilder.color(colour))


static func apply(theme: Theme) -> void:
	_theme = theme
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
	_label(&"FieldLabel", "font_tab", "muted")
	_label(&"TagLabel", "font_row", "muted")
	_label(&"PreviewTagLabel", "font_preview_tag", "preview_tag_ink")
	_label(&"PreviewDangerLabel", "font_preview_tag", "preview_tag_danger_ink")
	_label(&"PreviewWarnLabel", "font_preview_tag", "warn")
	_label(&"SideTitle", "font_meta", "muted")
	_label(&"StatName", "font_stat_name", "muted")
	_label(&"StatValue", "font_body", "text")
	_label(&"TipLabel", "font_tip", "text")
	_label(&"MonoLabel", "font_meta", "text")
	var bold := ThemeBuilder.font(int(ThemeBuilder.size("font_bold")))
	for name: String in ThemeBuilder.tokens().bold_types:
		_theme.set_font(&"font", StringName(name), bold)
	_theme.set_font(&"font", &"TitleBig", ThemeBuilder.font(int(ThemeBuilder.size("font_black"))))
	var mono := SystemFont.new()
	mono.font_names = PackedStringArray(ThemeBuilder.tokens().font_mono)
	_theme.set_font(&"font", &"MonoLabel", mono)
	_tooltip()


## The tooltip the engine draws for a `tooltip_text` no HudTip reads: the interface's own look (shell.css .v2-tip).
static func _tooltip() -> void:
	var sb := ThemeBuilder.box("bg_solid", "kbd_radius", "line")
	sb.set_content_margin_all(ThemeBuilder.size("pad"))
	_theme.set_stylebox(&"panel", &"TooltipPanel", sb)
	_theme.set_color(&"font_color", &"TooltipLabel", ThemeBuilder.color("text"))
	_theme.set_font_size(&"font_size", &"TooltipLabel", int(ThemeBuilder.size("font_tip")))
