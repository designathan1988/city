## A real building's 3D model in the Build tool, translucent over the building picked, to build over it or to read it into a building (the 4.0's referenceModel.ts). Loaded at run
## time from a .glb or .gltf by GLTFDocument, once: the scene it makes is kept as it came (`_pristine`), and what is read off the model (its triangles and its pictures,
## drawn flat, unlit, from each side and from above, in a viewport of its own) comes from a copy of that scene, sharing its meshes. Its look over the city is a material
## per surface, made once a file and changed in place by fade; a move only moves it.
class_name ReferenceModel
extends Node3D

const PX_PER_M := 8.0
const PICTURE_CAP := 2048.0
const PICTURE_FLOOR := 8
const OPACITY_GRAIN := 0.1
const LEAST_OPACITY := 0.1
const QUARTER := PI / 2.0
const REACH_PAD_M := 10.0
const DRAWN_FRAMES := 2
const OPAQUE := 0.999
## Where each picture looks from: front (plan -y), right, back, left and above, in Godot's axes.
const SIDES: Array[Vector3] = [Vector3.BACK, Vector3.RIGHT, Vector3.FORWARD, Vector3.LEFT, Vector3.UP]

var loaded := false
var opacity := 0.5
var file_name := ""
var size_m := Vector3.ZERO

var _inner := Node3D.new()
var _pristine: Node3D # the scene as the file gave it, outside the tree: what capture() copies
var _seen: Array[StandardMaterial3D] = [] # the translucent look of each surface shown
var _quarter := 0
var _mirrored := false
var _target := {"x": 0.0, "z": 0.0, "y": 0.0, "rot": 0.0}


func _init() -> void:
	add_child(_inner)
	visible = false


## Its plan centred on the origin and its foot at 0. False when the file is no model.
func load_file(path: String) -> bool:
	var doc := GLTFDocument.new()
	var state := GLTFState.new()
	if doc.append_from_file(path, state) != OK:
		return false
	var generated: Node3D = doc.generate_scene(state)
	if generated == null:
		return false
	clear()
	_pristine = generated
	var model: Node3D = generated.duplicate()
	_inner.add_child(model)
	var box := _bounds(model)
	if box.size == Vector3.ZERO:
		clear()
		return false
	model.position = -Vector3(box.get_center().x, box.position.y, box.get_center().z)
	size_m = box.size
	file_name = path.get_file()
	loaded = true
	visible = true
	_quarter = 0
	_mirrored = false
	_dress(model)
	_apply()
	return true


func clear() -> void:
	for child in _inner.get_children():
		_inner.remove_child(child)
		child.queue_free()
	if _pristine != null:
		_pristine.free()
		_pristine = null
	_seen = []
	loaded = false
	visible = false


func _notification(what: int) -> void:
	if what == NOTIFICATION_PREDELETE and _pristine != null:
		_pristine.free() # outside the tree: nothing else frees it


func place_on(x: float, z: float, y: float, rot: float) -> void:
	_target = {"x": x, "z": z, "y": y, "rot": rot}
	_apply()


func turn() -> void:
	_quarter = (_quarter + 1) % 4
	_apply()


func flip() -> void:
	_mirrored = not _mirrored
	_apply()


func fade(by: int) -> void:
	opacity = clampf(snappedf(opacity + by * OPACITY_GRAIN, OPACITY_GRAIN), LEAST_OPACITY, 1.0)
	_look()


func toggle() -> void:
	visible = loaded and not visible


func frame() -> Dictionary:
	return _target


func _apply() -> void:
	position = Vector3(_target.x, _target.y, _target.z)
	rotation.y = _target.rot + _quarter * QUARTER
	_inner.scale = Vector3(1, 1, -1 if _mirrored else 1)


## Each surface shown through a material of its own, a copy of the file's seen from both sides: made once a file.
func _dress(model: Node3D) -> void:
	_seen = []
	for mesh: MeshInstance3D in _meshes(model):
		mesh.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		for i in mesh.mesh.get_surface_count():
			var source: Material = mesh.mesh.surface_get_material(i)
			var seen: StandardMaterial3D = source.duplicate() if source is StandardMaterial3D else StandardMaterial3D.new()
			seen.cull_mode = BaseMaterial3D.CULL_DISABLED
			mesh.set_surface_override_material(i, seen)
			_seen.push_back(seen)
	_look()


## The opacity on every surface shown.
func _look() -> void:
	for seen in _seen:
		seen.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA if opacity < OPAQUE else BaseMaterial3D.TRANSPARENCY_DISABLED
		seen.albedo_color.a = opacity


## The model as a surveyor reads it, in plan axes (x, y on the plan, z up): {triangles, views}. Takes a few frames: the pictures are drawn by the renderer.
func capture() -> Dictionary:
	var copy := Node3D.new()
	var model: Node3D = _pristine.duplicate()
	var turned := Node3D.new()
	turned.rotation.y = _quarter * QUARTER
	turned.scale = Vector3(1, 1, -1 if _mirrored else 1)
	turned.add_child(model)
	copy.add_child(turned)
	var viewport := SubViewport.new()
	viewport.own_world_3d = true
	viewport.transparent_bg = true
	viewport.debug_draw = Viewport.DEBUG_DRAW_UNSHADED
	viewport.render_target_update_mode = SubViewport.UPDATE_ALWAYS
	var camera := Camera3D.new()
	camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	viewport.add_child(camera)
	viewport.add_child(copy)
	add_child(viewport)
	var box0 := _bounds(model)
	model.position = -Vector3(box0.get_center().x, box0.position.y, box0.get_center().z)
	var box := _bounds(copy)
	var triangles := _triangles(copy)
	var views: Array = []
	for side: Vector3 in SIDES:
		views.push_back(await _picture(viewport, camera, box, side))
	viewport.queue_free()
	return {"triangles": triangles, "views": views}


func _picture(viewport: SubViewport, camera: Camera3D, box: AABB, side: Vector3) -> Dictionary:
	var top := side == Vector3.UP
	var across := box.size.x if top or side.x == 0.0 else box.size.z
	var tall := box.size.z if top else box.size.y
	var px := minf(PX_PER_M, PICTURE_CAP / maxf(maxf(across, tall), 1.0))
	viewport.size = Vector2i(maxi(PICTURE_FLOOR, ceili(across * px)), maxi(PICTURE_FLOOR, ceili(tall * px)))
	var reach := box.size.length() + REACH_PAD_M
	var centre := box.get_center()
	camera.size = tall
	camera.keep_aspect = Camera3D.KEEP_HEIGHT
	camera.near = 1.0
	camera.far = reach * 2.0
	camera.position = centre + side * reach
	camera.look_at_from_position(camera.position, centre, Vector3.FORWARD if top else Vector3.UP)
	for i in DRAWN_FRAMES:
		await RenderingServer.frame_post_draw
	var image := viewport.get_texture().get_image()
	image.convert(Image.FORMAT_RGBA8)
	return {"rgba": image.get_data(), "w": image.get_width(), "h": image.get_height(), "centre": _plan(centre), "right": _plan(camera.global_basis.x), "up": _plan(camera.global_basis.y),
		"out": _plan(side), "half_w": across / 2.0, "half_h": tall / 2.0}


func _triangles(root: Node3D) -> PackedVector3Array:
	var out := PackedVector3Array()
	for mesh: MeshInstance3D in _meshes(root):
		var to_world := mesh.global_transform
		for i in mesh.mesh.get_surface_count():
			var arrays: Array = mesh.mesh.surface_get_arrays(i)
			var points: PackedVector3Array = arrays[Mesh.ARRAY_VERTEX]
			var index: PackedInt32Array = arrays[Mesh.ARRAY_INDEX] if arrays[Mesh.ARRAY_INDEX] != null else PackedInt32Array()
			if index.is_empty():
				for p: Vector3 in points:
					out.push_back(_plan(to_world * p))
			else:
				for k: int in index:
					out.push_back(_plan(to_world * points[k]))
	return out


## Godot's axes on the plan's: front is -y, up is z.
func _plan(p: Vector3) -> Vector3:
	return Vector3(p.x, -p.z, p.y)


func _bounds(root: Node3D) -> AABB:
	var box := AABB()
	var first := true
	for mesh: MeshInstance3D in _meshes(root):
		var part: AABB = mesh.global_transform * mesh.get_aabb() if mesh.is_inside_tree() else _chain(mesh, root) * mesh.get_aabb()
		box = part if first else box.merge(part)
		first = false
	return box


## A node's transform up to `root`, for a tree not in the scene yet.
func _chain(node: Node3D, root: Node3D) -> Transform3D:
	var t := Transform3D.IDENTITY
	var at: Node = node
	while at != root and at is Node3D:
		t = (at as Node3D).transform * t
		at = at.get_parent()
	return t


func _meshes(root: Node) -> Array[MeshInstance3D]:
	var out: Array[MeshInstance3D] = []
	for child in root.get_children():
		if child is MeshInstance3D and (child as MeshInstance3D).mesh != null:
			out.push_back(child)
		out.append_array(_meshes(child))
	return out
