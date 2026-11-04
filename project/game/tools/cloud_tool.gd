## Landscape › Sky › Clouds: puts a cloud in the sky, moves one, sets one to the tool's size, height and density, or
## takes one away. The wind carries the clouds (render/sky_view.gd drift): what is kept is where they are with the
## drift taken off.
extends ToolBase

enum Mode { ADD, MOVE, EDIT, REMOVE }

const PERCENT := 100.0
## The way a new cloud's puffs lie follows where it is put.
const YAW_PER_M_X := 0.013
const YAW_PER_M_Z := 0.007
const LEVEL_RAY := 1e-4

var mode := 0
var size := 0.0
var height := 0.0
var density := 0.0
var _held := -1
var _grip := Vector2.ZERO
var _spec: Dictionary


func enter() -> void:
	_spec = DataTables.catalog_table("nature").clouds
	var s: Dictionary = BrushDefaults.of("CLOUD", params)
	mode = s.mode
	size = s.size
	height = s.height
	density = s.density / PERCENT
	ctl.hint.emit(tr("hint.tool.cloud"), true)


func _ray() -> Array:
	var cam := ctl.get_viewport().get_camera_3d()
	var at := ctl.get_viewport().get_mouse_position()
	return [cam.project_ray_origin(at), cam.project_ray_normal(at)]


## Where the pointer's ray meets the level `y`.
func _at_height(y: float) -> Vector3:
	var ray := _ray()
	var o: Vector3 = ray[0]
	var d: Vector3 = ray[1]
	if absf(d.y) < LEVEL_RAY:
		return o
	return o + d * ((y - o.y) / d.y)


func _drift() -> Vector2:
	var sky := ctl.get_tree().get_first_node_in_group(&"sky")
	return sky.drift if sky else Vector2.ZERO


func _cloud(id: int) -> Dictionary:
	for c in CityStore.query(&"landscape").clouds:
		if c.id == id:
			return c
	return {}


func _like(x: float, z: float, yaw: float) -> Dictionary:
	return {"x": x, "z": z, "height": height, "size": size, "density": density, "yaw": yaw}


func click(_p: Vector3) -> void:
	var drift := _drift()
	if mode == Mode.ADD:
		var at := _at_height(height + size * _spec.grip_height)
		var r: Dictionary = CityStore.dispatch(&"add_cloud", [_like(at.x - drift.x, at.z - drift.y, fposmod(at.x * YAW_PER_M_X + at.z * YAW_PER_M_Z, TAU))])
		if not r.get("ok", false):
			ctl.hint.emit(refusal(r), false)
		return
	var ray := _ray()
	var id: int = CityStore.query(&"cloud_at", [ray[0], ray[1], drift.x, drift.y])
	if id < 0:
		return
	if mode == Mode.REMOVE:
		CityStore.dispatch(&"remove_cloud", [id])
	elif mode == Mode.EDIT:
		var c := _cloud(id)
		CityStore.dispatch(&"edit_cloud", [id, _like(c.x, c.z, c.yaw)])
	else:
		var c := _cloud(id)
		var at := _at_height(c.height + c.size * _spec.grip_height)
		_held = id
		_grip = Vector2(c.x + drift.x - at.x, c.z + drift.y - at.z)
		CityStore.dispatch(&"begin_stroke", ["cmd.cloud.edit"])


func drag(_p: Vector3) -> void:
	if _held < 0:
		return
	var c := _cloud(_held)
	if c.is_empty():
		return
	var at := _at_height(c.height + c.size * _spec.grip_height)
	var drift := _drift()
	c.x = at.x + _grip.x - drift.x
	c.z = at.z + _grip.y - drift.y
	CityStore.dispatch(&"edit_cloud", [_held, c])


func release(_p: Vector3) -> void:
	if _held >= 0:
		_held = -1
		CityStore.dispatch(&"end_stroke")


func exit() -> void:
	release(Vector3.INF)
