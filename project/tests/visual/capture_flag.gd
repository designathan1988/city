## A lantern with its flag on the tallest building around the road, for the feature captures (capture_features.gd): the cloth
## downwind of the map's wind, seen from above its neighbours at the Roadcraft's day hour (16h), the mast in the middle of the frame.
class_name CaptureFlag
extends RefCounted

const PartCases := preload("res://tests/core/building_part_cases.gd")
const AROUND_M := 400.0
const VIEW := [35.0, 40.0, 90.0] # yaw, pitch, distance: over the roofs around
const LANTERN_H := 19.4
const HOLD_FRAMES := 90


## `cap` is the capture scene (its rig, world, ui and _check); `near` the point the building is looked for around.
static func shot(cap: Node, near: Vector3) -> void:
	var id := _tallest_around(near)
	cap._check(id >= 0, "um prédio alto para o farol")
	if id < 0:
		return
	PartCases.fly_flag(id, cap._check)
	cap.world.set_sky_mode("day")
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var info: Dictionary = CityStore.query(&"body_info", [id])
	var top: Vector3 = p.position + Vector3(0, info.ground_h + info.storey_h * (info.volumes[0].storeys - 1) + LANTERN_H * 0.5, 0)
	cap.rig.jump(p.position, VIEW[0], VIEW[1], VIEW[2])
	cap.rig.jump(_ground_focus_for(cap.rig, top), VIEW[0], VIEW[1], VIEW[2])
	await cap.ui.drawn()
	await cap.ui.frames(HOLD_FRAMES)
	await cap.ui.shot("feat_build_bandeira")


## The rig orbits a ground point: the one on its line of sight through `top`, with the tilt the rig settled on.
static func _ground_focus_for(rig: CityCamera, top: Vector3) -> Vector3:
	var d: Vector3 = Basis.from_euler(Vector3(-rig.pitch, rig.yaw, 0.0)).z
	var lift: float = top.y - rig.focus.y
	return top - d * (lift / d.y)


static func _tallest_around(near: Vector3) -> int:
	var id := -1
	var most := 0
	for b: int in CityStore.query(&"list_buildings"):
		var p: Dictionary = CityStore.query(&"probe_building", [b])
		if p.position.distance_to(near) > AROUND_M:
			continue
		var volumes: Array = CityStore.query(&"body_info", [b]).volumes
		if volumes.is_empty() or volumes[0].base != 0 or volumes[0].storeys <= most:
			continue
		most = volumes[0].storeys
		id = b
	return id
