## Who walks as whom, read back from the renderer's casting (people_casting.h census, the lab's citizenCasting.ts): the walkers of the
## town in the afternoon each wear a body of their own age, and the crowd wears many bodies of the cast, not a few over and over.
## Run: godot --path project res://tests/core/people_cast_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const WARMUP_TICKS := 4800
const WARMUP_FROM := 12.0
const DRAW_FRAMES := 8 # the renderer takes up the poses a tick published on its next frames
const MIN_BODIES := 8 # half the cast of sixteen: the lab casts by wardrobe and keeps twins 25 m apart, so a town's crowd wears most of it


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
	for i in DRAW_FRAMES:
		await get_tree().process_frame
	var cast: Dictionary = CityStore.query(&"agent_cost").get("people_cast", {})
	print("cast: ", cast)
	check(int(cast.get("walkers", 0)) > 0, "walkers cast: %s" % cast)
	check(int(cast.get("wrong_age", -1)) == 0, "a walker in a body not of its age: %s" % cast)
	check(int(cast.get("bodies", 0)) >= MIN_BODIES, "bodies the crowd wears: %s" % cast)
	finish()
