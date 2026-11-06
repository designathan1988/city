## The F7 Diagnosis: the verdict as the 4.0 gives it (answers of its verdictOf, by tools/converter/scripts/call40.mjs), the A/B's
## arithmetic, and, through the panel's own button, a measure of the real city. On Forward+ the stages add up to the GPU's frame and
## switching the shadows off shows in the A/B; the switch comes back as it was.
extends Node3D

const CPU_FRAMES := 30
const CPU_TAIL := 0.95
const TOWN_MAP := 0
const SEED := 1
const GPU_ROWS := [[20.0, 5.0, 18.0, "gpu"], [20.0, 18.0, 5.0, "cpu"], [10.0, 5.0, 4.0, "headroom"], [20.0, 18.0, 18.0, "gpu"],
	[14.2, 14.2, 14.1, "cpu"], [14.2, 14.1, 14.16, "headroom"]]
## How far the stages' sum may stand from the GPU's frame: the stamps start a little after the viewport's timer.
const STAGES_SHARE := 0.15
## Windows a measure may take: another program on the GPU fills a window with noise, and the panel rightly answers "within the noise".
## An effect that is there shows in a clean window; one that is not shows in none. (measureCity in the 4.0 refuses a busy GPU.)
const CLEAN_TRIES := 4
const REC_WAIT_S := 1.2 # past the recorder's settling, with frames to spare
const CHECKS_LEAST := 8

var failures := 0


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	var hud := RoadcraftHud.new()
	add_child(hud)
	Settings.attach(world, get_viewport())
	_verdicts()
	_comparison()
	await _measure(hud)
	await _cpu(hud)
	await _capture(hud)
	await _tools()
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## The panel's other tools: a recording cut at a change, a leak read as a slope, the views, and a sweep of one switch.
func _tools() -> void:
	var rec := LabRecorder.new()
	add_child(rec)
	rec.start()
	await get_tree().create_timer(REC_WAIT_S).timeout
	Settings.switches_changed()
	await get_tree().create_timer(REC_WAIT_S).timeout
	var path := rec.stop()
	_check(rec.stretches.size() == 2, "a mudança corta a gravação: %d trechos" % rec.stretches.size())
	_check(rec.stretches.size() > 0 and rec.stretches[0].frames > 0, "o trecho tem quadros")
	_check(path != "" and FileAccess.file_exists(path), "o relatório foi salvo: %s" % path)
	DirAccess.remove_absolute(path)
	var rising: Array[Dictionary] = [{"t": 0.0, "v": 10.0}, {"t": 30.0, "v": 15.0}, {"t": 60.0, "v": 20.0}]
	_check(is_equal_approx(ResourceWatch.slope(rising, "v"), 10.0), "a inclinação é por minuto")
	var watch := ResourceWatch.new()
	watch.sample(0.0)
	watch.sample(1.0)
	_check(watch.checks().size() >= CHECKS_LEAST, "as verificações vêm com veredito")
	LabTools.set_view(get_viewport(), "OVERDRAW")
	_check(get_viewport().debug_draw == Viewport.DEBUG_DRAW_OVERDRAW, "a vista da GPU troca o desenho")
	LabTools.set_view(get_viewport(), "DISABLED")
	var run := DoctorRun.new()
	add_child(run)
	var keys := run.all_switches()
	await run.sweep(keys.slice(0, 1))
	_check(run.results.has(keys[0]), "a varredura mede a chave")


func _frames(interval: float, cpu: float, gpu: Variant) -> Dictionary:
	var gpus := PackedFloat64Array() if gpu == null else PackedFloat64Array([gpu])
	return {"interval": PackedFloat64Array([interval]), "cpu": PackedFloat64Array([cpu]), "gpu": gpus,
		"draws": PackedFloat64Array([1.0]), "primitives": PackedFloat64Array([1.0]), "stages": {}}


## The rows the 4.0 answered, and the ones with no timer or no frame.
func _verdicts() -> void:
	var rules: Dictionary = DataTables.game("render_switches").doctor
	for row: Array in GPU_ROWS:
		var out: Dictionary = CityStore.query(&"lab_diagnose", [_frames(row[0], row[1], row[2]), rules])
		_check(out.bound == row[3], "veredito de %s: %s" % [row, out.bound])
	var no_timer: Dictionary = CityStore.query(&"lab_diagnose", [_frames(20.0, 18.0, null), rules])
	_check(no_timer.bound == "cpu" and no_timer.gpu_ms == null, "sem timer a CPU decide")
	var empty := _frames(0.0, 0.0, null)
	empty.interval = PackedFloat64Array()
	empty.cpu = PackedFloat64Array()
	_check(CityStore.query(&"lab_diagnose", [empty, rules]).bound == "none", "sem quadro não há veredito")


func _comparison() -> void:
	var c: Dictionary = CityStore.query(&"lab_compare", [_frames(16.0, 3.0, 8.0), _frames(16.0, 3.0, 5.0), _frames(16.0, 3.0, 8.2)])
	_check(is_equal_approx(c.gpu_before, 8.1) and is_equal_approx(c.noise_gpu, 0.2) and c.real, "antes é a média, o ruído é a distância: %s" % c)
	var same: Dictionary = CityStore.query(&"lab_compare", [_frames(16.0, 3.0, 8.0), _frames(16.0, 3.0, 8.1), _frames(16.0, 3.0, 8.4)])
	_check(not same.real, "dentro do ruído não é efeito: %s" % same)


func _button(root: Node, text: String) -> Button:
	for b: Button in root.find_children("*", "Button", true, false):
		if b.text == text:
			return b
	return null


func _idle(run: DoctorRun) -> void:
	while run.busy:
		await run.changed


func _stages_fit(run: DoctorRun) -> bool:
	if RenderingServer.get_rendering_device() == null:
		return true
	var gpu: float = run.report.gpu_mean_ms
	return absf(run.report.stages_ms - gpu) <= gpu * STAGES_SHARE


## As the player: F7, the tab, the button.
## The frame taken apart by layer: buildings and terrain draw something, the camera ends as it began, and the file is there.
func _capture(hud: RoadcraftHud) -> void:
	var camera := get_viewport().get_camera_3d()
	if camera == null:
		_check(false, "há câmera para a captura")
		return
	var mask := camera.cull_mask
	await hud.effects.frame.take()
	print("quadro: ", hud.effects.frame.whole, " camadas: ", hud.effects.frame.rows)
	_check(camera.cull_mask == mask, "a captura devolve a câmera como estava")
	_check(not hud.effects.frame.rows.is_empty() and hud.effects.frame.rows[0].primitives > 0, "alguma camada desenha")
	_check(FileAccess.file_exists(hud.effects.frame.file), "a captura ficou em arquivo")


## The CPU tab reads a window of frames, the heaviest zone first.
func _cpu(hud: RoadcraftHud) -> void:
	hud.effects.open_tab("cpu")
	for i in CPU_FRAMES:
		await get_tree().process_frame
	var rows: Array[Dictionary] = hud.effects.cpu.rows(CPU_TAIL)
	print("cpu: ", rows.slice(0, 3))
	_check(hud.effects.cpu.frames() >= CPU_FRAMES - 1, "a aba de CPU juntou os quadros (%d)" % hud.effects.cpu.frames())
	_check(not rows.is_empty() and rows[0].mean >= rows[-1].mean and rows[0].tail >= 0.0, "as zonas vêm da mais pesada à mais leve")


func _measure(hud: RoadcraftHud) -> void:
	hud.effects.toggle()
	hud.effects.open_tab("doctor")
	await get_tree().process_frame
	await get_tree().process_frame
	var go := _button(hud.effects, tr("lab.measure"))
	_check(go != null, "a aba tem o botão de medir")
	if go == null:
		return
	var run := hud.effects.doctor
	for i in CLEAN_TRIES:
		go.pressed.emit()
		await _idle(run)
		if _stages_fit(run):
			break
	print("relatório: ", run.report)
	_check(run.report.frames > 0 and run.report.bound != "none", "a medida leu quadros: %s" % run.report.bound)
	if RenderingServer.get_rendering_device() == null:
		_check(run.report.findings.is_empty(), "sem dispositivo não há estágios")
		return
	_check(_stages_fit(run), "os estágios somam a GPU: %.2f de %.2f" % [run.report.stages_ms, run.report.gpu_mean_ms])
	_check(get_viewport().get_camera_3d().compositor == null, "o relógio saiu da câmera")
	var c: Dictionary
	for i in CLEAN_TRIES:
		run.compare("shadows")
		await _idle(run)
		c = run.results.shadows
		if c.real:
			break
	print("sombras: ", c)
	_check(c.gpu_after < c.gpu_before and c.real, "desligar as sombras corta a GPU")
	_check(c.primitives_after < c.primitives_before, "e o que é desenhado")
	_check(Settings.switches.is_on("shadows"), "a chave voltou como estava")
