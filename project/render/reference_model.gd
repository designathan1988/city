## A real building's 3D model in the Build tool, translucent over the building picked, to build over it or to read it into a building (the 4.0's referenceModel.ts). Loaded at run
## time from a .glb or .gltf by GLTFDocument. The pictures it is read from are a copy of it drawn flat, unlit, from each side and from above, in a viewport of its own.
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
var _state: GLTFState
var _doc: GLTFDocument
var _quarter := 0
var _mirrored := false
var _target := {"x": 0.0, "z": 0.0, "y": 0.0, "rot": 0.0}
var _materials: Array[StandardMaterial3D] = []
var _capture_epoch := 0
var _capture_ticket := 0


func _init() -> void:
	add_child(_inner)
	visible = false


## Its plan centred on the origin and its foot at 0. False when the file is no model.
func load_file(path: String) -> bool:
	var document := GLTFDocument.new()
	var state := GLTFState.new()
	if document.append_from_file(path, state) != OK:
		return false
	var model: Node3D = document.generate_scene(state)
	if model == null:
		return false
	clear()
	_doc = document
	_state = state
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
	_prepare_materials()
	_apply_pose()
	return true


func clear() -> void:
	_capture_epoch += 1
	_materials.clear()
	_state = null
	_doc = null
	for child in _inner.get_children():
		_inner.remove_child(child)
		child.queue_free()
	loaded = false
	visible = false


func place_on(x: float, z: float, y: float, rot: float) -> void:
	if _target.x == x and _target.z == z and _target.y == y and _target.rot == rot:
		return
	_capture_epoch += 1
	_target = {"x": x, "z": z, "y": y, "rot": rot}
	_apply_pose()


func turn() -> void:
	_capture_epoch += 1
	_quarter = (_quarter + 1) % 4
	_apply_pose()


func flip() -> void:
	_capture_epoch += 1
	_mirrored = not _mirrored
	_apply_pose()


func fade(by: int) -> void:
	var next := clampf(snappedf(opacity + by * OPACITY_GRAIN, OPACITY_GRAIN), LEAST_OPACITY, 1.0)
	if next == opacity:
		return
	opacity = next
	_apply_opacity()


func toggle() -> void:
	visible = loaded and not visible


func frame() -> Dictionary:
	return _target


func _apply_pose() -> void:
	position = Vector3(_target.x, _target.y, _target.z)
	rotation.y = _target.rot + _quarter * QUARTER
	_inner.scale = Vector3(1, 1, -1 if _mirrored else 1)


## Each imported surface owns one appearance resource; moving and mirroring change transforms only.
func _prepare_materials() -> void:
	for mesh: MeshInstance3D in _meshes(_inner):
		mesh.cast_shadow = GeometryInstance3D.SHADOW_CASTING_SETTING_OFF
		for i in mesh.mesh.get_surface_count():
			var source: Material = mesh.mesh.surface_get_material(i)
			var seen: StandardMaterial3D = source.duplicate() if source is StandardMaterial3D else StandardMaterial3D.new()
			seen.cull_mode = BaseMaterial3D.CULL_DISABLED
			mesh.set_surface_override_material(i, seen)
			_materials.push_back(seen)
	_apply_opacity()


func _apply_opacity() -> void:
	for material in _materials:
		material.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA if opacity < OPAQUE else BaseMaterial3D.TRANSPARENCY_DISABLED
		material.albedo_color.a = opacity


## The model as a surveyor reads it, in plan axes (x, y on the plan, z up): {triangles, views}. Takes a few frames: the pictures are drawn by the renderer.
func capture() -> Dictionary:
	if not loaded or _doc == null or _state == null:
		return {"error": "hint.reference.failed"}
	_capture_ticket += 1
	var ticket := _capture_ticket
	var epoch := _capture_epoch
	var placement: Dictionary = _target.duplicate(true)
	var copy := Node3D.new()
	var model: Node3D = _doc.generate_scene(_state)
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
	var triangles: Variant = CityStore.query(&"prepare_reference", [_mesh_input(copy)])
	if triangles == null:
		viewport.queue_free()
		return {"error": "hint.reference.failed"}
	var views: Array = []
	for side: Vector3 in SIDES:
		var picture: Dictionary = await _picture(viewport, camera, box, side, epoch, ticket)
		if picture.is_empty() or not _capture_valid(epoch, ticket):
			if is_instance_valid(viewport):
				viewport.queue_free()
			return {"error": "hint.reference.failed"}
		views.push_back(picture)
	viewport.queue_free()
	return {"triangles": triangles, "views": views, "frame": placement}


func _capture_valid(epoch: int, ticket: int) -> bool:
	return loaded and is_inside_tree() and epoch == _capture_epoch and ticket == _capture_ticket


func _picture(viewport: SubViewport, camera: Camera3D, box: AABB, side: Vector3, epoch: int, ticket: int) -> Dictionary:
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
		if not _capture_valid(epoch, ticket) or not is_instance_valid(viewport) or not is_instance_valid(camera):
			return {}
	var image := viewport.get_texture().get_image()
	if image == null:
		return {}
	image.convert(Image.FORMAT_RGBA8)
	return {"rgba": image.get_data(), "w": image.get_width(), "h": image.get_height(), "centre": _plan(centre), "right": _plan(camera.global_basis.x), "up": _plan(camera.global_basis.y),
		"out": _plan(side), "half_w": across / 2.0, "half_h": tall / 2.0}


func _mesh_input(root: Node3D) -> Array:
	var out: Array = []
	for mesh: MeshInstance3D in _meshes(root):
		var to_world := mesh.global_transform
		for i in mesh.mesh.get_surface_count():
			var arrays: Array = mesh.mesh.surface_get_arrays(i)
			var points: PackedVector3Array = arrays[Mesh.ARRAY_VERTEX]
			var index: PackedInt32Array = arrays[Mesh.ARRAY_INDEX] if arrays[Mesh.ARRAY_INDEX] != null else PackedInt32Array()
			if points.is_empty() and index.is_empty():
				continue
			if not index.is_empty() and index.size() < points.size():
				# A sparse surface should not allocate/transform all its unused vertices: this representation is smaller.
				var selected := PackedVector3Array()
				if selected.resize(index.size()) != OK:
					return []
				for at in index.size():
					var k := index[at]
					if k < 0 or k >= points.size():
						return []
					selected[at] = to_world * points[k]
				out.push_back({"points": selected, "index": PackedInt32Array()})
			else:
				# Engine bulk transform once per vertex; the native root pins these packed arrays and validates references.
				out.push_back({"points": to_world * points, "index": index})
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
