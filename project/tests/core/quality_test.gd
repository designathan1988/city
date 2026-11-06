## The graphics tiers on what is really drawn: each field read back from the renderer (instances per level, the camera's mask, the
## viewport), the Effects panel's new switches and the Optimisations tab's options; the entry the sky's owner reads.
extends Node3D

const CAR_FRAMES := 1800
const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 240
const NEAR_DISTANCE := 90.0
const SETTLE_MIN := 30
const WILD_PAST_M := 500.0
const STREET_PITCH := 8.0
const PERSON_DISTANCE := 18.0
const PERSON_FRAMES := 8
const SETTLE_LIMIT := 1500
const FAR_DISTANCE := 5000.0
const OTHER_GPUS := ["NVIDIA GeForce RTX 4090", "NVIDIA GeForce RTX 3060", "AMD Radeon RX 6600", "Intel(R) UHD Graphics 630", "Unknown"]

var failures := 0


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	failures += 0 if ok else 1


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _ready() -> void:
	_player_file_read()
	var kept := FileAccess.get_file_as_bytes(Settings.PATH)
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, true)
	Settings.attach(world, get_viewport())
	await _frames(SETTLE_FRAMES)
	await _tiers(rig)
	await _occlusion(rig)
	await _switches(world, rig)
	_options(world)
	_entry()
	_presets()
	_check(FileAccess.get_file_as_bytes(Settings.PATH) == kept, "um teste não muda o settings.cfg do jogador")
	print("quality_test: %d falhas" % failures)
	get_tree().quit(1 if failures > 0 else 0)


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
	var left := SETTLE_LIMIT
	while left > 0 and CityStore.query(&"render_pending") > 0:
		left -= 1
		await get_tree().process_frame
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
		print("  %s: alcance da cabine %.0f m, %s" % [level, seen[level], _cost()])
	_check(seen.low < seen.ultra, "o nível baixo tem cabine mais perto que o ultra")
	Settings.set_preset("ultra")
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)
	await _frames(2)
	var names: Dictionary = CityStore.constants.render_layers
	_check(rig.camera.cull_mask & int(names.people) != 0, "de perto as pessoas estão na máscara")
	await _counts(rig)
	rig.jump(rig.focus, 0.0, 60.0, FAR_DISTANCE)
	await _frames(2)
	_check(rig.camera.cull_mask & int(names.people) == 0, "passou do corte de detalhe: pessoas fora")
	_check(rig.camera.cull_mask & int(names.vehicles) == 0, "passou do corte de detalhe: veículos fora")
	_check(Settings.switches.globals[&"road_paint"] == 0.0, "passou do corte: pintura fora")
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)
	await _frames(2)
	_check(Settings.switches.globals[&"road_paint"] == 1.0, "voltou: pintura de novo")


## Instances per level, close to the street: what the tiers change is what is drawn.
func _counts(rig: CityCamera) -> void:
	await _settled()
	var c := _cost()
	for f in CAR_FRAMES: # the city settles before a car need have come by: what is asked is that a car near is drawn with its cabin
		if int(c.cabins) > 0:
			break
		await get_tree().process_frame
		c = _cost()
	var cabins: int = c.cabins
	var people := await _people_close(rig)
	print("  ultra de perto: %d cabines, %d pessoas no nível perto, %d árvores perto" % [cabins, people, c.trees_near])
	_check(cabins > 0, "ultra de perto desenha cabines")
	_check(people > 0, "ultra de perto desenha pessoas no nível perto")
	rig.jump(rig.focus, 0.0, 60.0, NEAR_DISTANCE)
	_check(c.trees_near > 0, "ultra de perto desenha árvores")
	Settings.optimizations["vehicleWhole"] = 1
	Settings.optimizations["pedestrianDetail"] = 0
	Settings.switches_changed()
	await _settled()
	c = _cost()
	people = await _people_close(rig)
	print("  com as opções: %d cabines, %d pessoas perto, %d árvores perto" % [c.cabins, people, c.trees_near])
	_check(c.cabins <= 2, "teto de veículos com cabine (um quadro atrás, na conta do renderizador)")
	_check(people == 0, "detalhe 0: pessoas só no nível distante")
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
	print("  mata fora da cidade: %d árvores, %d com metade, %d sem" % [all, half, none])
	_check(all > 0 and half < all and none < half, "a parte da vegetação tira árvores soltas")
	Settings.optimizations.clear()
	Settings.switches_changed()
	rig.jump(back, 0.0, 60.0, NEAR_DISTANCE)
	await _settled()


func _switches(world: CityWorld, rig: CityCamera) -> void:
	for key: String in ["bGlass", "bNightWindows"]:
		Settings.switches.set_on(key, false)
	Settings.switches_changed()
	_check(Settings.switches.globals[&"building_glass"] == 0.0, "chave do vidro chega ao shader")
	_check(Settings.switches.globals[&"building_night_windows"] == 0.0, "chave das janelas chega ao shader")
	Settings.switches.reset()
	Settings.switches_changed()
	_check(Settings.switches.globals[&"building_glass"] == 1.0, "vidro de volta")
	Settings.set_layer("grid", true)
	_check(Settings.switches.globals[&"map_grid"] == 1.0, "camada Grade liga")
	Settings.set_layer("grid", false)
	_check(world.sun.shadow_caster_mask != 0 and rig != null, "sol com máscara de sombra")


func _options(world: CityWorld) -> void:
	var names: Dictionary = CityStore.constants.render_layers
	Settings.optimizations["agentShadows"] = false
	Settings.switches_changed()
	_check(world.sun.shadow_caster_mask & int(names.people) == 0, "pessoas fora da sombra pela opção")
	Settings.optimizations["agentShadows"] = true
	Settings.optimizations["waterMaterial"] = true
	Settings.switches_changed()
	_check(world.water.own.shader.resource_path == CityWater.PLAIN_SHADER, "água plana pela opção")
	Settings.optimizations.clear()
	Settings.switches_changed()
	_check(world.water.own.shader.resource_path == CityWater.WATER_SHADER, "água própria de volta")
	Settings.optimizations["terrainLod"] = 2
	Settings.switches_changed()
	var half_size: int = world.terrain.get("mesh_size")
	Settings.optimizations.clear()
	Settings.switches_changed()
	_check(half_size * 2 <= int(world.terrain.get("mesh_size")), "malha do terreno pela opção (%d)" % half_size)


## A run driven by tests starts on the tables, whatever the file says: the same picture on every machine and every clone.
func _player_file_read() -> void:
	var t: Dictionary = DataTables.game("settings")
	_check(Settings.preset == t.tests_preset and Settings.kept.is_empty(), "teste começa no preset dos dados, não no do arquivo")
	_check(t.tests_preset != "auto" and t.tiers.has(t.tests_preset), "o preset dos testes é um nível, nunca auto")
	for gpu: String in OTHER_GPUS:
		var hardware := QualityTier.tier_for_gpu(t, gpu)
		_check(QualityTier.level_of(Settings.preset, hardware) == t.tests_preset, "o nível dos testes não depende da placa: %s" % gpu)
	_check(Settings.level == t.tests_preset, "o teste roda no nível dos dados")
	_check(Settings.layers == t.layers, "teste começa nas camadas dos dados")


## A settings file from another version, or with no sense in it, never reaches the tier table: it opens at the default.
func _presets() -> void:
	var t: Dictionary = DataTables.game("settings")
	for wrong: Variant in ["nivel_antigo", "", 7, null]:
		_check(Settings.known_preset(wrong) == t.default, "preset desconhecido cai no padrão: %s" % [wrong])
	for p: String in t.presets:
		_check(Settings.known_preset(p) == p and (p == "auto" or t.tiers.has(p)), "preset da tabela existe: %s" % p)
	var before: String = Settings.preset
	Settings.set_preset("nivel_antigo")
	_check(Settings.preset == t.default and not Settings.current().is_empty(), "o diálogo não grava preset desconhecido")
	_check(Settings.level == QualityTier.tier_for_gpu(t, RenderingServer.get_video_adapter_name()), "auto pedido por nome segue a placa")
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
	print("  oclusão: %d objetos ligada, %d desligada" % [on, off])
	_check(get_viewport().use_occlusion_culling == false, "chave desliga a oclusão")
	Settings.switches.set_on("occlusion", true)
	Settings.switches_changed()
	_check(get_viewport().use_occlusion_culling == true, "chave liga a oclusão")


## Sky fields: what the sky's owner reads on `changed`.
func _entry() -> void:
	Settings.set_preset("low")
	_check(Settings.current().sky_clouds == false, "baixa sem nuvens para quem desenha o céu")
	Settings.set_preset("ultra")
	_check(Settings.current().cloud_shadows == true, "ultra com sombra de nuvem para quem desenha o céu")
