extends SceneTree
# Godot's side of scripts/frame-replay.mjs: each captured three.js draw alone, with the program translated in
# "three matrices" mode (three's own matrices as uniforms, its clip position converted), the mesh as the translation
# placed each attribute, every texture uploaded through the RenderingDevice in the format three's GPU held (sRGB
# decoded by the format, like WebGL; uint and int as integer formats; arrays; every mip level), every uniform three
# set. Drawn in a SubViewport with HDR 2D (the values the program wrote, unencoded), read back as RGBA8.
# Prints REPLAY BEGIN/END <draw> (its log says which draws finished); skips the draws res://live-skip.txt names.

var rd: RenderingDevice
var cache := {}
# the RenderingDevice textures made here, freed before quitting (after their Texture2DRD wrappers)
var rids: Array[RID] = []

func _initialize() -> void:
	print("LIVE PID ", OS.get_process_id())
	rd = RenderingServer.get_rendering_device()
	RenderingServer.set_default_clear_color(Color(0, 0, 0, 0))
	DirAccess.make_dir_recursive_absolute("res://out")
	var skip := {}
	if FileAccess.file_exists("res://live-skip.txt"):
		for l in FileAccess.get_file_as_string("res://live-skip.txt").split("\n"):
			if l.strip_edges() != "":
				skip[l.strip_edges()] = true
	var cases: Array = JSON.parse_string(FileAccess.get_file_as_string("res://cases.json"))
	for c in cases:
		# a case's id: its draw, or <draw>c<k> for one component pass of a float target
		var id := str(c.get("id", str(int(c.index))))
		if skip.has(id):
			continue
		print("REPLAY BEGIN ", id)
		await _draw(c)
		print("REPLAY END ", id)
	# the last viewport is freed at the end of its frame: two more drawn frames, then the textures
	for i in 2:
		await RenderingServer.frame_post_draw
	cache.clear()
	for r in rids:
		if rd.texture_is_valid(r):
			rd.free_rid(r)
	print("REPLAY DONE")
	quit()

func _floats(name) -> PackedFloat32Array:
	return FileAccess.get_file_as_bytes("res://data/" + name).to_float32_array()

func _ints(name) -> PackedInt32Array:
	return FileAccess.get_file_as_bytes("res://data/" + name).to_int32_array()

func _matrix(m: Array) -> Transform3D:
	return Transform3D(Basis(Vector3(m[0], m[1], m[2]), Vector3(m[4], m[5], m[6]), Vector3(m[8], m[9], m[10])), Vector3(m[12], m[13], m[14]))

func _value(p: Dictionary) -> Variant:
	var t: String = p.type
	var els: Array = p.elements
	var one := func(e: Array) -> Variant:
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
	if not p.array:
		return one.call(els[0])
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
			for e in els: a.append(one.call(e))
			return a
		"vec3":
			var a := PackedVector3Array()
			for e in els: a.append(one.call(e))
			return a
		"vec4":
			var a := PackedVector4Array()
			for e in els: a.append(one.call(e))
			return a
	var arr := []
	for e in els: arr.append(one.call(e))
	return arr

# a texture as three's GPU held it: the RenderingDevice format of the captured kind, every level, every layer
func _texture(t: Dictionary) -> Texture:
	var key := JSON.stringify(t.levels)
	if cache.has(key):
		return cache[key]
	var fmt := RDTextureFormat.new()
	var bpp := 16
	match t.kind:
		"unorm":
			# UNORM storage, shareable as its sRGB view: Godot views a Texture2DRD through its Image format (UNORM),
			# and samples the sRGB view (decoded by the hardware, like WebGL's SRGB8_ALPHA8) for a source_color uniform
			fmt.format = RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM
			if t.get("srgb", false):
				fmt.add_shareable_format(RenderingDevice.DATA_FORMAT_R8G8B8A8_UNORM)
				fmt.add_shareable_format(RenderingDevice.DATA_FORMAT_R8G8B8A8_SRGB)
			bpp = 4
		"uint16":
			# 16-bit uint: the widest integer format Godot 4.7 maps (Image FORMAT_RGBA16I); wider ones come as float bits
			fmt.format = RenderingDevice.DATA_FORMAT_R16G16B16A16_UINT
			bpp = 8
		"uint": fmt.format = RenderingDevice.DATA_FORMAT_R32G32B32A32_UINT
		"int": fmt.format = RenderingDevice.DATA_FORMAT_R32G32B32A32_SINT
		_: fmt.format = RenderingDevice.DATA_FORMAT_R32G32B32A32_SFLOAT
	var levels: Array = t.levels
	fmt.width = int(levels[0].width)
	fmt.height = int(levels[0].height)
	fmt.mipmaps = levels.size()
	var layers := int(t.get("layers", 1))
	fmt.array_layers = layers
	fmt.texture_type = RenderingDevice.TEXTURE_TYPE_2D_ARRAY if t.get("array", false) else RenderingDevice.TEXTURE_TYPE_2D
	fmt.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_CAN_UPDATE_BIT
	var data := []
	for layer in layers:
		var bytes := PackedByteArray()
		for l in levels:
			var name = l.files[layer] if l.has("files") else l.file
			var b := FileAccess.get_file_as_bytes("res://data/" + name)
			var want := int(l.width) * int(l.height) * bpp
			if b.size() != want:
				b.resize(want)
			bytes.append_array(b)
		data.append(bytes)
	var rid := rd.texture_create(fmt, RDTextureView.new(), data)
	if rid.is_valid():
		rids.append(rid)
	if not rid.is_valid():
		push_error("REPLAY texture not created: " + t.uniform)
		return null
	var tex: Texture
	if t.get("array", false):
		tex = Texture2DArrayRD.new()
		tex.texture_rd_rid = rid
	else:
		tex = Texture2DRD.new()
		tex.texture_rd_rid = rid
	cache[key] = tex
	return tex

func _mesh(c: Dictionary, material: Material) -> ArrayMesh:
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	var n := int(c.vertexCount)
	var a: Dictionary = c.arrays
	var pos := PackedVector3Array()
	if a.has("vertex"):
		var p := _floats(a.vertex)
		for i in n: pos.append(Vector3(p[i * 3], p[i * 3 + 1], p[i * 3 + 2]))
	else:
		pos.resize(n)
	arrays[Mesh.ARRAY_VERTEX] = pos
	if a.has("normal"):
		var q := _floats(a.normal)
		var nor := PackedVector3Array()
		for i in n: nor.append(Vector3(q[i * 3], q[i * 3 + 1], q[i * 3 + 2]))
		arrays[Mesh.ARRAY_NORMAL] = nor
	if a.has("tangent"):
		arrays[Mesh.ARRAY_TANGENT] = _floats(a.tangent)
	if a.has("uv"):
		var u := _floats(a.uv)
		var uv := PackedVector2Array()
		for i in n: uv.append(Vector2(u[i * 2], u[i * 2 + 1]))
		arrays[Mesh.ARRAY_TEX_UV] = uv
	if a.has("uv2"):
		var u2 := _floats(a.uv2)
		var uv2 := PackedVector2Array()
		for i in n: uv2.append(Vector2(u2[i * 2], u2[i * 2 + 1]))
		arrays[Mesh.ARRAY_TEX_UV2] = uv2
	if a.has("color"):
		var cl := _floats(a.color)
		var col := PackedColorArray()
		for i in n: col.append(Color(cl[i * 4], cl[i * 4 + 1], cl[i * 4 + 2], cl[i * 4 + 3]))
		arrays[Mesh.ARRAY_COLOR] = col
	var flags := 0
	var shifts := [Mesh.ARRAY_FORMAT_CUSTOM0_SHIFT, Mesh.ARRAY_FORMAT_CUSTOM1_SHIFT, Mesh.ARRAY_FORMAT_CUSTOM2_SHIFT, Mesh.ARRAY_FORMAT_CUSTOM3_SHIFT]
	for k in 4:
		if c.customs[k] != null:
			arrays[Mesh.ARRAY_CUSTOM0 + k] = _floats(c.customs[k])
			flags |= Mesh.ARRAY_CUSTOM_RGBA_FLOAT << shifts[k]
	if c.bones != null:
		arrays[Mesh.ARRAY_BONES] = _ints(c.bones)
		# the translation reads three's skinWeight from a CUSTOM slot (exact); Godot still requires weights with bones
		if c.weights != null:
			arrays[Mesh.ARRAY_WEIGHTS] = _floats(c.weights)
		else:
			var z := PackedFloat32Array()
			z.resize(_ints(c.bones).size())
			arrays[Mesh.ARRAY_WEIGHTS] = z
	# the indices as frame-replay.mjs wrote them (triangles already reversed: Godot's front faces are clockwise)
	arrays[Mesh.ARRAY_INDEX] = _ints(c.indices)
	var prim := Mesh.PRIMITIVE_TRIANGLES
	if c.primitive == "lines": prim = Mesh.PRIMITIVE_LINES
	elif c.primitive == "points": prim = Mesh.PRIMITIVE_POINTS
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(prim, arrays, [], {}, flags)
	mesh.surface_set_material(0, material)
	# the positions are three's (uniforms), not Godot's camera: never culled
	mesh.custom_aabb = AABB(Vector3(-1e6, -1e6, -1e6), Vector3(2e6, 2e6, 2e6))
	return mesh

func _draw(c: Dictionary) -> void:
	var vp := SubViewport.new()
	vp.size = Vector2i(int(c.size[0]), int(c.size[1]))
	vp.own_world_3d = true
	vp.transparent_bg = true
	vp.use_hdr_2d = true
	vp.msaa_3d = Viewport.MSAA_DISABLED
	vp.screen_space_aa = Viewport.SCREEN_SPACE_AA_DISABLED
	vp.use_taa = false
	# three's texture.anisotropy: Godot 4.7 sets it per viewport
	vp.anisotropic_filtering_level = int(c.get("anisotropy", 0))
	vp.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	root.add_child(vp)
	var env := Environment.new()
	env.background_mode = Environment.BG_CLEAR_COLOR
	env.tonemap_mode = Environment.TONE_MAPPER_LINEAR
	env.ambient_light_source = Environment.AMBIENT_SOURCE_DISABLED
	var we := WorldEnvironment.new()
	we.environment = env
	vp.add_child(we)
	var cam := Camera3D.new()
	vp.add_child(cam)
	cam.make_current()
	var shader := Shader.new()
	shader.code = FileAccess.get_file_as_string(c.shader)
	var mat := ShaderMaterial.new()
	mat.shader = shader
	for p in c.params:
		mat.set_shader_parameter(p.godot, _value(p))
	# textures: one per uniform, or an array of them (sampler2D x[n])
	var groups := {}
	for t in c.textures:
		var tex := _texture(t)
		if t.has("element"):
			if not groups.has(t.uniform): groups[t.uniform] = []
			var g: Array = groups[t.uniform]
			while g.size() <= int(t.element): g.append(null)
			g[int(t.element)] = tex
		else:
			mat.set_shader_parameter(t.uniform, tex)
	for u in groups:
		mat.set_shader_parameter(u, groups[u])
	var mesh := _mesh(c, mat)
	if c.instances != null:
		var mm := MultiMesh.new()
		mm.transform_format = MultiMesh.TRANSFORM_3D
		mm.use_colors = true
		mm.use_custom_data = true
		mm.mesh = mesh
		mm.instance_count = c.instances.size()
		for i in c.instances.size():
			mm.set_instance_transform(i, _matrix(c.instances[i].matrix))
			var col: Array = c.instances[i].color
			mm.set_instance_color(i, Color(col[0], col[1], col[2], col[3]))
			var cd: Array = c.instances[i].custom
			mm.set_instance_custom_data(i, Color(cd[0], cd[1], cd[2], cd[3]))
		mm.custom_aabb = AABB(Vector3(-1e6, -1e6, -1e6), Vector3(2e6, 2e6, 2e6))
		var mmi := MultiMeshInstance3D.new()
		mmi.multimesh = mm
		vp.add_child(mmi)
	else:
		var mi := MeshInstance3D.new()
		mi.mesh = mesh
		vp.add_child(mi)
	for i in 3:
		await RenderingServer.frame_post_draw
	var out := vp.get_texture().get_image()
	# a component pass of a float target wrote bytes k/255: kept as floats (scripts/replay-judge.mjs rounds them);
	# any other case as RGBA8
	var bits: bool = c.get("bits", false)
	out.convert(Image.FORMAT_RGBAF if bits else Image.FORMAT_RGBA8)
	var f := FileAccess.open("res://out/%s.%s" % [str(c.get("id", str(int(c.index)))), "f32" if bits else "rgba"], FileAccess.WRITE)
	f.store_buffer(out.get_data())
	f.close()
	vp.queue_free()
