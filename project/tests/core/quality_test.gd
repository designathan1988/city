## The graphics tiers on what is really drawn: each field read back from the renderer (instances per level, the camera's mask, the
## viewport), the Effects panel's new switches and the Optimisations tab's options; the entry the sky's owner reads.
extends TestCase

## The city warmed by the test before the close-up counts, then stepped a little at a time until a car stands near the eye:
## the map is paused, so only these ticks move it (30 s, then rounds of 1 s, at most 90 more seconds).
const WARM_TICKS := 600
const CAR_TICKS := 20
const CAR_ROUNDS := 90
const DRAW_FRAMES := 2 # the renderer takes up the poses a tick published on its next frames
const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 240
const NEAR_DISTANCE := 90.0
const SETTLE_MIN := 30
const WILD_PAST_M := 500.0
const STREET_PITCH := 8.0
const PERSON_DISTANCE := 18.0
const PERSON_FRAMES := 8
const SETTLE_LIMIT_S := 25.0 # real time the mesh jobs may take (1500 frames of the old 60 Hz loop): StateWait
const FAR_DISTANCE := 5000.0
const OTHER_GPUS := ["NVIDIA GeForce RTX 4090", "NVIDIA GeForce RTX 3060", "AMD Radeon RX 6600", "Intel(R) UHD Graphics 630", "Unknown"]


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


var _sky: SkyView


func _ready() -> void:
	_player_file_read()
	var kept := FileAccess.get_file_as_bytes(Settings.PATH)
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	_sky = world.sky
	await _frames(SETTLE_FRAMES)
	await _tiers(rig)
	await _occlusion(rig)
	await _switches(rig)
	_options()
	_entry()
	_presets()
	check(FileAccess.get_file_as_bytes(Settings.PATH) == kept, "a test does not change the player's settings.cfg")
	finish()


## People on the near level, the camera close over the first walker there is.
func _people_close(rig: CityCamera) -> int:
	var walkers: Array = CityStore.query(&"traffic_probe", [1]).get("walkers", [])
	if walkers.is_empty():
		return -1
	rig.jump(walkers[0], 0.0, 60.0, PERSON_DISTANCE)
	await _frames(PERSON_FRAMES)
	return _cost().people_near


## Until everything asked of the renderer is drawn.
func _settled() -> void:
	await _frames(SETTLE_MIN)
	await StateWait.drawn(get_tree(), SETTLE_LIMIT_S)
	await _frames(SETTLE_MIN)


func _cost() -> Dictionary:
	return CityStore.query(&"agent_cost")


func _tiers(rig: CityCamera) -> void:
	var seen := {}
	for level: String in ["low", "ultra"]:
		Settings.set_preset(level)
		await _frames(SETTLE_FRAMES)
		var q := Settings.current()
		seen[level] = QualityTier.cabin_reach(q, get_viewport().get_visible_rect().size.y)
		print("  %s: cabin reach %.0f m, %s" % [level, seen[level], _cost()])
	check(seen.low < seen.ultra, "the low tier has its cabin nearer than ultra")
	Settings.set_preset("ultra")
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)
	await _frames(2)
	var names: Dictionary = CityStore.constants.render_layers
	check(rig.camera.cull_mask & int(names.people) != 0, "close up the people are in the mask")
	await _counts(rig)
	rig.jump(rig.focus, 0.0, 60.0, FAR_DISTANCE)
	await _frames(2)
	check(rig.camera.cull_mask & int(names.people) == 0, "past the detail cut: people out")
	check(rig.camera.cull_mask & int(names.vehicles) == 0, "past the detail cut: vehicles out")
	check(Settings.switches.globals[&"road_paint"] == 0.0, "past the cut: paint out")
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)
	await _frames(2)
	check(Settings.switches.globals[&"road_paint"] == 1.0, "back: paint again")


## Instances per level, close to the street: what the tiers change is what is drawn.
func _counts(rig: CityCamera) -> void:
	ticks(WARM_TICKS)
	await _settled()
	var c := _cost()
	for r in CAR_ROUNDS: # what is asked is that a car near is drawn with its cabin, once one has come by
		if int(c.cabins) > 0:
			break
		ticks(CAR_TICKS)
		await _frames(DRAW_FRAMES)
		c = _cost()
	var cabins: int = c.cabins
	var people := await _people_close(rig)
	print("  ultra close up: %d cabins, %d people at the near level, %d trees near" % [cabins, people, c.trees_near])
	check(cabins > 0, "ultra close up draws cabins")
	check(people > 0, "ultra close up draws people at the near level")
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)
	check(c.trees_near > 0, "ultra close up draws trees")
	Settings.optimizations["vehicleWhole"] = 1
	Settings.optimizations["pedestrianDetail"] = 0
	Settings.switches_changed()
	await _settled()
	c = _cost()
	people = await _people_close(rig)
	print("  with the options: %d cabins, %d people near, %d trees near" % [c.cabins, people, c.trees_near])
	check(c.cabins <= 2, "the cap of vehicles with a cabin (a frame behind, in the renderer's count)")
	check(people == 0, "detail 0: people at the far level only")
	Settings.optimizations.clear()
	Settings.switches_changed()
	await _settled()
	await _wild_trees(rig)
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)


## Out of town, where the woods are: the share of them the tier leaves.
func _wild_trees(rig: CityCamera) -> void:
	var back := rig.focus
	var edge: Vector3 = CityStore.query(&"city_bounds").max + Vector3(WILD_PAST_M, 0.0, WILD_PAST_M)
	rig.jump(edge, 0.0, 40.0, NEAR_DISTANCE)
	await _settled()
	var all: int = _cost().trees_near
	Settings.optimizations["vegetation"] = 0.5
	Settings.switches_changed()
	await _settled()
	var half: int = _cost().trees_near
	Settings.optimizations["vegetation"] = 0
	Settings.switches_changed()
	await _settled()
	var none: int = _cost().trees_near
	print("  woods outside the city: %d trees, %d with half, %d with none" % [all, half, none])
	check(all > 0 and half < all and none < half, "the vegetation's share takes loose trees away")
	Settings.optimizations.clear()
	Settings.switches_changed()
	rig.jump(back, 0.0, 60.0, NEAR_DISTANCE)
	await _settled()


func _switches(rig: CityCamera) -> void:
	for key: String in ["bGlass", "bNightWindows"]:
		Settings.switches.set_on(key, false)
	Settings.switches_changed()
	check(Settings.switches.globals[&"building_glass"] == 0.0, "the glass switch reaches the shader")
	check(Settings.switches.globals[&"building_night_windows"] == 0.0, "the windows switch reaches the shader")
	Settings.switches.reset()
	Settings.switches_changed()
	check(Settings.switches.globals[&"building_glass"] == 1.0, "glass back")
	Settings.set_layer("grid", true)
	check(Settings.switches.globals[&"map_grid"] == 1.0, "the Grid layer turns on")
	Settings.set_layer("grid", false)
	check(world.sun.shadow_caster_mask != 0 and rig != null, "a sun with a shadow mask")


func _options() -> void:
	var names: Dictionary = CityStore.constants.render_layers
	Settings.optimizations["agentShadows"] = false
	Settings.switches_changed()
	check(world.sun.shadow_caster_mask & int(names.people) == 0, "people out of the shadow by the option")
	Settings.optimizations["agentShadows"] = true
	Settings.optimizations["waterMaterial"] = true
	Settings.switches_changed()
	check(world.water.own.shader.resource_path == CityWater.PLAIN_SHADER, "flat water by the option")
	Settings.optimizations.clear()
	Settings.switches_changed()
	check(world.water.own.shader.resource_path == CityWater.WATER_SHADER, "its own water back")
	Settings.optimizations["terrainLod"] = 2
	Settings.switches_changed()
	var half_size: int = world.terrain.get("mesh_size")
	Settings.optimizations.clear()
	Settings.switches_changed()
	check(half_size * 2 <= int(world.terrain.get("mesh_size")), "the terrain's mesh by the option (%d)" % half_size)


## A run driven by tests starts on the tables, whatever the file says: the same picture on every machine and every clone.
func _player_file_read() -> void:
	var t: Dictionary = DataTables.game("settings")
	check(Settings.preset == t.tests_preset and Settings.kept.is_empty(), "a test starts on the data's preset, not the file's")
	check(t.tests_preset != "auto" and t.tiers.has(t.tests_preset), "the tests' preset is a tier, never auto")
	for gpu: String in OTHER_GPUS:
		var hardware := QualityTier.tier_for_gpu(t, gpu)
		check(QualityTier.level_of(Settings.preset, hardware) == t.tests_preset, "the tests' tier does not depend on the card: %s" % gpu)
	check(Settings.level == t.tests_preset, "the test runs on the data's tier")
	check(Settings.layers == t.layers, "a test starts on the data's layers")


## A settings file from another version, or with no sense in it, never reaches the tier table: it opens at the default.
func _presets() -> void:
	var t: Dictionary = DataTables.game("settings")
	for wrong: Variant in ["old_level", "", 7, null]:
		check(Settings.known_preset(wrong) == t.default, "an unknown preset falls to the default: %s" % [wrong])
	for p: String in t.presets:
		check(Settings.known_preset(p) == p and (p == "auto" or t.tiers.has(p)), "the table's preset exists: %s" % p)
	var before: String = Settings.preset
	Settings.set_preset("old_level")
	check(Settings.preset == t.default and not Settings.current().is_empty(), "the dialog does not save an unknown preset")
	check(Settings.level == QualityTier.tier_for_gpu(t, RenderingServer.get_video_adapter_name()), "auto asked by name follows the card")
	Settings.set_preset(before)


## The city as its own occluder: from the street, on and off, what the engine says it drew.
func _occlusion(rig: CityCamera) -> void:
	rig.jump(rig.focus, 0.0, STREET_PITCH, NEAR_DISTANCE)
	await _settled()
	var on := RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TOTAL_OBJECTS_IN_FRAME)
	Settings.switches.set_on("occlusion", false)
	Settings.switches_changed()
	await _frames(SETTLE_MIN)
	var off := RenderingServer.get_rendering_info(RenderingServer.RENDERING_INFO_TOTAL_OBJECTS_IN_FRAME)
	print("  occlusion: %d objects on, %d off" % [on, off])
	check(get_viewport().use_occlusion_culling == false, "the switch turns occlusion off")
	Settings.switches.set_on("occlusion", true)
	Settings.switches_changed()
	check(get_viewport().use_occlusion_culling == true, "the switch turns occlusion on")


## Sky fields: what the sky's owner reads on `changed`.
func _entry() -> void:
	Settings.set_preset("low")
	check(not _sky.clouds_drawn, "low draws no clouds")
	Settings.set_preset("ultra")
	check(_sky.clouds_drawn, "ultra draws the clouds")
