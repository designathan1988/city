## People for the visual capture, with the city's clock at 16:00 and the light pinned there: the nearest couple, family, child,
## elder, walker coming out of a door, walker going in at one and walker at a kerb, each from over the street and a little above,
## as the pictures of the Roadcraft 4.0 frame them; and a crowd past the real shadows, where a soft disc stands for the shadow.
class_name CapturePeople
extends RefCounted

const HOUR := 16.0
const NEAR_M := 400.0
const SHOT_M := 14.0 # a person fills an eighth of the picture at the capture's lens
const SHOT_UP_RAD := 0.157 # 9 degrees
const SHOT_ALONG := 0.5 # the camera stands over the street, this share of the way ahead of the walker
const LOOK_Y := 0.9
const SHOTS := {"couple": "30_casal", "family": "31_familia", "child": "32_crianca", "elder": "33_idoso",
	"coming_out": "34_saindo", "going_in": "35_entrando", "wait": "36_espera"}
const FAR_FROM := Vector3(-58, 9, 150) # from the ground under FAR_AT
const FAR_AT := Vector3(-58, 1, 0)
const WAIT_TICKS := 200
const TRIES := 16


## `cap` is the capture scene (its _shot, world and camera).
static func shots(cap: Node) -> void:
	Settings.apply(cap.world, cap.get_viewport())
	cap.world.set_hour(HOUR)
	var left: Dictionary = SHOTS.duplicate()
	for key in SHOTS: # --only=pessoas,34_saindo: only those
		if cap.only.size() > 1 and not cap.only.has(SHOTS[key]):
			left.erase(key)
	for t in TRIES:
		CityStore.query(&"set_city_hour", [HOUR])
		var census: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, NEAR_M])
		print("censo às 16 h: ", census)
		for key in left.keys():
			if not census.spots.has(key):
				continue
			var spot: Array = census.spots[key]
			await cap._shot(left[key], _from(spot[0], spot[1], spot[2]), spot[0] + Vector3.UP * LOOK_Y)
			left.erase(key)
		if left.is_empty():
			break
		CityStore.step_ticks(WAIT_TICKS)
	print("sem foto: ", left.keys())
	CityStore.query(&"set_city_hour", [HOUR])
	var ground: Vector3 = cap._ground(FAR_AT.x, FAR_AT.z)
	await cap._shot("37_multidao", ground + FAR_FROM - Vector3(FAR_AT.x, 0, FAR_AT.z), ground + FAR_AT - Vector3(FAR_AT.x, 0, FAR_AT.z))


static func _from(at: Vector3, way: Vector3, road: Vector3) -> Vector3:
	var out := (road + way * SHOT_ALONG).normalized()
	return at + out * SHOT_M * cos(SHOT_UP_RAD) + Vector3.UP * (SHOT_M * sin(SHOT_UP_RAD) + LOOK_Y)
