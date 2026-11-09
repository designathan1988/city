## Captures of the Build tool driven as the player drives it: the drawer's controls found by what they say (their translated texts),
## a building of the town found by what it is (one block on the ground under a flat roof) and picked by a click on its own roof as the
## camera sees it, the Selection panel's numbers set in its own fields (a SpinBox set sends value_changed, as a typed number does), the
## advanced toggle and the mirror, a setback, the roof's slope, the Facade's zone, and the free sketch of a new building; each shot
## close and from several angles, day and night.
## Run: godot --path project --resolution 1600x900 res://tests/visual/capture_construction.tscn -- --no-flow --capture-dir=<folder>
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 20
const DAY_HOUR := 16.0
const NIGHT_HOUR := 22.0
const CLOSE_ZOOM := 20.0 # pixels a metre at the building (PairCamera's zoom)
const WIDE_ZOOM := 12.0
const HIGH_TILT := 50.0
const LOW_TILT := 30.0
const FRONT_DEG := 30.0
const SIDE_DEG := 120.0
const BACK_DEG := 210.0
const STOREYS := 4
const WIDTH_M := 12.0
const DEPTH_M := 9.0
const PITCH_DEG := 35.0
const ROOF_PICK_SHARE := 0.3 # a click this share of the way from a roof's middle to its first corner: clear of the handles at the middle
const SKETCH_M := 9.0 # the side of the L drawn as a new building
const SKETCH_AWAY_M := -45.0 # how far along x from the building the L is drawn: to its west, on the open lawn at the town's edge (east is a street)
const ZONE_FROM := [0, 1] # the zone's corners: bay and storey of the first click, then of the second
const ZONE_TO := [2, 3]

var out_dir := "user://captures"
var world: CityWorld
var rig: CityCamera
var ui: UiDriver
var failures := 0
var shots := 0


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
	ui = UiDriver.new(rig, get_tree(), out_dir)
	add_child(ToolController.new(rig))
	add_child(RoadcraftHud.new())
	add_child(InteriorController.new())
	await ui.frames(SETTLE_FRAMES)
	var id := _a_plain_building()
	if id < 0:
		_fail("no building of one block under a flat roof")
	else:
		await _session(id)
	if shots == 0:
		_fail("no shot")
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


func _session(id: int) -> void:
	UiDriver.at_hour(world, DAY_HOUR)
	await _look(id, 0, FRONT_DEG, HIGH_TILT, CLOSE_ZOOM)
	await _shot("00-before")
	await _control("v2.dock.build")
	await _control("builder.category.mass")
	await _pick_roof(id, 0)
	await _shot("01-mass-picked-panel")
	await _type("builder.field.storeys", STOREYS)
	await _type("builder.field.width", WIDTH_M)
	await _type("builder.field.depth.plan", DEPTH_M)
	await _shot("02-mass-typed-4-storeys-12x9")
	await _look(id, 0, SIDE_DEG, LOW_TILT, CLOSE_ZOOM)
	await _shot("03-mass-typed-other-angle")
	await _control("builder.advanced.off")
	await _control("builder.quick.mirror")
	await _shot("04-mass-advanced-mirrored")
	await _control("builder.tool.setback")
	await _look(id, 0, FRONT_DEG, HIGH_TILT, CLOSE_ZOOM)
	var top := int((CityStore.query(&"body_info", [id]) as Dictionary).volumes.size()) - 1
	await _pick_roof(id, top)
	await _shot("05-setback-picked")
	await _control("builder.category.roof")
	await _pick_roof(id, top)
	await _control("builder.tool.roofGable")
	await _type("builder.field.pitch", PITCH_DEG)
	await _control("builder.roof.side.back")
	await _shot("06-roof-gable-pitch-panel")
	await _control("builder.tool.roofShed")
	await _control("builder.roof.side.left")
	await _look(id, 0, BACK_DEG, HIGH_TILT, CLOSE_ZOOM)
	await _shot("07-roof-shed-falls-left")
	await _control("builder.category.facade")
	await _look(id, 0, FRONT_DEG, LOW_TILT, CLOSE_ZOOM)
	await _control("creator.dock.scope.zone")
	await _control("builder.tool.balcony")
	await _pick_wall(id, ZONE_FROM[0], ZONE_FROM[1])
	await _pick_wall(id, ZONE_TO[0], ZONE_TO[1])
	await _shot("08-facade-zone-balconies")
	await _control("builder.category.draw")
	await _control("builder.tool.sketch")
	await _sketch(id)
	await _shot("09-draw-sketch-new-building")
	UiDriver.at_hour(world, NIGHT_HOUR)
	await _look(id, 0, FRONT_DEG, LOW_TILT, CLOSE_ZOOM)
	await _shot("10-night-front")
	await _look(id, 0, BACK_DEG, HIGH_TILT, CLOSE_ZOOM)
	await _shot("11-night-back")


## The first building of one block standing on the ground under a flat roof.
func _a_plain_building() -> int:
	for id: int in CityStore.query(&"list_buildings"):
		var v: Array = (CityStore.query(&"body_info", [id]) as Dictionary).get("volumes", [])
		if v.size() == 1 and v[0].base == 0 and v[0].roof == CityStore.code(&"Roof", &"ROOF_FLAT"):
			return id
	return -1


## The camera at the middle of block `vol`'s roof, turned `azimuth` degrees and tilted `tilt`, `zoom` pixels a metre.
func _look(id: int, vol: int, azimuth: float, tilt: float, zoom: float) -> void:
	await PairCamera.stand(rig, ui, _roof_middle(id, vol), azimuth, tilt, zoom)
	await ui.frames(SETTLE_FRAMES)


## The top ring of block `vol` (body_outline gives the ring at its base, then at its top).
func _roof_ring(id: int, vol: int) -> PackedVector3Array:
	var ring: PackedVector3Array = CityStore.query(&"body_outline", [id, vol])
	return ring.slice(ring.size() / 2)


func _roof_middle(id: int, vol: int) -> Vector3:
	var top := _roof_ring(id, vol)
	var sum := Vector3.ZERO
	for p in top:
		sum += p
	return sum / maxf(1.0, top.size())


## A click on block `vol`'s roof where the camera sees it.
func _pick_roof(id: int, vol: int) -> void:
	var at := rig.camera.unproject_position(_roof_middle(id, vol).lerp(_roof_ring(id, vol)[0], ROOF_PICK_SHARE))
	await _click_screen(at)


## A click on bay `bay` of storey `storey` of block 0's first wall, where the camera sees it.
func _pick_wall(id: int, bay: int, storey: int) -> void:
	var ring: PackedVector3Array = CityStore.query(&"body_outline", [id, 0])
	var n := ring.size() / 2
	var bays: int = (CityStore.query(&"body_info", [id]) as Dictionary).volumes[0].bays[0]
	var storeys: int = (CityStore.query(&"body_info", [id]) as Dictionary).volumes[0].storeys
	var p := ring[0].lerp(ring[1], (bay + 0.5) / bays) + Vector3.UP * (ring[n].y - ring[0].y) * (storey + 0.5) / storeys
	await _click_screen(rig.camera.unproject_position(p))


func _click_screen(at: Vector2) -> void:
	await ui.move(at)
	await ui.button(at, true)
	await ui.button(at, false)
	await ui.frames(SETTLE_FRAMES)


## The free sketch: an L beside the building, corner by corner on the ground, then Enter.
func _sketch(id: int) -> void:
	var o := _roof_middle(id, 0)
	var at := ui.ground(o.x + SKETCH_AWAY_M, o.z)
	var h := SKETCH_M / 2
	await PairCamera.stand(rig, ui, at, FRONT_DEG, HIGH_TILT, WIDE_ZOOM)
	await ui.frames(SETTLE_FRAMES)
	for c: Vector2 in [Vector2(-h, -h), Vector2(h, -h), Vector2(h, 0), Vector2(0, 0), Vector2(0, h), Vector2(-h, h)]:
		await ui.click(at + Vector3(c.x, 0, c.y))
	await ui.key(KEY_ENTER, false)
	await ui.frames(SETTLE_FRAMES)


## The control of the drawer that says the text of `key` (translated as the player reads it).
func _control(key: String) -> void:
	var said := tr(key)
	var found: BaseButton = null
	for n in _all(self):
		if n is BaseButton and n.is_visible_in_tree() and (n.get("text") == said or n.tooltip_text.begins_with(said)):
			found = n
	if found == null:
		_fail("no control says %s (%s)" % [said, key])
		return
	await ui.press(found)
	await ui.frames(SETTLE_FRAMES)


## A number set in the Selection panel's field named by `key` (its label).
func _type(key: String, value: float) -> void:
	var said := tr(key)
	for n in _all(self):
		if n is Label and n.text == said and n.is_visible_in_tree() and n.get_parent() is VBoxContainer:
			for c in n.get_parent().get_children():
				if c is SpinBox:
					ui.step("sets %s to %s" % [said, value])
					c.value = value
					await ui.frames(SETTLE_FRAMES)
					return
	_fail("no field %s (%s)" % [said, key])


func _all(root: Node) -> Array[Node]:
	var out: Array[Node] = []
	var todo: Array[Node] = [root]
	while not todo.is_empty():
		var n: Node = todo.pop_back()
		out.append(n)
		todo.append_array(n.get_children())
	return out


func _shot(name: String) -> void:
	await ui.drawn()
	await ui.shot(name)
	shots += 1


func _fail(why: String) -> void:
	failures += 1
	print("FAIL ", why)
