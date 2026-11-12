## A node's paving wears, on each arm's mouth, that arm's own asphalt, and one colour at every point; and every asphalt piece of a
## carriageway carries its wear: how far it is from the kerb, a lane also where it is across itself and how far from a mouth that
## opens on a node's paving (probe road_overlaps, its `wear` census of the meshes as built). On a junction laid by hand, with no
## generated city: an avenue with parking bays crossed by a local street, whose asphalts differ in tone.
## Run: godot --path project res://tests/core/plate_tone_test.tscn -- --no-flow
extends TestCase

const LAND_MAP := 0
const SEED := 7
const AVENUE := "avenueWithParking"
const STREET := "localStreet"
const HALF_M := 150.0
const PX := 0.25 # metres a pixel the road tool snaps by (road_commands_test.gd)
const ARMS := 4


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(LAND_MAP, SEED, false)
	var avenue := _profile(AVENUE)
	var street := _profile(STREET)
	var a: Dictionary = CityStore.dispatch(&"build_road", [Vector3(-HALF_M, 0, 0), Vector3(HALF_M, 0, 0), false, Vector3.ZERO, avenue, PX])
	var b: Dictionary = CityStore.dispatch(&"build_road", [Vector3(0, 0, -HALF_M), Vector3(0, 0, HALF_M), false, Vector3.ZERO, street, PX])
	if not check(avenue >= 0 and street >= 0 and a.get("ok", false) and b.get("ok", false), "an avenue with bays crossed by a local street (%s, %s)" % [a, b]):
		finish()
		return
	var w: Dictionary = CityStore.query(&"road_overlaps").wear
	print("the asphalt's wear as built: ", w)
	check(int(w.mouths) >= ARMS, "the two roads meet in a junction: %d mouths on a node's paving" % w.mouths)
	check(int(w.mouth_points) > 0 and int(w.mouth_off) == 0, "a node's paving wears each arm's own asphalt on that arm's mouth: of %d vertices on a mouth %d do not, the first (x, y, z) %s"
			% [w.mouth_points, w.mouth_off, w.first_off])
	check(int(w.split) == 0, "a node's paving has one colour at every point: %d points carry two" % w.split)
	check(int(w.kerb_points) > 0 and int(w.kerb_off) == 0, "a node's paving knows where its kerb is: of %d vertices on a corner's kerb line %d are not at nought from it"
			% [w.kerb_points, w.kerb_off])
	check(int(w.lane_wide) > 0 and int(w.lane_off) == 0, "a lane knows where it is across itself: of %d lane vertices %d are at its width, %d before its own edge"
			% [w.lane_points, w.lane_wide, w.lane_off])
	check(int(w.lane_past_mouth) > 0, "a lane knows how far it is from a mouth on a node's paving: %d of its vertices stand off one" % w.lane_past_mouth)
	check(int(w.bay_points) > 0 and int(w.bay_inner) > 0, "a parking bay knows how far it is from the kerb: %d of its %d vertices stand off it" % [w.bay_inner, w.bay_points])
	finish()


func _profile(id: String) -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if p.id == id:
			return p.index
	return -1
