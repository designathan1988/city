## The camera on somebody for a shot in a look plan: the walker nearest the map's middle, the clock stopped, so the same plan aims at
## a body in both games (the 4.0's plan picks the same with `sim.pedViews`).
class_name LookAim
extends RefCounted

const WALKERS_READ := 4096


## `view` is the setView's zoom, azimuth and elevation.
static func stand_on_walker(rig: CityCamera, ui: UiDriver, view: Array) -> void:
	CityStore.dispatch(&"set_speed", [0])
	var best := Vector3.INF
	for w: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ]).walkers:
		if not w.fallen and Vector2(w.position.x, w.position.z).length() < Vector2(best.x, best.z).length():
			best = w.position
	print("walker at ", best)
	await PairCamera.stand(rig, ui, best, view[1], view[2], view[0])
