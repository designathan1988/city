## The graphics options: a dialog of choices, each applied to the running game as it is picked (Settings).
class_name GraphicsPanel
extends HudDialog


## A row of choices, each added to the row it returns (HudWidgets.choice).
func _row(name_key: String, value: Variant, pick: Callable, note_key := "") -> HBoxContainer:
	column.add_child(HSeparator.new())
	var box := HudWidgets.vbox(column)
	HudWidgets.label(box, &"MutedLabel", tr(name_key))
	var row := HudWidgets.choices(box, value, func(v: Variant) -> void:
		if v != value:
			pick.call(v)
			refill.call_deferred())
	if note_key != "":
		var note := HudWidgets.label(box, &"MetaLabel", tr(note_key))
		note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	return row


func _on_off(row: HBoxContainer) -> void:
	HudWidgets.choice(row, false, tr("gfx.off"))
	HudWidgets.choice(row, true, tr("gfx.on"))


func fill() -> void:
	var t: Dictionary = DataTables.game("settings")
	var now := Settings.current()
	column.theme_type_variation = &"VDialog"
	HudWidgets.label(column, &"HeadLabel", tr("gfx.title"))
	var presets := _row("gfx.preset", Settings.preset, Settings.set_preset)
	for p: String in t.presets:
		HudWidgets.choice(presets, p, tr("quality." + p))
	var shadows := _row("gfx.shadows", int(now.shadow_px) if now.shadows else 0, _keep.bind("shadows"))
	for s: float in t.shadow_choices:
		HudWidgets.choice(shadows, int(s), tr("gfx.shadows.%d" % int(s)))
	_on_off(_row("gfx.ao", now.ssao, _keep.bind("ssao")))
	if QualityTier.has_smaa():
		_on_off(_row("gfx.aa", now.smaa, _keep.bind("smaa")))
	_on_off(_row("gfx.dynamicResolution", Settings.dynamic_resolution, Settings.set_dynamic_resolution, "gfx.dynamicResolution.note"))
	var fps := _row("gfx.fpsLimit", Settings.fps_limit, Settings.set_fps_limit)
	for f: float in t.fps_choices:
		HudWidgets.choice(fps, int(f), tr("gfx.fpsLimit.none") if f == 0 else Formatters.format_int(f))
	_on_off(_row("gfx.vsync", Settings.vsync, Settings.set_vsync))
	_on_off(_row("gfx.fullscreen", Settings.fullscreen, Settings.set_fullscreen))
	var gpu := HudWidgets.label(column, &"MetaLabel", tr("gfx.gpu.named").format({"name": RenderingServer.get_video_adapter_name()}))
	gpu.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	close_row()


func _keep(value: Variant, key: String) -> void:
	Settings.keep(key, value)
