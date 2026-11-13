## The street's two gestures of the Inspector, seen working in traffic: a car gives way to who is on a zebra between junctions and never reaches them (a
## scene of its own: a street, a zebra, somebody bound for the other sidewalk and a car coming while they are on it), and a car turns back through the
## opening of a median; and nobody walks along or across a highway. Run: godot --path project res://tests/core/street_crossing_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const WARM_TICKS := 1200
const SAMPLE := 200
const ZEBRA := 1
const NEAR_M := 14.0
const CLEAR_M := 1.5 # the middle of a car to the middle of a walker: under this they touch
const STEPS := 4000
const TICKS := 4
const OPENINGS := 10
const STREET_HALF_M := 150.0
const PX := 0.25 # metres a pixel the road tool snaps by (road_commands_test.gd)
const SETTLE_TICKS := 4 # the lane graph and the junction's control take what was built
const SCENE_TICKS := 2400 # two minutes of the clock: the most any step of the scene takes
const CAR_MS := 8.0
const CAR_M := 15.0 # half as far again as a comfortable stop from CAR_MS takes (traffic.json classes, sedan: b 2,9): it can stop, and gets there before anybody is across
const STREET_LANES := 2
const SIDEWALKS := 2
const HOLD_CROSSING := 8 # lane_graph_types.h Hold::CROSSING
const SIDEWALK := 0 # ped_state.h PedMode::SIDEWALK
const EVERYBODY := 1.0 # the Optimisations tab's share of the fleet and the walkers: all of them again after the scene
const NO_CAR := -1
const BARRED_SAMPLES := 300 # looks at the walkers, TICKS apart: twenty seconds of the clock
const HIGHWAY_HALF_M := 250.0 # the expressway laid through the town's middle, over its streets

func _ready() -> void:
	if not boot():
		finish()
		return
	_zebra()
	new_map(TOWN_MAP, SEED, true)
	ticks(WARM_TICKS)
	_openings()
	_off_highways()
	finish()


## Nobody walks along a highway or a ramp, nor crosses one at grade (the Brazilian traffic code, annex I): an expressway laid through the
## town at grade, over its streets, and the town's walkers watched while its traffic runs.
func _off_highways() -> void:
	var expressway := -1
	for p: Dictionary in CityStore.query(&"road_profiles"):
		if String(p.id) == "expressway":
			expressway = int(p.index)
	var built: Dictionary = CityStore.dispatch(&"build_road", [Vector3(-HIGHWAY_HALF_M, 0, 0), Vector3(HIGHWAY_HALF_M, 0, 0), false, Vector3.ZERO, expressway, PX])
	if not check(expressway >= 0 and built.get("ok", false), "an expressway laid across the town (%s)" % built):
		return
	ticks(WARM_TICKS)
	var worst := 0
	var walking := 0
	var closed := 0
	for i in BARRED_SAMPLES:
		ticks(TICKS)
		var probe: Dictionary = CityStore.query(&"traffic_probe", [0])
		worst = maxi(worst, int(probe.walkers_barred))
		walking = maxi(walking, int(probe.walking))
		closed = int(probe.closed_roads)
	check(closed > 0 and walking > 0, "the town has highways (%d pieces) and walkers (%d at most)" % [closed, walking])
	check(worst == 0, "no walker along a highway or over one: %d at worst in %d looks" % [worst, BARRED_SAMPLES])


func _middle_of(at: Vector3) -> Variant:
	var seg: Dictionary = CityStore.query(&"segment_at", [at])
	if seg.get("seg", -1) < 0:
		return null
	var points: Array = seg.points
	return points[points.size() / 2]


## The first profile of a street with a lane each way, a sidewalk each side and no median.
func _two_way_street() -> int:
	for p: Dictionary in CityStore.query(&"road_profiles"):
		var kinds := {}
		for strip: Dictionary in p.strips:
			kinds[strip.kind] = int(kinds.get(strip.kind, 0)) + 1
		if not p.one_way and not p.rail and not p.ring and int(p.lanes) == STREET_LANES and int(kinds.get("sidewalk", 0)) == SIDEWALKS and not kinds.has("median"):
			return p.index
	return -1


## The scene stepped tick by tick until `done` says so of what it shows, SCENE_TICKS at most: what it shows then.
func _until(walker: Dictionary, car: int, done: Callable) -> Dictionary:
	var now: Dictionary = CityStore.query(&"scene_watch", [walker.id, walker.look, car])
	for i in SCENE_TICKS:
		if done.call(now):
			break
		ticks(1)
		now = CityStore.query(&"scene_watch", [walker.id, walker.look, car])
	return now


## A zebra on a street of its own, with nobody else on it: somebody bound for the other sidewalk steps onto it; a car is then stood on the lane beside
## the kerb they are going to, where it can still stop and gets there before they are across.
func _zebra() -> void:
	new_map(TOWN_MAP, SEED, false)
	CityStore.dispatch(&"set_population", [0.0])
	var profile := _two_way_street()
	var built: Dictionary = CityStore.dispatch(&"build_road", [Vector3(-STREET_HALF_M, 0, 0), Vector3(STREET_HALF_M, 0, 0), false, Vector3.ZERO, profile, PX])
	var street: bool = profile >= 0 and built.get("ok", false) and CityStore.dispatch(&"add_crossing", [Vector3.ZERO, ZEBRA]).get("ok", false)
	ticks(SETTLE_TICKS)
	var walker: Dictionary = CityStore.query(&"scene_walker", [Vector3.ZERO]) if street else {"id": -1}
	if check(street and int(walker.id) >= 0, "a street with a zebra at its middle, and somebody before it bound for the other sidewalk (profile %d, %s)" % [profile, built]):
		_car_gives_way(walker)
	CityStore.dispatch(&"set_population", [EVERYBODY])


func _car_gives_way(walker: Dictionary) -> void:
	var now := _until(walker, NO_CAR, func(w: Dictionary) -> bool: return not w.walker or w.crossing)
	if not check(now.walker and now.crossing, "they step onto the zebra"):
		return
	var car: int = CityStore.query(&"scene_car", [walker.id, CAR_M, CAR_MS])
	if not check(car >= 0, "a car comes on the lane beside the kerb they are going to, %.0f m before its line at %.0f m/s" % [CAR_M, CAR_MS]):
		return
	# giving way is braking for them, to a stand only when they are still in its way as it gets to its line: what is asked is that the
	# crossing holds it at its line (the reason the game itself gives), that it slows, and that it never reaches them
	var refused := false
	var slowest := CAR_MS
	var closest := INF
	for i in SCENE_TICKS:
		ticks(1)
		now = CityStore.query(&"scene_watch", [walker.id, walker.look, car])
		if not now.walker or not now.crossing:
			break
		if now.car:
			closest = minf(closest, now.apart_m)
			slowest = minf(slowest, now.speed)
			refused = refused or (int(now.ds) >= 0 and int(now.hold) == HOLD_CROSSING)
	print("the zebra's scene: held by the crossing ", refused, ", its slowest ", slowest, ", the nearest it came ", closest, ", at the end ", now)
	check(refused and slowest < CAR_MS, "a car gives way to who crosses: held at its line by the crossing, slowed from %.0f to %.1f m/s" % [CAR_MS, slowest])
	check(closest >= CLEAR_M, "no car reaches who is on the crossing (%.2f m)" % closest)
	now = _until(walker, car, func(w: Dictionary) -> bool: return not w.walker or int(w.mode) == SIDEWALK)
	check(now.walker and int(now.seg) == int(walker.goal_seg) and int(now.side) == int(walker.goal_side), "they reach the other sidewalk")
	now = _until(walker, car, func(w: Dictionary) -> bool: return not w.car or int(w.ds) < 0)
	check(not now.car or int(now.ds) < 0, "the car goes on once they are across")


## Openings along the avenues, and somebody using one.
func _openings() -> void:
	var census: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	var made := 0
	for p: Vector3 in census:
		var info: Dictionary = CityStore.query(&"inspect", [p])
		if not info.get("median", false):
			continue
		var at: Variant = _middle_of(p)
		if at != null and CityStore.dispatch(&"add_uturn", [at, true]).get("ok", false):
			made += 1
		if made == OPENINGS:
			break
	check(made > 0, "openings put in the median: %d" % made)
	var turning := 0
	for i in STEPS:
		ticks(TICKS)
		turning = maxi(turning, CityStore.query(&"street_watch", [NEAR_M]).turning_back)
		if turning > 0:
			break
	check(turning > 0, "a car turns back through the median's opening")
