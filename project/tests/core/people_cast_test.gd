## Who walks as whom and what they play, read back from the renderer (people_casting.h census, people_renderer.h plays census; the lab's
## citizenCasting.ts and citizenGait.ts): the walkers of the town in the afternoon each wear a body of their own age, the crowd wears many
## bodies of the cast, a party is dressed in one code; near the eye the walkers lead their poses with more than one walk of the cast's.
## Run: godot --path project res://tests/core/people_cast_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const WARMUP_TICKS := 4800
const WARMUP_FROM := 12.0
const DRAW_FRAMES := 8 # the renderer takes up the poses a tick published on its next frames
const MIN_BODIES := 8 # half the cast of sixteen: the lab casts by wardrobe and keeps twins 25 m apart, so a town's crowd wears most of it
const LOOKS := 12 # walkers the camera is put over, one after the other
const LOOK_FROM := Vector3(0.0, 6.0, 9.0) # the camera's place over a walker: inside the near level's 40 m (agents.json people.lod_m)
const STEP_TICKS := 30 # a second between two looks: the walkers walk on
const WALKS := ["walk", "walkElder", "walkShuffle", "walkSlow", "walkN1", "walkN2", "walkN3", "walkStroll", "walkCool", "walkFast", "run"]


func _ready() -> void:
	var cam := Camera3D.new()
	add_child(cam)
	cam.look_at_from_position(Vector3(0, 120, 120), Vector3.ZERO)
	cam.current = true
	if not boot():
		finish()
		return
	check(new_map(TOWN_MAP, SEED, true).get("ok", false), "map made")
	CityStore.query(&"set_city_hour", [WARMUP_FROM])
	ticks(WARMUP_TICKS)
	await _frames(DRAW_FRAMES)
	var cast: Dictionary = CityStore.query(&"agent_cost").get("people_cast", {})
	print("cast: ", cast)
	check(int(cast.get("walkers", 0)) > 0, "walkers cast: %s" % cast)
	check(int(cast.get("wrong_age", -1)) == 0, "a walker in a body not of its age: %s" % cast)
	check(int(cast.get("bodies", 0)) >= MIN_BODIES, "bodies the crowd wears: %s" % cast)
	check(int(cast.get("in_party", 0)) > 0, "walkers in a party: %s" % cast)
	check(int(cast.get("party_off_code", -1)) == 0, "one of a party out of its code: %s" % cast)
	var walks := {}
	var near := 0
	var walkers: Array = CityStore.query(&"traffic_probe", [LOOKS]).get("walkers", [])
	for at: Vector3 in walkers:
		cam.look_at_from_position(at + LOOK_FROM, at)
		await _frames(DRAW_FRAMES)
		var led: Dictionary = CityStore.query(&"agent_cost").get("people_plays", {})
		for clip: String in led:
			near += int(led[clip])
			if clip in WALKS:
				walks[clip] = true
		ticks(STEP_TICKS)
	print("walks leading: ", walks.keys(), " of ", near, " poses near")
	check(near > 0, "walkers near the eye: %d" % near)
	check(walks.size() > 1, "the walks the near walkers lead with: %s" % [walks.keys()])
	finish()


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame
