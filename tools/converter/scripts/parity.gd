# Godot's side of scripts/parity.mjs: each case's mesh, transform, instances, texture and uniform values (what
# three.js uploaded) drawn with the translated shader in a 64x64 SubViewport, the same camera as three's
# (fov 50 vertical, near 0.1, far 100, at z = 3 looking at the origin), linear tone mapping; written as raw RGBA8.
extends SceneTree

const SIZE := 64
# The captured values as Godot takes them (scripts/three_values.gd, copied beside this script by parity.mjs): Three.value
# for each uniform in _material, Three.matrix for each transform in _node; replay.gd reads the same owner.
const Three := preload("res://three_values.gd")

func _initialize() -> void:
	var cases: Array = JSON.parse_string(FileAccess.get_file_as_string("res://cases.json"))
	for c in cases:
		await _draw(c)
	print("PARITY DONE")
	quit()

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

## The case's viewport: its own world, no antialiasing, a black background, linear tone mapping and three's camera.
func _viewport() -> SubViewport:
	var vp := SubViewport.new()
	vp.size = Vector2i(SIZE, SIZE)
	vp.own_world_3d = true
	vp.msaa_3d = Viewport.MSAA_DISABLED
	vp.screen_space_aa = Viewport.SCREEN_SPACE_AA_DISABLED
	vp.use_taa = false
	vp.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	root.add_child(vp)
	# the shared stage (Three.stage: linear, no ambient, a current camera) on Environment's black, the camera as three's
	var cam := Three.stage(vp, Environment.BG_COLOR)
	cam.fov = 50.0
	cam.near = 0.1
	cam.far = 100.0
	cam.position = Vector3(0, 0, 3)
	return vp


## The case's material: the translated shader, its uniforms, and every texture three bound, its bytes as uploaded.
func _material(c: Dictionary) -> ShaderMaterial:
	var mat := Three.material(c.shader, c.params) # the translated shader and every uniform three set
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
	return mat


## The node that draws the case's mesh: a MultiMesh of its instances, or the mesh alone, at the case's transform.
func _node(c: Dictionary, mesh: ArrayMesh) -> Node3D:
	if c.instances != null:
		var mmi := Three.instances(mesh, c.instances) # each with its matrix, colour (alpha 1) and custom data
		mmi.transform = Three.matrix(c.transform)
		return mmi
	var mi := MeshInstance3D.new()
	mi.mesh = mesh
	mi.transform = Three.matrix(c.transform)
	return mi


## One case drawn and read back: four drawn frames, then the viewport's pixels as raw RGBA8.
func _draw(c: Dictionary) -> void:
	var vp := _viewport()
	vp.add_child(_node(c, _mesh(c.mesh, _material(c))))
	for i in 4:
		await RenderingServer.frame_post_draw
	var out := vp.get_texture().get_image()
	out.convert(Image.FORMAT_RGBA8)
	var f := FileAccess.open("res://godot_%s.rgba" % c.id, FileAccess.WRITE)
	f.store_buffer(out.get_data())
	f.close()
	print("PARITY ", c.id)
	vp.queue_free()
