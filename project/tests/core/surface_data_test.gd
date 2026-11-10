## The mesh surfaces the core makes whole (native/src/render/surface_data.h, so every mesh is created with its materials by
## mesh_create_from_surfaces, whose pipelines the engine makes as it makes the mesh) against the engine's own: the same arrays given to
## RenderingServer.mesh_add_surface_from_arrays and read back by mesh_get_surface must hold the same bytes, flags, box, UV scale and
## levels. Packed and plain, with normals and no tangent, a tangent, colours, both UVs (inside 0..1 and past it), a custom array,
## bones and weights, and levels.
## Run: godot --path project res://tests/core/surface_data_test.tscn -- --no-flow
extends TestCase

const KEYS := ["format", "vertex_data", "attribute_data", "skin_data", "vertex_count", "index_data", "index_count", "aabb", "uv_scale"]
const SIDE := 6 # a grid of SIDE x SIDE vertices
const BONES := 4 # bones a vertex


func _ready() -> void:
	if not boot():
		finish()
		return
	var packed := RenderingServer.ARRAY_FLAG_COMPRESS_ATTRIBUTES
	var custom := RenderingServer.ARRAY_CUSTOM_RGBA8_UNORM << RenderingServer.ARRAY_FORMAT_CUSTOM0_SHIFT
	_same("packed, normals, colours, both UVs, a custom array, levels", _grid(true, false, true, false, false), packed | custom, true)
	_same("packed, UVs past 0..1 (scaled)", _grid(false, false, false, true, false), packed, false)
	_same("packed, positions only", _positions(), packed, false)
	_same("plain, a tangent, bones and weights", _grid(false, true, false, false, true), 0, false)
	_same("plain, normals and no tangent, colours, a custom array", _grid(true, false, true, false, false), custom, false)
	finish()


## The surface of `arrays` both ways, compared key by key and level by level.
func _same(what: String, arrays: Array, format: int, levels: bool) -> void:
	var lods := {}
	if levels:
		var fewer := PackedInt32Array(arrays[Mesh.ARRAY_INDEX]).slice(0, 6)
		lods[2.0] = fewer
	var mesh := RenderingServer.mesh_create()
	RenderingServer.mesh_add_surface_from_arrays(mesh, RenderingServer.PRIMITIVE_TRIANGLES, arrays, [], lods, format)
	var engine: Dictionary = RenderingServer.mesh_get_surface(mesh, 0)
	var ours: Dictionary = CityStore.query(&"surface_probe", [arrays, lods, format])
	RenderingServer.free_rid(mesh)
	for key: String in KEYS:
		check(engine.get(key) == ours.get(key), "%s: %s the engine's (%s against %s)" % [what, key, _short(ours.get(key)), _short(engine.get(key))])
	var a: Array = engine.get("lods", [])
	var b: Array = ours.get("lods", [])
	check(a.size() == b.size(), "%s: %d levels as the engine's %d" % [what, b.size(), a.size()])
	for i in mini(a.size(), b.size()):
		check(a[i].edge_length == b[i].edge_length and a[i].index_data == b[i].index_data, "%s: level %d as the engine's" % [what, i])


func _short(v: Variant) -> String:
	return ("%d bytes" % v.size()) if v is PackedByteArray else str(v)


## A wavy grid of triangles with the arrays asked for.
func _grid(normals: bool, tangent: bool, extras: bool, wide_uv: bool, skin: bool) -> Array:
	var v := PackedVector3Array()
	var n := PackedVector3Array()
	var t := PackedFloat32Array()
	var c := PackedColorArray()
	var uv := PackedVector2Array()
	var uv2 := PackedVector2Array()
	var cu := PackedByteArray()
	var bones := PackedInt32Array()
	var weights := PackedFloat32Array()
	for j in SIDE:
		for i in SIDE:
			var p := Vector3(i * 1.5, sin(i * 0.7 + j) * 0.8, j * 1.25)
			v.append(p)
			var normal := Vector3(-cos(i * 0.7 + j) * 0.3, 1.0, 0.2).normalized()
			n.append(normal)
			var along := Vector3(1, 0.1, 0).normalized()
			t.append_array(PackedFloat32Array([along.x, along.y, along.z, -1.0 if (i + j) % 2 else 1.0]))
			c.append(Color(i / float(SIDE), j / float(SIDE), 0.5, 1.0))
			uv.append(Vector2(i, j) / float(SIDE - 1) * (7.5 if wide_uv else 1.0) - (Vector2(2, 1) if wide_uv else Vector2()))
			uv2.append(Vector2(j, i) / float(SIDE - 1))
			cu.append_array(PackedByteArray([i * 9, j * 7, (i + j) * 5, 255]))
			bones.append_array(PackedInt32Array([i % BONES, (i + 1) % BONES, j % BONES, 3]))
			weights.append_array(PackedFloat32Array([0.4, 0.3, 0.2, 0.1]))
	var idx := PackedInt32Array()
	for j in SIDE - 1:
		for i in SIDE - 1:
			var a := j * SIDE + i
			idx.append_array(PackedInt32Array([a, a + SIDE, a + 1, a + 1, a + SIDE, a + SIDE + 1]))
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	arrays[Mesh.ARRAY_VERTEX] = v
	arrays[Mesh.ARRAY_INDEX] = idx
	arrays[Mesh.ARRAY_TEX_UV] = uv
	if normals or tangent or skin:
		arrays[Mesh.ARRAY_NORMAL] = n
	if tangent:
		arrays[Mesh.ARRAY_TANGENT] = t
	if extras:
		arrays[Mesh.ARRAY_COLOR] = c
		arrays[Mesh.ARRAY_TEX_UV2] = uv2
		arrays[Mesh.ARRAY_CUSTOM0] = cu
	if skin:
		arrays[Mesh.ARRAY_BONES] = bones
		arrays[Mesh.ARRAY_WEIGHTS] = weights
	return arrays


func _positions() -> Array:
	var arrays := _grid(false, false, false, false, false)
	arrays[Mesh.ARRAY_TEX_UV] = null
	return arrays
