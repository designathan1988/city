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
const SHOTS := {"couple": "30_couple", "family": "31_family", "child": "32_child", "elder": "33_elder",
	"coming_out": "34_coming_out", "going_in": "35_going_in", "wait": "36_waiting", "phone": "39_phone", "bag": "40_bag", "talk": "41_talk",
	"look": "42_looking", "headphones": "43_headphones", "listen": "44_listening"}
const FAR_FROM := Vector3(-58, 9, 150) # from the ground under FAR_AT
const FAR_AT := Vector3(-58, 1, 0)
const CLIPS := ["chairSit", "carDrive", "carRide", "cabDrive", "motoRide", "motoStop", "bikeStop", "bikePedal"]
const HANDS := ["phone", "bag"] # looked at again from where the hand reads
const HANDS_M := 2.2
const HANDS_Y := 1.1
const BANG := {"crouch": "45_crouching", "photo": "46_filming"}
const BANG_HEAD := {"crouch": 0.85, "photo": 1.55}
const FACE_M := 0.8
const TALK_HEAD := 1.55
const BANG_FROM := Vector3(2, 40, 2) # straight down, this far beside somebody
const BANG_NEAR_M := 60.0
const BANG_TRIES := 12
const BANG_STRIDE := 7
const BANG_CROWD := 300
const BANG_LOOKS := 6
const BANG_TICKS := 16
const BANG_CALM_TICKS := 500
const WAIT_TICKS := 200
const TRIES := 16


## `cap` is the capture scene (its _shot, world and camera).
static func shots(cap: Node) -> void:
	Settings.apply(cap.world, cap.get_viewport())
	UiDriver.at_hour(cap.world, HOUR)
	var left: Dictionary = SHOTS.duplicate()
	for key in SHOTS: # --only=people,34_coming_out: only those, a moment later
		if cap.only.size() > 1 and not cap.only.has(SHOTS[key]):
			left.erase(key)
	if cap.only.size() > 1:
		CityStore.step_ticks(WAIT_TICKS)
	for t in TRIES:
		CityStore.query(&"set_city_hour", [HOUR])
		var census: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, NEAR_M])
		print("census at 16 h: ", census)
		for key in left.keys():
			if not census.spots.has(key):
				continue
			var spot: Array = census.spots[key]
			await cap._shot(left[key], _from(spot[0], spot[1], spot[2]), spot[0] + Vector3.UP * LOOK_Y)
			if key == "talk":
				var head: Vector3 = spot[0] + Vector3.UP * TALK_HEAD
				var facing: Vector3 = spot[1]
				await cap._shot(left[key] + "_face", head + facing * FACE_M, head)
			if key in HANDS:
				await cap._shot(left[key] + "_near", spot[0] + (_from(spot[0], spot[1], spot[2]) - spot[0]).normalized() * HANDS_M + Vector3.UP, spot[0] + Vector3.UP * HANDS_Y)
			left.erase(key)
		if left.is_empty():
			break
		CityStore.step_ticks(WAIT_TICKS)
	print("no photo: ", left.keys())
	CityStore.query(&"set_city_hour", [HOUR])
	var ground: Vector3 = cap._ground(FAR_AT.x, FAR_AT.z)
	await cap._shot("37_crowd", ground + FAR_FROM - Vector3(FAR_AT.x, 0, FAR_AT.z), ground + FAR_AT - Vector3(FAR_AT.x, 0, FAR_AT.z))


## Who has just heard a shot: down where they were near it, a phone held up to it far off: --only=shot.
static func bang(cap: Node) -> void:
	Settings.apply(cap.world, cap.get_viewport())
	UiDriver.at_hour(cap.world, HOUR)
	CityStore.step_ticks(WAIT_TICKS)
	var left: Dictionary = BANG.duplicate()
	for t in BANG_TRIES:
		var found: Array = CityStore.query(&"shot_targets", [BANG_CROWD]).walkers
		if found.is_empty():
			break
		var at: Vector3 = found[(t * BANG_STRIDE) % found.size()].position
		print("shot at ", at, ": ", CityStore.dispatch(&"shoot", [at + BANG_FROM, Vector3.DOWN]))
		for k in BANG_LOOKS:
			CityStore.step_ticks(BANG_TICKS)
			var census: Dictionary = CityStore.query(&"people_census", [at, BANG_NEAR_M])
			for key in left.keys():
				if not census.spots.has(key):
					continue
				var spot: Array = census.spots[key]
				var seen := _from(spot[0], spot[1], spot[2])
				await cap._shot(left[key], seen, spot[0] + Vector3.UP * LOOK_Y)
				await cap._shot(left[key] + "_near", spot[0] + (seen - spot[0]).normalized() * HANDS_M + Vector3.UP, spot[0] + Vector3.UP * HANDS_Y)
				var head: Vector3 = spot[0] + Vector3.UP * BANG_HEAD[key]
				var facing: Vector3 = spot[1]
				await cap._shot(left[key] + "_face", head + facing * FACE_M, head)
				left.erase(key)
		if left.is_empty():
			break
		CityStore.step_ticks(BANG_CALM_TICKS)
	print("no photo: ", left.keys())


## The clips of people in vehicles, on who waits at a kerb: --only=clips. They are drawn with no vehicle round them, as the
## skin of people.gdshader reads them from the palette.
static func clips(cap: Node) -> void:
	Settings.apply(cap.world, cap.get_viewport())
	UiDriver.at_hour(cap.world, HOUR)
	for t in TRIES:
		CityStore.query(&"set_city_hour", [HOUR])
		var census: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, NEAR_M])
		if census.spots.has("wait"):
			var spot: Array = census.spots.wait
			for clip: String in CLIPS:
				print("clip ", clip, ": ", CityStore.query(&"show_people_clip", [clip]))
				await cap._shot("38_" + clip, _from(spot[0], spot[1], spot[2]), spot[0] + Vector3.UP * LOOK_Y)
			CityStore.query(&"show_people_clip", ["idle"])
			return
		CityStore.step_ticks(WAIT_TICKS)
	print("no photo: nobody waiting")


static func _from(at: Vector3, way: Vector3, road: Vector3) -> Vector3:
	var out := (road + way * SHOT_ALONG).normalized()
	return at + out * SHOT_M * cos(SHOT_UP_RAD) + Vector3.UP * (SHOT_M * sin(SHOT_UP_RAD) + LOOK_Y)
