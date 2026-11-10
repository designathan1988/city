## The 3D reference: a model written to a .glb and read back, then read into a building: two blocks, the tower on the base, the colours of their walls, undone in one step.
## Run: godot --path project res://tests/core/reference_test.tscn -- --no-flow
extends TestCase

const FILE := "reference_test.glb"
const TOWN_MAP := 0
const SEED := 1
const BASE := Vector3(24, 6, 18)
const TOWER := Vector3(12, 24, 10)
const WALL := Color(0.8, 0.5, 0.3)
const AT := Vector2(-200, 300)
const COLOUR_OFF := 0.02


func _ready() -> void:
	var rig := CityCamera.new()
	add_child(rig)
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, false)
	await get_tree().process_frame
	var file := scratch(FILE)
	_write_model(file)
	var model := ReferenceModel.new()
	add_child(model)
	check(not model.load_file(scratch("nothing_here.glb")), "a file that does not exist is refused")
	check(model.load_file(file), "the .glb opens")
	check(is_equal_approx(model.size_m.y, BASE.y + TOWER.y), "the model's height: %s" % model.size_m)
	model.place_on(AT.x, AT.y, 0.0, 0.0)
	var read: Dictionary = await model.capture()
	check(read.views.size() == 5, "five photos: %d" % read.views.size())
	check(not read.triangles.is_empty(), "%d triangle points" % read.triangles.size())
	var r: Dictionary = CityStore.dispatch(&"build_reference", [read.triangles, read.views, AT.x, AT.y, 0.0, -1])
	check(r.get("ok", false), "became a building: %s" % r)
	if r.get("ok", false):
		var info: Dictionary = CityStore.query(&"body_info", [r.placed])
		check(info.volumes.size() == 2, "two blocks: %d" % info.volumes.size())
		check(info.volumes[1].base > 0, "the tower stands on the base: %s" % info.volumes[1])
		var paint: Dictionary = CityStore.query(&"body_paint_at", [r.placed, 0, 0, 0])
		var seen := Color(paint.rgb)
		check(absf(seen.r - WALL.r) < COLOUR_OFF and absf(seen.g - WALL.g) < COLOUR_OFF and absf(seen.b - WALL.b) < COLOUR_OFF, "the wall has the model's colour: %s" % paint)
		check(CityStore.dispatch(&"undo") and CityStore.query(&"body_info", [r.placed]).is_empty(), "one undo step")
	DirAccess.remove_absolute(ProjectSettings.globalize_path(file))
	finish()


## Two boxes on the origin, the one on top of the other, as a glTF file.
func _write_model(file: String) -> void:
	var root := Node3D.new()
	_box(root, BASE, BASE.y / 2.0)
	_box(root, TOWER, BASE.y + TOWER.y / 2.0)
	var doc := GLTFDocument.new()
	var state := GLTFState.new()
	check(doc.append_from_scene(root, state) == OK, "a scene for glTF")
	check(doc.write_to_filesystem(state, file) == OK, "the .glb was written")
	root.free()


func _box(root: Node3D, size: Vector3, y: float) -> void:
	var box := BoxMesh.new()
	box.size = size
	var paint := StandardMaterial3D.new()
	paint.albedo_color = WALL
	box.material = paint
	var mesh := MeshInstance3D.new()
	mesh.mesh = box
	mesh.position.y = y
	root.add_child(mesh)
