## Captures of the Interior layer, driven as the player does it (the layer's switch through the interior controller, the floor
## buttons' own command): each shot names a kind of building of the town (found by what it is, never by a generated id), the floor,
## the way the camera looks relative to the lot's front and the scale on the ground; the picture is saved when the chunks the cut asked for have landed.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_interior.tscn -- --no-flow --capture-dir=<folder>
extends Node3D

const TOWN_MAP := 0
const SEED := 7
## Kind of building (KINDS), floor, turn from the lot's front (degrees), pixels a metre at the focus, tilt (degrees), hour, name, and
## "toggle" for the layer's switch (the building in the middle of the view) in place of the floor buttons' command, or a piece of
## furniture (data/catalog/interiors.json) put on the floor as the Interior tab does, and framed close (its edges and its books).
const SHOTS := [
	["house", 0, 0.0, 38.0, 75.0, 12.0, "house-ground"],
	["house", 1, 90.0, 38.0, 75.0, 12.0, "house-floor1-turn90"],
	["tower", 10, 0.0, 20.0, 65.0, 12.0, "tower-floor10"],
	["shop", 1, 0.0, 30.0, 70.0, 12.0, "commercial-floor1"],
	["house", 0, 0.0, 38.0, 75.0, 22.0, "house-night-ground"],
	["house", 0, 0.0, 8.0, 60.0, 12.0, "city-layer-house", "toggle"],
	["house", 0, 20.0, 110.0, 35.0, 12.0, "house-bookshelf", "bookshelf"],
	["house", 0, 200.0, 90.0, 45.0, 12.0, "house-sofa", "sofa"],
]
## Where a piece is tried on the floor, metres from the building's focus along x and z: the first spot the floor takes. The
## focus is the middle of the house, where its stair core stands: the spots start clear of it.
const PUT_SPOTS := [[3.5, 0], [-3.5, 0], [0, 3.5], [0, -3.5], [3.5, 3.5], [-3.5, -3.5], [3.5, -3.5], [-3.5, 3.5], [2.5, 0], [-2.5, 0]]
## What each kind is, read off the town as generated: its zone (data/catalog/zones.json, by name) and its floors.
const KINDS := {"house": ["residential", 2, 3], "tower": ["residential", 11, 1000], "shop": ["commercial", 2, 1000]}
const SETTLE_FRAMES := 60
const WAIT_LIMIT_S := 60.0
const CENTRE_PIXELS := 450.0

var out_dir := "user://captures"
var rig: CityCamera
var world: CityWorld
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	world = CityWorld.new()
	add_child(world)
	rig = CityCamera.new()
	add_child(rig)
	if not CityStore.boot(world.terrain, world.materials()).is_empty():
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	add_child(ToolController.new(rig))
	var hud := RoadcraftHud.new()
	add_child(hud)
	var layer := InteriorController.new()
	add_child(layer)
	await _frames(SETTLE_FRAMES)
	var found := _kinds()
	for s in SHOTS:
		await _shot(layer, [found.get(s[0], -1)] + s.slice(1))
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


func _shot(layer: InteriorController, s: Array) -> void:
	var info: Dictionary = CityStore.query(&"probe_building", [s[0]])
	if info.is_empty():
		failures += 1
		print("FAIL: no building ", s[0])
		return
	UiDriver.at_hour(world, s[5])
	var yaw: float = rad_to_deg(float(info.rot)) + float(s[2])
	rig.jump(info.focus, yaw, s[4], _distance_for(info.focus, yaw, s[4], s[3]))
	await _frames(SETTLE_FRAMES)
	layer.set_enabled(false)
	if s.size() > 7 and s[7] == "toggle":
		if not layer.set_enabled(true):
			failures += 1
			print("FAIL: the switch found no building in the middle of the view for ", s[6])
		rig.jump(info.focus, yaw, s[4], _distance_for(info.focus, yaw, s[4], s[3])) # the switch frames the building: this shot is the city
	elif s.size() > 7:
		layer.open(s[0], s[1])
		var at: Variant = _put(s[0], s[1], s[7], info.focus, deg_to_rad(float(s[2])))
		if at == null:
			failures += 1
			print("FAIL: no spot on the floor took a ", s[7], " for ", s[6])
			return
		rig.jump(at, yaw, s[4], _distance_for(at, yaw, s[4], s[3]))
	else:
		layer.open(s[0], s[1])
		_aim_at_floor(info.focus, yaw, s[4], s[3])
	await _settled()
	get_viewport().get_texture().get_image().save_png("%s/%s.png" % [out_dir, s[6]])
	print("shot ", s[6], " ", CityStore.query(&"interior").level, "/", CityStore.query(&"interior").floors)


## Puts a `kind` on floor `level` of building `id` at the first of PUT_SPOTS the floor takes (the Interior tab's own command),
## facing the camera; where it stands, or null.
func _put(id: int, level: int, kind: String, focus: Vector3, angle: float) -> Variant:
	var y: float = CityStore.query(&"body_floor_y", [id, level])
	for spot: Array in PUT_SPOTS:
		var at := Vector3(focus.x + spot[0], y, focus.z + spot[1])
		var r: Dictionary = CityStore.dispatch(&"body_put_piece", [id, level, kind, at, angle])
		if r.get("ok", false):
			return at
	return null


## The rig stands on the ground: the floor open high in a tower is framed by the point of the ground behind it on the line of sight.
func _aim_at_floor(at: Vector3, yaw: float, pitch: float, px: float) -> void:
	var lamps: Array = CityStore.query(&"interior").lamps
	if lamps.is_empty():
		return
	var ahead := -rig.camera.global_transform.basis.z
	ahead.y = 0.0
	var ground: Vector3 = at + ahead.normalized() * ((lamps[0].y - at.y) / tan(deg_to_rad(pitch)))
	rig.jump(ground, yaw, pitch, _distance_for(ground, yaw, pitch, px))


## The camera's distance that gives `px` pixels a metre at the focus: the lens depends on the distance, so it is searched.
func _distance_for(at: Vector3, yaw: float, pitch: float, px: float) -> float:
	var dist := CENTRE_PIXELS / px
	for i in 8:
		rig.jump(at, yaw, pitch, dist)
		dist *= rig.px_per_m() / px
	return dist


func _settled() -> void:
	var t0 := Time.get_ticks_msec()
	await _frames(SETTLE_FRAMES)
	while int(CityStore.query(&"render_pending")) > 0 and (Time.get_ticks_msec() - t0) / 1000.0 < WAIT_LIMIT_S:
		await _frames(SETTLE_FRAMES)
	await _frames(SETTLE_FRAMES)


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The first building of each kind in the town, or -1 (its shots then fail by name).
func _kinds() -> Dictionary:
	var zones: Array = DataTables.catalog_table("zones").zones.map(func(z: Dictionary) -> String: return z.id)
	var found := {}
	for id: int in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		for kind: String in KINDS:
			var want: Array = KINDS[kind]
			if not found.has(kind) and info.zone == zones.find(want[0]) and info.floors >= want[1] and info.floors <= want[2]:
				found[kind] = id
	print("buildings by kind: ", found)
	return found
