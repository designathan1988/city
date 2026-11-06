## The 3D reference: a model written to a .glb and read back, then read into a building: two blocks, the tower on the base, the colours of their walls, undone in one step.
## Run: godot --path project res://tests/core/reference_test.tscn -- --no-flow
extends Node3D

const FILE := "user://reference_test.glb"
const TOWN_MAP := 0
const SEED := 1
const BASE := Vector3(24, 6, 18)
const TOWER := Vector3(12, 24, 10)
const WALL := Color(0.8, 0.5, 0.3)
const AT := Vector2(-200, 300)
const COLOUR_OFF := 0.02

var failures := 0


func _ready() -> void:
	var world := CityWorld.new()
	add_child(world)
	var rig := CityCamera.new()
	add_child(rig)
	_check(CityStore.boot(world.terrain, world.materials()).is_empty(), "boot")
	CityStore.new_map(TOWN_MAP, SEED, false, true)
	CityStore.dispatch(&"set_speed", [0])
	await get_tree().process_frame
	_write_model()
	var model := ReferenceModel.new()
	add_child(model)
	_check(not model.load_file("user://nothing_here.glb"), "arquivo que não existe é recusado")
	_check(model.load_file(FILE), "o .glb abre")
	_check(is_equal_approx(model.size_m.y, BASE.y + TOWER.y), "altura do modelo: %s" % model.size_m)
	model.place_on(AT.x, AT.y, 0.0, 0.0)
	var read: Dictionary = await model.capture()
	_check(read.views.size() == 5, "cinco fotos")
	_check(not read.triangles.is_empty(), "%d pontos de triângulo" % read.triangles.size())
	var r: Dictionary = CityStore.dispatch(&"build_reference", [read.triangles, read.views, AT.x, AT.y, 0.0, -1])
	_check(r.get("ok", false), "virou prédio: %s" % r)
	if r.get("ok", false):
		var info: Dictionary = CityStore.query(&"body_info", [r.placed])
		_check(info.volumes.size() == 2, "dois blocos: %d" % info.volumes.size())
		_check(info.volumes[1].base > 0, "a torre fica sobre a base: %s" % info.volumes[1])
		var paint: Dictionary = CityStore.query(&"body_paint_at", [r.placed, 0, 0, 0])
		var seen := Color(paint.rgb)
		_check(absf(seen.r - WALL.r) < COLOUR_OFF and absf(seen.g - WALL.g) < COLOUR_OFF and absf(seen.b - WALL.b) < COLOUR_OFF, "a parede tem a cor do modelo: %s" % paint)
		_check(CityStore.dispatch(&"undo") and CityStore.query(&"body_info", [r.placed]).is_empty(), "um passo de desfazer")
	DirAccess.remove_absolute(ProjectSettings.globalize_path(FILE))
	print("FALHAS: ", failures)
	get_tree().quit(1 if failures > 0 else 0)


## Two boxes on the origin, the one on top of the other, as a glTF file.
func _write_model() -> void:
	var root := Node3D.new()
	_box(root, BASE, BASE.y / 2.0)
	_box(root, TOWER, BASE.y + TOWER.y / 2.0)
	var doc := GLTFDocument.new()
	var state := GLTFState.new()
	_check(doc.append_from_scene(root, state) == OK, "cena para glTF")
	_check(doc.write_to_filesystem(state, FILE) == OK, "o .glb foi escrito")
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


func _check(ok: bool, what: String) -> void:
	print(("ok   " if ok else "FAIL ") + what)
	if not ok:
		failures += 1
