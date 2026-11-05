## Visual capture: generates a city through the store (the same path as the start screen), waits until every chunk is
## drawn and saves screenshots to user://captures (or --capture-dir=PATH). Views match the Roadcraft reference shots
## (docs/referencia-visual: street from above, neighbourhood, whole town). --map=N --seed=S choose the city.
extends Node3D

const SETTLE_FRAMES := 4
const JUMP_FRAMES := 30 # after a jump the world has not asked for the new view yet, and nothing pending does not mean all drawn
const JUMP_M := 60.0
const CHUNK_NEAR_BUILT := 1 # building_renderer.h chunk_flags
const NEAR_PER_M := 0.004
const MIN_NEAR_M := 0.3
const MAX_NEAR_M := 8.0
const TRAFFIC_WARMUP_TICKS := 2400 # run before shooting so traffic and pedestrians fill the streets (20 ticks = 1 s)
## --soak=N: instead of the warm-up, N simulated minutes, one traffic_probe report per minute (CaptureSoak).
const TRAFFIC_SAMPLE := 400
const FASTEST_SPEED := 3
const JAM_SHOTS := 1
## Close view of the vehicle nearest to the centre: camera offset and look-at offset from it.
## Offsets in the vehicle frame: x to its left, y up, z ahead.
## Busiest lit junction for pedestrians (traffic_probe busy_crossing): oblique view and a closer one.
const CROSSING_FROM := Vector3(-8, 42, 16)
const CROSSING_NEAR_FROM := Vector3(-6, 22, 10)
## Close look at the pedestrian nearest to the centre (same frame as the car view) and the height looked at.
const PERSON_VIEW_FROM := Vector3(1.5, 1.4, 4.5)
const PERSON_VIEW_AT_Y := 0.95
const DAY := CaptureViews.DAY
const NIGHT := CaptureViews.NIGHT

## Functions shot by _civic_tour (sim/buildings/building_store.h BuildingFn) and the view offset per lot size.
const CIVIC_SHOTS := {1: "escola", 2: "igreja", 3: "hospital", 4: "clinica", 5: "praca", 6: "parque", 7: "posto", 8: "mercado",
	9: "prefeitura", 10: "policia", 11: "bombeiros", 12: "shopping", 13: "quadra"}
const CIVIC_VIEW := Vector3(0.5, 0.75, 1.1)
const CIVIC_LOOK_UP_M := 4.0

var world: CityWorld
var camera: Camera3D
var pairs := ""
var out_dir := "user://captures"
var map := 0
var seed := 1
var only := PackedStringArray()
var soak := 0
var measure := false
## --wear=N: N frames of traffic at full speed before the shots, the agents drawn: the wear the streets take.
var wear_frames := 0


func _ready() -> void:
	_parse_args()
	if not _boot_world():
		get_tree().quit(1)
		return
	await _wait_render()
	if soak > 0:
		CaptureSoak.report(soak)
	else:
		CityStore.step_ticks(120 if measure else TRAFFIC_WARMUP_TICKS)
	if wear_frames > 0:
		CityStore.dispatch(&"set_speed", [FASTEST_SPEED])
		await _frames(wear_frames)
		CityStore.dispatch(&"set_speed", [0])
		var cost: Dictionary = CityStore.query(&"render_cost")
		print("desgaste: ", cost.worn_cells, " células, ", cost.wear_publications, " publicações")
	print("tráfego: ", CityStore.snapshot.get("vehicles"), " veículos, ", CityStore.snapshot.get("people"), " pessoas")
	_memory("tráfego aquecido")
	var ground := _ground(0, 0)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [TRAFFIC_SAMPLE])
	print("sonda: ", probe.vehicles, " veículos, ", probe.moving, " andando, ", probe.waiting, " parados, ", probe.on_connectors, " em conectores, ", probe.signals, " semáforos, parados por ", probe.held_by)
	if measure:
		_memory("medido")
		get_tree().quit(0)
		return
	await _capture_all(probe, ground)
	print("desenhados: ", CityStore.query(&"traffic_probe", [0]))
	print("snapshot: ", CityStore.snapshot)
	_memory("fim")
	get_tree().quit(0)


## The shot's hour: the light and the clock the top bar shows.
func set_hour(hour: float) -> void:
	world.set_hour(hour)
	CityStore.dispatch(&"set_day_hour", [hour])


func _boot_world() -> bool:
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	camera = Camera3D.new()
	camera.far = 30000.0
	camera.fov = 50.0
	add_child(camera)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		return false
	Settings.apply(world, get_viewport()) # as game/play.gd: the picture the player sees (MSAA, shadows, scale)
	print("qualidade: ", Settings.current(), " msaa_3d ", get_viewport().msaa_3d)
	add_child(RoadcraftHud.new())
	Settings.apply(world, get_viewport()) # the shots show what the player's preset shows: its antialiasing and shadows
	var report: Dictionary = CityStore.new_map(map, seed, true, true)
	print("cidade: ", report)
	_memory("cidade gerada")
	camera.global_position = Vector3(0, 300, 300)
	camera.look_at(Vector3.ZERO)
	return true


func _wait_render() -> void:
	var waited: int = await CaptureWait.until_drawn(self, 2, "cidade")
	print("quadros até desenhar tudo: ", waited, " ms/quadro ~", Performance.get_monitor(Performance.TIME_PROCESS) * 1000.0)
	_memory("cidade desenhada")


func _capture_all(probe: Dictionary, ground: Vector3) -> void:
	if only.has("transporte"):
		await CaptureTransit.shots(self)
		return
	var hour := DAY
	await CaptureSignals.at_hour(self, hour)
	if await CaptureTraffic.directed(self, probe, ground):
		return
	for v in CaptureViews.VIEWS:
		if not only.is_empty() and not only.has(v[0]):
			continue
		var h: float = v[3] if v.size() > 3 else DAY
		if h != hour: # the sky's radiance (ambient and reflections) follows a new hour over several frames
			hour = h
			await CaptureSignals.at_hour(self, h)
		var base := _ground(v[2].x, v[2].z) # each view stands on the ground under its target (hilly maps)
		await _shot(v[0], base + v[1] - Vector3(v[2].x, 0, v[2].z), base + v[2] - Vector3(v[2].x, 0, v[2].z))
		print("custo dos veículos em ", v[0], ": ", CityStore.query(&"agent_cost"))
	await CaptureVehicles.car_shots(self, probe, ground)
	await CaptureVehicles.shots(self, probe, ground)
	await _person_shot(probe, ground)
	if not only.has("none") or soak > 0:
		var cross: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
		await _shot("15_travessia", cross + CROSSING_FROM, cross)
		await _shot("16_travessia_perto", cross + CROSSING_NEAR_FROM, cross)
		await CaptureKerb.shots(self, CityStore.query(&"traffic_probe", [0]).get("crossing_kerb", []))
		await CaptureSignals.shots(self, cross, [DAY, NIGHT])
	for i in mini(JAM_SHOTS, probe.jams.size()):
		var j: Vector3 = probe.jams[i]
		print("retido em ", j, " ", probe.jam_info[i])
		await _shot("09_retido_%d" % i, j + Vector3(0, 40, 25), j)
	await _civic_tour()


func _parse_args() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
		elif arg.begins_with("--pairs="):
			pairs = arg.trim_prefix("--pairs=")
		elif arg.begins_with("--map="):
			map = int(arg.trim_prefix("--map="))
		elif arg.begins_with("--only="):
			only = arg.trim_prefix("--only=").split(",")
		elif arg.begins_with("--wear="):
			wear_frames = int(arg.trim_prefix("--wear="))
		elif arg.begins_with("--soak="):
			soak = int(arg.trim_prefix("--soak="))
		elif arg == "--measure":
			measure = true
		elif arg.begins_with("--seed="):
			seed = int(arg.trim_prefix("--seed="))


## Frames the sky's radiance map takes to follow a change (Sky PROCESS_MODE_INCREMENTAL, chosen for a sky shader with
## its own uniforms: one roughness layer per frame).
func _sky_frames() -> int:
	return int(ProjectSettings.get_setting("rendering/reflections/sky_reflections/roughness_layers")) + 1


func _shot(name: String, from: Vector3, at: Vector3) -> void:
	var settle := JUMP_FRAMES if camera.global_position.distance_to(from) > JUMP_M else SETTLE_FRAMES
	camera.global_position = from
	camera.look_at(at)
	camera.near = clampf(from.distance_to(at) * NEAR_PER_M, MIN_NEAR_M, MAX_NEAR_M)
	await CaptureWait.until_drawn(self, settle, name)
	var img := get_viewport().get_texture().get_image()
	var path := "%s/%s.png" % [out_dir, name]
	img.save_png(path)
	print("captura: ", path)


## Close look at the pedestrian nearest to `ground`, from ahead of it and a little to its side.
func _person_shot(probe: Dictionary, ground: Vector3) -> void:
	if probe.walkers.is_empty():
		return
	var w := _nearest(probe.walkers, ground)
	var at: Vector3 = probe.walkers[w]
	var dir: Vector3 = probe.walker_dirs[w]
	var side := Vector3(dir.z, 0, -dir.x)
	await _shot("14_pessoa", at + dir * PERSON_VIEW_FROM.z + side * PERSON_VIEW_FROM.x + Vector3.UP * PERSON_VIEW_FROM.y, at + Vector3.UP * PERSON_VIEW_AT_Y)


## One shot of the first record of each function in CIVIC_SHOTS (civic buildings, squares, parks), seen from the
## street side at CIVIC_VIEW (lot frame: x along the front, y up, z out over the street; scaled by the lot size).
func _civic_tour() -> void:
	if not only.is_empty() and not only.has("civico"):
		return
	var seen := {}
	for id in CityStore.query(&"list_buildings"):
		var b: Dictionary = CityStore.query(&"probe_building", [id])
		var fn: int = b.get("fn", 0)
		if not CIVIC_SHOTS.has(fn) or seen.has(fn):
			continue
		seen[fn] = true
		var size: float = maxf(b.w, b.d)
		var out := Vector3(sin(b.rot), 0, cos(b.rot)) # lot +z: toward the street
		var along := Vector3(cos(b.rot), 0, -sin(b.rot))
		var at: Vector3 = b.position
		var from := at + out * size * CIVIC_VIEW.z + along * size * CIVIC_VIEW.x + Vector3.UP * size * CIVIC_VIEW.y
		await _shot("12_%s" % CIVIC_SHOTS[fn], from, at + Vector3.UP * CIVIC_LOOK_UP_M)
		var chunk: int = CityStore.query(&"building_chunk", [at.x, at.z])
		print("chunk de 12_%s: %d" % [CIVIC_SHOTS[fn], chunk])
		if chunk & CHUNK_NEAR_BUILT == 0:
			push_warning("FOTO INCOMPLETA: 12_%s saiu sem o nível perto do prédio" % CIVIC_SHOTS[fn])


## Index of the point nearest to `to` (0 when the list is empty).
func _nearest(points: Array, to: Vector3) -> int:
	var best := 0
	var best_d := INF
	for i in points.size():
		var d: float = (points[i] as Vector3).distance_to(to)
		if d < best_d:
			best_d = d
			best = i
	return best


## Resident memory of the process (whole process, GPU buffers included under a software renderer) and the
## renderer's own counts, so a big map's budget is visible in the capture log.
func _memory(stage: String) -> void:
	CaptureMemory.report(stage)


func _ground(x: float, z: float) -> Vector3:
	var hit: Dictionary = CityStore.query(&"raycast", [Vector3(x, 2000, z), Vector3(0, -1, 0)])
	return hit.position if hit.get("hit", false) else Vector3(x, 0, z)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
