## The pictures of the Models shelf: each body drawn alone in a small world of its own and photographed, one per frame and only while
## the shelf asks. Pictures are kept for the session, so reopening the shelf costs nothing.
class_name ModelStudio
extends SubViewport

signal pictured(key: String, picture: Texture2D)

static var SHELF_PICTURE_PX: int = int(DataTables.game("tools").model_studio.picture_px) # data/game/tools.json model_studio
const VIEW_FROM := Vector3(0.62, 0.5, 0.6) # front right and above: the lot's front looks towards +z
static var FRAME_MARGIN: float = DataTables.game("tools").model_studio.frame_margin
const CLOSEST_M := 1.0
static var LIGHT_ANGLES_DEG := Vector3(DataTables.game("tools").model_studio.light_angles_deg[0], DataTables.game("tools").model_studio.light_angles_deg[1], DataTables.game("tools").model_studio.light_angles_deg[2])
static var LIGHT_GAIN: float = DataTables.game("tools").model_studio.light_gain
static var AMBIENT_ENERGY: float = DataTables.game("tools").model_studio.ambient_energy
static var CAMERA_REACH_M: float = DataTables.game("tools").model_studio.camera_reach_m
const SETTLE_FRAMES := 3 # a viewport new to the tree draws nothing (measured: empty at once, right after three frames)

static var pictures := {}

var _queue: Array[Dictionary] = []
var _asked := {} # the keys queued or being photographed: asked again, they wait for the same picture
var _busy := true
var _camera: Camera3D


func _init() -> void:
	size = Vector2i(SHELF_PICTURE_PX, SHELF_PICTURE_PX)
	world_3d = World3D.new()
	transparent_bg = true
	render_target_update_mode = SubViewport.UPDATE_DISABLED


func _ready() -> void:
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
	_next()


## The picture comes through `pictured`, or at once from `pictures` when it was taken before. `bytes` gives the body's bytes
## (a Callable returning a PackedByteArray): called only when the picture is taken, so a picture already kept or already asked
## for reads nothing.
func ask(key: String, bytes: Callable) -> void:
	if pictures.has(key):
		pictured.emit(key, pictures[key])
		return
	if _asked.has(key):
		return
	_asked[key] = true
	_queue.append({"key": key, "bytes": bytes})
	if not _busy:
		_next()


func _next() -> void:
	_busy = not _queue.is_empty()
	if _busy:
		_take.call_deferred(_queue.pop_front())


func _take(job: Dictionary) -> void:
	var bytes: PackedByteArray = job.bytes.call()
	var extent: Vector3 = CityStore.query(&"body_size", [bytes])
	var handle: int = CityStore.query(&"stage_show", [bytes, world_3d.scenario, false])
	var middle := Vector3(0.0, extent.z / 2.0, -extent.y / 2.0)
	_camera.size = maxf(Vector2(extent.x, extent.y).length(), extent.z) * FRAME_MARGIN
	_camera.look_at_from_position(middle + VIEW_FROM.normalized() * CAMERA_REACH_M, middle)
	render_target_update_mode = SubViewport.UPDATE_ONCE
	await RenderingServer.frame_post_draw
	var picture := ImageTexture.create_from_image(get_texture().get_image())
	CityStore.query(&"stage_free", [handle])
	pictures[job.key] = picture
	_asked.erase(job.key)
	pictured.emit(job.key, picture)
	_next()
