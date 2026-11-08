## Four tabs of the F7 panel: GPU (what is drawn and the engine's debug views), Checks (ResourceWatch), Recorder (LabRecorder) and Tests (the A/B of every
## switch, DoctorRun.sweep). Draws what they hold; keeps nothing.
class_name LabTools
extends RefCounted

const DRAW_CONSTANT := "DEBUG_DRAW_"


## A name and its value; the value's label, for one written as given.
static func _line(body: Control, name_text: String, value := "") -> Label:
	var row := HudWidgets.hbox(body)
	HudWidgets.label(row, &"MutedLabel", name_text).size_flags_horizontal = Control.SIZE_EXPAND_FILL
	return HudWidgets.label(row, &"BodyLabel", value)


static func _note(body: Control, text: String) -> void:
	HudWidgets.label(body, &"MetaLabel", text).autowrap_mode = TextServer.AUTOWRAP_WORD_SMART


static func _button(body: Control, text: String, disabled: bool, run: Callable) -> void:
	var b := HudWidgets.button(&"Choice", "", text, "", "", "opt_row_h")
	b.disabled = disabled
	b.pressed.connect(run)
	body.add_child(b)


## The view of the frame by its name in lab.json: the engine's own debug draws.
static func set_view(viewport: Viewport, name_text: String) -> void:
	var mode := ClassDB.class_get_integer_constant(&"Viewport", DRAW_CONSTANT + name_text)
	if mode == Viewport.DEBUG_DRAW_WIREFRAME:
		RenderingServer.set_debug_generate_wireframes(true) # for the meshes made from now on: what is drawn already has none
	viewport.debug_draw = mode


## What the frame draws, read again as the panel stands open.
static func drawn(body: Control) -> void:
	var rs := RenderingServer
	var rid := body.get_viewport().get_viewport_rid()
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.gpu.drawn").to_upper())
	for kind: Array in [["lab.gpu.seen", rs.VIEWPORT_RENDER_INFO_TYPE_VISIBLE], ["lab.gpu.shadow", rs.VIEWPORT_RENDER_INFO_TYPE_SHADOW]]:
		_line(body, body.tr(kind[0]), body.tr("lab.gpu.counts") % [Formatters.format_int(rs.viewport_get_render_info(rid, kind[1], rs.VIEWPORT_RENDER_INFO_OBJECTS_IN_FRAME)),
			Formatters.format_int(rs.viewport_get_render_info(rid, kind[1], rs.VIEWPORT_RENDER_INFO_DRAW_CALLS_IN_FRAME)),
			Formatters.format_int(rs.viewport_get_render_info(rid, kind[1], rs.VIEWPORT_RENDER_INFO_PRIMITIVES_IN_FRAME))])


## The frame by layer, after the button: what each draws, heaviest first.
static func capture(body: Control, frame: FrameCapture) -> void:
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.capture.title").to_upper())
	_button(body, body.tr("lab.capture.take"), frame.busy, frame.take)
	if frame.busy:
		HudWidgets.label(body, &"MetaLabel", body.tr("lab.capture.busy"))
	for r: Dictionary in frame.rows:
		if r.primitives > 0 or r.draws > 0:
			_line(body, body.tr("lab.layer." + String(r.layer)), body.tr("lab.capture.row") % [r.gpu_ms, Formatters.format_int(r.draws), Formatters.format_int(r.primitives), Formatters.format_int(r.shadow_primitives)])
	if frame.file != "":
		HudWidgets.label(body, &"MutedLabel", body.tr("lab.capture.saved") % frame.file).autowrap_mode = TextServer.AUTOWRAP_WORD_SMART


static func views(body: Control, redraw: Callable) -> void:
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.gpu.views").to_upper())
	var now := body.get_viewport().debug_draw
	var shown := ""
	for v: String in DataTables.game("lab").views:
		if ClassDB.class_get_integer_constant(&"Viewport", DRAW_CONSTANT + v) == now:
			shown = v
	var row := HudWidgets.choices(body, shown, func(v: String) -> void:
		set_view(body.get_viewport(), v)
		redraw.call_deferred())
	for v: String in DataTables.game("lab").views:
		HudWidgets.choice(row, v, body.tr("lab.view." + v))


## Where the CPU's frame goes over the window: heaviest first.
static func cpu(body: Control, watch: CpuWatch) -> void:
	HudWidgets.label(body, &"TitleLabel", (body.tr("lab.cpu.title") % watch.frames()).to_upper())
	for r: Dictionary in watch.rows(DataTables.game("lab").cpu.tail):
		_line(body, body.tr("lab.core." + String(r.key)), body.tr("lab.cpu.row") % [r.mean, r.tail, r.worst, roundi(r.share)])
	HudWidgets.label(body, &"MutedLabel", body.tr("lab.cpu.note")).autowrap_mode = TextServer.AUTOWRAP_WORD_SMART


static func checks(body: Control, watch: ResourceWatch) -> void:
	for c: Dictionary in watch.checks():
		HudWidgets.verbatim(_line(body, body.tr("lab.check." + c.id)), "%s · %s" % [c.value, body.tr("lab.verdict." + c.verdict + ".word")])


static func recorder(body: Control, rec: LabRecorder, redraw: Callable) -> void:
	_button(body, body.tr("lab.rec.stop") if rec.recording else body.tr("lab.rec.start"), false, func() -> void:
		if rec.recording:
			rec.stop()
		else:
			rec.start()
		redraw.call_deferred())
	_note(body, body.tr("lab.rec.help"))
	for i in rec.stretches.size():
		var s: Dictionary = rec.stretches[i]
		_line(body, body.tr("lab.rec.stretch") % [i + 1, s.seconds], body.tr("lab.rec.row") % [s.get("interval_p50", 0.0), s.get("interval_p99", 0.0), s.hitches, s.dropped])
	if rec.saved != "":
		_note(body, body.tr("lab.rec.saved") % ProjectSettings.globalize_path(rec.saved))


static func tests(body: Control, run: DoctorRun) -> void:
	_button(body, body.tr("lab.sweep"), run.busy, func() -> void: run.sweep(run.all_switches()))
	if run.status != "":
		_note(body, body.tr(run.status))
	var rows: Array = run.results.keys()
	rows.sort_custom(func(a: String, b: String) -> bool: return _gain(run.results[a]) > _gain(run.results[b]))
	for key: String in rows:
		var c: Dictionary = run.results[key]
		var value := body.tr("lab.ab.row") % [c.gpu_before if c.gpu_before != null else 0.0, c.gpu_after if c.gpu_after != null else 0.0]
		_line(body, body.tr("fx." + key), value if c.real else "%s · %s" % [value, body.tr("lab.ab.noise")])


static func _gain(c: Dictionary) -> float:
	return 0.0 if c.gpu_before == null or c.gpu_after == null else c.gpu_before - c.gpu_after
