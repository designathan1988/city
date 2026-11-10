## The pull in front of a building: down in the street the eye never stands behind one. The core says how much of the line of sight to take
## off (query sight_pull); the eye moves up that line after a moment and goes back slowly once it is free (Roadcraft isoViewport settle,
## after Cinemachine's Deoccluder: Minimum Occlusion Time, Damping When Occluded, Smoothing Time and Damping). What is asked jumps (a
## tower comes into the line, the wait ends), so the share moves on two springs in a row (CameraDamp.ease_to): one alone takes its
## whole acceleration in the first frame, the taller the building the harder, and that was a jolt of the eye. Numbers:
## data/game/camera.json pull_*.
class_name CameraPull
extends RefCounted

const SETTLED := 1e-4 # a share of the line of sight this small is no move

var cfg: Dictionary
var _queries: CameraQueries
var wanted := 0.0 # the share of the line taken off to see clear
var _share := CameraDamp.new()
var _blocked := 0.0
var _hold := 0.0


func _init(config: Dictionary, queries: CameraQueries) -> void:
	cfg = config
	_queries = queries


## The share of the line taken off now.
func shown() -> float:
	return _share.x


## What the buildings ask for this view, at this zoom's weight.
func ask(focus: Vector3, eye_at: Vector3, half: float) -> void:
	var weight := 1.0 - CameraProfile.blend(half, cfg.pull_full_half, cfg.pull_none_half)
	wanted = float(_queries.ask(&"sight_pull", [focus, eye_at, int(cfg.pull_samples), cfg.pull_soft_m])) * weight if weight > 0.0 else 0.0


## What is left of the line of sight when another share of it is taken off too (the way out of a building, CameraAhead.out, on
## springs of its own): one after the other, the second a share of what the first left. The eye is at least as near as either asks
## and the sum moves as smoothly as they do: the larger of the two turned a corner, a jolt, wherever one overtook the other.
func left_with(other_share: float) -> float:
	return (1.0 - _share.x) * (1.0 - other_share)


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
		_share.ease_to(wanted if _blocked >= cfg.pull_wait_s else _share.x, dt, cfg.pull_in_s)
	else:
		_blocked = 0.0
		_hold -= dt
		_share.ease_to(wanted if _hold <= 0.0 else _share.x, dt, cfg.pull_out_s)
