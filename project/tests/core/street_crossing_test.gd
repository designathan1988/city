## The street's two gestures of the Inspector, seen working in traffic: a car stops for who is on a zebra between junctions and never reaches them, and a car
## turns back through the opening of a median. Run: godot --path project res://tests/core/street_crossing_test.tscn -- --no-flow
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
const BUSY_STREETS := 6
const OPENINGS := 10

func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	ticks(WARM_TICKS)
	_zebras()
	_openings()
	finish()


func _middle_of(at: Vector3) -> Variant:
	var seg: Dictionary = CityStore.query(&"segment_at", [at])
	if seg.get("seg", -1) < 0:
		return null
	var points: Array = seg.points
	return points[points.size() / 2]


## Zebras on the streets with cars on them, the camera's window over the town.
func _zebras() -> void:
	var sample: Array = CityStore.query(&"traffic_probe", [SAMPLE]).sample
	var made := 0
	for p: Vector3 in sample:
		var at: Variant = _middle_of(p)
		if at != null and CityStore.dispatch(&"add_crossing", [at, ZEBRA]).get("ok", false):
			made += 1
		if made == BUSY_STREETS:
			break
	check(made > 0, "crossings put on streets with cars: %d" % made)
	var crossers := 0
	var near := 0
	var held := 0
	var closest := INF
	for i in STEPS:
		ticks(TICKS)
		var w: Dictionary = CityStore.query(&"street_watch", [NEAR_M])
		crossers = maxi(crossers, w.crossers)
		near += w.near
		held += w.held
		closest = minf(closest, w.closest_m)
		if near > 0 and held > 0 and i * TICKS > WARM_TICKS:
			break
	print("on the crossing ", crossers, " cars near ", near, " stopped ", held, " the nearest ", closest)
	check(crossers > 0, "people cross")
	check(near > 0, "with a car coming")
	check(held > 0, "a car stops for who crosses")
	check(closest >= CLEAR_M, "no car reaches who is on the crossing (%.2f m)" % closest)


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
