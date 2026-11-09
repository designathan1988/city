# Godot's side of scripts/frame-replay.mjs: each captured three.js draw alone, with the program translated in
# "three matrices" mode (three's own matrices as uniforms, its clip position converted), the mesh as the translation
# placed each attribute, every texture uploaded through the RenderingDevice in the format three's GPU held (sRGB
# decoded by the format, like WebGL; uint and int as integer formats; arrays; every mip level), every uniform three
# set. Drawn in a SubViewport with HDR 2D (the values the program wrote, unencoded), read back as RGBA8.
# Prints REPLAY BEGIN/END <draw> (its log says which draws finished); skips the draws res://live-skip.txt names.
extends SceneTree

# The captured values as Godot takes them (scripts/three_values.gd, copied beside this script by frame-replay.mjs):
# Three.value for each uniform and Three.matrix for each instance; parity.gd reads the same owner.
const Three := preload("res://three_values.gd")
# The textures uploaded as three's GPU held them, kept by their files and freed at the end (scripts/replay_textures.gd,
# copied beside this script by frame-replay.mjs); a captured texture whose bytes are not its levels' size is refused, told.
var textures: RefCounted = preload("res://replay_textures.gd").new()

func _initialize() -> void:
	print("LIVE PID ", OS.get_process_id())
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
	textures.release()
	print("REPLAY DONE")
	quit()

func _floats(name) -> PackedFloat32Array:
	return FileAccess.get_file_as_bytes("res://data/" + name).to_float32_array()

func _ints(name) -> PackedInt32Array:
	return FileAccess.get_file_as_bytes("res://data/" + name).to_int32_array()

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

## The draw's viewport: its size, HDR 2D (the values the program wrote, unencoded), no antialiasing, three's anisotropy.
func _viewport(c: Dictionary) -> SubViewport:
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
	# the shared stage (Three.stage) on the clear colour; the camera stays where it is: three's matrices are uniforms
	Three.stage(vp, Environment.BG_CLEAR_COLOR)
	return vp


## The draw's material: the translated shader, every uniform three set, its textures (one per uniform, or an array).
func _material(c: Dictionary) -> ShaderMaterial:
	var mat := Three.material(c.shader, c.params) # the translated shader and every uniform three set
	# textures: one per uniform, or an array of them (sampler2D x[n])
	var groups := {}
	for t in c.textures:
		var tex: Texture = textures.texture(t)
		if t.has("element"):
			if not groups.has(t.uniform): groups[t.uniform] = []
			var g: Array = groups[t.uniform]
			while g.size() <= int(t.element): g.append(null)
			g[int(t.element)] = tex
		else:
			mat.set_shader_parameter(t.uniform, tex)
	for u in groups:
		mat.set_shader_parameter(u, groups[u])
	return mat


## The node that draws the mesh: a MultiMesh of three's instances (never culled), or the mesh alone.
func _node(c: Dictionary, mesh: ArrayMesh) -> Node3D:
	if c.instances != null:
		return Three.instances(mesh, c.instances) # each with its matrix, colour and custom data, never culled
	var mi := MeshInstance3D.new()
	mi.mesh = mesh
	return mi


## One draw rebuilt and read back after three drawn frames: floats for a component pass of a float target, else RGBA8.
func _draw(c: Dictionary) -> void:
	var vp := _viewport(c)
	vp.add_child(_node(c, _mesh(c, _material(c))))
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
