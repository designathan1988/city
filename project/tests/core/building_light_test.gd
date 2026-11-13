## The buildings as the screen shows them, through the game's own Environment and its "building" and "building_far"
## materials (CityWorld), on each renderer (tools/tasks/suites.json runs it on Forward+ and on Compatibility (its
## `@forward_plus` sibling)):
## - by night, with the ambient and the moon off so only a surface's own light is left, a lamp face of the near level
##   (shell.h SK_LAMP) must show its colour x night x lamp_glow, and a lamp face of the far level its colour x
##   wall_gain x night, tone mapped as render/tone_map.gd does (its colour a mid grey: inside ACES's range);
## - by day, a plain wall face of the near level and of the far level, and an instanced part with no finish layer, must
##   each show what the engine's own material shows for the same colour x wall_gain under the same light (a
##   StandardMaterial3D below it, vertex colour as sRGB albedo, the same roughness): each albedo path against the engine's.
## Colours are sRGB as the meshes carry them (shell.cpp); their linear value is the sRGB decoding (IEC 61966-2-1).
## The tolerance is one 8-bit step, as in light_color_test.gd.
## Run: godot --path project [--rendering-method gl_compatibility] res://tests/core/building_light_test.tscn -- --no-flow
extends TestCase

const LAMP_KIND := 4 # shell.h SK_LAMP (COLOR.a), the near level; the far level reads kind 4 the same (building_far)
const SHELL_KIND := 0 # shell.h SK_SHELL, no finish layer (UV2.x = 0)
const WALL_ROUGHNESS := 0.86 # building.gdshader and building_parts.gdshader, a face with no finish
const FAR_ROUGHNESS := 0.9 # building_far.gdshader
const NO_FINISH := -1.0 # BoxInstance::fin of a part with no finish layer (INSTANCE_CUSTOM.a)
const SIDE_M := 1.0
const PITCH_M := 1.5
const VIEW_M := 6.0
const CAMERA_Z := 5.0
const GREY := 0.5 # sRGB
const FRAMES := 10
const BYTE := 255.0
const TOLERANCE := 1
const LAMP_GLOW := 0.5

var cam: Camera3D


func _ready() -> void:
	world = CityWorld.new()
	add_child(world)
	cam = Camera3D.new()
	cam.projection = Camera3D.PROJECTION_ORTHOGONAL
	cam.size = VIEW_M
	cam.position = Vector3(0, 0, CAMERA_Z)
	add_child(cam)
	cam.make_current()
	await _wall_by_day()
	await _lamps_by_night()
	print("renderer: ", RenderingServer.get_current_rendering_method())
	finish()


func _wall_by_day() -> void:
	world.environment.fog_enabled = false # the surface under the same light: the air over it is another thing
	var cases := [["near wall", "building", WALL_ROUGHNESS, false], ["far wall", "building_far", FAR_ROUGHNESS, false],
			["a piece with no finish", "building_parts", WALL_ROUGHNESS, true]]
	var made: Array[Node3D] = []
	var spots := []
	for k in cases.size():
		var m: ShaderMaterial = world.materials()[cases[k][1]]
		var x := (k - (cases.size() - 1) * 0.5) * PITCH_M
		var ours: Node3D = _part(m, Vector3(x, PITCH_M * 0.5, 0)) if cases[k][3] else _quad(Color(GREY, GREY, GREY, SHELL_KIND / BYTE), m, Vector3(x, PITCH_M * 0.5, 0))
		var ref := StandardMaterial3D.new()
		ref.vertex_color_use_as_albedo = true
		ref.vertex_color_is_srgb = true
		ref.roughness = cases[k][2]
		var lin := _decode(GREY) * _param(m, "wall_gain")
		var srgb := Color(lin, lin, lin).linear_to_srgb()
		var theirs := _quad(Color(srgb.r, srgb.g, srgb.b), ref, Vector3(x, -PITCH_M * 0.5, 0))
		made.append_array([ours, theirs])
		spots.push_back([ours.position, theirs.position, cases[k][0]])
	await _frames(FRAMES)
	var img := get_viewport().get_texture().get_image()
	for sp in spots:
		var want := _pixel(img, sp[1])
		_compare(img, sp[0], want, "%s by day (reference %s)" % [sp[2], want.to_html(false)], true)
	for n in made:
		n.queue_free()


## One instanced part (building_parts): a square in a MultiMesh, its tone and finish in INSTANCE_CUSTOM as shell.h writes them.
func _part(m: ShaderMaterial, at: Vector3) -> MultiMeshInstance3D:
	var mm := MultiMesh.new()
	mm.transform_format = MultiMesh.TRANSFORM_3D
	mm.use_custom_data = true
	mm.mesh = _square(Color.WHITE)
	mm.instance_count = 1
	mm.set_instance_transform(0, Transform3D())
	mm.set_instance_custom_data(0, Color(GREY, GREY, GREY, NO_FINISH))
	var node := MultiMeshInstance3D.new()
	node.multimesh = mm
	node.material_override = m
	node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	node.position = at
	add_child(node)
	return node


func _lamps_by_night() -> void:
	world.set_hour(DataTables.game("daylight").night_hour)
	world.environment.ambient_light_energy = 0.0
	world.environment.glow_enabled = false # a lamp's own light: what it blooms over is another thing
	world.materials()["building"].set_shader_parameter("lamp_glow", LAMP_GLOW) # under 1 after the exposure: without the glow, Compatibility's picture before the tone mapping stops there
	world.sun.light_energy = 0.0
	var near: ShaderMaterial = world.materials()["building"]
	var far: ShaderMaterial = world.materials()["building_far"]
	var a := _quad(Color(GREY, GREY, GREY, LAMP_KIND / BYTE), near, Vector3(-PITCH_M * 0.5, 0, 0))
	var b := _quad(Color(GREY, GREY, GREY, LAMP_KIND / BYTE), far, Vector3(PITCH_M * 0.5, 0, 0))
	await _frames(FRAMES)
	var night: float = world.daylight.night
	var img := get_viewport().get_texture().get_image()
	var lit: Color = Color(near.get_shader_parameter("lamp_light")).srgb_to_linear() # what a lit lamp gives, whatever its body is
	_compare(img, a.position, _shown_rgb(Vector3(lit.r, lit.g, lit.b) * night * _param(near, "lamp_glow")), "lamp near (night %.2f)" % night)
	_compare(img, b.position, _shown(_decode(GREY) * _param(far, "wall_gain") * night), "lamp far (night %.2f)" % night)


## The colour a linear grey radiance shows on screen, through the Environment's tone mapping (render/tone_map.gd).
func _shown(radiance: float) -> Color:
	return _shown_rgb(Vector3(radiance, radiance, radiance))


func _shown_rgb(radiance: Vector3) -> Color:
	var s := ToneMap.aces(radiance, world.environment.tonemap_exposure, world.environment.tonemap_white)
	return Color(s.x, s.y, s.z).linear_to_srgb()


func _decode(c: float) -> float:
	return Color(c, c, c).srgb_to_linear().r


## A uniform as the material runs with it: its value when set, else the shader's default.
func _param(m: ShaderMaterial, name: String) -> float:
	var v: Variant = m.get_shader_parameter(name)
	if v == null:
		v = RenderingServer.shader_get_parameter_default(m.shader.get_rid(), name)
	return float(v)


## A square facing the camera at `at`, every vertex in `col` (sRGB rgb, kind in alpha), drawn with `m`.
func _quad(col: Color, m: Material, at: Vector3) -> MeshInstance3D:
	var node := MeshInstance3D.new()
	node.mesh = _square(col)
	node.material_override = m
	node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	node.position = at
	add_child(node)
	return node


func _square(col: Color) -> ArrayMesh:
	var h := SIDE_M * 0.5
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = PackedVector3Array([Vector3(-h, -h, 0), Vector3(h, -h, 0), Vector3(h, h, 0), Vector3(-h, h, 0)])
	arrays[Mesh.ARRAY_NORMAL] = PackedVector3Array([Vector3.BACK, Vector3.BACK, Vector3.BACK, Vector3.BACK])
	arrays[Mesh.ARRAY_COLOR] = PackedColorArray([col, col, col, col])
	arrays[Mesh.ARRAY_TEX_UV] = PackedVector2Array([Vector2.ZERO, Vector2.ZERO, Vector2.ZERO, Vector2.ZERO])
	arrays[Mesh.ARRAY_TEX_UV2] = PackedVector2Array([Vector2.ZERO, Vector2.ZERO, Vector2.ZERO, Vector2.ZERO])
	arrays[Mesh.ARRAY_INDEX] = PackedInt32Array([0, 2, 1, 0, 3, 2]) # clockwise seen from the camera: Godot's front faces
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	return mesh


func _pixel(img: Image, at: Vector3) -> Color:
	return img.get_pixelv(Vector2i(cam.unproject_position(at).round()))


func _compare(img: Image, at: Vector3, wanted: Color, what: String, on_screen := false) -> void:
	var want: Color = wanted if on_screen else CityStore.query(&"film_graded", [DataTables.game("daylight").grade, wanted])
	var got := _pixel(img, at)
	var off := maxi(maxi(_step(got.r, want.r), _step(got.g, want.g)), _step(got.b, want.b))
	check(off <= TOLERANCE, "%s: screen %s, wanted %s (%d steps of 8 bits)" % [what, got.to_html(false), want.to_html(false), off])


func _step(a: float, b: float) -> int:
	return absi(roundi(a * BYTE) - roundi(b * BYTE))


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
