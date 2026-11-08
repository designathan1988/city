## The interface's rules without the screen: the tier the GPU's name gives, the tier governor and the dynamic resolution, the chosen
## options over a tier, the render switches on the cull mask, the Simulation panel's levels reaching the core, keys and Escape.
extends TestCase

const DRAWER_FRAMES := 6
const FORCE_PRESSES := 7
const RELIEF_SLIDERS := 3
const TOWN_MAP := 0
const SEED := 1
const CHOSEN_SEED := 4321
const COST_SLOW_MS := 30.0
const COST_FAST_MS := 4.0
const BUDGET_MS := 16.0
const DT := 0.016
## Free ground past the town, where a road can start.
const FIELD_M := 100.0


func _ready() -> void:
	_gpu_tiers()
	_governor()
	_resolution()
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	add_child(ToolController.new(rig))
	var hud := RoadcraftHud.new()
	add_child(hud)
	Settings.attach(world, get_viewport())
	_options(rig)
	_levels()
	_panels(hud)
	await _session(rig)
	await _drawers(hud)
	await _bomb_force(hud)
	_new_city(hud)
	finish()


func _gpu_tiers() -> void:
	var t: Dictionary = DataTables.game("settings")
	for pair: Array in [["NVIDIA GeForce RTX 3060", "ultra"], ["NVIDIA GeForce RTX 2060", "high"], ["Intel(R) UHD Graphics 770", "low"],
			["AMD Radeon RX 6600", "high"], ["Microsoft Basic Render Driver", "low"], ["Unknown chip", "medium"]]:
		check(QualityTier.tier_for_gpu(t, pair[0]) == pair[1], "%s -> %s" % pair)


func _governor() -> void:
	var t: Dictionary = DataTables.game("settings")
	var g := FrameGovernor.Quality.new(t.governor, t.levels, "high", "high")
	var next := ""
	var frames := 0
	while next == "" and frames < 2000:
		next = g.sample(DT, COST_SLOW_MS, BUDGET_MS)
		frames += 1
	check(next == "medium", "frames over budget go down a tier (%s in %d frames)" % [next, frames])
	var expected: int = int(t.governor.settle) + int(t.governor.window) * int(t.governor.down_windows)
	check(frames == expected, "goes down after %d windows, the wait over" % int(t.governor.down_windows))
	g.set_level(next)
	next = ""
	frames = 0
	while next == "" and frames < 4000:
		next = g.sample(DT, COST_FAST_MS, BUDGET_MS)
		frames += 1
	check(next == "high", "headroom goes up again (%s)" % next)
	g.set_level("high")
	var stay := ""
	for i in 2000:
		stay = g.sample(DT, COST_FAST_MS, BUDGET_MS)
		if stay != "":
			break
	check(stay == "", "never past the hardware's ceiling")
	check(g.sample(1.0, COST_SLOW_MS, BUDGET_MS) == "", "a hitch is no window")


func _resolution() -> void:
	var r := FrameGovernor.Resolution.new(DataTables.game("settings").dynamic_resolution)
	var scale := 0.0
	for i in 200:
		var s := r.sample(COST_SLOW_MS, BUDGET_MS)
		if s > 0.0:
			scale = s
			break
	check(scale > 0.0 and scale < 1.0, "dynamic resolution falls when the GPU does not fit (%.2f)" % scale)
	r.reset()
	check(is_equal_approx(r.scale, 1.0), "off, it returns to the whole resolution")


func _options(rig: CityCamera) -> void:
	Settings.set_preset("ultra")
	var aa := Viewport.SCREEN_SPACE_AA_SMAA if QualityTier.has_smaa() else Viewport.SCREEN_SPACE_AA_DISABLED
	check(world.occlusion.visible == world.occlusion.available() and get_viewport().screen_space_aa == aa, "ultra turns occlusion and SMAA on where they exist")
	Settings.set_preset("low")
	check(not world.sun.shadow_enabled and not world.occlusion.visible, "low takes shadow and occlusion off")
	var names: Dictionary = CityStore.constants.render_layers
	check(rig.camera.cull_mask & int(names.furniture) == 0, "low hides the furniture by the mask")
	Settings.keep("shadows", 2048)
	check(world.sun.shadow_enabled, "a chosen shadow stands over the tier")
	Settings.set_preset("high")
	check(Settings.kept.is_empty(), "a chosen tier forgets the previous one's choices")
	Settings.switches.set_on("roads", false)
	Settings.switches_changed()
	check(rig.camera.cull_mask & int(names.roads) == 0, "the switch turns the road layer off")
	Settings.switches.reset()
	Settings.switches_changed()
	check(rig.camera.cull_mask & int(names.roads) != 0, "all on gives the layer back")
	Settings.optimizations = {"shadowMapSize": 1024}
	check(int(Settings.current().shadow_px) == 1024, "an optimisation stands over the tier")
	Settings.optimizations = {}


func _levels() -> void:
	CityStore.set_levels(2.0, 0.5, 1.55)
	var s := CityStore.snapshot
	check(is_equal_approx(s.traffic_level, 2.0) and is_equal_approx(s.people_level, 0.5) and is_equal_approx(s.demand, 1.55), "the panel reaches the core")
	CityStore.set_levels(1.0, 1.0, 1.0)
	var m: Dictionary = CityStore.query(&"traffic_metrics")
	check(m.has("mean_speed_ms") and m.has("queued"), "traffic metrics")


## A city comes back with its levels and its view; a save that does not read goes aside, not deleted.
func _session(rig: CityCamera) -> void:
	CityStore.set_levels(2.0, 0.5, 1.55)
	rig.jump(Vector3(100, 0, 50), 30.0, 50.0, 800.0)
	var saved: Dictionary = await save()
	check(saved.get("ok", false), "the city is written to its own slot: %s" % saved)
	var slot := CityStore.saves.city_slot()
	CityStore.set_levels(1.0, 1.0, 1.0)
	rig.jump(Vector3.ZERO, 0.0, 50.0, 400.0)
	check((await CityStore.saves.load_game(slot, true)).get("ok", false), "opened the saved city")
	check(is_equal_approx(CityStore.snapshot.traffic_level, 2.0), "the levels came back with the city")
	check(Vector2(rig.focus.x, rig.focus.z).distance_to(Vector2(100, 50)) < 1.0, "the view came back with the city (%s)" % rig.focus)
	CityStore.set_levels(1.0, 1.0, 1.0)
	check(SaveSlots.quarantine(slot) == OK and not SaveSlots.exists(slot), "an unreadable save goes to quarantine")
	var aside := "%s/%s" % [SaveSlots.dir(), SaveSlots.QUARANTINE_DIR]
	var kept := Array(DirAccess.get_files_at(aside)).filter(func(f: String) -> bool: return f.begins_with(slot))
	check(kept.size() == 1, "kept, never deleted")
	for f: String in kept:
		DirAccess.remove_absolute("%s/%s" % [aside, f])
	DirAccess.remove_absolute(Session.path(slot))


## The title screen's form: left alone it asks for nothing, a new seed asks for that city.
func _new_city(hud: RoadcraftHud) -> void:
	check(HudActions.map_index("town") == 0, "the small city is map 0")
	check(HudActions.map_index("no_such_map") == -1, "a map the table does not have")
	var asked := []
	var keep := StartScreen.new()
	keep.city_chosen.connect(func(map: int, seed: int) -> void: asked.append([map, seed]))
	hud.root.add_child(keep)
	keep.pick("play")
	check(asked.is_empty(), "with the form untouched no new city opens")
	var form := StartScreen.new()
	form.city_chosen.connect(func(map: int, seed: int) -> void: asked.append([map, seed]))
	hud.root.add_child(form)
	form.seed_box.value = CHOSEN_SEED
	form.pick("play")
	check(asked == [[0, CHOSEN_SEED]], "the form's seed goes to the new city: %s" % [asked])
	hud.actions.new_city_at(0, CHOSEN_SEED)
	check(int(CityStore.snapshot.seed) == CHOSEN_SEED, "the core opens that seed's city")


## The force steps on at every press, and the tool in hand is the one that has it.
func _bomb_force(hud: RoadcraftHud) -> void:
	hud.open_tab("demolish", "bomb")
	await _wait(DRAWER_FRAMES)
	var keys: Array[Node] = hud.drawer.options.find_children("*", "Button", true, false)
	var at := int(DataTables.tuning("strike_force_start"))
	for i in FORCE_PRESSES:
		keys.back().pressed.emit()
		at = StrikeStrip.next_force(at, 1)
	check(hud.drawer.param("STRIKE", "strength", 0) == at, "the bomb's force steps on at every press (%d)" % at)


## The Landscape drawer: the brush's sliders on the left, inside its width; the Models shelf scrolls inside the window.
func _drawers(hud: RoadcraftHud) -> void:
	hud.open_category("landscape")
	await _wait(DRAWER_FRAMES)
	var options: Control = hud.drawer.options
	check(options.find_children("*", "HSlider", true, false).size() == RELIEF_SLIDERS, "the Relief sliders in the options panel")
	check(hud.drawer.strip.find_children("*", "HSlider", true, false).is_empty(), "no slider in the drawer")
	check(hud.drawer.options_panel.size.x <= HudWidgets.px("opts_w"), "a panel at the 4.0's width (%.0f)" % hud.drawer.options_panel.size.x)
	hud.drawer.select_tab(hud.drawer.category.tabs[1])
	await _wait(DRAWER_FRAMES)
	check(hud.drawer.options_panel.size.x <= HudWidgets.px("opts_w"), "biomes fit in the panel (%.0f)" % hud.drawer.options_panel.size.x)
	hud.open_category("build")
	await _wait(DRAWER_FRAMES)
	var right := hud.drawer.get_global_rect().end.x
	check(right <= hud.drawer.get_viewport_rect().size.x, "the model shelf stays inside the window (%.0f)" % right)
	hud.drawer.close()


func _wait(frames: int) -> void:
	for i in frames:
		await get_tree().process_frame


func _panels(hud: RoadcraftHud) -> void:
	hud.open_popover("menu")
	check(hud.popover.visible, "the menu opens")
	check(hud.escape() and not hud.popover.visible, "Esc closes the menu before the tool")
	hud.open_category("roads")
	var road: ToolBase = hud.tools.active
	var b: Dictionary = CityStore.query(&"city_bounds")
	road.click(Vector3(b.max.x + FIELD_M, 0, (b.min.z + b.max.z) * 0.5))
	check(road.in_progress(), "road started")
	check(hud.escape() and not road.in_progress() and not hud.drawer.category.is_empty(), "Esc drops the road being drawn, not the tool")
	check(hud.escape() and hud.drawer.category.is_empty(), "Esc drops the tool")
	check(not hud.escape(), "Esc with nothing in hand passes on")
	hud.health.toggle()
	Health.record("error", "broken", "test", "detail")
	Health.record("error", "broken", "test", "detail")
	check(Health.entries[0].count == 2 and Health.status() == "broken", "health counts repeats and turns red")
	hud.health.close()
