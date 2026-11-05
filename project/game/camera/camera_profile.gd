## The camera's shape by distance: how the perspective rig looks at the town as it
## comes closer. Everything is a function of `half`, the half height of the ground in view at the point looked at
## (metres): the lens widens, the tilt eases towards a pedestrian's view and the point looked at rises to eye height.
## Values come from data/game/camera.json; angles there are degrees, here radians.
class_name CameraProfile
extends RefCounted

## The least `half` the logarithm reads, as a share of `near`.
const LOG_FLOOR := 1e-6


## 0 at `near` and below, 1 at `far` and above, smooth in between, on the logarithm of `half`.
static func blend(half: float, near: float, far: float) -> float:
	var t := clampf(log(maxf(half, near * LOG_FLOOR) / near) / log(far / near), 0.0, 1.0)
	return t * t * (3.0 - 2.0 * t)


## The tilt the camera takes by itself at this distance (the player's own tilt is added to it).
static func profile_tilt(cfg: Dictionary, half: float) -> float:
	return lerpf(deg_to_rad(cfg.near_tilt_deg), deg_to_rad(cfg.far_tilt_deg),
			blend(half, cfg.profile_near_half, cfg.tilt_far_half))


## The lowest tilt at this distance: below zero close up (looking up at a facade), never the horizon far away.
static func min_tilt(cfg: Dictionary, half: float) -> float:
	return lerpf(deg_to_rad(cfg.near_min_tilt_deg), deg_to_rad(cfg.far_min_tilt_deg),
			blend(half, cfg.min_tilt_near_half, cfg.min_tilt_far_half))


## How far over the ground the point looked at stands: eye height in the street, the ground far away.
static func eye_lift(cfg: Dictionary, half: float) -> float:
	return cfg.eye_height * (1.0 - blend(half, cfg.profile_near_half, cfg.eye_lift_far_half))


## Vertical field of view, degrees: long far away, wider in the street.
static func fov_deg(cfg: Dictionary, half: float) -> float:
	return lerpf(cfg.near_fov_deg, cfg.far_fov_deg, blend(half, cfg.profile_near_half, cfg.fov_far_half))


## The camera's distance from the point it looks at, for the view to be `half` tall there.
static func view_distance(cfg: Dictionary, half: float) -> float:
	return half / tan(deg_to_rad(fov_deg(cfg, half)) * 0.5)


## The `half` whose view stands `distance` from its point (tests and framing jumps): the lens depends on `half`,
## so the fixed point is iterated.
static func half_for_distance(cfg: Dictionary, distance: float) -> float:
	var half := distance * tan(deg_to_rad(cfg.far_fov_deg) * 0.5)
	for i in int(cfg.view_half_passes):
		half = distance * tan(deg_to_rad(fov_deg(cfg, half)) * 0.5)
	return clampf(half, cfg.view_half_min, cfg.view_half_max)
