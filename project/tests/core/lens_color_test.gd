## Shows each signal lens lit through the game's own Environment and "prop" material (CityWorld) and reads the screen:
## the colour there must be the lens colour of data/render/streets.json props.signal. render/tone_map.gd mirrors
## Godot's ACES by hand; this checks the mirror against the engine, on each renderer (tools/build/run_tests.sh runs it on
## Forward+ and on Compatibility). The tolerance is one 8-bit step: the screen keeps 8 bits a channel, and the scene
## radiance (half floats, 11-bit mantissa) is far finer than that; measured, both renderers stay within it (at most one
## step off, on one channel of the amber or the green).
## Run: godot --path project [--rendering-method gl_compatibility] res://tests/core/lens_color_test.tscn -- --no-flow
extends Node3D

const LENSES := 5 # prop_kit.h Lens::COUNT
const LENS_TAG := 3.0 # prop_kit.h PropTag::LENS (UV2.x)
const NONE_LIT := -1.0 # INSTANCE_CUSTOM of the other lens slot (signal_lamps.cpp)
const SIDE_M := 1.0 # each lens: a square this wide, facing the camera
const PITCH_M := 1.5
const VIEW_M := 9.0 # orthographic camera's height
const CAMERA_Z := 5.0
const FRAMES := 10 # the sky's radiance and the tone mapping settle
const BYTE := 255.0
const TOLERANCE := 1 # in 8-bit steps

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var material: ShaderMaterial = world.materials()["prop"]
	var cam := Camera3D.new()
	cam.projection = Camera3D.PROJECTION_ORTHOGONAL
	cam.size = VIEW_M
	cam.position = Vector3(0, 0, CAMERA_Z)
	add_child(cam)
	cam.make_current()
	var centres: Array[Vector3] = []
	for k in LENSES:
		var at := Vector3((k - (LENSES - 1) * 0.5) * PITCH_M, 0, 0)
		add_child(_lens(k, at, material))
		centres.push_back(at)
	await _frames(FRAMES)
	var img := get_viewport().get_texture().get_image()
	var want := _lens_colours()
	for k in LENSES:
		var px := Vector2i(cam.unproject_position(centres[k]).round())
		var got := img.get_pixelv(px)
		var off := maxi(maxi(_step(got.r, want[k].r), _step(got.g, want[k].g)), _step(got.b, want[k].b))
		_check(off <= TOLERANCE, "lente %d: tela %s, desejada %s (%d passos de 8 bits)" % [k, got.to_html(false),
				want[k].to_html(false), off])
	print("renderizador: ", RenderingServer.get_current_rendering_method(), " FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## One lens lit, as signal_lamps.h draws it: a MultiMesh instance whose custom data names lens `k` lit.
func _lens(k: int, at: Vector3, material: ShaderMaterial) -> MultiMeshInstance3D:
	var h := SIDE_M * 0.5
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = PackedVector3Array([Vector3(-h, -h, 0), Vector3(h, -h, 0), Vector3(h, h, 0), Vector3(-h, h, 0)])
	arrays[Mesh.ARRAY_NORMAL] = PackedVector3Array([Vector3.BACK, Vector3.BACK, Vector3.BACK, Vector3.BACK])
	arrays[Mesh.ARRAY_COLOR] = PackedColorArray([Color.WHITE, Color.WHITE, Color.WHITE, Color.WHITE])
	var tag := Vector2(LENS_TAG, k)
	arrays[Mesh.ARRAY_TEX_UV2] = PackedVector2Array([tag, tag, tag, tag])
	arrays[Mesh.ARRAY_INDEX] = PackedInt32Array([0, 2, 1, 0, 3, 2]) # clockwise seen from the camera: Godot's front faces
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
	var mm := MultiMesh.new()
	mm.transform_format = MultiMesh.TRANSFORM_3D
	mm.use_custom_data = true
	mm.mesh = mesh
	mm.instance_count = 1
	mm.set_instance_transform(0, Transform3D(Basis(), at))
	mm.set_instance_custom_data(0, Color(k, NONE_LIT, 0, 0))
	var node := MultiMeshInstance3D.new()
	node.multimesh = mm
	node.material_override = material
	node.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
	return node


## Each lens's colour (sRGB), from its parts in streets.json props.signal.
func _lens_colours() -> Array[Color]:
	var out: Array[Color] = []
	out.resize(LENSES)
	for part: Dictionary in DataTables.render().streets.props.signal:
		if part.has("lens"):
			out[int(part.lens)] = Color.html(part.color)
	return out


func _step(a: float, b: float) -> int:
	return absi(roundi(a * BYTE) - roundi(b * BYTE))


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
