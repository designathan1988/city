## The interface's rules without the screen: the tier the GPU's name gives, the tier governor and the dynamic resolution, the chosen
## options over a tier, the render switches on the cull mask, the Simulation panel's levels reaching the core, keys and Escape.
extends Node3D

const DRAWER_FRAMES := 6
const RELIEF_SLIDERS := 3
const TOWN_MAP := 0
const SEED := 1
const CHOSEN_SEED := 4321
const COST_SLOW_MS := 30.0
const COST_FAST_MS := 4.0
const BUDGET_MS := 16.0
const DT := 0.016

var failures := 0


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _ready() -> void:
	_gpu_tiers()
	_governor()
	_resolution()
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	_check(errors.is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	add_child(ToolController.new(rig))
	var hud := RoadcraftHud.new()
	add_child(hud)
	Settings.attach(world, get_viewport())
	_options(world, rig)
	_levels()
	_panels(hud)
	await _session(rig)
	await _drawers(hud)
	_new_city(hud)
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


func _gpu_tiers() -> void:
	var t: Dictionary = DataTables.game("settings")
	for pair: Array in [["NVIDIA GeForce RTX 3060", "ultra"], ["NVIDIA GeForce RTX 2060", "high"], ["Intel(R) UHD Graphics 770", "low"],
			["AMD Radeon RX 6600", "high"], ["Microsoft Basic Render Driver", "low"], ["Chip desconhecido", "medium"]]:
		_check(QualityTier.tier_for_gpu(t, pair[0]) == pair[1], "%s -> %s" % pair)


func _governor() -> void:
	var t: Dictionary = DataTables.game("settings")
	var g := FrameGovernor.Quality.new(t.governor, t.levels, "high", "high")
	var next := ""
	var frames := 0
	while next == "" and frames < 2000:
		next = g.sample(DT, COST_SLOW_MS, BUDGET_MS)
		frames += 1
	_check(next == "medium", "quadros acima do orçamento descem um nível (%s em %d quadros)" % [next, frames])
	var expected: int = int(t.governor.settle) + int(t.governor.window) * int(t.governor.down_windows)
	_check(frames == expected, "desce depois de %d janelas, passada a espera" % int(t.governor.down_windows))
	g.set_level(next)
	next = ""
	frames = 0
	while next == "" and frames < 4000:
		next = g.sample(DT, COST_FAST_MS, BUDGET_MS)
		frames += 1
	_check(next == "high", "folga sobe de novo (%s)" % next)
	g.set_level("high")
	var stay := ""
	for i in 2000:
		stay = g.sample(DT, COST_FAST_MS, BUDGET_MS)
		if stay != "":
			break
	_check(stay == "", "nunca passa do teto do hardware")
	_check(g.sample(1.0, COST_SLOW_MS, BUDGET_MS) == "", "travada não é janela")


func _resolution() -> void:
	var r := FrameGovernor.Resolution.new(DataTables.game("settings").dynamic_resolution)
	var scale := 0.0
	for i in 200:
		var s := r.sample(COST_SLOW_MS, BUDGET_MS)
		if s > 0.0:
			scale = s
			break
	_check(scale > 0.0 and scale < 1.0, "resolução dinâmica cai quando a GPU não cabe (%.2f)" % scale)
	r.reset()
	_check(is_equal_approx(r.scale, 1.0), "desligada volta à resolução inteira")


func _options(world: CityWorld, rig: CityCamera) -> void:
	Settings.set_preset("ultra")
	var aa := Viewport.SCREEN_SPACE_AA_SMAA if QualityTier.has_smaa() else Viewport.SCREEN_SPACE_AA_DISABLED
	_check(world.environment.ssao_enabled and get_viewport().screen_space_aa == aa, "ultra liga oclusão e SMAA onde ele existe")
	Settings.set_preset("low")
	_check(not world.sun.shadow_enabled and not world.environment.ssao_enabled, "baixa tira sombra e oclusão")
	var names: Dictionary = CityStore.constants.render_layers
	_check(rig.camera.cull_mask & int(names.furniture) == 0, "baixa esconde o mobiliário pela máscara")
	Settings.keep("shadows", 2048)
	_check(world.sun.shadow_enabled, "sombra escolhida vale sobre o nível")
	Settings.set_preset("high")
	_check(Settings.kept.is_empty(), "um nível escolhido esquece as escolhas do anterior")
	Settings.switches.set_on("roads", false)
	Settings.switches_changed()
	_check(rig.camera.cull_mask & int(names.roads) == 0, "chave desliga a camada de vias")
	Settings.switches.reset()
	Settings.switches_changed()
	_check(rig.camera.cull_mask & int(names.roads) != 0, "tudo ligado devolve a camada")
	Settings.optimizations = {"shadowMapSize": 1024}
	_check(int(Settings.current().shadow_px) == 1024, "otimização vale sobre o nível")
	Settings.optimizations = {}


func _levels() -> void:
	CityStore.set_levels(2.0, 0.5, 1.55)
	var s := CityStore.snapshot
	_check(is_equal_approx(s.traffic_level, 2.0) and is_equal_approx(s.people_level, 0.5) and is_equal_approx(s.demand, 1.55), "painel chega ao núcleo")
	CityStore.set_levels(1.0, 1.0, 1.0)
	var m: Dictionary = CityStore.query(&"traffic_metrics")
	_check(m.has("mean_speed_ms") and m.has("queued"), "métricas de trânsito")


## A city comes back with its levels and its view; a save that does not read goes aside, not deleted.
func _session(rig: CityCamera) -> void:
	CityStore.set_levels(2.0, 0.5, 1.55)
	rig.jump(Vector3(100, 0, 50), 30.0, 50.0, 800.0)
	CityStore.saves.save_city()
	await CityStore.saved
	var slot := CityStore.saves.city_slot()
	CityStore.set_levels(1.0, 1.0, 1.0)
	rig.jump(Vector3.ZERO, 0.0, 50.0, 400.0)
	_check(CityStore.saves.load_game(slot).get("ok", false), "abriu a cidade salva")
	_check(is_equal_approx(CityStore.snapshot.traffic_level, 2.0), "os níveis voltaram com a cidade")
	_check(Vector2(rig.focus.x, rig.focus.z).distance_to(Vector2(100, 50)) < 1.0, "a vista voltou com a cidade (%s)" % rig.focus)
	CityStore.set_levels(1.0, 1.0, 1.0)
	_check(SaveSlots.quarantine(slot) == OK and not SaveSlots.exists(slot), "save ilegível vai para a quarentena")
	var aside := "%s/%s" % [SaveSlots.dir(), SaveSlots.QUARANTINE_DIR]
	var kept := Array(DirAccess.get_files_at(aside)).filter(func(f: String) -> bool: return f.begins_with(slot))
	_check(kept.size() == 1, "guardado, nunca apagado")
	for f: String in kept:
		DirAccess.remove_absolute("%s/%s" % [aside, f])
	DirAccess.remove_absolute(Session.path(slot))


## The title screen's form: left alone it asks for nothing, a new seed asks for that city.
func _new_city(hud: RoadcraftHud) -> void:
	_check(HudActions.map_index("town") == 0, "a cidade pequena é o mapa 0")
	_check(HudActions.map_index("nao_existe") == -1, "mapa que a tabela não tem")
	var asked := []
	var keep := StartScreen.new()
	keep.city_chosen.connect(func(map: int, seed: int) -> void: asked.append([map, seed]))
	hud.root.add_child(keep)
	keep._pick("play")
	_check(asked.is_empty(), "sem mexer no formulário não abre cidade nova")
	var form := StartScreen.new()
	form.city_chosen.connect(func(map: int, seed: int) -> void: asked.append([map, seed]))
	hud.root.add_child(form)
	form.seed_box.value = CHOSEN_SEED
	form._pick("play")
	_check(asked == [[0, CHOSEN_SEED]], "a semente do formulário vai para a cidade nova: %s" % [asked])
	hud.actions.new_city_at(0, CHOSEN_SEED)
	_check(int(CityStore.snapshot.seed) == CHOSEN_SEED, "o núcleo abre a cidade dessa semente")


## The Landscape drawer: the brush's sliders on the left, inside its width; the Models shelf scrolls inside the window.
func _drawers(hud: RoadcraftHud) -> void:
	hud.open_category("landscape")
	await _wait(DRAWER_FRAMES)
	var options: Control = hud.drawer.options
	_check(options.find_children("*", "HSlider", true, false).size() == RELIEF_SLIDERS, "sliders do Relevo no painel de opções")
	_check(hud.drawer.strip.find_children("*", "HSlider", true, false).is_empty(), "nenhum slider na gaveta")
	_check(hud.drawer.options_panel.size.x <= HudWidgets.px("opts_w"), "painel na largura do 4.0 (%.0f)" % hud.drawer.options_panel.size.x)
	hud.drawer.select_tab(hud.drawer.category.tabs[1])
	await _wait(DRAWER_FRAMES)
	_check(hud.drawer.options_panel.size.x <= HudWidgets.px("opts_w"), "biomas cabem no painel (%.0f)" % hud.drawer.options_panel.size.x)
	hud.open_category("build")
	await _wait(DRAWER_FRAMES)
	var right := hud.drawer.get_global_rect().end.x
	_check(right <= hud.drawer.get_viewport_rect().size.x, "a prateleira de modelos fica dentro da janela (%.0f)" % right)
	hud.drawer.close()


func _wait(frames: int) -> void:
	for i in frames:
		await get_tree().process_frame


func _panels(hud: RoadcraftHud) -> void:
	hud.open_popover("menu")
	_check(hud.popover.visible, "menu abre")
	_check(hud.escape() and not hud.popover.visible, "Esc fecha o menu antes da ferramenta")
	hud.open_category("roads")
	_check(hud.escape() and hud.drawer.category.is_empty(), "Esc larga a ferramenta")
	_check(not hud.escape(), "Esc sem nada na mão passa adiante")
	hud.health.toggle()
	Health.record("error", "broken", "teste", "detalhe")
	Health.record("error", "broken", "teste", "detalhe")
	_check(Health.entries[0].count == 2 and Health.status() == "broken", "saúde conta repetição e fica vermelha")
	hud.health.close()
