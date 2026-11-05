## A lantern with its flag on a tall building near the road, for the feature captures (capture_features.gd): the cloth downwind
## of the map's wind at the Roadcraft's day hour (16h). The camera is framed as a player would turn it: a bearing and an aim
## from which its line of sight is clear of the buildings (the pull in front of a building, camera_pull.gd, would otherwise
## bring the eye to a facade), with the flag inside the frame. Aiming at the mast's top makes that line run down through the
## building behind it, so the aim is raised over the top by the least that clears the roof.
class_name CaptureFlag
extends RefCounted

const PartCases := preload("res://tests/core/building_part_cases.gd")
const AROUND_M := 400.0
const CANDIDATES := 6 # the tallest buildings around tried, tallest first
const VIEW := [35.0, 40.0, 90.0] # yaw, pitch, and how far the eye stands beyond the aim: over the roofs around
const FRAME_PASSES := 4 # the rig's tilt and lens follow its distance: the pose is solved again a few times
const BEARINGS := 12
const AIM_STEP_M := 3.0
const AIM_STEPS := 10
const CLEAR := 0.999 # share of the line of sight free of buildings: the pull takes nothing off it
const SAMPLES := 48
const IN_FRAME := 0.8 # of the half lens: the flag well inside the picture
const LANTERN_H := 19.4
const HOLD_FRAMES := 90


## `cap` is the capture scene (its rig, world, ui and _check); `near` the point the buildings are looked for around.
static func shot(cap: Node, near: Vector3) -> void:
	cap.world.set_sky_mode("day")
	var pose := _clear_pose(cap.rig, near)
	cap._check(not pose.is_empty(), "um prédio alto com o farol à vista, sem prédio na frente")
	if pose.is_empty():
		return
	PartCases.fly_flag(pose.id, cap._check)
	cap.rig.jump(pose.focus, pose.yaw, VIEW[1], pose.distance)
	await cap.ui.drawn()
	await cap.ui.frames(HOLD_FRAMES)
	cap._check(_in_frame(cap.rig.camera, pose.top), "a bandeira dentro da foto")
	await cap.ui.shot("feat_build_bandeira")


## The first building, bearing and aim (tallest buildings first, the least lift first) whose line of sight is clear; {} if none.
static func _clear_pose(rig: CityCamera, near: Vector3) -> Dictionary:
	for id: int in _tallest_around(near):
		var top := _mast_top(id)
		for b in BEARINGS:
			var yaw: float = VIEW[0] + 360.0 * b / BEARINGS
			for step in AIM_STEPS:
				var aim := top + Vector3.UP * AIM_STEP_M * step
				var focus := _frame(rig, aim, yaw)
				var eye: Vector3 = rig.focus + Basis.from_euler(Vector3(-rig.pitch, rig.yaw, 0.0)).z * rig.distance
				if float(CityStore.query(&"clear_reach", [rig.focus, eye, SAMPLES])) < CLEAR:
					continue
				if _sees(eye, rig, top):
					return {"id": id, "yaw": yaw, "focus": focus, "top": top, "distance": rig.distance}
	return {}


## The rig on the ground point whose line of sight runs through `aim`, its eye VIEW[2] beyond it: the point is where that line
## meets the ground, so the eye stands as far from it as the aim plus VIEW[2] (nearer, it would stand below the aim, the
## flag behind it). The tilt and the lens move with the distance, so the pose is solved FRAME_PASSES times. Returns the point.
static func _frame(rig: CityCamera, aim: Vector3, yaw: float) -> Vector3:
	var dist: float = VIEW[2]
	var focus := aim
	for k in FRAME_PASSES:
		rig.jump(focus, yaw, VIEW[1], dist)
		var d: Vector3 = Basis.from_euler(Vector3(-rig.pitch, rig.yaw, 0.0)).z
		var along: float = (aim.y - rig.focus.y) / d.y
		focus = aim - d * along
		dist = along + VIEW[2]
	rig.jump(focus, yaw, VIEW[1], dist)
	return focus


## The top of the lantern's mast the building will carry (body_info: its first volume's roof).
static func _mast_top(id: int) -> Vector3:
	var p: Dictionary = CityStore.query(&"probe_building", [id])
	var info: Dictionary = CityStore.query(&"body_info", [id])
	return p.position + Vector3(0, info.ground_h + info.storey_h * (info.volumes[0].storeys - 1) + LANTERN_H * 0.5, 0)


## Whether a point stands inside the frame of an eye at `eye` looking along the rig's view (vertical lens: KEEP_HEIGHT).
static func _sees(eye: Vector3, rig: CityCamera, at: Vector3) -> bool:
	var basis := Basis.from_euler(Vector3(-rig.pitch, rig.yaw, 0.0))
	var local := basis.inverse() * (at - eye)
	if local.z >= 0.0:
		return false
	var tan_v := tan(deg_to_rad(rig.camera.fov) * 0.5) * IN_FRAME
	var size: Vector2 = rig.get_viewport().get_visible_rect().size
	return absf(local.y / -local.z) < tan_v and absf(local.x / -local.z) < tan_v * size.x / size.y


static func _in_frame(camera: Camera3D, at: Vector3) -> bool:
	return not camera.is_position_behind(at) and camera.get_viewport().get_visible_rect().has_point(camera.unproject_position(at))



static func _tallest_around(near: Vector3) -> Array[int]:
	var found: Array = []
	for b: int in CityStore.query(&"list_buildings"):
		var p: Dictionary = CityStore.query(&"probe_building", [b])
		if p.position.distance_to(near) > AROUND_M:
			continue
		var volumes: Array = CityStore.query(&"body_info", [b]).volumes
		if volumes.is_empty() or volumes[0].base != 0:
			continue
		found.append([volumes[0].storeys, b])
	found.sort_custom(func(x: Array, y: Array) -> bool: return x[0] > y[0])
	var out: Array[int] = []
	for f in found.slice(0, CANDIDATES):
		out.append(f[1])
	return out
