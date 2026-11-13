## The eye among the buildings: what the buildings' rules ask of the orbit's eye (CityCore.eye_free: never inside a building, out of a
## wall's margin, the point raised only when the eye and it are both inside), eased so it never reaches the screen as a step, the walls
## themselves kept at once, and the near plane short of the nearest building. The margin grows with the view so the eased eye never
## trails a frame's move past it. Superseded by specification 003's research R1b (task T027: the camera floor, the eye's body, the pull).
## Numbers: data/game/camera.json eye_*, near_*.
class_name CameraWalls
extends RefCounted

const STILL := -1.0 # a placing between frames (a gesture placing the view again): the springs hold

var cfg: Dictionary
## How far the walls moved the eased eye at once in the last placing, metres: 0 unless the eased correction trailed into a wall.
var snapped := 0.0
var _queries: CameraQueries
var _shift: Array[CameraDamp] = [CameraDamp.new(), CameraDamp.new(), CameraDamp.new()] # the eye's correction, metres (x, y, z)
var _lift := CameraDamp.new() # the point's rise over a building the eye and it both stand in, metres


func _init(config: Dictionary, queries: CameraQueries) -> void:
	cfg = config
	_queries = queries


## The buildings' room for the view at `half`: (how far round a building its floor eases down to its foot, the clearance over a roof, how far
## the eye moves along its line out of a margin), the margin at least `eye_margin_share` of the half height.
func room(half: float) -> Vector3:
	var scale := maxf(1.0, half * cfg.eye_margin_share / cfg.eye_wall_reach_m)
	return Vector3(cfg.eye_wall_reach_m * scale, cfg.eye_roof_clearance_m, cfg.eye_step_back_m * scale)


## The eye's correction now, metres: where the rig reads the ground under the eye.
func shift() -> Vector3:
	return Vector3(_shift[0].x, _shift[1].x, _shift[2].x)


## The point's rise now, metres.
func lift() -> float:
	return _lift.x


## [the point looked at, the eye] for the orbit's `focus` and `eye` at the zoom `half`, `eye_lift` the eye's lift over the ground. `step` is
## the frame's time (0 takes the correction at once, as a jump does; STILL holds it). Where the eased eye would stand in a wall it is taken
## out at once and the springs go on from there.
func place(focus: Vector3, eye: Vector3, eye_lift: float, half: float, step: float) -> PackedVector3Array:
	var r := room(half)
	var free: PackedVector3Array = _queries.ask(&"eye_free", [focus, eye, eye_lift, r])
	var at := focus + Vector3.UP * _ease(_lift, free[0].y - focus.y, step)
	var moved := free[1] - eye
	var pos := eye + Vector3(_ease(_shift[0], moved.x, step), _ease(_shift[1], moved.y, step), _ease(_shift[2], moved.z, step))
	var walls: PackedVector3Array = _queries.ask(&"eye_free", [at, pos, eye_lift, Vector3(r.x, r.y, 0.0)])
	snapped = walls[1].distance_to(pos)
	if snapped > 0.0 or walls[0] != at:
		_lift.snap(walls[0].y - focus.y)
		var held := walls[1] - eye
		for i in 3:
			_shift[i].snap(held[i])
		return walls
	return PackedVector3Array([at, pos])


## The near plane for the eye at `pos` looking at `at`: a share of the distance, and short of the nearest building's floor under the eye.
func near(pos: Vector3, at: Vector3, half: float) -> float:
	var gap: float = pos.y - float(_queries.ask(&"eye_floor", [pos.x, pos.z, room(half)]))
	var short := minf(pos.distance_to(at) * cfg.near_per_m, (gap + cfg.near_gap_pad_m) * cfg.near_gap_share)
	return clampf(short, cfg.near_min, cfg.near_max)


func _ease(spring: CameraDamp, target: float, step: float) -> float:
	if step < 0.0:
		return spring.x
	if step == 0.0:
		spring.snap(target)
		return target
	return spring.ease_to(target, step, cfg.eye_correct_s)
