## Visual capture: generates a city through the store (the same path as the start screen), waits until every chunk is
## drawn and saves screenshots to user://captures (or --capture-dir=PATH). Views match the Roadcraft reference shots
## (docs/referencia-visual: street from above, neighbourhood, whole town). --map=N --seed=S choose the city.
extends Node3D

const SETTLE_FRAMES := 4
const BUILD_FRAMES := 400
const NEAR_PER_M := 0.004
const MIN_NEAR_M := 0.3
const MAX_NEAR_M := 8.0
## Simulated ticks run before shooting so traffic and pedestrians fill the streets (20 ticks = 1 s).
const TRAFFIC_WARMUP_TICKS := 2400
## --soak=N: instead of the warm-up, N simulated minutes, one traffic_probe report per minute (CaptureSoak).
const TRAFFIC_SAMPLE := 400
const JAM_SHOTS := 1
## Close view of the vehicle nearest to the centre: camera offset and look-at offset from it.
## Offsets in the vehicle frame: x to its left, y up, z ahead.
const CAR_VIEW_FROM := Vector3(3, 2.5, 14)
const CAR_HIGH_FROM := Vector3(0, 45, 50)
const CAR_VIEW_AT := Vector3(0, 0.5, 0)
## Busiest lit junction for pedestrians (traffic_probe busy_crossing): oblique view and a closer one.
const CROSSING_FROM := Vector3(-8, 42, 16)
const CROSSING_NEAR_FROM := Vector3(-6, 22, 10)
## Close look at the pedestrian nearest to the centre (same frame as the car view) and the height looked at.
const PERSON_VIEW_FROM := Vector3(1.5, 1.4, 4.5)
const PERSON_VIEW_AT_Y := 0.95
## Hours the light is pinned to (Roadcraft "always day" / "always night"); a view may name its own.
const DAY := 16.0
const NIGHT := 22.0
## Views: name, camera offset from the city centre, look-at offset (metres); 17-18: superchunk far switch.
const VIEWS := [
	["01_rua", Vector3(-58, 62, 40), Vector3(-58, 0, -70)],
	["02_bairro", Vector3(-260, 260, 260), Vector3(40, 0, -40)],
	["03_cidade", Vector3(-900, 700, 900), Vector3(0, 0, 0)],
	["04_rua_baixa", Vector3(-62, 14, 30), Vector3(-62, 6, -60)],
	["05_transito", Vector3(-60, 6, 18), Vector3(-62, 1, -30)],
	["06_cruzamento", Vector3(-20, 30, 40), Vector3(-60, 0, -20)],
	["13_calcada", Vector3(-53, 2.2, 6), Vector3(-55, 1.0, -22)],
	["10_noite", Vector3(-58, 62, 40), Vector3(-58, 0, -70), NIGHT],
	["11_relevo", Vector3(1750, 150, 1950), Vector3(2050, 0, 1650), DAY],
	["12_encosta", Vector3(3750, 70, 300), Vector3(4400, 0, 80), DAY],
	["17_horizonte", Vector3(-1500, 140, 1500), Vector3(600, 0, -600), DAY],
	["18_alto", Vector3(0, 2600, 2600), Vector3(0, 0, 0), DAY],
]

## Functions shot by _civic_tour (sim/buildings/building_store.h BuildingFn) and the view offset per lot size.
const CIVIC_SHOTS := {1: "escola", 2: "igreja", 3: "hospital", 4: "clinica", 5: "praca", 6: "parque", 7: "posto", 8: "mercado",
	9: "prefeitura", 10: "policia", 11: "bombeiros", 12: "shopping", 13: "quadra"}
const CIVIC_VIEW := Vector3(0.5, 0.75, 1.1)
const CIVIC_LOOK_UP_M := 4.0

var world: CityWorld
var camera: Camera3D
var out_dir := "user://captures"
var map := 0
var seed := 1
var only := PackedStringArray()
var soak := 0
var measure := false


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
	add_child(RoadcraftHud.new())
	var report: Dictionary = CityStore.new_map(map, seed, true)
	print("cidade: ", report)
	_memory("cidade gerada")
	CityStore.dispatch(&"set_speed", [0])
	camera.global_position = Vector3(0, 300, 300)
	camera.look_at(Vector3.ZERO)
	return true


func _wait_render() -> void:
	var waited := 0
	await _frames(2)
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await _frames(1)
		waited += 1
	print("quadros até desenhar tudo: ", waited, " ms/quadro ~", Performance.get_monitor(Performance.TIME_PROCESS) * 1000.0)
	_memory("cidade desenhada")


func _capture_all(probe: Dictionary, ground: Vector3) -> void:
	if only.has("transporte"):
		await CaptureTransit.shots(self)
		return
	var hour := DAY
	for v in VIEWS:
		if not only.is_empty() and not only.has(v[0]):
			continue
		var h: float = v[3] if v.size() > 3 else DAY
		world.set_hour(h)
		if h != hour: # the sky's radiance (ambient and reflections) follows a new hour over several frames
			hour = h
			await _frames(_sky_frames())
		var base := _ground(v[2].x, v[2].z) # each view stands on the ground under its target (hilly maps)
		await _shot(v[0], base + v[1] - Vector3(v[2].x, 0, v[2].z), base + v[2] - Vector3(v[2].x, 0, v[2].z))
		print("custo dos veículos em ", v[0], ": ", CityStore.query(&"agent_cost"))
	var k := _nearest(probe.sample, ground)
	var car: Vector3 = probe.sample[k]
	var ahead: Vector3 = probe.sample_dir[k]
	var left := Vector3(ahead.z, 0, -ahead.x)
	await _shot("07_carro", car + ahead * CAR_VIEW_FROM.z + left * CAR_VIEW_FROM.x + Vector3.UP * CAR_VIEW_FROM.y, car + CAR_VIEW_AT)
	await _shot("08_carro_alto", car + ahead * CAR_HIGH_FROM.z + Vector3.UP * CAR_HIGH_FROM.y, car)
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
		elif arg.begins_with("--map="):
			map = int(arg.trim_prefix("--map="))
		elif arg.begins_with("--only="):
			only = arg.trim_prefix("--only=").split(",")
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
	camera.global_position = from
	camera.look_at(at)
	camera.near = clampf(from.distance_to(at) * NEAR_PER_M, MIN_NEAR_M, MAX_NEAR_M)
	await _frames(SETTLE_FRAMES)
	var waited := 0
	while CityStore.query(&"render_pending") > 0 and waited < BUILD_FRAMES:
		await _frames(1)
		waited += 1
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
