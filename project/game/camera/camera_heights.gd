## What stands under the view, read once a frame by the rig's settle: the height the view rests on (the ground under the point looked at)
## and how far the eye stands over its orbit to keep clear of the ground under it, where the buildings hold it (CameraSolids); each on its
## own critically damped spring, so no terrain step, road deck or kerb reaches the screen as one. What the player sees at a point is a road
## deck where one is, the ground elsewhere (the pointer's rule), the terrain where the ray finds nothing (off the map). Numbers:
## data/game/camera.json ground_smooth_s, ground_clearance, eye_rise_s, eye_fall_s, ray_top.
class_name CameraHeights
extends RefCounted

var cfg: Dictionary
## The ground under the eye, as last read.
var under_eye := -INF
var _queries: CameraQueries
var _ground := CameraDamp.new()
var _rise := CameraDamp.new()


func _init(config: Dictionary, queries: CameraQueries) -> void:
	cfg = config
	_queries = queries


## The height the view rests on now.
func ground() -> float:
	return _ground.x


## How far the eye stands over its orbit now.
func rise() -> float:
	return _rise.x


## What the player sees at (x, z), asked during a settle (CameraQueries).
func seen(x: float, z: float) -> float:
	var hit: Dictionary = _queries.ask(&"raycast", [Vector3(x, cfg.ray_top, z), Vector3.DOWN])
	return hit.position.y if hit.get("hit", false) else _queries.ask(&"height_at", [x, z])


## The view's rest under the point looked at `focus`, `dt` on (0: at once, as a jump).
func rest_on(focus: Vector3, dt: float) -> void:
	var h := seen(focus.x, focus.z)
	if dt > 0.0:
		_ground.to(h, dt, cfg.ground_smooth_s)
	else:
		_ground.snap(h)


## The eye's rise for the eye at `eye` before it: up quickly, down slowly, `dt` on (0: at once).
func clear(eye: Vector3, dt: float) -> void:
	under_eye = seen(eye.x, eye.z)
	var r := maxf(0.0, under_eye + cfg.ground_clearance - eye.y)
	if dt > 0.0:
		_rise.to(r, dt, cfg.eye_rise_s if r > _rise.x else cfg.eye_fall_s)
	else:
		_rise.snap(r)


## The terrain's height under a camera's eye: the one reading of it the daylight's shadows and the weather's rain take.
static func below(cam: Camera3D) -> float:
	return CityStore.query(&"height_at", [cam.global_position.x, cam.global_position.z])
