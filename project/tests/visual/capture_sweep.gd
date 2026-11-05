## The sweep of the visual capture (`--only=varredura`): the town seen from many angles, distances and along a line, to judge how
## everything behaves together. Where the camera stands is defined here, never found in the photo: the targets come from the
## game (the probe's busiest crossing, the middle and the east rim of the built area, the first school), and each is taken from
## fixed azimuths (the orbit), three distances (the zoom) and two tilts; then a pan along a straight line through the town.
class_name CaptureSweep
extends RefCounted

const PREFIX := "30_"
const PITCH_DEG := 35.0
const LOW_DEG := 15.0
const HIGH_DEG := 80.0
const ORBIT_DEG := [45.0, 135.0, 225.0, 315.0]
const ZOOM_YAW_DEG := 45.0
const SCHOOL := 1 # BuildingFn escola (sim/buildings/building_store.h), as capture.gd's CIVIC_SHOTS
## Per target the height looked at (metres over the ground) and the distances far, mid and near.
const TARGETS := {
	"cruzamento": {"look_m": 0.0, "from_m": [160.0, 60.0, 20.0]},
	"miolo": {"look_m": 30.0, "from_m": [320.0, 140.0, 60.0]},
	"borda": {"look_m": 2.0, "from_m": [160.0, 60.0, 25.0]},
	"escola": {"look_m": 4.0, "from_m": [120.0, 45.0, 18.0]},
}
const FAR := 0
const MID := 1
const NEAR := 2
## The pan: stops on a line along x through the middle of the built area, a share of its width, looked at from the same side.
const PAN_STOPS := 6
const PAN_SPAN_SHARE := 0.8
const PAN_DIST_M := 140.0
const PAN_LOOK_M := 10.0
const PAN_YAW_DEG := 0.0


## `cap` is the capture scene (its _shot and _ground).
static func shots(cap: Node) -> void:
	await CaptureSignals.at_hour(cap, CaptureViews.DAY)
	var places := _places()
	for name: String in TARGETS:
		if not places.has(name):
			push_warning("VARREDURA: sem alvo para %s" % name)
			continue
		await _target(cap, name, places[name])
	await _pan(cap)


## The targets, each a point of the map (x, z): what the game says, in the order of its ids.
static func _places() -> Dictionary:
	var places := {}
	var cross: Vector3 = CityStore.query(&"traffic_probe", [0]).get("busy_crossing", Vector3.ZERO)
	if cross != Vector3.ZERO:
		places["cruzamento"] = cross
	var b: Dictionary = CityStore.query(&"city_bounds")
	var mid: Vector3 = (b.min + b.max) / 2.0
	places["miolo"] = mid
	places["borda"] = Vector3(b.max.x, 0.0, mid.z)
	for id in CityStore.query(&"list_buildings"):
		var info: Dictionary = CityStore.query(&"probe_building", [id])
		if info.get("fn", 0) == SCHOOL:
			places["escola"] = info.position
			break
	return places


static func _target(cap: Node, name: String, at: Vector3) -> void:
	var t: Dictionary = TARGETS[name]
	var ground: Vector3 = cap._ground(at.x, at.z)
	var look := ground + Vector3.UP * float(t.look_m)
	var d: Array = t.from_m
	for yaw: float in ORBIT_DEG:
		await _look(cap, "%s%s_orb%d" % [PREFIX, name, int(yaw)], look, yaw, PITCH_DEG, d[MID])
	await _look(cap, "%s%s_zoom_far" % [PREFIX, name], look, ZOOM_YAW_DEG, PITCH_DEG, d[FAR])
	await _look(cap, "%s%s_zoom_near" % [PREFIX, name], look, ZOOM_YAW_DEG, PITCH_DEG, d[NEAR])
	await _look(cap, "%s%s_tilt_low" % [PREFIX, name], look, ZOOM_YAW_DEG, LOW_DEG, d[MID])
	await _look(cap, "%s%s_tilt_high" % [PREFIX, name], look, ZOOM_YAW_DEG, HIGH_DEG, d[MID])


static func _pan(cap: Node) -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var mid: Vector3 = (b.min + b.max) / 2.0
	var half: float = (b.max.x - b.min.x) / 2.0 * PAN_SPAN_SHARE
	for i in PAN_STOPS:
		var x := mid.x - half + 2.0 * half * i / (PAN_STOPS - 1)
		var ground: Vector3 = cap._ground(x, mid.z)
		await _look(cap, "%span_%d" % [PREFIX, i], ground + Vector3.UP * PAN_LOOK_M, PAN_YAW_DEG, PITCH_DEG, PAN_DIST_M)


## The camera `dist` metres from `at`, toward the azimuth and the tilt, looking at `at`.
static func _look(cap: Node, name: String, at: Vector3, yaw_deg: float, pitch_deg: float, dist: float) -> void:
	var yaw := deg_to_rad(yaw_deg)
	var pitch := deg_to_rad(pitch_deg)
	var dir := Vector3(sin(yaw) * cos(pitch), sin(pitch), cos(yaw) * cos(pitch))
	await cap._shot(name, at + dir * dist, at)
