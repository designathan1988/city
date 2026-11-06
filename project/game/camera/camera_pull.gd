## The pull in front of a building: down in the street the eye never stands behind one. The core says how much of the line of sight to take
## off (query sight_pull); the eye moves up that line after a moment and goes back slowly once it is free (Roadcraft
## isoViewport settle, after Cinemachine's Deoccluder: Minimum Occlusion Time, Damping When Occluded, Smoothing Time and Damping).
## Numbers: data/game/camera.json pull_*.
class_name CameraPull
extends RefCounted

const SETTLED := 1e-4 # a share of the line of sight this small is no move

var cfg: Dictionary
var wanted := 0.0 # the share of the line taken off to see clear
var shown := 0.0
var _blocked := 0.0
var _hold := 0.0


func _init(config: Dictionary) -> void:
	cfg = config


## The eye for this view: `eye` drawn towards `focus` by what the buildings ask, at this zoom's weight.
func eye(focus: Vector3, eye_at: Vector3, half: float) -> Vector3:
	var weight := 1.0 - CameraProfile.blend(half, cfg.pull_full_half, cfg.pull_none_half)
	if weight <= 0.0:
		wanted = 0.0
		shown = 0.0
		return eye_at
	wanted = float(CityStore.query(&"sight_pull", [focus, eye_at, int(cfg.pull_samples), cfg.pull_soft_m])) * weight
	return eye_at.lerp(focus, shown)


## One frame of the move; true when the eye moved.
func settle(dt: float) -> bool:
	var gap := wanted - shown
	if absf(gap) < SETTLED:
		_blocked = 0.0
		var moved := gap != 0.0
		shown = wanted
		return moved
	if gap > 0.0:
		_blocked += dt
		if _blocked < cfg.pull_wait_s:
			return false
		shown += gap * (1.0 - exp(-dt / cfg.pull_in_s))
		_hold = cfg.pull_hold_s
		return true
	_blocked = 0.0
	if _hold > 0.0:
		_hold -= dt
		return false
	shown += gap * (1.0 - exp(-dt / cfg.pull_out_s))
	return true
