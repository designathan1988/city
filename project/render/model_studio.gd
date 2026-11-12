## Session-owned model pictures. Requests have content identity, subscribers and cancellation tokens; only wanted views are captured.
class_name ModelStudio
extends SubViewport

signal pictured(key: String, picture: Texture2D)

const SHELF_PICTURE_PX := 128
const VIEW_FROM := Vector3(0.62, 0.5, 0.6)
const FRAME_MARGIN := 1.12
const CLOSEST_M := 1.0
const LIGHT_ANGLES_DEG := Vector3(-48.0, 35.0, 0.0)
const LIGHT_GAIN := 1.1
const AMBIENT_ENERGY := 0.55
const CAMERA_REACH_M := 600.0
const SETTLE_FRAMES := 3
const RGBA_BYTES := 4
const BYTES_PER_MIB := 1024 * 1024

var _pictures: Dictionary = {}
var _recent: Array[String] = []
var _queue: Array[Dictionary] = []
var _pending: Dictionary = {} # key -> request token, including the active request
var _failed: Dictionary = {}
var _failure_order: Array[String] = []
var _wanted: Dictionary = {} # shelf instance -> currently displayed picture keys
var _active: Dictionary = {}
var _serial := 0
var _handle := 0
var _busy := false
var _warming := true
var _generation := -1
var _camera: Camera3D


func _init() -> void:
	size = Vector2i(SHELF_PICTURE_PX, SHELF_PICTURE_PX)
	world_3d = World3D.new()
	transparent_bg = true
	render_target_update_mode = SubViewport.UPDATE_DISABLED


func _ready() -> void:
	_generation = CityStore.snapshot.get("city_generation", -1)
	CityStore.watch(self, ["city_generation"], _on_snapshot)
	_camera = Camera3D.new()
	_camera.projection = Camera3D.PROJECTION_ORTHOGONAL
	_camera.near = CLOSEST_M
	_camera.far = CAMERA_REACH_M * 2.0
	add_child(_camera)
	var sun := DirectionalLight3D.new()
	sun.rotation_degrees = LIGHT_ANGLES_DEG
	sun.light_energy = LIGHT_GAIN
	add_child(sun)
	var env := Environment.new()
	env.ambient_light_source = Environment.AMBIENT_SOURCE_COLOR
	env.ambient_light_color = Color.WHITE
	env.ambient_light_energy = AMBIENT_ENERGY
	_camera.environment = env
	for i in SETTLE_FRAMES:
		await get_tree().process_frame
	_warming = false
	_next()


func _exit_tree() -> void:
	_free_handle()
	_queue.clear()
	_pending.clear()
	_wanted.clear()
	_pictures.clear()
	_recent.clear()
	_failed.clear()
	_failure_order.clear()


func _on_snapshot(snapshot: Dictionary) -> void:
	var generation: int = snapshot.get("city_generation", -1)
	if generation == _generation:
		return
	_generation = generation
	_queue.clear()
	_pending.clear()
	if not _active.is_empty():
		_active.cancelled = true
	_free_handle() # publication already retired the old stage; handles themselves never recycle on clear
	render_target_update_mode = SubViewport.UPDATE_DISABLED


func peek(key: String) -> Texture2D:
	if not _pictures.has(key):
		return null
	_recent.erase(key)
	_recent.push_back(key)
	return _pictures[key]


## Consult before reading/encoding body bytes. Pending and failed requests also need no repeated work.
func needs(key: String) -> bool:
	return not _pictures.has(key) and not _pending.has(key) and not _failed.has(key)


func set_wanted(owner: int, keys: Array[String]) -> void:
	var wanted := {}
	for key in keys:
		wanted[key] = true
	_wanted[owner] = wanted
	_cancel_unwanted()


func release(owner: int) -> void:
	_wanted.erase(owner)
	_cancel_unwanted()
	if _wanted.is_empty():
		_failed.clear()
		_failure_order.clear()


func _is_wanted(key: String) -> bool:
	for wanted: Dictionary in _wanted.values():
		if wanted.has(key):
			return true
	return false


func _cancel_unwanted() -> void:
	var kept: Array[Dictionary] = []
	for job in _queue:
		if _current(job):
			kept.push_back(job)
		elif _pending.get(job.key) == job.token:
			_pending.erase(job.key)
	_queue = kept
	if not _active.is_empty() and not _is_wanted(_active.key):
		_active.cancelled = true
		if _pending.get(_active.key) == _active.token:
			_pending.erase(_active.key)
		render_target_update_mode = SubViewport.UPDATE_DISABLED
		_free_handle()


func invalidate(key: String) -> void:
	_pictures.erase(key)
	_recent.erase(key)
	_failed.erase(key)
	_failure_order.erase(key)
	_pending.erase(key) # an old active result may no longer publish under this identity
	if not _active.is_empty() and _active.key == key:
		_active.cancelled = true
		_free_handle()
	_cancel_unwanted()


func ask(key: String, bytes: PackedByteArray) -> void:
	if not _is_wanted(key) or not needs(key):
		return
	_serial += 1
	_pending[key] = _serial
	_queue.push_back({"key": key, "bytes": bytes, "token": _serial, "generation": _generation, "cancelled": false})
	_next()


func _current(job: Dictionary) -> bool:
	return not job.cancelled and job.generation == _generation and _pending.get(job.key) == job.token and _is_wanted(job.key)


func _next() -> void:
	if _warming or _busy:
		return
	while not _queue.is_empty():
		var job: Dictionary = _queue.pop_front()
		if not _current(job):
			if _pending.get(job.key) == job.token:
				_pending.erase(job.key)
			continue
		_busy = true
		_active = job
		_take.call_deferred(job)
		return


func _free_handle() -> void:
	if _handle != 0:
		CityStore.query(&"stage_free", [_handle])
		_handle = 0


func _finish(job: Dictionary) -> void:
	_free_handle()
	if _pending.get(job.key) == job.token:
		_pending.erase(job.key)
	_active = {}
	_busy = false
	_next()


func _capacity() -> int:
	var bytes := SHELF_PICTURE_PX * SHELF_PICTURE_PX * RGBA_BYTES
	return maxi(0, int(DataTables.ui("builder").model_picture_cache_mb * BYTES_PER_MIB / bytes))


func _keep(key: String, picture: Texture2D) -> void:
	var count := _capacity()
	_pictures.erase(key)
	_recent.erase(key)
	if count == 0:
		return
	while _recent.size() >= count:
		_pictures.erase(_recent.pop_front())
	_pictures[key] = picture
	_recent.push_back(key)


func _fail(key: String) -> void:
	var count := _capacity()
	if count == 0:
		return
	while _failure_order.size() >= count:
		_failed.erase(_failure_order.pop_front())
	_failed[key] = true
	_failure_order.push_back(key)


func _take(job: Dictionary) -> void:
	if not _current(job):
		_finish(job)
		return
	var prepared: RefCounted = CityStore.query(&"body_prepare", [job.bytes])
	var extent: Vector3 = CityStore.query(&"body_size", [prepared])
	_handle = CityStore.query(&"stage_show", [prepared, world_3d.scenario, false])
	if _handle == 0:
		_fail(job.key)
		_finish(job)
		return
	var middle := Vector3(0.0, extent.z / 2.0, -extent.y / 2.0)
	_camera.size = maxf(Vector2(extent.x, extent.y).length(), extent.z) * FRAME_MARGIN
	_camera.look_at_from_position(middle + VIEW_FROM.normalized() * CAMERA_REACH_M, middle)
	render_target_update_mode = SubViewport.UPDATE_ONCE
	await RenderingServer.frame_post_draw
	if _current(job):
		var picture := ImageTexture.create_from_image(get_texture().get_image())
		_keep(job.key, picture)
		pictured.emit(job.key, picture)
	_finish(job)
