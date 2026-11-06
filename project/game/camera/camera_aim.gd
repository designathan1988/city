## Whether the pointer can hold a height: how far the ray may run before the ground it grabs is past the view.
class_name CameraAim
extends RefCounted


## The plane a zoom or turn holds: the ground's own if the ray meets it within `hold_reach` view distances, else
## the height the ray has reached there (a pointer over the horizon holds a point in the air, not one at infinity).
static func hold_height(rig: CityCamera, screen: Vector2, h: float) -> float:
	var o := rig.camera.project_ray_origin(screen)
	var d := rig.camera.project_ray_normal(screen)
	var limit: float = rig.cfg.hold_reach * rig.camera.global_position.distance_to(rig.focus)
	if absf(d.y) >= CityCamera.PARALLEL:
		var t := (h - o.y) / d.y
		if t > 0.0 and t <= limit:
			return h
	return o.y + d.y * limit


## How far a change may bring the held point back. A ray grazing the plane meets it ever farther for ever less of a turn, so the hold lets go between
## `keep_firm_deg` and `keep_grazing_deg` of the ray over the plane and under that the view turns about its centre: what `swing` moved the ray's own point
## over the sine of the shallowest hold, and the share of the way the hold still has.
static func firm_reach(rig: CityCamera, screen: Vector2, point: Vector3, h: float, swing: float) -> float:
	var found: Variant = rig.hit_at(screen, h)
	if found == null:
		return 0.0
	var loose := sin(deg_to_rad(rig.cfg.keep_grazing_deg))
	var share := smoothstep(loose, sin(deg_to_rad(rig.cfg.keep_firm_deg)), -rig.camera.project_ray_normal(screen).y)
	if share >= 1.0:
		return INF
	return minf(swing / loose, share * Vector2(point.x - found.x, point.z - found.z).length())


## Moves the point looked at until `point` (on the plane at `h`) is under `screen`, by at most `most` metres.
static func bring(rig: CityCamera, screen: Vector2, point: Vector3, h: float, most := INF) -> void:
	for pass_i in int(rig.cfg.keep_passes):
		var found: Variant = rig.hit_at(screen, h)
		if found == null:
			return
		var d: Vector3 = point - found
		if absf(d.x) + absf(d.z) < rig.cfg.keep_tolerance_cm * CityCamera.CM:
			return
		var step := Vector2(d.x, d.z).limit_length(most)
		most -= step.length()
		rig.focus.x += step.x
		rig.focus.z += step.y
		rig._apply()
		if most <= CityCamera.PARALLEL:
			return


## Whether a pan can grab the ground under the pointer (else it slides by the drag).
static func holds(rig: CityCamera, screen: Vector2, h: float) -> bool:
	return is_equal_approx(hold_height(rig, screen, h), h)
