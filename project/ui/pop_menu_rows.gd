## The Menu popover's segment rows (data/ui/drawers.json "popovers", a row's "seg"): the graphics quality, the road paint's style and
## the language, each a choice row that acts as it is picked. ui/hud_popover.gd builds the rest of the menu.
class_name PopMenuRows
extends RefCounted


static func quality(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, TranslationServer.translate(r.label_key))
	var box := HFlowContainer.new()
	box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	row.add_child(box)
	var presets := HudWidgets.choices(box, Settings.preset, Settings.set_preset)
	for p: String in DataTables.game("settings").presets:
		HudWidgets.choice(presets, p, TranslationServer.translate("quality." + p))


static func paint(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, TranslationServer.translate(r.label_key))
	var styles: Array = DataTables.render_table("streets").markings.styles
	var paints := HudWidgets.choices(row, int(CityStore.snapshot.get("marking_style", 0)), func(v: int) -> void:
		CityStore.dispatch(&"set_marking_style", [v, true]))
	for i in styles.size():
		HudWidgets.choice(paints, i, TranslationServer.translate("paint." + styles[i].id))


static func language(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, TranslationServer.translate(r.label_key))
	var languages := HudWidgets.choices(row, TranslationServer.get_locale(), Settings.set_language)
	for locale: String in StartScreen.locales():
		HudWidgets.choice(languages, locale, StartScreen.language_name(locale))
