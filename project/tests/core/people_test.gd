## Who is on foot, through the census of the core: out of the doors and in at them, of each age, in pairs that stay together,
## no child alone, fewer in the small hours than in the afternoon, and nobody inside a street piece or another walker.
## Run: godot --path project res://tests/core/people_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 1
const SETTLE_FRAMES := 2
const RIDER_CLIPS := ["chairSit", "carDrive", "carRide", "carRearRide", "cabDrive", "cabRide", "motoRide", "motoStop", "bikeStop", "bikePedal"]
const WARMUP_TICKS := 4800
const WARMUP_FROM := 12.0
const SMALL_HOURS := 3.0
const MIN_PARTY_SHARE := 0.4 # Moussaid 2010: 55 % and 70 % of the people; the census of the Roadcraft 4.0: 55 %
const MAX_PARTY_SHARE := 0.75
const MIN_ELDERS := 0.08
const MAX_ELDERS := 0.25
const MIN_CHILDREN := 0.05
const MAX_CHILDREN := 0.25
const FAR_PARTY_M := 4.0 # the two of a pair are never further apart than a few steps
const MAX_TOUCHING := 0.01 # a stride's overlap as one turns while the other stands: not more than one pair in a hundred
const CROSS_STEPS := 1500 # a street sees few people an hour: as long as it takes somebody to cross
const CROSS_TICKS := 30
const NEAR_M := 150.0 # the census of the Roadcraft 4.0: 86 people within this of the camera at 16:00
const MORE_TICKS := 9000
const MIN_INDOORS := 0.5 # of those who arrive; the others, on a street without a door, end at the node


func _ready() -> void:
	var cam := Camera3D.new()
	add_child(cam)
	cam.look_at_from_position(Vector3(0, 300, 300), Vector3.ZERO)
	cam.current = true
	if not boot():
		finish()
		return
	check(new_map(TOWN_MAP, SEED, true).get("ok", false), "map made")
	await _frames(SETTLE_FRAMES)
	CityStore.query(&"set_city_hour", [WARMUP_FROM]) # the clock runs: at the census it reads the afternoon's hour
	ticks(WARMUP_TICKS)
	var c: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
	print("census: ", c)
	print("near: ", CityStore.query(&"people_census", [Vector3.ZERO, NEAR_M]).total, " within ", NEAR_M, " m")
	var total := float(c.total)
	check(total > 0, "people in the street")
	check(int(c.doors) > 0, "doors read: %d" % c.doors)
	check(int(c.doors_across) == 0, "a door's way crosses the street: %d" % c.doors_across)
	check(int(c.came_out) > 0, "people left by a door: %d" % c.came_out)
	check(int(c.went_in) > 0, "people went in by a door: %d" % c.went_in)
	var share := 1.0 - float(c.solo) / total
	check(share >= MIN_PARTY_SHARE and share <= MAX_PARTY_SHARE, "in a group: %.2f" % share)
	check(float(c.elders) / total >= MIN_ELDERS and float(c.elders) / total <= MAX_ELDERS, "elders: %d" % c.elders)
	check(float(c.children) / total >= MIN_CHILDREN and float(c.children) / total <= MAX_CHILDREN, "children: %d" % c.children)
	check(int(c.children_alone) == 0, "a child alone: %d" % c.children_alone)
	check(float(c.party_gap_m) <= FAR_PARTY_M, "the two of a pair apart: %.2f m" % c.party_gap_m)
	var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
	print("bodies: ", probe.people_overlaps, " furniture: ", probe.people_in_furniture)
	check(int(probe.people_in_furniture) == 0, "inside furniture: %d" % probe.people_in_furniture)
	check(float(c.party_touching) <= MAX_TOUCHING * float(c.parties), "pairs touching: %d" % c.party_touching)
	_clips()
	_card()
	ticks(MORE_TICKS)
	var later: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
	print("later: ", later)
	var doing := 0
	for act in later.acts:
		doing += int(later.acts[act])
	check(doing > 0, "who waits at the kerb does nothing: %s" % later.acts)
	var indoors := float(later.went_in) / maxf(float(later.arrived), 1.0)
	check(indoors >= MIN_INDOORS, "arrived by a door: %.2f" % indoors)
	_mid_block()
	var afternoon := float(c.wanted)
	CityStore.query(&"set_city_hour", [SMALL_HOURS])
	var night: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
	print("in the afternoon ", afternoon, " at 3 h ", night.wanted)
	check(float(night.wanted) < afternoon, "fewer people before dawn")
	finish()


func _frames(n: int) -> void:
	for i in n:
		await get_tree().process_frame


## The clips of people in vehicles (tools/assets/rider_poses.py): every one in the file, in the block, and a body seated in it.
func _clips() -> void:
	var clips: Array = CityStore.query(&"people_clips")
	var named := {}
	for c: Dictionary in clips:
		named[c.name] = c
	for name: String in RIDER_CLIPS:
		check(named.has(name), "a clip in the file: %s" % name)
		check(named.has(name) and float(named[name].pelvis.y) > 0.0 and float(named[name].above) > 0.0, "pelvis and reach of %s" % name)
	check(CityStore.query(&"show_people_clip", ["chairSit"]), "plays a clip by name")
	check(not CityStore.query(&"show_people_clip", ["no name"]), "refuses a clip that does not exist")
	CityStore.query(&"show_people_clip", ["idle"])


## The far card reads its strip of the atlas as the file gives it: a Color on a vec4 uniform would be turned linear by the engine and
## the card would show one black texel (the owner's review, 2026-11-09).
func _card() -> void:
	var card: Dictionary = CityStore.query(&"people_card")
	var strip: Variant = card.get("material")
	check(typeof(strip) == TYPE_VECTOR4, "the card's strip is a Vector4, not a Color: %s" % type_string(typeof(strip)))
	check(typeof(strip) == TYPE_VECTOR4 and strip == card.file and (strip as Vector4).z > 0.0 and (strip as Vector4).w > 0.0, "the card's strip is the file's: %s" % [card])


## People go over a crossing between junctions, a zebra and one with a signal of its own: put where walkers are, and counted while they use it.
func _mid_block() -> void:
	for kind: int in [CityStore.code(&"NodeCrossing", &"CROSSING_ZEBRA"), CityStore.code(&"NodeCrossing", &"CROSSING_SIGNAL")]:
		var made := false
		var census: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
		for key: String in census.spots:
			var seg: Dictionary = CityStore.query(&"segment_at", [census.spots[key][0]])
			if seg.get("seg", -1) >= 0:
				var points: Array = seg.points
				made = CityStore.dispatch(&"add_crossing", [points[points.size() / 2], kind]).get("ok", false)
			if made:
				break
		check(made, "a mid-block crossing where there are people, kind %d" % kind)
		var crossed := 0
		var waited := 0
		for i in CROSS_STEPS:
			ticks(CROSS_TICKS)
			var now: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0])
			crossed = maxi(crossed, now.mid_crossing)
			waited = maxi(waited, now.mid_waiting)
			if crossed > 0:
				break
		print("crossing kind ", kind, ": crossing ", crossed, " waiting ", waited)
		check(crossed > 0, "nobody used the mid-block crossing, kind %d (waiting: %d)" % [kind, waited])
		CityStore.dispatch(&"undo")
