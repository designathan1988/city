## How the town's drivers drive, over a morning's minutes (probe driving_census): nobody takes a turn faster than its own driver
## takes that bend; the turns are taken at their own speeds, by their curves, not at one speed; the drivers differ; whoever turns
## shows that turn's indicator, and lane changes are signalled before they are made; a street that curves is slowed for too.
## Run: godot --path project res://tests/core/driving_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const HOUR := 8.0 # the morning's traffic
const WARM_TICKS := 1200
const SAMPLES := 60
const EVERY_TICKS := 10
const VIEW_M := 300.0
const AT := Vector3.ZERO # the window's middle: the camera of a test stands at the origin
const LEAST_SEEN := 20
const OVER_MS := 1.0 # over its bend speed by less than this: the end of its braking as it comes onto the curve
const TURN_SPREAD_MS := 0.5 # the map's turns differ at least this much in the speed they are taken at
const TEMPER_SPAN := 0.6 # the most timid and the most pushy seen are at least this far apart
const TEMPER_MEAN := 0.25 # and the fleet's mean this near the ordinary driver
## The ring of curved corners (_street_bends): an avenue (road_profiles.json, four lanes), on free ground past the town.
const AVENUE := 3
const OUT_M := 300.0
const STRAIGHT_M := 160.0 # a straight between two corners: room to reach the street's speed and brake again
const CORNER_M := 40.0 # a corner's curve starts and ends this far from the corner: a radius of some 28 m at its middle
const PX := 0.25
const SETTLE_TICKS := 4 # the lane graph takes the edit up
const RING_VIEW_M := 100.0 # how far the view over the ring sees: the window at its least
const LOOK_TICKS := 40 # past the window's own look (a second)
const CARS := 6
const CAR_EVERY_TICKS := 100 # the car before has left the mouth
const HOPS := 64 # streets on: eight times round the ring
const RING_SAMPLES := 240
const RING_REACH_M := 400.0


func _ready() -> void:
	if not boot():
		finish()
		return
	new_map(TOWN_MAP, SEED, true)
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(WARM_TICKS)
	var worst := 0.0
	var on_bends := 0
	var turning := 0
	var shown := 0
	var signalling := 0
	var wanting := 0
	var c := {}
	for i in SAMPLES:
		ticks(EVERY_TICKS)
		c = CityStore.query(&"driving_census", [AT, VIEW_M])
		worst = maxf(worst, c.over_bend_ms)
		on_bends += c.on_bends
		turning += c.turning
		shown += c.turning_shown
		signalling += c.signalling
		wanting += c.wanting
	check(c.vehicles >= LEAST_SEEN, "the town drives round the camera: %d vehicles" % c.vehicles)
	check(on_bends > 0 and worst <= OVER_MS,
			"nobody takes a turn faster than its own driver takes that bend: %d seen on bends, the worst %.2f m/s over" % [on_bends, worst])
	check(c.turn_bends > 0 and c.turn_fastest_ms - c.turn_slowest_ms >= TURN_SPREAD_MS,
			"the turns are taken by their curves: %d of them, an ordinary driver from %.1f to %.1f m/s; %d streets curve" %
			[c.turn_bends, c.turn_slowest_ms, c.turn_fastest_ms, c.curved_streets])
	check(c.temper_highest - c.temper_lowest >= TEMPER_SPAN and absf(c.temper_mean) <= TEMPER_MEAN,
			"the drivers differ: tempers from %.2f to %.2f, mean %.2f" % [c.temper_lowest, c.temper_highest, c.temper_mean])
	check(turning > 0 and shown == turning, "whoever turns shows that turn's indicator: %d of %d" % [shown, turning])
	check(signalling > shown, "turns are signalled before their line: %d signals seen, %d of them in the turn" % [signalling, shown])
	check(c.lane_changes > 0 and c.lane_signals >= c.lane_changes and wanting > 0,
			"lane changes are signalled, then made: %d signalled, %d made after their signal, %d forced at a line (%d streets of more than one lane)" %
			[c.lane_signals, c.lane_changes, c.lane_forced, c.wide_streets])
	_street_bends()
	finish()


## A street that curves is slowed for as a turn is (the generated town has none). A ring laid on free ground past the town: four
## straights of an avenue joined by four tight corners, each corner one curved street; the view stands over it (the window of
## detailed traffic at its least, round the ring), cars are set on a straight with a way round it several times (probe drive_from:
## no trip of the town leads there), and each one seen on a corner's bend is under the pace its own driver takes that bend at,
## which is under the street's limit, while the straights are driven faster.
func _street_bends() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	var o := Vector3(b.max.x + OUT_M, 0, (b.min.z + b.max.z) * 0.5)
	var side := STRAIGHT_M + 2.0 * CORNER_M
	_ring(o, side)
	var centre := o + Vector3(side, 0, side) * 0.5
	CityStore.query(&"far_traffic", [centre, RING_VIEW_M])
	ticks(LOOK_TICKS)
	var hit: Dictionary = CityStore.query(&"inspect", [o + Vector3(side * 0.5, 0, 0)])
	var seg: int = hit.get("id", -1) if hit.get("kind", "") == "road" else -1
	var made := 0
	for i in CARS:
		made += 1 if CityStore.query(&"drive_from", [seg, 0, HOPS]) >= 0 else 0
		ticks(CAR_EVERY_TICKS)
	var seen := 0
	var worst := 0.0
	var straight := 0.0
	var c := {}
	for i in RING_SAMPLES:
		ticks(EVERY_TICKS)
		c = CityStore.query(&"driving_census", [centre, RING_REACH_M])
		seen += c.street_bends
		worst = maxf(worst, c.over_street_bend_ms)
		straight = maxf(straight, c.straight_fastest_ms)
	check(made > 0 and c.curved_streets > 0, "a ring with curved corners carries cars: %d set on it, %d directed streets curve" % [made, c.curved_streets])
	check(c.street_bend_pace_ms > 0.0 and c.street_bend_pace_ms < c.street_bend_limit_ms,
			"its corners ask a driver to slow: an ordinary one takes the sharpest at %.1f m/s on a street of %.1f m/s" % [c.street_bend_pace_ms, c.street_bend_limit_ms])
	check(seen > 0 and worst <= OVER_MS,
			"nobody takes a street's bend faster than its own driver takes it: %d seen on the bends, the worst %.2f m/s over" % [seen, worst])
	check(straight > c.street_bend_pace_ms, "and the straights are driven faster: up to %.1f m/s" % straight)


## The ring's eight streets from the corner `o`, `side` metres a side: each corner's curve (its control point the corner itself)
## and the straight from it to the next.
func _ring(o: Vector3, side: float) -> void:
	var corners: Array[Vector3] = [Vector3.ZERO, Vector3(side, 0, 0), Vector3(side, 0, side), Vector3(0, 0, side)]
	for i in corners.size():
		var at: Vector3 = o + corners[i]
		var next: Vector3 = o + corners[(i + 1) % corners.size()]
		var ahead: Vector3 = (next - at).normalized()
		var behind: Vector3 = (corners[i] - corners[(i + corners.size() - 1) % corners.size()]).normalized()
		_road(at - behind * CORNER_M, at + ahead * CORNER_M, true, at)
		_road(at + ahead * CORNER_M, next - ahead * CORNER_M, false, Vector3.ZERO)
	ticks(SETTLE_TICKS)


func _road(a: Vector3, e: Vector3, curved: bool, control: Vector3) -> void:
	var r: Dictionary = CityStore.dispatch(&"build_road", [a, e, curved, control, AVENUE, PX])
	check(r.get("ok", false), "road %s -> %s laid: %s" % [a, e, r])
