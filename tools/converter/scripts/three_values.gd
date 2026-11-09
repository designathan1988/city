# What three.js handed a draw, as Godot takes it: a matrix of 16 numbers (column-major) as a Transform3D, a uniform's
# captured value (its GLSL type, its elements, whether an array) as the Variant set_shader_parameter takes, the draw's
# material (its translated shader and uniforms), and the stage it is drawn on (linear tone mapping, no ambient light, a
# current camera: the values the program wrote). Shared by scripts/parity.gd and scripts/replay.gd; copied beside them
# into each generated project (parity.mjs, frame-replay.mjs).
extends RefCounted


## A draw's material: the translated shader at `path` and every uniform three set (Godot's name, its captured value).
static func material(path: String, params: Array) -> ShaderMaterial:
	var shader := Shader.new()
	shader.code = FileAccess.get_file_as_string(path)
	var mat := ShaderMaterial.new()
	mat.shader = shader
	for p in params:
		mat.set_shader_parameter(p.godot, value(p))
	return mat


## Three's instances of `mesh` as one MultiMesh: each its matrix, its colour (alpha 1 when three gave three channels) and
## its custom data when it has some; never culled (three's matrices, not Godot's camera, place what it draws).
static func instances(mesh: ArrayMesh, items: Array) -> MultiMeshInstance3D:
	var mm := MultiMesh.new()
	mm.transform_format = MultiMesh.TRANSFORM_3D
	mm.use_colors = true
	mm.use_custom_data = true
	mm.mesh = mesh
	mm.instance_count = items.size()
	for i in items.size():
		mm.set_instance_transform(i, matrix(items[i].matrix))
		var col: Array = items[i].color
		mm.set_instance_color(i, Color(col[0], col[1], col[2], col[3] if col.size() > 3 else 1.0))
		if items[i].has("custom"):
			var cd: Array = items[i].custom
			mm.set_instance_custom_data(i, Color(cd[0], cd[1], cd[2], cd[3]))
	mm.custom_aabb = AABB(Vector3(-1e6, -1e6, -1e6), Vector3(2e6, 2e6, 2e6))
	var mmi := MultiMeshInstance3D.new()
	mmi.multimesh = mm
	return mmi


## The stage a draw is drawn on inside `vp`: linear tone mapping and no ambient light on `background` (Environment's own
## black for BG_COLOR), and a camera made current, returned for a caller that places it as three's camera stood.
static func stage(vp: SubViewport, background: Environment.BGMode) -> Camera3D:
	var env := Environment.new()
	env.background_mode = background
	env.tonemap_mode = Environment.TONE_MAPPER_LINEAR
	env.ambient_light_source = Environment.AMBIENT_SOURCE_DISABLED
	var we := WorldEnvironment.new()
	we.environment = env
	vp.add_child(we)
	var cam := Camera3D.new()
	vp.add_child(cam)
	cam.make_current()
	return cam


static func matrix(m: Array) -> Transform3D:
	return Transform3D(Basis(Vector3(m[0], m[1], m[2]), Vector3(m[4], m[5], m[6]), Vector3(m[8], m[9], m[10])), Vector3(m[12], m[13], m[14]))


## One element of a uniform of GLSL type `t`.
static func element(t: String, e: Array) -> Variant:
	match t:
		"float": return float(e[0])
		"int", "uint": return int(e[0])
		"bool": return bool(e[0])
		"vec2": return Vector2(e[0], e[1])
		"vec3": return Vector3(e[0], e[1], e[2])
		"vec4": return Vector4(e[0], e[1], e[2], e[3])
		"ivec2", "uvec2": return Vector2i(int(e[0]), int(e[1]))
		"ivec3", "uvec3": return Vector3i(int(e[0]), int(e[1]), int(e[2]))
		"ivec4", "uvec4": return Vector4i(int(e[0]), int(e[1]), int(e[2]), int(e[3]))
		"bvec2": return [bool(e[0]), bool(e[1])]
		"mat2": return Transform2D(Vector2(e[0], e[1]), Vector2(e[2], e[3]), Vector2.ZERO)
		"mat3": return Basis(Vector3(e[0], e[1], e[2]), Vector3(e[3], e[4], e[5]), Vector3(e[6], e[7], e[8]))
		"mat4": return Projection(Vector4(e[0], e[1], e[2], e[3]), Vector4(e[4], e[5], e[6], e[7]), Vector4(e[8], e[9], e[10], e[11]), Vector4(e[12], e[13], e[14], e[15]))
	return null


## A captured uniform: one element, or an array as the packed array Godot takes for that type.
static func value(p: Dictionary) -> Variant:
	var t: String = p.type
	var els: Array = p.elements
	if not p.array:
		return element(t, els[0])
	match t:
		"float":
			var a := PackedFloat32Array()
			for e in els: a.append(float(e[0]))
			return a
		"int", "uint", "bool":
			var a := PackedInt32Array()
			for e in els: a.append(int(e[0]))
			return a
		"vec2":
			var a := PackedVector2Array()
			for e in els: a.append(element(t, e))
			return a
		"vec3":
			var a := PackedVector3Array()
			for e in els: a.append(element(t, e))
			return a
		"vec4":
			var a := PackedVector4Array()
			for e in els: a.append(element(t, e))
			return a
	var arr := []
	for e in els: arr.append(element(t, e))
	return arr
