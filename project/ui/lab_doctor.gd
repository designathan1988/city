## The Diagnosis tab of the F7 panel (the 4.0's doctor/index.ts): what bounds the frame, the GPU's stages by weight, and every switch
## with its button, whose measured cost replaces a guess. Draws what game/lab/doctor_run.gd holds; keeps nothing.
class_name LabDoctor
extends RefCounted



static func draw(body: Control, run: DoctorRun) -> void:
	var go := HudWidgets.button(&"Choice", "", body.tr("lab.measure"), "", "", "opt_row_h")
	go.disabled = run.busy
	go.pressed.connect(run.measure)
	body.add_child(go)
	if run.status != "":
		HudWidgets.label(body, &"MetaLabel", body.tr(run.status))
	if not run.report.is_empty():
		_verdict(body, run.report)
		_stages(body, run)
	_switches(body, run)


static func _ms(value: Variant) -> String:
	return "—" if value == null else "%.1f" % value


static func _verdict(body: Control, report: Dictionary) -> void:
	var line := body.tr("lab.verdict.none")
	if report.bound != "none":
		line = body.tr("lab.verdict." + report.bound) % [_ms(report.gpu_ms), _ms(report.cpu_ms), _ms(report.budget_ms)]
	HudWidgets.label(body, &"BodyLabel", line).autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
	var pace := body.tr("lab.pace") % [_ms(report.frame_ms), _ms(report.fps), report.frames]
	HudWidgets.label(body, &"MetaLabel", pace)
	if report.hitches > 0:
		var key := "lab.hitches.many" if report.many_hitches else "lab.hitches"
		HudWidgets.label(body, &"MutedLabel", body.tr(key) % report.hitches)


static func _stages(body: Control, run: DoctorRun) -> void:
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.stages").to_upper())
	for f: Dictionary in run.report.findings:
		var row := HudWidgets.hbox(body, &"Pad")
		var text := body.tr("lab.stage.row") % [body.tr("lab.stage." + f.id), _ms(f.ms), _ms(f.tail_ms), int(f.share * Units.PERCENT), body.tr("lab.severity." + f.severity)]
		var name := HudWidgets.label(row, &"BodyLabel", text)
		name.size_flags_horizontal = Control.SIZE_EXPAND_FILL
		name.autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
		var key := run.switch_of(f.id)
		if key != "":
			_test(row, run, key)
		if run.results.has(key):
			_result(body, run.results[key])


static func _switches(body: Control, run: DoctorRun) -> void:
	HudWidgets.label(body, &"TitleLabel", body.tr("lab.switches").to_upper())
	for g: Dictionary in DataTables.game("render_switches").groups:
		for item: Dictionary in g.items:
			var row := HudWidgets.hbox(body, &"Pad")
			HudWidgets.label(row, &"BodyLabel", body.tr("fx." + item.key)).size_flags_horizontal = Control.SIZE_EXPAND_FILL
			_test(row, run, item.key)
			if run.results.has(item.key):
				_result(body, run.results[item.key])


static func _test(row: Control, run: DoctorRun, key: String) -> void:
	var b := HudWidgets.button(&"Choice", "", row.tr("lab.test"), "", "", "opt_row_h")
	b.disabled = run.busy
	b.pressed.connect(run.compare.bind(key))
	row.add_child(b)


static func _result(body: Control, c: Dictionary) -> void:
	var line := body.tr("lab.ab") % [_ms(c.gpu_before), _ms(c.gpu_after), _ms(c.noise_gpu), _ms(c.cpu_before), _ms(c.cpu_after), _ms(c.noise_cpu)]
	if not c.real:
		line += " · " + body.tr("lab.ab.noise")
	HudWidgets.label(body, &"MetaLabel", line).autowrap_mode = TextServer.AUTOWRAP_WORD_SMART
