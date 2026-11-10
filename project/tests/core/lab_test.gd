## The F7 Diagnosis: the verdict as the 4.0 gives it (answers of its verdictOf, by tools/converter/scripts/call40.mjs), the A/B's
## arithmetic, and, through the panel's own button, a measure of the real city. On Forward+ the stages add up to the GPU's frame and
## switching the shadows off shows in the A/B; the switch comes back as it was.
extends TestCase

const CPU_FRAMES := 30
const CPU_TAIL := 0.95
const TOWN_MAP := 0
const SEED := 1
const GPU_ROWS := [[20.0, 5.0, 18.0, "gpu"], [20.0, 18.0, 5.0, "cpu"], [10.0, 5.0, 4.0, "headroom"], [20.0, 18.0, 18.0, "gpu"],
	[14.2, 14.2, 14.1, "cpu"], [14.2, 14.1, 14.16, "headroom"]]
## What the GPU's clock says depends on what else the machine draws: the measures print their times and the checks hold what
## does not move with the load (frames read, stages present, what is drawn, what is given back).
const STRETCH_FRAMES := 10 # frames in a stretch once it measures
const SETTLE_FRAMES_MOST := 3000 # the recorder's settle runs on a real timer: past this many frames the wait is a failure
const CHECKS_LEAST := 8


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	var hud := RoadcraftHud.new()
	add_child(hud)
	Settings.attach(world, get_viewport())
	_verdicts()
	_comparison()
	await _measure(hud)
	await _cpu(hud)
	await _capture(hud)
	await _tools()
	finish()


## The panel's other tools: a recording cut at a change, a leak read as a slope, the views, and a sweep of one switch.
func _tools() -> void:
	var rec := LabRecorder.new()
	add_child(rec)
	rec.start()
	await _stretch(rec)
	Settings.switches_changed()
	await _stretch(rec)
	var path := rec.stop()
	check(rec.stretches.size() == 2, "the change cuts the recording: %d stretches" % rec.stretches.size())
	check(rec.stretches.size() > 0 and rec.stretches[0].frames > 0, "the stretch has frames")
	check(path != "" and FileAccess.file_exists(path), "the report was saved: %s" % path)
	DirAccess.remove_absolute(path)
	var rising: Array[Dictionary] = [{"t": 0.0, "v": 10.0}, {"t": 30.0, "v": 15.0}, {"t": 60.0, "v": 20.0}]
	check(is_equal_approx(ResourceWatch.slope(rising, "v"), 10.0), "the slope is per minute")
	var watch := ResourceWatch.new()
	watch.sample(0.0)
	watch.sample(1.0)
	check(watch.checks().size() >= CHECKS_LEAST, "the checks come with a verdict")
	LabTools.set_view(get_viewport(), "OVERDRAW")
	check(get_viewport().debug_draw == Viewport.DEBUG_DRAW_OVERDRAW, "the GPU view changes the drawing")
	LabTools.set_view(get_viewport(), "DISABLED")
	var run := DoctorRun.new()
	add_child(run)
	var keys := run.all_switches()
	await run.sweep(keys.slice(0, 1))
	check(run.results.has(keys[0]), "the sweep measures the switch")


func _frames(interval: float, cpu: float, gpu: Variant) -> Dictionary:
	var gpus := PackedFloat64Array() if gpu == null else PackedFloat64Array([gpu])
	return {"interval": PackedFloat64Array([interval]), "cpu": PackedFloat64Array([cpu]), "gpu": gpus,
		"draws": PackedFloat64Array([1.0]), "primitives": PackedFloat64Array([1.0]), "stages": {}}


## The rows the 4.0 answered, and the ones with no timer or no frame.
func _verdicts() -> void:
	var rules: Dictionary = DataTables.game("render_switches").doctor
	for row: Array in GPU_ROWS:
		var out: Dictionary = CityStore.query(&"lab_diagnose", [_frames(row[0], row[1], row[2]), rules])
		check(out.bound == row[3], "verdict of %s: %s" % [row, out.bound])
	var no_timer: Dictionary = CityStore.query(&"lab_diagnose", [_frames(20.0, 18.0, null), rules])
	check(no_timer.bound == "cpu" and no_timer.gpu_ms == null, "with no timer the CPU decides")
	var empty := _frames(0.0, 0.0, null)
	empty.interval = PackedFloat64Array()
	empty.cpu = PackedFloat64Array()
	check(CityStore.query(&"lab_diagnose", [empty, rules]).bound == "none", "with no frame there is no verdict")


func _comparison() -> void:
	var c: Dictionary = CityStore.query(&"lab_compare", [_frames(16.0, 3.0, 8.0), _frames(16.0, 3.0, 5.0), _frames(16.0, 3.0, 8.2)])
	check(is_equal_approx(c.gpu_before, 8.1) and is_equal_approx(c.noise_gpu, 0.2) and c.real, "before is the mean, the noise is the distance: %s" % c)
	var same: Dictionary = CityStore.query(&"lab_compare", [_frames(16.0, 3.0, 8.0), _frames(16.0, 3.0, 8.1), _frames(16.0, 3.0, 8.4)])
	check(not same.real, "within the noise is no effect: %s" % same)


func _button(root: Node, text: String) -> Button:
	for b: Button in root.find_children("*", "Button", true, false):
		if b.text == text:
			return b
	return null


func _idle(run: DoctorRun) -> void:
	while run.busy:
		await run.changed


## The recorder settled and measuring a stretch of some frames (its settle is a real timer: waited for by its state).
func _stretch(rec: LabRecorder) -> void:
	var waited := 0
	while not rec.measuring() and waited < SETTLE_FRAMES_MOST:
		await get_tree().process_frame
		waited += 1
	check(rec.measuring(), "the recorder measures once the change settled (%d frames)" % waited)
	for i in STRETCH_FRAMES:
		await get_tree().process_frame


## As the player: F7, the tab, the button.
## The frame taken apart by layer: buildings and terrain draw something, the camera ends as it began, and the file is there.
func _capture(hud: RoadcraftHud) -> void:
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		check(false, "there is a camera for the capture")
		return
	var mask := camera.cull_mask
	await hud.effects.frame.take()
	print("frame: ", hud.effects.frame.whole, " layers: ", hud.effects.frame.rows)
	check(camera.cull_mask == mask, "the capture gives the camera back as it was")
	check(not hud.effects.frame.rows.is_empty() and hud.effects.frame.rows[0].primitives > 0, "some layer draws")
	check(FileAccess.file_exists(hud.effects.frame.file), "the capture stayed in a file")


## The CPU tab reads a window of frames, the heaviest zone first.
func _cpu(hud: RoadcraftHud) -> void:
	hud.effects.open_tab("cpu")
	for i in CPU_FRAMES:
		await get_tree().process_frame
	var rows: Array[Dictionary] = hud.effects.cpu.rows(CPU_TAIL)
	print("cpu: ", rows.slice(0, 3))
	check(hud.effects.cpu.frames() >= CPU_FRAMES - 1, "the CPU tab gathered the frames (%d)" % hud.effects.cpu.frames())
	check(not rows.is_empty() and rows[0].mean >= rows[-1].mean and rows[0].tail >= 0.0, "the zones come heaviest first")


func _measure(hud: RoadcraftHud) -> void:
	hud.effects.toggle()
	hud.effects.open_tab("doctor")
	await get_tree().process_frame
	await get_tree().process_frame
	var go := _button(hud.effects, tr("lab.measure"))
	check(go != null, "the tab has the measure button")
	if go == null:
		return
	var run := hud.effects.doctor
	go.pressed.emit()
	await _idle(run)
	print("report: ", run.report)
	check(run.report.frames > 0 and run.report.bound != "none", "the measure read frames: %s" % run.report.bound)
	if RenderingServer.get_rendering_device() == null:
		check(run.report.findings.is_empty(), "with no device there are no stages")
		return
	print("stages %.2f ms of a GPU frame of %.2f ms" % [run.report.stages_ms, run.report.gpu_mean_ms])
	check(run.report.stages_ms > 0.0, "the stages are timed: %.2f ms" % run.report.stages_ms)
	check(get_viewport().get_camera_3d().compositor == null, "the clock left the camera")
	run.compare("shadows")
	await _idle(run)
	var c: Dictionary = run.results.shadows
	print("shadows: ", c)
	check(c.primitives_after < c.primitives_before, "turning shadows off draws less: %d -> %d" % [c.primitives_before, c.primitives_after])
	check(Settings.switches.is_on("shadows"), "the switch came back as it was")
