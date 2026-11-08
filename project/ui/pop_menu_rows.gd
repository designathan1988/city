## The Menu popover's segment rows (data/ui/drawers.json "popovers", a row's "seg"): the graphics quality, the road paint's style and
## the language, each a choice row that acts as it is picked. ui/hud_popover.gd builds the rest of the menu.
class_name PopMenuRows
extends RefCounted


static func quality(col: Control, r: Dictionary) -> void:
	var items := []
	for p: String in DataTables.game("settings").presets:
		items.append([p, TranslationServer.translate("quality." + p)])
	var row := HudWidgets.option_row(col, TranslationServer.translate(r.label_key))
	var box := HFlowContainer.new()
	box.size_flags_horizontal = Control.SIZE_EXPAND_FILL
	row.add_child(box)
	HudWidgets.choices(box, items, Settings.preset, Settings.set_preset)


static func paint(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, TranslationServer.translate(r.label_key))
	var items := []
	var styles: Array = DataTables.render_table("streets").markings.styles
	for i in styles.size():
		items.append([i, TranslationServer.translate("paint." + styles[i].id)])
	HudWidgets.choices(row, items, int(CityStore.snapshot.get("marking_style", 0)), func(v: int) -> void:
		CityStore.dispatch(&"set_marking_style", [v, true]))


static func language(col: Control, r: Dictionary) -> void:
	var row := HudWidgets.option_row(col, TranslationServer.translate(r.label_key))
	HudWidgets.choices(row, StartScreen.languages(), TranslationServer.get_locale(), Settings.set_language)
