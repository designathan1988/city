## The lights of the street as the screen shows them, through the game's own Environment and "prop" and "road" materials
## (CityWorld), on each renderer (tools/build/run_tests.sh runs it on Forward+ and on Compatibility):
## - by day, each signal lens lit must show the lens colour of data/render/streets.json props.signal (render/tone_map.gd
##   mirrors Godot's ACES by hand; this checks the mirror against the engine);
## - by night, with the ambient, the moon and the glow (a screen effect added after) off so only the part's own light
##   is left, a part that glows at night
##   (PropTag::NIGHT_GLOW, lamp glass) must show glow_color x its colour x glow_gain x night, tone mapped as
##   render/tone_map.gd does. Its colour is a mid grey so that the glow stays inside ACES's range (a white one would
##   saturate and hide an error);
## - by night likewise, a grey road surface under the even light of a junction plate, seen as the road draws it past the lamps
##   with a light of their own (`lamp_lit` 0), must show its albedo x pool_color x plate_glow x pool_gain x night (road.gdshader);
##   the concrete's own mottle is +-6% of that, so this one is looser.
## The tolerance is one 8-bit step: the screen keeps 8 bits a channel, and the scene radiance (half floats, 11-bit
## mantissa) is far finer than that.
## Run: godot --path project [--rendering-method gl_compatibility] res://tests/core/light_color_test.tscn -- --no-flow
extends TestCase

const BYTE_TOP := 255.0
const GRADE_LEVELS_OFF := 1.0
const GRADE_TABLE := {0.0: [0.0, 0.0, 0.0], 46.0: [38.2, 38.2, 38.1], 127.0: [129.4, 127.9, 125.1], 255.0: [255.0, 255.0, 251.4]}
const NONE_LIT := -1.0 # INSTANCE_CUSTOM of the other lens slot (signal_lamps.cpp)
const SIDE_M := 1.0 # each part: a square this wide, facing the camera
const PITCH_M := 1.5
const VIEW_M := 9.0 # orthographic camera's height
const CAMERA_Z := 5.0
const GLOW_AT := Vector3(-1.0, -2.0, 0)
const ROAD_AT := Vector3(1.0, -2.0, 0)
const ROAD_GREY := 0.5 # sRGB
const ROAD_STEPS := 16 # the mottle
const ROAD_PLATE_GLOW := 1.0
const GLOW_GREY := 0.2 # sRGB
const FRAMES := 10 # the sky's radiance and the tone mapping settle

# The shaders' encodings (CityStore.constants.shader, tools/build/shader_contracts.json), read when the test starts: the lenses, a
# prop vertex's tag (UV2.x), the road material with no map (its albedo the strip colour) and its scale in COLOR.a, a junction
# plate's UV2 (lit evenly).
var lenses := 0
var lens_tag := 0.0
var glow_tag := 0.0
var road_concrete := 0
var road_ids := 0.0
var road_plate := Vector2.ZERO
const BYTE := 255.0
const TOLERANCE := 1 # in 8-bit steps
const LENS_STEPS := 3 # the signal green sits at the edge of the gamut: ACES has no radiance that lands on it exactly

var cam: Camera3D


func _ready() -> void:
	var shared: Dictionary = CityStore.constants.shader
	lenses = int(shared.LENS_COUNT)
	lens_tag = shared.PROP_LENS
	glow_tag = shared.PROP_NIGHT_GLOW
	road_concrete = int(shared.ROAD_MAT_CONCRETE)
	road_ids = shared.ROAD_MAT_SCALE
	road_plate = Vector2(shared.ROAD_POOL_PLATE, 0.0)
	world = CityWorld.new()
	add_child(world)
	var material: ShaderMaterial = world.materials()["prop"]
	cam = Camera3D.new()
	cam.projection = Camera3D.PROJECTION_ORTHOGONAL
	cam.size = VIEW_M
	cam.position = Vector3(0, 0, CAMERA_Z)
	add_child(cam)
	cam.make_current()
	await _lenses(material)
	await _night(material, world.materials()["road"])
	print("renderer: ", RenderingServer.get_current_rendering_method())
	_check_grade()
	finish()


func _lenses(material: ShaderMaterial) -> void:
	world.environment.glow_enabled = false # a lens's own colour: the glow of the one beside it is another thing
	var centres: Array[Vector3] = []
	var nodes: Array[Node3D] = []
	for k in lenses:
		var at := Vector3((k - (lenses - 1) * 0.5) * PITCH_M, 0, 0)
		var mm := MultiMesh.new()
		mm.transform_format = MultiMesh.TRANSFORM_3D
		mm.use_custom_data = true
		mm.mesh = _square(Vector2(lens_tag, k), Color.WHITE)
		mm.instance_count = 1
		mm.set_instance_transform(0, Transform3D(Basis(), at))
		mm.set_instance_custom_data(0, Color(k, NONE_LIT, 0, 0)) # lens k lit, as signal_lamps.h draws it
		var node := MultiMeshInstance3D.new()
		node.multimesh = mm
		node.material_override = material
		node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		add_child(node)
		nodes.push_back(node)
		centres.push_back(at)
	await _frames(FRAMES)
	var img := get_viewport().get_texture().get_image()
	var want := _lens_colours()
	for k in lenses:
		_compare(img, centres[k], want[k], "lens %d" % k, LENS_STEPS)
	for n in nodes:
		n.queue_free()


func _night(material: ShaderMaterial, road: ShaderMaterial) -> void:
	world.set_hour(DataTables.game("daylight").night_hour)
	world.environment.ambient_light_energy = 0.0
	world.sun.light_energy = 0.0
	world.environment.glow_enabled = false
	road = road.duplicate()
	road.set_shader_parameter("plate_glow", ROAD_PLATE_GLOW)
	RenderingServer.global_shader_parameter_set(&"lamp_lit", Vector2.ZERO)
	var node := MeshInstance3D.new()
	node.mesh = _square(Vector2(glow_tag, 0), Color(GLOW_GREY, GLOW_GREY, GLOW_GREY))
	node.material_override = material
	node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	node.position = GLOW_AT
	add_child(node)
	var plate := MeshInstance3D.new()
	plate.mesh = _square(road_plate, Color(ROAD_GREY, ROAD_GREY, ROAD_GREY, (road_concrete + 0.5) / road_ids))
	plate.material_override = road
	plate.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	plate.position = ROAD_AT
	add_child(plate)
	await _frames(FRAMES)
	var night: float = world.daylight.night # what daylight.gd published to the shaders
	var g: Color = Color(material.get_shader_parameter("glow_color")).srgb_to_linear()
	var c := Color(GLOW_GREY, GLOW_GREY, GLOW_GREY).srgb_to_linear()
	var k: float = float(material.get_shader_parameter("glow_gain")) * night
	var radiance := Vector3(g.r * c.r, g.g * c.g, g.b * c.b) * k
	var shown := ToneMap.aces(radiance, world.environment.tonemap_exposure, world.environment.tonemap_white)
	var want := Color(shown.x, shown.y, shown.z).linear_to_srgb()
	var img := get_viewport().get_texture().get_image()
	_compare(img, GLOW_AT, want, "night glow (night %.2f)" % night)
	var p: Color = Color(road.get_shader_parameter("pool_color")).srgb_to_linear()
	var lit: float = float(road.get_shader_parameter("plate_glow")) * float(road.get_shader_parameter("pool_gain")) * night
	var albedo: float = Color(ROAD_GREY, ROAD_GREY, ROAD_GREY).srgb_to_linear().r
	var pool := ToneMap.aces(Vector3(p.r, p.g, p.b) * lit * albedo, world.environment.tonemap_exposure, world.environment.tonemap_white)
	_compare(img, ROAD_AT, Color(pool.x, pool.y, pool.z).linear_to_srgb(), "road light (junction plate)", ROAD_STEPS)


## The grade against the 4.0's own (postprocess.ts GRADED_OUTPUT_FRAGMENT), levels of grey in and out.
func _check_grade() -> void:
	var grade: Dictionary = DataTables.game("daylight").grade
	for level: float in GRADE_TABLE:
		var got: Color = CityStore.query(&"film_graded", [grade, Color(level / BYTE_TOP, level / BYTE_TOP, level / BYTE_TOP)])
		var want: Array = GRADE_TABLE[level]
		var off := maxf(maxf(absf(got.r * BYTE_TOP - want[0]), absf(got.g * BYTE_TOP - want[1])), absf(got.b * BYTE_TOP - want[2]))
		check(off <= GRADE_LEVELS_OFF, "grade of %d: %s, the 4.0 gives %s" % [int(level), got.to_html(false), want])


func _compare(img: Image, at: Vector3, wanted: Color, what: String, steps := TOLERANCE) -> void:
	var want: Color = CityStore.query(&"film_graded", [DataTables.game("daylight").grade, wanted])
	var got := img.get_pixelv(Vector2i(cam.unproject_position(at).round()))
	var off := maxi(maxi(_step(got.r, want.r), _step(got.g, want.g)), _step(got.b, want.b))
	check(off <= steps, "%s: screen %s, wanted %s (%d steps of 8 bits)" % [what, got.to_html(false),
			want.to_html(false), off])


## A square facing the camera, every vertex in `col` with UV2 `tag` (the PropTag in x; a lens's Lens in y).
func _square(tag: Vector2, col: Color) -> ArrayMesh:
	var h := SIDE_M * 0.5
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = PackedVector3Array([Vector3(-h, -h, 0), Vector3(h, -h, 0), Vector3(h, h, 0), Vector3(-h, h, 0)])
	arrays[Mesh.ARRAY_NORMAL] = PackedVector3Array([Vector3.BACK, Vector3.BACK, Vector3.BACK, Vector3.BACK])
	arrays[Mesh.ARRAY_COLOR] = PackedColorArray([col, col, col, col])
	arrays[Mesh.ARRAY_TEX_UV2] = PackedVector2Array([tag, tag, tag, tag])
	arrays[Mesh.ARRAY_INDEX] = PackedInt32Array([0, 2, 1, 0, 3, 2]) # clockwise seen from the camera: Godot's front faces
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	return mesh


## Each lens's colour (sRGB), from its parts in streets.json props.signal.
func _lens_colours() -> Array[Color]:
	var out: Array[Color] = []
	out.resize(lenses)
	for part: Dictionary in DataTables.render_table("streets").props.signal:
		if part.has("lens"):
			out[int(part.lens)] = Color.html(part.color)
	return out


func _step(a: float, b: float) -> int:
	return absi(roundi(a * BYTE) - roundi(b * BYTE))


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
