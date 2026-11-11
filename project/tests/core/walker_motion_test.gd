## The people on foot as the player sees them round the camera, tick by tick: no walker jumps farther than its pace carries it,
## none turns faster than a body turns, none appears or vanishes on a street in view, none stands in another, none walks the
## carriageway away from a crossing (nor would round any corner of the town), none stands in a vehicle or in a street piece. Run: godot --path project res://tests/core/walker_motion_test.tscn -- --no-flow
extends TestCase

const TOWN_MAP := 0
const SEED := 7
const HOUR := 8.0 # the morning's walkers
const WARM_TICKS := 1200
const WATCH_TICKS := 600
const VIEW_M := 300.0
const EDGE_M := 40.0 # a walker this near the view's edge may walk into it or out of it
const CATCH := 1.2 # traffic.json ped_company_catch: closing on the one it walks with, a walker goes this much faster than its pace
const STEP_SLACK_M := 0.05 # a stripe change's step aside over a tick, and the tick's rounding
const STAY_M := 0.001 # a body put onto its sidewalk moves less than this: the rounding of the curve's nearest point
const REBORN_M := 5.0 # farther in one tick is another walker given the same index
const MOVING_M := 0.01 # a walker that moved this much in a tick is walking
const TURN_RAD_S := 6.0 # traffic.json ped_turn_rad_s: how fast a body turns
const TURN_SLACK_RAD := 0.05
const LEAST_SEEN := 20
const MOST_TOLD := 40 # cases printed, each walker's first of a kind
const PIECES_NEAR_M := 4.0 # a walker found in a street piece: the pieces of its sidewalk this near it along are printed
const FOLLOW_REACH_M := 5000.0 # `--follow=<walker>`: its row printed each tick, wherever it is in the town
const AROUND_M := 3.0 # a case told: the walkers this near it are printed too (who holds it where it is)
const AT := Vector3.ZERO # the window's middle: the camera of a test stands at the origin
# the probe's row of a walker (city_core_people.cpp walker_bodies)
const ID := 0
const X := 1
const Z := 2
const SPEED := 3
const MODE := 4
const FX := 12
const FZ := 13
const SEG := 5
const SIDE := 6
const ARC := 7
const WITH := 9
const COMPANY_REACH_M := 6.0 # traffic.json ped_company_reach_m: within it two who walk together keep level
const MODE_SIDEWALK := 0
const MODE_KERB := 3 # PedMode::KERB: between a car at the kerb and the footway (people get out of cars and into them)
const MODE_LOT := 4 # PedMode::LOT: between a lot's door and the sidewalk (people come out of doors and go in at them)

var _dt := 0.0
var _jumps := 0
var _jump_worst: Array = []
var _turns := 0
var _turn_worst: Array = []
var _popped := 0
var _pop_case: Array = []
var _vanished := 0
var _vanish_case: Array = []
var _in_another := 0
var _another_case: Array = []
var _off_way := 0
var _off_case: Array = []
var _in_vehicle := 0
var _vehicle_case: Array = []
var _in_furniture := 0
var _apart := 0
var _apart_case: Array = []
var _pairs := 0
var _seen := {}
var _told := {}
var _last_piece := {} # each walker's last case in a street piece, and how many ticks it was in one
var _stale := 0 # segments a tick whose walkers' furniture is not the owner's placement


func _ready() -> void:
	if not boot():
		finish()
		return
	_dt = 1.0 / float(DataTables.tuning("sim_tick_hz"))
	new_map(TOWN_MAP, SEED, true)
	var onto: Dictionary = CityStore.query(&"onto_sidewalk_moves")
	check(int(onto.laid) > 0 and float(onto.worst_m) < STAY_M, "a body put onto its sidewalk where it stands stays there: the farthest moved %.3f m of %d laid, at (street, side, arc, place across, metres) %s" % [onto.worst_m, onto.laid, onto.worst_case])
	var corners: Dictionary = CityStore.query(&"corner_ways")
	check(int(corners.ways) > 0 and int(corners.off) == 0, "every way round a corner with no crosswalk keeps off the carriageway: %d of %d step on it, the first (node, from, side, to, place across, where, the run, the node, its arms: dir x, dir z, asphalt left, right, setback; its corners: mode, trims, radius, centre) %s" % [corners.off, corners.ways, corners.cases])
	CityStore.query(&"set_city_hour", [HOUR])
	ticks(WARM_TICKS)
	var before := _walkers(CityStore.query(&"walker_bodies", [AT, VIEW_M]))
	var follow := _followed()
	for i in WATCH_TICKS:
		ticks(1)
		if follow >= 0:
			for w: Array in CityStore.query(&"walker_bodies", [AT, FOLLOW_REACH_M]).bodies:
				if w[ID] == follow:
					print("followed %d: %s" % [i, w])
		var watch: Dictionary = CityStore.query(&"walker_bodies", [AT, VIEW_M])
		var now := _walkers(watch)
		_compare(before, now)
		_met(watch, before)
		_together(before, now)
		before = now
	for id: int in _last_piece:
		var last: Array = _last_piece[id][1]
		print("in a street piece %d ticks, the last: %s" % _last_piece[id])
		for spot: Dictionary in CityStore.query(&"furniture_spots", [last[0][SEG]]):
			if spot.side == last[0][SIDE] and absf(spot.v - last[0][ARC]) < PIECES_NEAR_M:
				print("  the sidewalk's piece near it: %s" % spot)
	print("rows: id, x, z, pace, mode, seg, side, s, u, with, mate, crossing arm, heading x, heading z, rest, node, next point")
	check(_seen.size() >= LEAST_SEEN, "the town walks round the camera: %d walkers seen" % _seen.size())
	check(_jumps == 0, "no walker jumps farther than its pace carries it in a tick: %d, the worst (metres, before, after) %s" % [_jumps, _jump_worst])
	check(_turns == 0, "no walker turns faster than a body turns: %d, the worst (radians, before, after) %s" % [_turns, _turn_worst])
	check(_popped == 0, "no walker appears on a street in view: %d, the first %s" % [_popped, _pop_case])
	check(_vanished == 0, "no walker vanishes from a street in view: %d, the first %s" % [_vanished, _vanish_case])
	check(_in_another == 0, "no walker stands in another: %d times, the first %s" % [_in_another, _another_case])
	check(_off_way == 0, "no walker walks the carriageway away from a crossing: %d times, the first %s" % [_off_way, _off_case])
	check(_in_vehicle == 0, "no walker stands in a vehicle: %d times, the first %s" % [_in_vehicle, _vehicle_case])
	check(_in_furniture == 0, "no walker stands in a street piece: %d times" % _in_furniture)
	check(_stale == 0, "the furniture the walkers keep is the owner's placement: %d stale segments over the ticks" % _stale)
	check(_pairs > 0 and _apart == 0, "two who walk together within %.0f m on one sidewalk stay within it: left behind %d of %d times, the first %s" % [COMPANY_REACH_M, _apart, _pairs, _apart_case])
	finish()


## The walker `--follow=<index>` names, to print tick by tick (how a case came about); -1 with none.
func _followed() -> int:
	for arg: String in OS.get_cmdline_user_args():
		if arg.begins_with("--follow="):
			return int(arg.get_slice("=", 1))
	return -1


## How far apart along it two who walk together are, both on the same sidewalk; -1 when they are not.
func _pair_gap(rows: Dictionary, id: int) -> float:
	var w: Variant = rows.get(id)
	var lead: Variant = rows.get(w[WITH]) if w != null and w[WITH] >= 0 else null
	if lead == null or w[MODE] != MODE_SIDEWALK or lead[MODE] != MODE_SIDEWALK or w[SEG] != lead[SEG] or w[SIDE] != lead[SIDE]:
		return -1.0
	return absf(w[ARC] - lead[ARC])


## A pair within reach of each other on a sidewalk keeps level by its pace (the reference game's rule: only within reach; a pair a
## crossing split is not brought together again): a tick later, still on that sidewalk, neither has been left out of reach.
func _together(before: Dictionary, now: Dictionary) -> void:
	for id: int in now:
		var was := _pair_gap(before, id)
		var gap := _pair_gap(now, id)
		if was < 0.0 or was > COMPANY_REACH_M or gap < 0.0:
			continue
		_pairs += 1
		if gap > COMPANY_REACH_M:
			_apart += 1
			if _apart_case.is_empty():
				_apart_case = [now[id], now[now[id][WITH]]]


## The probe's rows by index.
func _walkers(watch: Dictionary) -> Dictionary:
	var out := {}
	for w: Array in watch.bodies:
		out[w[ID]] = w
		_seen[w[ID]] = true
	return out


func _inside(w: Array) -> bool:
	return Vector2(w[X] - AT.x, w[Z] - AT.z).length() < VIEW_M - EDGE_M


func _compare(before: Dictionary, now: Dictionary) -> void:
	for id: int in now:
		var w: Array = now[id]
		var was: Variant = before.get(id)
		var step := INF if was == null else Vector2(w[X] - was[X], w[Z] - was[Z]).length()
		if step > REBORN_M:
			if not _at_a_door(w) and _inside(w):
				_popped += 1
				if _pop_case.is_empty():
					_pop_case = w
			if was != null:
				_gone(was)
			continue
		if step > maxf(w[SPEED], was[SPEED]) * CATCH * _dt + STEP_SLACK_M:
			_jumps += 1
			if _jump_worst.is_empty() or step > _jump_worst[0]:
				_jump_worst = [step, was, w]
		var turn := absf(Vector2(was[FX], was[FZ]).angle_to(Vector2(w[FX], w[FZ])))
		if step > MOVING_M and turn > TURN_RAD_S * _dt + TURN_SLACK_RAD:
			_turns += 1
			if _turn_worst.is_empty() or turn > _turn_worst[0]:
				_turn_worst = [turn, was, w]
	for id: int in before:
		if not now.has(id):
			_gone(before[id])


## Somebody coming out of a door or a car, or going in: the door or the car is where it appears or goes.
func _at_a_door(w: Array) -> bool:
	return w[MODE] == MODE_LOT or w[MODE] == MODE_KERB


func _gone(was: Array) -> void:
	if _at_a_door(was) or not _inside(was):
		return
	_vanished += 1
	if _vanish_case.is_empty():
		_vanish_case = was


func _met(watch: Dictionary, before: Dictionary) -> void:
	_in_another += int(watch.in_another)
	if _another_case.is_empty() and not watch.in_another_cases.is_empty():
		_another_case = watch.in_another_cases[0]
	_off_way += int(watch.off_way)
	if _off_case.is_empty() and not watch.off_way_cases.is_empty():
		_off_case = watch.off_way_cases[0]
	_in_vehicle += int(watch.in_vehicle)
	if _vehicle_case.is_empty() and not watch.in_vehicle_cases.is_empty():
		_vehicle_case = watch.in_vehicle_cases[0]
	_in_furniture += int(watch.in_furniture)
	_stale += int(watch.stale_furniture)
	for pair: Array in watch.in_another_cases:
		_tell("in another", pair[0][ID], pair, before)
	for w: Array in watch.off_way_cases:
		_tell("off the way", w[ID], w, before)
	for pair: Array in watch.in_vehicle_cases:
		_tell("in a vehicle", pair[0][ID], pair, before)
	for case: Array in watch.in_furniture_cases:
		_tell("in a street piece", case[0][ID], case, before)
		var id: int = case[0][ID]
		_last_piece[id] = [int(_last_piece.get(id, [0])[0]) + 1, case]


## Each walker's first case of a kind, printed once with its row the tick before: what the failure's count is made of, and how the
## walker came to it.
func _tell(kind: String, id: int, case: Array, before: Dictionary) -> void:
	var key := "%s %d" % [kind, id]
	if _told.has(key) or _told.size() >= MOST_TOLD:
		return
	_told[key] = true
	print("case %s: %s" % [kind, case])
	print("  the tick before: %s" % [before.get(id, [])])
	var me: Variant = before.get(id)
	if me == null:
		return
	for w: Array in before.values():
		if w[ID] != id and Vector2(w[X] - me[X], w[Z] - me[Z]).length() < AROUND_M:
			print("  near it: %s" % [w])
