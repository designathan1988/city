## The graphics options: a dialog of choices, each applied to the running game as it is picked (Settings).
class_name GraphicsPanel
extends HudDialog


func _row(name_key: String, items: Array, value: Variant, pick: Callable, note_key := "") -> void:
	column.add_child(HSeparator.new())
	var box := HudWidgets.vbox(column)
	HudWidgets.label(box, &"MutedLabel", tr(name_key))
	HudWidgets.choices(box, items, value, func(v: Variant) -> void:
		if v != value:
			pick.call(v)
			refill.call_deferred())
	if note_key != "":
		var note := HudWidgets.label(box, &"MetaLabel", tr(note_key))
		note.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART


func _on_off() -> Array:
	return [[false, tr("gfx.off")], [true, tr("gfx.on")]]


func fill() -> void:
	var t: Dictionary = DataTables.game("settings")
	var now := Settings.current()
	column.theme_type_variation = &"VDialog"
	HudWidgets.label(column, &"HeadLabel", tr("gfx.title"))
	var presets := []
	for p: String in t.presets:
		presets.append([p, tr("quality." + p)])
	_row("gfx.preset", presets, Settings.preset, Settings.set_preset)
	var shadows := []
	for s: float in t.shadow_choices:
		shadows.append([int(s), tr("gfx.shadows.%d" % int(s))])
	_row("gfx.shadows", shadows, int(now.shadow_px) if now.shadows else 0, _keep.bind("shadows"))
	_row("gfx.ao", _on_off(), now.ssao, _keep.bind("ssao"))
	if RenderMethod.draws(&"smaa"):
		_row("gfx.aa", _on_off(), now.smaa, _keep.bind("smaa"))
	_row("gfx.dynamicResolution", _on_off(), Settings.dynamic_resolution, Settings.set_dynamic_resolution, "gfx.dynamicResolution.note")
	var fps := []
	for f: float in t.fps_choices:
		fps.append([int(f), tr("gfx.fpsLimit.none") if f == 0 else str(int(f))])
	_row("gfx.fpsLimit", fps, Settings.fps_limit, Settings.set_fps_limit)
	_row("gfx.vsync", _on_off(), Settings.vsync, Settings.set_vsync)
	_row("gfx.fullscreen", _on_off(), Settings.fullscreen, Settings.set_fullscreen)
	var gpu := HudWidgets.label(column, &"MetaLabel", "%s: %s" % [tr("gfx.gpu"), RenderingServer.get_video_adapter_name()])
	gpu.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	close_row()


func _keep(value: Variant, key: String) -> void:
	Settings.keep(key, value)
