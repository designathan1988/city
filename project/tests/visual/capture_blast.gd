## The bomb photographs of the demolition captures: the bomb at a point, photographed at the instants the effects' own clock
## reaches (the file is named by the instant it caught), from the views the reference game was photographed from.
class_name CaptureBlast
extends RefCounted

const SMALL := 5
const BIG := 45
const VIEW_YAW := 25.0
const VIEW_PITCH := 38.0
const SETTLE_FRAMES := 4
const EARTH_OUT_M := 500.0
const EARTH_NEXT_M := 200.0
## The reference game's camera for these photos, read out of its own page (`viewCamera()` after `setView` at zoom 8, azimuth 0.44,
## elevation 0.66): where it stands relative to the point looked at (metres) and its vertical field of view (degrees).
const REF_CAMERA_OFFSET := Vector3(127.5, 109.3, 60.0)
const REF_FOV_DEG := 35.0
## Straight down, for measuring: the height the camera looks from (pixels a metre are then the same everywhere on the ground).
const RELIEF_STEPS := 8
const TOP_HEIGHT_M := 120.0
const TOP_TILT_M := 0.01 # a hair off the vertical: look_at needs an up that is not the view direction
## The instants the reference game was photographed at, after the click.
const EARTH_MOMENTS := [0.4, 1.2, 4.0] # the instants of the reference photographs, its effects clock stepped exactly (look-demolition-5.json)

var rig: CityCamera
var ui: UiDriver
var failures := 0
var camera_offset := REF_CAMERA_OFFSET


func _init(camera_rig: CityCamera, driver: UiDriver) -> void:
	rig = camera_rig
	ui = driver


## A bomb on bare earth outside the city, seen from the reference game's own camera.
func earth() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var at: Vector3 = ui.ground(b.max.x + EARTH_OUT_M, (b.min.z + b.max.z) * 0.5)
	at = ui.ground(at.x + EARTH_NEXT_M, at.z) # a clearing: the first stretch past the town is forest
	await blow("terra_f%d" % SMALL, at, SMALL, 0.0, EARTH_MOMENTS)
	await ui.frames(SETTLE_FRAMES)
	at = ui.ground(at.x, at.z) # the first crater lowered the ground: a point above it would read as a roof
	await blow("terra_f%d" % BIG, at, BIG, 0.0, EARTH_MOMENTS)


## A force 45 bomb seen from straight above, 4 s after: the decal's size in pixels against the pixels a metre the log prints.
func topdown() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var at: Vector3 = ui.ground(b.max.x + EARTH_OUT_M + 2.0 * EARTH_NEXT_M, (b.min.z + b.max.z) * 0.5)
	camera_offset = Vector3(0, TOP_HEIGHT_M, TOP_TILT_M)
	await blow("topo_f%d" % BIG, at, BIG, 0.0, [4.0])
	camera_offset = REF_CAMERA_OFFSET


## One bomb: photographed before, then at each moment of the effects' clock. `dist` 0: the reference game's camera.
func blow(label: String, at: Vector3, force: int, dist: float, moments: Array) -> void:
	_check(at != Vector3.INF, "%s: ponto achado" % label)
	if at == Vector3.INF:
		return
	if dist > 0.0:
		rig.jump(at, VIEW_YAW, VIEW_PITCH, dist)
	else: # the rig stands aside
		_pin_camera(at)
	await ui.drawn()
	await ui.shot("%s_0_antes" % label)
	var probe: Dictionary = CityStore.query(&"blast_probe", [float(force), 1.0])
	print(label, " sonda: ", probe)
	_report_scale(at, probe.crater_radius_m)
	_report_relief(at, probe.decal_half_m)
	var before: Dictionary = CityStore.query(&"damage_probe")
	var since: float = CityStore.query(&"effects_since_blast")
	var r: Dictionary = CityStore.dispatch(&"strike", [at, force])
	_check(r.ok, "%s: golpe" % label)
	while true: # the clock restarts when the effects play this blow (a frame later)
		var now: float = CityStore.query(&"effects_since_blast")
		if now >= 0.0 and (since < 0.0 or now < since):
			break
		await ui.frames(1)
	for t in moments:
		# the photograph is of the effects' own clock (it never runs faster than a frame cap), named by the instant it caught
		while CityStore.query(&"effects_since_blast") < t:
			await ui.frames(1)
		await ui.shot("%s_%04dms" % [label, roundi(CityStore.query(&"effects_since_blast") * 1000.0)], 0)
	var after: Dictionary = CityStore.query(&"damage_probe")
	print(label, ": ", before, " -> ", after, " tremor ", CityStore.query(&"effects_shake"))


## The scale on screen at the point (pixels a metre along the ground's two axes, and where the crater's physical edge and the decal's
## hole edge fall), as the camera sees it: the number a crater's measured size on a photograph is read against.
func _report_scale(at: Vector3, crater_m: float) -> void:
	var cam := rig.camera
	var c := cam.unproject_position(at)
	var along_x := cam.unproject_position(at + Vector3(1, 0, 0)).distance_to(c)
	var along_z := cam.unproject_position(at + Vector3(0, 0, 1)).distance_to(c)
	print("escala: centro ", c, ", pixels por metro ", snappedf(along_x, 0.01), " in x and ", snappedf(along_z, 0.01), " in z; viewport ",
			get_viewport_size(), "; raio fisico ", snappedf(crater_m, 0.01), " m = ", snappedf(crater_m * along_x, 0.1), " px em x e ",
			snappedf(crater_m * along_z, 0.1), " px em z")


func get_viewport_size() -> Vector2:
	return rig.camera.get_viewport().get_visible_rect().size


## How much the ground rises and falls under the crater's decal (a flat quad laid at one height is hidden where the ground is higher).
func _report_relief(at: Vector3, half_m: float) -> void:
	var lo := INF
	var hi := -INF
	for j in range(-RELIEF_STEPS, RELIEF_STEPS + 1):
		for i in range(-RELIEF_STEPS, RELIEF_STEPS + 1):
			var h: float = CityStore.query(&"height_at", [at.x + half_m * i / RELIEF_STEPS, at.z + half_m * j / RELIEF_STEPS])
			lo = minf(lo, h - at.y)
			hi = maxf(hi, h - at.y)
	print("relief under the decal (half width ", snappedf(half_m, 0.1), " m): from ", snappedf(lo, 0.01), " a ", snappedf(hi, 0.01), " m round the point")


## The camera where the reference game's stood for the same photograph (the rig is off, so nothing moves it).
func _pin_camera(at: Vector3) -> void:
	rig.process_mode = Node.PROCESS_MODE_DISABLED
	rig.camera.fov = REF_FOV_DEG
	rig.camera.global_position = at + camera_offset
	rig.camera.look_at(at)


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
