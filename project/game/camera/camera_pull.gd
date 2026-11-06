## The pull in front of a building: down in the street the eye never stands behind one. The core says how much of the line of sight to take
## off (query sight_pull); the eye moves up that line after a moment and goes back slowly once it is free (Roadcraft isoViewport settle,
## after Cinemachine's Deoccluder: Minimum Occlusion Time, Damping When Occluded, Smoothing Time and Damping), on a critically damped
## spring, so it never starts or stops at a jerk. Numbers: data/game/camera.json pull_*.
class_name CameraPull
extends RefCounted

const SETTLED := 1e-4 # a share of the line of sight this small is no move

var cfg: Dictionary
var wanted := 0.0 # the share of the line taken off to see clear
var _share := CameraDamp.new()
var _blocked := 0.0
var _hold := 0.0


func _init(config: Dictionary) -> void:
	cfg = config


## The share of the line taken off now.
func shown() -> float:
	return _share.x


## What the buildings ask for this view, at this zoom's weight.
func ask(focus: Vector3, eye_at: Vector3, half: float) -> void:
	var weight := 1.0 - CameraProfile.blend(half, cfg.pull_full_half, cfg.pull_none_half)
	wanted = float(CityStore.query(&"sight_pull", [focus, eye_at, int(cfg.pull_samples), cfg.pull_soft_m])) * weight if weight > 0.0 else 0.0


func snap() -> void:
	_share.snap(wanted)
	_blocked = 0.0
	_hold = 0.0


## One frame of the move.
func settle(dt: float) -> void:
	var gap := wanted - _share.x
	if absf(gap) < SETTLED and absf(_share.v) < SETTLED:
		_blocked = 0.0
		_share.snap(wanted)
	elif gap > 0.0:
		_blocked += dt
		_hold = cfg.pull_hold_s
		_share.to(wanted if _blocked >= cfg.pull_wait_s else _share.x, dt, cfg.pull_in_s)
	else:
		_blocked = 0.0
		_hold -= dt
		_share.to(wanted if _hold <= 0.0 else _share.x, dt, cfg.pull_out_s)
