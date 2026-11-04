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
## The instants the reference game was photographed at, after the click.
const EARTH_MOMENTS := [0.4, 1.2, 4.0] # the instants of the reference photographs, its effects clock stepped exactly (look-demolition-5.json)

var rig: CityCamera
var ui: UiDriver
var failures := 0


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
	print(label, " sonda: ", CityStore.query(&"blast_probe", [float(force), 1.0]))
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


## The camera where the reference game's stood for the same photograph (the rig is off, so nothing moves it).
func _pin_camera(at: Vector3) -> void:
	rig.process_mode = Node.PROCESS_MODE_DISABLED
	rig.camera.fov = REF_FOV_DEG
	rig.camera.global_position = at + REF_CAMERA_OFFSET
	rig.camera.look_at(at)


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
