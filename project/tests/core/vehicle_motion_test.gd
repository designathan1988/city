## The fleet as the player sees it round the camera, tick by tick: no body jumps farther than its speed carries it, none stands
## still while it drives, none appears or vanishes on a street in view, none stands in another, none climbs a footway.
## Run: godot --path project res://tests/core/vehicle_motion_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const HOUR := 8.0 # the morning's traffic
const WARM_TICKS := 1200
const WATCH_TICKS := 600
const VIEW_M := 300.0
const EDGE_M := 40.0 # a body this near the view's edge may drive into it or out of it
const STEP_SLACK_M := 0.1 # a lane change's slide over a tick, and the tick's rounding
const REBORN_M := 24.0 # farther in one tick is another vehicle given the same id
const MOVING_MS := 0.5
const STILL_M := 0.001
const LEAST_SEEN := 20
const PAIRS_SHOWN := 6 # the distinct pairs met that a failure names, each with the depth and rows of its first tick's deepest
const AT := Vector3.ZERO # the window's middle: the camera of a test stands at the origin
# the probe's row of a body (city_core_clearance_probe.cpp vehicle_bodies)
const ID := 0
const X := 1
const Z := 2
const SPEED := 3
const OFF := 4
const TYPE := 5
const ON_CONNECTOR := 6

var _dt := 0.0
var _lurch := 0
var _lurch_worst: Array = []
var _stalled := 0
var _stall_case: Array = []
var _popped := 0
var _vanished := 0
var _vanish_case: Array = []
var _in_another := 0
var _deepest := 0.0
var _deepest_pair: Array = []
var _pairs := {} # "low id-high id" of each tick's deepest pair -> [depth, rows] at its first tick
var _over := {}
var _footway_case: Array = []
var _seen := {}


func _ready() -> void:
	if not boot():
		finish()
		return
	_dt = 1.0 / float(DataTables.tuning("sim_tick_hz"))
	new_map(TOWN_MAP, SEED, true)
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(WARM_TICKS)
	var before := _bodies(CityStore.query(&"vehicle_bodies", [AT, VIEW_M]))
	_footway_rule(before)
	for i in WATCH_TICKS:
		ticks(1)
		var watch: Dictionary = CityStore.query(&"vehicle_bodies", [AT, VIEW_M])
		var now := _bodies(watch)
		_compare(before, now)
		_bodies_met(watch)
		before = now
	print("rows: id, x, z, speed, off, type, on a connector, ds, lane, s, cs, came, lane change offset, conn, route steps left, stop_at, lot car, its phase, along its way, closing its lane, lorry")
	check(_seen.size() >= LEAST_SEEN, "the town drives round the camera: %d vehicles seen" % _seen.size())
	check(_lurch == 0, "no body jumps farther than its speed carries it in a tick: %d jumps, the worst (metres, before, after) %s" % [_lurch, _lurch_worst])
	check(_stalled == 0, "no body stands still a tick while it drives: %d, the first (before, after) %s" % [_stalled, _stall_case])
	check(_popped == 0, "no vehicle appears on a street in view: %d" % _popped)
	check(_vanished == 0, "no vehicle vanishes from a street in view: %d, the first %s" % [_vanished, _vanish_case])
	check(_in_another == 0, "no body stands in another: %d times, the deepest %.2f m %s" % [_in_another, _deepest, _deepest_pair])
	if not _pairs.is_empty():
		print("pairs met (each tick's deepest): %d, the first %s" % [_pairs.size(), _pairs.values().slice(0, PAIRS_SHOWN)])
	check(_over.is_empty(), "no body on the lanes climbs a footway: %d bodies, %s" % [_over.size(), _footway_case])
	finish()


## The footway rule the probe judges by can say both: the street furniture of the streets the cars drive stands on a footway,
## the middle of a car on a lane does not.
func _footway_rule(bodies: Dictionary) -> void:
	var on_footway := 0
	var lane_middles := 0
	var lane_on_footway := 0
	for id: int in bodies:
		var b: Array = bodies[id]
		if b[OFF] or b[ON_CONNECTOR]:
			continue
		var at := Vector3(b[X], 0.0, b[Z])
		lane_middles += 1
		lane_on_footway += 1 if CityStore.query(&"footway_under", [at]) else 0
		if on_footway >= LEAST_SEEN:
			continue
		var seg: int = CityStore.query(&"segment_at", [at]).get("seg", -1)
		if seg < 0:
			continue
		for spot: Dictionary in CityStore.query(&"furniture_spots", [seg]):
			on_footway += 1 if CityStore.query(&"footway_under", [spot.position]) else 0
	check(on_footway > 0, "the footway rule finds the street furniture on its footway: %d pieces" % on_footway)
	check(lane_middles > 0 and lane_on_footway == 0, "and no lane's car on it: %d of %d" % [lane_on_footway, lane_middles])


## The probe's rows by id.
func _bodies(watch: Dictionary) -> Dictionary:
	var out := {}
	for b: Array in watch.bodies:
		out[b[ID]] = b
		_seen[b[ID]] = true
	return out


func _inside(b: Array) -> bool:
	return Vector2(b[X] - AT.x, b[Z] - AT.z).length() < VIEW_M - EDGE_M


func _compare(before: Dictionary, now: Dictionary) -> void:
	for id: int in now:
		var b: Array = now[id]
		var was: Variant = before.get(id)
		var step := INF if was == null else Vector2(b[X] - was[X], b[Z] - was[Z]).length()
		if step > REBORN_M:
			if not b[OFF] and _inside(b):
				_popped += 1
			if was != null:
				_gone(was)
			continue
		if b[OFF] or was[OFF]:
			continue
		if step > maxf(b[SPEED], was[SPEED]) * _dt + STEP_SLACK_M:
			_lurch += 1
			if _lurch_worst.is_empty() or step > _lurch_worst[0]:
				_lurch_worst = [step, was, b]
		if b[SPEED] > MOVING_MS and was[SPEED] > MOVING_MS and step < STILL_M:
			_stalled += 1
			if _stall_case.is_empty():
				_stall_case = [was, b]
	for id: int in before:
		if not now.has(id):
			_gone(before[id])


func _gone(was: Array) -> void:
	if was[OFF] or not _inside(was):
		return
	_vanished += 1
	if _vanish_case.is_empty():
		_vanish_case = [was[ID], was[TYPE], was[SPEED], Vector2(was[X], was[Z])]


func _bodies_met(watch: Dictionary) -> void:
	_in_another += int(watch.in_another)
	if float(watch.deepest_m) > _deepest:
		_deepest = watch.deepest_m
		_deepest_pair = watch.deepest_pair
	var pair: Array = watch.deepest_pair
	if pair.size() == 2:
		var a: int = pair[0][ID]
		var b: int = pair[1][ID]
		var key := "%d-%d" % [mini(a, b), maxi(a, b)]
		if not _pairs.has(key):
			_pairs[key] = [watch.deepest_m, pair]
	for c: Array in watch.footway_cases:
		_over[c[0]] = true
		if _footway_case.size() < 4:
			_footway_case.append(c)
