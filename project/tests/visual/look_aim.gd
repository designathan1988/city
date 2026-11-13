## Where a look plan stands the camera: the player's own rig, at the plan's view of a point or of somebody (the walker nearest the map's
## middle, the clock stopped). The rig goes on living, so it keeps its tilt within the player's, takes its lens from the distance and
## keeps the eye out of the buildings and over the ground: a photo shows what a player standing there sees, and nothing a player cannot.
class_name LookAim
extends RefCounted

const WALKERS_READ := 4096
const RIGHT_ANGLE := 90.0 # a plan's bearing runs from +x towards +z, the rig's from +z towards +x
const STAND_FRAMES := 2 # the frames the world takes to ask for what the new view shows


## `view` is the setView's zoom, azimuth and elevation.
static func stand_on_walker(rig: CityCamera, ui: UiDriver, view: Array) -> void:
	CityStore.dispatch(&"set_speed", [0])
	var best := Vector3.INF
	for w: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ]).walkers:
		if not w.fallen and Vector2(w.position.x, w.position.z).length() < Vector2(best.x, best.z).length():
			best = w.position
	print("walker at ", best)
	await stand(rig, ui, best, view)


## setViewRoof [x, y, radius, zoom, azimuth, elevation]: the plan's view of the highest roof within `radius` metres of (x, y) (y is the map's
## north, -z), found by the core (CityCore.highest_roof): the ground under a town's centre is its streets, its roofs tens of metres over them.
## The rig's own point stays on the ground (the arm never lifts it onto a roof), so the view is held on the roof itself (CityCamera.pin):
## the eye where the rig's orbit at that zoom, bearing and tilt would stand round that point, the rig's lens for that zoom. False, said,
## with no roof there.
static func stand_on_roof(rig: CityCamera, ui: UiDriver, args: Array) -> bool:
	var roof: Dictionary = CityStore.query(&"highest_roof", [float(args[0]), -float(args[1]), float(args[2])])
	if not roof.get("hit", false):
		print("FAIL no roof within %s m of %s" % [args[2], [args[0], args[1]]])
		return false
	var at: Vector3 = roof.position
	var half := _half_of(rig, float(args[3]))
	var yaw := deg_to_rad(RIGHT_ANGLE - float(args[4]))
	var eye := at + Basis.from_euler(Vector3(-deg_to_rad(float(args[5])), yaw, 0.0)).z * CameraProfile.view_distance(rig.cfg, half)
	print("roof at ", at)
	CameraProfile.lens(rig.camera, rig.cfg, half)
	rig.pin(eye, at)
	await ui.frames(STAND_FRAMES)
	return true


## The half height of the ground in view for a zoom in screen pixels a metre, within the rig's.
static func _half_of(rig: CityCamera, zoom: float) -> float:
	return clampf(rig.get_viewport().get_visible_rect().size.y / (2.0 * zoom), rig.cfg.view_half_min, rig.cfg.view_half_max)


## A plan's view of the point `p`: `view` is the setView's zoom (screen pixels a metre at the point looked at), azimuth and elevation
## (degrees). The zoom is the half height of the ground in view there, which the rig's distance and lens follow from.
static func stand(rig: CityCamera, ui: UiDriver, p: Vector3, view: Array) -> void:
	var half := _half_of(rig, float(view[0]))
	rig.unpin() # a view pinned before (a pair's camera) goes back to the rig
	rig.jump(p, RIGHT_ANGLE - float(view[1]), float(view[2]), CameraProfile.view_distance(rig.cfg, half))
	await ui.frames(STAND_FRAMES)
