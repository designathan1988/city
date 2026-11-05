extends SceneTree
# Godot's side of scripts/parity.mjs: each case's mesh, transform, instances, texture and uniform values (what
# three.js uploaded) drawn with the translated shader in a 64x64 SubViewport, the same camera as three's
# (fov 50 vertical, near 0.1, far 100, at z = 3 looking at the origin), linear tone mapping; written as raw RGBA8.

const SIZE := 64

func _initialize() -> void:
	var cases: Array = JSON.parse_string(FileAccess.get_file_as_string("res://cases.json"))
	for c in cases:
		await _draw(c)
	print("PARITY DONE")
	quit()

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
		"int", "uint":
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

func _mesh(m: Dictionary, material: Material) -> ArrayMesh:
	var arrays := []
	arrays.resize(Mesh.ARRAY_MAX)
	var pos := PackedVector3Array()
	var p: Array = m.position
	for i in range(0, p.size(), 3): pos.append(Vector3(p[i], p[i + 1], p[i + 2]))
	arrays[Mesh.ARRAY_VERTEX] = pos
	if m.normal != null:
		var nor := PackedVector3Array()
		var n: Array = m.normal
		for i in range(0, n.size(), 3): nor.append(Vector3(n[i], n[i + 1], n[i + 2]))
		arrays[Mesh.ARRAY_NORMAL] = nor
	if m.uv != null:
		var uv := PackedVector2Array()
		var u: Array = m.uv
		for i in range(0, u.size(), 2): uv.append(Vector2(u[i], u[i + 1]))
		arrays[Mesh.ARRAY_TEX_UV] = uv
	if m.color != null:
		var col := PackedColorArray()
		var cl: Array = m.color
		for i in range(0, cl.size(), 3): col.append(Color(cl[i], cl[i + 1], cl[i + 2], 1.0))
		arrays[Mesh.ARRAY_COLOR] = col
	var flags := 0
	var shifts := [Mesh.ARRAY_FORMAT_CUSTOM0_SHIFT, Mesh.ARRAY_FORMAT_CUSTOM1_SHIFT, Mesh.ARRAY_FORMAT_CUSTOM2_SHIFT, Mesh.ARRAY_FORMAT_CUSTOM3_SHIFT]
	for k in 4:
		if m.customs[k] != null:
			arrays[Mesh.ARRAY_CUSTOM0 + k] = PackedFloat32Array(m.customs[k])
			flags |= Mesh.ARRAY_CUSTOM_RGBA_FLOAT << shifts[k]
	if m.bones != null:
		var b := PackedInt32Array()
		for x in m.bones: b.append(int(x))
		arrays[Mesh.ARRAY_BONES] = b
		# the translation reads three's skinWeight from a CUSTOM slot (exact); Godot still requires weights with bones
		if m.weights != null:
			arrays[Mesh.ARRAY_WEIGHTS] = PackedFloat32Array(m.weights)
		else:
			var z := PackedFloat32Array()
			z.resize(b.size())
			arrays[Mesh.ARRAY_WEIGHTS] = z
	# three's front faces are counter-clockwise, Godot's clockwise: each triangle reversed
	var idx := PackedInt32Array()
	var ix: Array = m.index
	for i in range(0, ix.size(), 3): idx.append_array([ix[i], ix[i + 2], ix[i + 1]])
	arrays[Mesh.ARRAY_INDEX] = idx
	var mesh := ArrayMesh.new()
	mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays, [], {}, flags)
	mesh.surface_set_material(0, material)
	return mesh

func _draw(c: Dictionary) -> void:
	var vp := SubViewport.new()
	vp.size = Vector2i(SIZE, SIZE)
	vp.own_world_3d = true
	vp.msaa_3d = Viewport.MSAA_DISABLED
	vp.screen_space_aa = Viewport.SCREEN_SPACE_AA_DISABLED
	vp.use_taa = false
	vp.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	root.add_child(vp)
	var env := Environment.new()
	env.background_mode = Environment.BG_COLOR
	env.background_color = Color(0, 0, 0)
	env.tonemap_mode = Environment.TONE_MAPPER_LINEAR
	env.ambient_light_source = Environment.AMBIENT_SOURCE_DISABLED
	var we := WorldEnvironment.new()
	we.environment = env
	vp.add_child(we)
	var cam := Camera3D.new()
	cam.fov = 50.0
	cam.near = 0.1
	cam.far = 100.0
	cam.position = Vector3(0, 0, 3)
	vp.add_child(cam)
	cam.make_current()
	var shader := Shader.new()
	shader.code = FileAccess.get_file_as_string(c.shader)
	var mat := ShaderMaterial.new()
	mat.shader = shader
	for p in c.params:
		mat.set_shader_parameter(p.godot, _value(p))
	# every texture three bound for the program, its bytes as uploaded
	var formats := { "RGBA8": Image.FORMAT_RGBA8, "RGH": Image.FORMAT_RGH, "RGBAH": Image.FORMAT_RGBAH, "RGF": Image.FORMAT_RGF, "RGBAF": Image.FORMAT_RGBAF, "RF": Image.FORMAT_RF, "RH": Image.FORMAT_RH, "R8": Image.FORMAT_R8 }
	for tex in c.textures:
		if tex.format == "UINT16":
			# an integer texture whose values fit 16 bits: Godot 4.7's widest integer format, through the RenderingDevice
			var rd := RenderingServer.get_rendering_device()
			var fmt := RDTextureFormat.new()
			fmt.width = int(tex.width)
			fmt.height = int(tex.height)
			fmt.format = RenderingDevice.DATA_FORMAT_R16G16B16A16_UINT
			fmt.usage_bits = RenderingDevice.TEXTURE_USAGE_SAMPLING_BIT | RenderingDevice.TEXTURE_USAGE_CAN_UPDATE_BIT
			var t := Texture2DRD.new()
			t.texture_rd_rid = rd.texture_create(fmt, RDTextureView.new(), [Marshalls.base64_to_raw(tex.bytes)])
			mat.set_shader_parameter(tex.uniform, t)
			continue
		if tex.format == null:
			push_error("PARITY unknown texture format for " + tex.uniform)
			continue
		var img := Image.create_from_data(int(tex.width), int(tex.height), false, formats[tex.format], Marshalls.base64_to_raw(tex.bytes))
		mat.set_shader_parameter(tex.uniform, ImageTexture.create_from_image(img))
	var mesh := _mesh(c.mesh, mat)
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
			mm.set_instance_color(i, Color(col[0], col[1], col[2], 1.0))
			if c.instances[i].has("custom"):
				var cd: Array = c.instances[i].custom
				mm.set_instance_custom_data(i, Color(cd[0], cd[1], cd[2], cd[3]))
		var mmi := MultiMeshInstance3D.new()
		mmi.multimesh = mm
		mmi.transform = _matrix(c.transform)
		vp.add_child(mmi)
	else:
		var mi := MeshInstance3D.new()
		mi.mesh = mesh
		mi.transform = _matrix(c.transform)
		vp.add_child(mi)
	for i in 4:
		await RenderingServer.frame_post_draw
	var out := vp.get_texture().get_image()
	out.convert(Image.FORMAT_RGBA8)
	var f := FileAccess.open("res://godot_%s.rgba" % c.id, FileAccess.WRITE)
	f.store_buffer(out.get_data())
	f.close()
	print("PARITY ", c.id)
	vp.queue_free()
