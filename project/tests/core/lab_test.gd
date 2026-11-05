## The F7 Diagnosis: the verdict as the 4.0 gives it (answers of its verdictOf, by tools/converter/scripts/call40.mjs), the A/B's
## arithmetic, and, through the panel's own button, a measure of the real city. On Forward+ the stages add up to the GPU's frame and
## switching the shadows off shows in the A/B; the switch comes back as it was.
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const GPU_ROWS := [[20.0, 5.0, 18.0, "gpu"], [20.0, 18.0, 5.0, "cpu"], [10.0, 5.0, 4.0, "headroom"], [20.0, 18.0, 18.0, "gpu"],
	[14.2, 14.2, 14.1, "cpu"], [14.2, 14.1, 14.16, "headroom"]]
## How far the stages' sum may stand from the GPU's frame: the stamps start a little after the viewport's timer.
const STAGES_SHARE := 0.15

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
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


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


## As the player: F7, the tab, the button.
func _measure(hud: RoadcraftHud) -> void:
	hud.effects.toggle()
	hud.effects.open_tab("doctor")
	await get_tree().process_frame
	await get_tree().process_frame
	var go := _button(hud.effects, tr("lab.measure"))
	_check(go != null, "a aba tem o botão de medir")
	if go == null:
		return
	go.pressed.emit()
	var run := hud.effects.doctor
	await _idle(run)
	print("relatório: ", run.report)
	_check(run.report.frames > 0 and run.report.bound != "none", "a medida leu quadros: %s" % run.report.bound)
	if RenderingServer.get_rendering_device() == null:
		_check(run.report.findings.is_empty(), "sem dispositivo não há estágios")
		return
	var sum: float = run.report.stages_ms
	var gpu: float = run.report.gpu_mean_ms
	_check(absf(sum - gpu) <= gpu * STAGES_SHARE, "os estágios somam a GPU: %.2f de %.2f" % [sum, gpu])
	_check(get_viewport().get_camera_3d().compositor == null, "o relógio saiu da câmera")
	run.compare("shadows")
	await _idle(run)
	var c: Dictionary = run.results.shadows
	print("sombras: ", c)
	_check(c.gpu_after < c.gpu_before and c.real, "desligar as sombras corta a GPU")
	_check(c.primitives_after < c.primitives_before, "e o que é desenhado")
	_check(Settings.switches.is_on("shadows"), "a chave voltou como estava")
