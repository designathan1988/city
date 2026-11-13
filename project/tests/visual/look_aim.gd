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


## A plan's view of the point `p`: `view` is the setView's zoom (screen pixels a metre at the point looked at), azimuth and elevation
## (degrees). The zoom is the half height of the ground in view there, which the rig's distance and lens follow from.
static func stand(rig: CityCamera, ui: UiDriver, p: Vector3, view: Array) -> void:
	var half := clampf(rig.get_viewport().get_visible_rect().size.y / (2.0 * float(view[0])), rig.cfg.view_half_min, rig.cfg.view_half_max)
	rig.unpin() # a view pinned before (a pair's camera) goes back to the rig
	rig.jump(p, RIGHT_ANGLE - float(view[1]), float(view[2]), CameraProfile.view_distance(rig.cfg, half))
	await ui.frames(STAND_FRAMES)
