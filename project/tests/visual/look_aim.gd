## The camera on somebody or something for a shot in a look plan: the walker or the vehicle nearest the map's middle, the clock
## stopped, so the same plan aims at the same body every run.
class_name LookAim
extends RefCounted

const WALKERS_READ := 4096


## `view` is the setView's zoom, azimuth and elevation.
static func stand_on_walker(rig: CityCamera, ui: UiDriver, view: Array) -> void:
	await _stand_on(rig, ui, view, &"walkers")


## The vehicle nearest the map's middle, moving or stopped.
static func stand_on_vehicle(rig: CityCamera, ui: UiDriver, view: Array) -> void:
	await _stand_on(rig, ui, view, &"vehicles")


static func _stand_on(rig: CityCamera, ui: UiDriver, view: Array, kind: StringName) -> void:
	CityStore.dispatch(&"set_speed", [0])
	var best := Vector3.INF
	for w: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ])[kind]:
		if not w.get("fallen", false) and Vector2(w.position.x, w.position.z).length() < Vector2(best.x, best.z).length():
			best = w.position
	print(kind, " at ", best)
	await PairCamera.stand(rig, ui, best, view[1], view[2], view[0])
