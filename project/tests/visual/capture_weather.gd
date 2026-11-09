## Captures of the weather port: a bolt called down in view (the weather tool's own path, SkyView.strike_at) in a storm, by day
## and by night, from a street-level and a high view, each photographed the moment before and while the stroke burns, so the
## bolt and its flash on the sky and the ground stand side by side. A bolt photo no brighter than the one before it fails.
## Run: godot --path project --resolution 1920x1080 res://tests/visual/capture_weather.tscn -- --no-flow --capture-dir=DIR
extends Node3D

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 30
const STROKE_FRAMES := 2 # frames between the strike and its photo: inside the first stroke (sky.json stroke_s 0.06)
const QUIET_FRAMES := 60 # past a strike's whole life (sky.json strike_life_s 0.45) before the next view
const SHOT_SIZE := Vector2i(1920, 1080)
const MEAN_SIDE := Vector2i(64, 36) # the picture shrunk to this to read its mean brightness
## The storm photographed: rain and its overcast, no bolt of its own (every bolt here is the one called down).
const STORM := {"rain": 0.6, "wind": 6.0, "lightning": 0.0}
## The views: what the camera looks at (x, z), its bearing, tilt and zoom (PairCamera: the 4.0's pixels a metre), and how far beyond that
## point, straight ahead, the bolt falls (metres): from the street it falls over the roofs, against the sky.
const VIEWS := [
	{"name": "rua", "at": Vector2(0.0, 0.0), "azimuth": 60.0, "tilt": 4.0, "zoom": 2.0, "ahead": 600.0},
	{"name": "cidade", "at": Vector2(80.0, 40.0), "azimuth": 210.0, "tilt": 6.0, "zoom": 3.0, "ahead": 0.0},
	{"name": "alto", "at": Vector2(0.0, 0.0), "azimuth": 30.0, "tilt": 30.0, "zoom": 1.0, "ahead": 0.0},
]

var out_dir := "user://captures"
var rig: CityCamera
var ui: UiDriver
var world: CityWorld
var failures := 0


func _ready() -> void:
	for arg in OS.get_cmdline_user_args():
		if arg.begins_with("--capture-dir="):
			out_dir = arg.trim_prefix("--capture-dir=")
	DirAccess.make_dir_recursive_absolute(out_dir)
	get_window().borderless = true
	get_window().size = SHOT_SIZE
	world = CityWorld.new()
	add_child(world)
	PairCamera.quality(world, get_viewport())
	rig = CityCamera.new()
	add_child(rig)
	var errors := CityStore.boot(world.terrain, world.materials())
	if not errors.is_empty():
		push_error("boot: %s" % errors)
		get_tree().quit(1)
		return
	CityStore.new_map(TOWN_MAP, SEED, true, true)
	ui = UiDriver.new(rig, get_tree(), out_dir)
	CityStore.dispatch(&"set_weather", [STORM])
	var day: Dictionary = DataTables.game("daylight")
	for hour: float in [day.day_hour, day.night_hour]:
		UiDriver.at_hour(world, hour)
		for v: Dictionary in VIEWS:
			await _strike_shot("%s_%02d" % [v.name, int(hour)], v)
	print("FAILURES: ", failures)
	get_tree().quit(CaptureWait.exit_code(failures))


## The view, photographed still, then a bolt ahead of its middle and the photo while it burns.
func _strike_shot(name: String, v: Dictionary) -> void:
	var at := Vector3(v.at.x, CityStore.query(&"height_at", [v.at.x, v.at.y]), v.at.y)
	await PairCamera.stand(rig, ui, at, v.azimuth, v.tilt, v.zoom) # the exact eye, low over the street as asked
	await CaptureWait.until_drawn(self, SETTLE_FRAMES, name)
	for i in QUIET_FRAMES:
		await get_tree().process_frame
	var before := await _save(name + "_antes")
	var ahead := -rig.camera.global_basis.z
	var fall := Vector2(at.x, at.z) + Vector2(ahead.x, ahead.z).normalized() * float(v.ahead)
	world.sky.strike_at(Vector3(fall.x, CityStore.query(&"height_at", [fall.x, fall.y]), fall.y))
	for i in STROKE_FRAMES:
		await get_tree().process_frame
	var bolt := await _save(name + "_raio")
	print("%s: brightness before %.4f, with the bolt %.4f" % [name, before, bolt])
	if bolt <= before:
		failures += 1
		print("FAIL %s: the bolt and its flash did not light the picture" % name)


## The picture now, saved; returns its mean brightness.
func _save(name: String) -> float:
	await RenderingServer.frame_post_draw
	var img := get_viewport().get_texture().get_image()
	img.save_png(out_dir.path_join(name + ".png"))
	img.resize(MEAN_SIDE.x, MEAN_SIDE.y, Image.INTERPOLATE_BILINEAR)
	var sum := 0.0
	for y in MEAN_SIDE.y:
		for x in MEAN_SIDE.x:
			sum += img.get_pixel(x, y).get_luminance()
	return sum / float(MEAN_SIDE.x * MEAN_SIDE.y)
