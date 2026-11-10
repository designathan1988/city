## How a person is drawn at each distance (probe agent_cost: the people at each slot of the last frame, and the pixels their
## figures take): its meshes near, its coarsest mesh casting nothing past the shadows' distance, and the card of its pictures only
## where the whole figure stands under the card's pixels on the frame's own lens (data/render/agents.json people.card): never a
## card where a figure is tall enough to read, never a mesh where it is not. A rider stays a mesh (the card's pictures stand).
## Run: godot --path project res://tests/core/people_levels_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 4
const WARMUP_TICKS := 2400
const NOON := 12.0
const WALKERS_READ := 4096
const LENS_DEG := 35.0 # the game's far lens (data/game/camera.json far_fov_deg)
const NEAR_M := 30.0 # inside the first mesh level
const FAR_M := 200.0 # past the shadows' distance, inside the people's radius
const PITCH := 0.5 # the eye's height over its distance on the ground
const SMALL_WINDOW := Vector2i(320, 180) # a screen on which a figure 200 m away is a few pixels tall
const PX_SLACK := 0.01 # of a pixel: the probe's floats

var cam: Camera3D


func _ready() -> void:
	cam = Camera3D.new()
	add_child(cam)
	cam.fov = LENS_DEG
	cam.look_at_from_position(Vector3(0, 300, 300), Vector3.ZERO)
	cam.current = true
	if not boot():
		finish()
		return
	check(new_map(TOWN_MAP, SEED, true).get("ok", false), "map made")
	await _frames(SETTLE_FRAMES)
	CityStore.query(&"set_city_hour", [NOON])
	ticks(WARMUP_TICKS)
	var at := _walker()
	check(at != Vector3.INF, "somebody walks in the town")
	var near := await _seen(at, NEAR_M)
	var slots: Array = near.people_slots
	var meshes := slots.size() - 2
	print("from %d m: %s, the card from %.0f m, shadows to %.0f m" % [NEAR_M, slots, near.people_card_from_m, near.people_shadow_to_m])
	check(meshes >= 1, "a body has its mesh levels, the mesh with no shadow and the card: %d slots" % slots.size())
	check(int(slots[0]) > 0, "somebody %d m away is the finest mesh: %s" % [NEAR_M, slots])
	check(float(near.people_card_from_m) > FAR_M, "on this screen a grown figure %d m away stands over the card's pixels (the card from %.0f m)" % [FAR_M, near.people_card_from_m])
	_edges(near, "from %d m" % NEAR_M)
	var far := await _seen(at, FAR_M)
	slots = far.people_slots
	print("from %d m: %s" % [FAR_M, slots])
	check(int(slots[meshes]) > int(slots[meshes + 1]), "people past the shadows' distance are their coarsest mesh, casting nothing: %s" % [slots])
	_edges(far, "from %d m" % FAR_M)
	get_window().size = SMALL_WINDOW
	var small := await _seen(at, FAR_M)
	slots = small.people_slots
	print("from %d m on %s: %s, the card from %.0f m" % [FAR_M, SMALL_WINDOW, slots, small.people_card_from_m])
	check(float(small.people_card_from_m) < FAR_M, "on a small screen the same figure stands under the card's pixels (the card from %.0f m)" % small.people_card_from_m)
	check(int(slots[meshes + 1]) > int(slots[meshes]), "there the people are cards: %s" % [slots])
	_edges(small, "on the small screen")
	finish()


## Every card's figure stands under the card's pixels and every walker's mesh over them, each within the hold of the edge.
func _edges(seen: Dictionary, where: String) -> void:
	var px := float(seen.people_card_px)
	var hold := 1.0 + float(seen.people_level_hold)
	var card := float(seen.people_card_px_most)
	var mesh := float(seen.people_mesh_px_least)
	check(card <= px * hold + PX_SLACK, "%s no card's figure stands taller than the card's %.1f px (the tallest: %.2f px)" % [where, px, card])
	check(mesh == 0.0 or mesh >= px / hold - PX_SLACK, "%s no walker's mesh stands under the card's %.1f px (the smallest: %.2f px)" % [where, px, mesh])


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The walker nearest the town's middle.
func _walker() -> Vector3:
	var best := Vector3.INF
	for w: Dictionary in CityStore.query(&"shot_targets", [WALKERS_READ]).walkers:
		if not w.fallen and Vector2(w.position.x, w.position.z).length() < Vector2(best.x, best.z).length():
			best = w.position
	return best


## What the people's probe says with the eye `dist` metres from `at`.
func _seen(at: Vector3, dist: float) -> Dictionary:
	var back := dist / sqrt(1.0 + PITCH * PITCH)
	cam.look_at_from_position(at + Vector3(0, back * PITCH, back), at)
	await _frames(SETTLE_FRAMES) # the core reads the eye's new place
	ticks(1) # a publication for it: the walkers round where it looks
	await _frames(SETTLE_FRAMES)
	return CityStore.query(&"agent_cost")
