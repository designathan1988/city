## The shoot_test's cases of somebody lying wounded among the living (ShootBodyCases brings them down): the crowd goes round the body
## and never stands in it, and with the walking window gone from them they stay till they bleed out, their body kept, dead; the dead lie
## in the simulation, a body the rounds meet and the crowd goes round.
class_name ShootLyingCases
extends RefCounted

const LIMIT := 40
const BLEED_TICKS := 2400
const BODY_SETTLE_FRAMES := 30
const AWAY_M := 2000.0 # well past where the walking window takes walkers away (traffic.json: 600 m x 1.2)
const AWAY_TICKS := 60 # three seconds (tuning.json tick_hz 20): the window has passed over everybody, and a leg's wound (3 hp a second) leaves them alive
const SAME_SPOT_M := 0.5
const BODY_HEIGHT_M := 1.85 # blast.json shot.body_height_m: somebody lying is that long by their scale (ped_system.h lying_half)
const LYING_R := 0.3 # and that thick either side of their middle line (ped_system.h)
const GRACE_TICKS := 20 # a second: who stands where a body fell (or gets up there) steps off it within this
const SAMPLE_TICKS := 5
const PASS_TICKS := 200 # ten seconds of passers, while the casualty bleeds (a leg's wound: 3 hp a second, from some 45)
const MAIM_FORCE := 5 # 50 kg: a limb can go from 120.7 kPa, some 11 m off, and death begins at some 7 m (blast.json)
const MAIM_OFF_M := 9.0
const MAIM_TRIES := 12
const STREET_LOOK_M := 3.0 # round the body's middle: its feet are within half a body of it
const DEAD_ABOVE_M := 4.0 # a round straight down from this far over a dead body's middle meets its trunk

var t: Node # the shoot_test (TestCase): its check and ticks
var _walker_of: Callable
var _lamed: Callable # ShootBodyCases._lamed: walker id, the round's way -> them, down for good and bleeding (awaited)
var _census: Callable


func _init(test: Node, walker_of: Callable, lamed: Callable, census: Callable) -> void:
	t = test
	_walker_of = walker_of
	_lamed = lamed
	_census = census


func _frames(n: int) -> void:
	for f in n:
		await t.get_tree().process_frame


## One of two walking together shot down for good: the other, and whoever passes, steps off and round the body lying there and never
## stands in it (Pwalkers-15: the crowd once saw somebody lying as a walker on their feet, a point, while the body lies along the ground;
## ped_seen.cpp; and lay them along the way they walked while the body fell away from the round: damage_event.h fall_way). A second
## first: who stood where the body fell steps off it at their pace aside.
func gone_round() -> void:
	var standing: Array = []
	for w: Dictionary in CityStore.query(&"shot_targets", [LIMIT * 4]).walkers:
		if not w.fallen:
			standing.push_back(w)
	var pair := LookAim.nearest_pair(standing.map(func(w: Dictionary) -> Vector3: return w.position), Vector2.ZERO, INF)
	if not t.check(not pair.is_empty(), "two people walking together, for a body among them"):
		return
	var by_pair := func(a: Dictionary, w: Dictionary) -> Dictionary: return w if w.position.distance_to(pair[0]) < a.position.distance_to(pair[0]) else a
	var hurt: Dictionary = standing.reduce(by_pair)
	var mate: Dictionary = standing.filter(func(w: Dictionary) -> bool: return w.id != hurt.id).reduce(by_pair)
	# the round from the side away from the other: the body goes down toward them, onto where they stand (the wounded plan's 00)
	var toward: Vector3 = mate.position - hurt.position
	toward.y = 0.0
	var down: Dictionary = await _lamed.call(hurt.id, toward.normalized())
	if not t.check(not down.is_empty() and down.fallen and int(down.lost) != 0, "one of the two down for good (%s)" % [down]):
		return
	var half: float = BODY_HEIGHT_M * down.scale * 0.5
	var middle := Vector2(down.position.x, down.position.z)
	var facing: Vector2 = down.facing
	var foot := middle - facing * half
	var head := middle + facing * half
	var inside := {} # walker id -> ticks on their feet in the body without a break, now
	var longest := 0
	var longest_who := {}
	var trail := [] # the other's distance from the body's middle line at each look, "f" while on the ground
	for k in PASS_TICKS / SAMPLE_TICKS:
		t.ticks(SAMPLE_TICKS)
		var still: Dictionary = _walker_of.call(down.id)
		if still.is_empty() or not still.fallen:
			break # dead (a dead body now: dead_met) or up again
		var now_in := {}
		for w: Dictionary in CityStore.query(&"shot_targets", [LookAim.WALKERS_READ]).walkers:
			var at := Vector2(w.position.x, w.position.z)
			var d := at.distance_to(Geometry2D.get_closest_point_to_segment(at, foot, head))
			if w.id == mate.id:
				trail.push_back("f" if w.fallen else "%.2f" % d)
			if w.id == down.id or w.fallen or d >= LYING_R:
				continue
			now_in[w.id] = int(inside.get(w.id, 0)) + SAMPLE_TICKS
			if now_in[w.id] > longest:
				longest = now_in[w.id]
				longest_who = w
		inside = now_in
	print("the other by the body (m from its middle line, each quarter second): ", trail)
	t.check(longest <= GRACE_TICKS, "nobody stands in the body lying there longer than a step off it takes, the one they walked with included (%d ticks at most: %s)" % [longest, longest_who])


## The dead lie in the simulation (Pwalkers-11: the dead left it, a round passed through their bodies and the crowd through them): a round
## from above meets a dead body where it lies, the same body takes it (no second one), and whoever passes steps round it, never in it.
func dead_met() -> void:
	var dead: Array = CityStore.query(&"shot_targets", [LIMIT]).dead
	if not t.check(not dead.is_empty(), "the dead lie in the simulation (%d dead bodies kept)" % dead.size()):
		return
	var on_walk: Array = dead.filter(func(d: Dictionary) -> bool: return int(d.sidewalk) >= 0) # lying on a sidewalk, where the crowd passes
	var body: Dictionary = on_walk[0] if not on_walk.is_empty() else dead[0]
	var round: Dictionary = CityStore.dispatch(&"shoot", [body.position + Vector3(0, DEAD_ABOVE_M, 0), Vector3.DOWN])
	await _frames(BODY_SETTLE_FRAMES)
	t.check(int(round.get("hit", 0)) == 1 and not round.get("killed", true), "a round from above meets the dead body where it lies (%s)" % [round])
	# by its own id, not the town's count of bodies, which another getting up in these frames changes: a second body for it would carry the
	# same id (BodyRenderer census who)
	var of_it: int = Array(_census.call().who).count(body.id)
	t.check(of_it == 1, "the dead body takes the round, no second body (%d bodies of id %d)" % [of_it, body.id])
	var half: float = BODY_HEIGHT_M * body.scale * 0.5
	var middle := Vector2(body.position.x, body.position.z)
	var facing: Vector2 = body.facing
	var inside := {}
	var longest := 0
	for k in PASS_TICKS / SAMPLE_TICKS:
		t.ticks(SAMPLE_TICKS)
		var now_in := {}
		for w: Dictionary in CityStore.query(&"shot_targets", [LookAim.WALKERS_READ]).walkers:
			var at := Vector2(w.position.x, w.position.z)
			if w.fallen or at.distance_to(Geometry2D.get_closest_point_to_segment(at, middle - facing * half, middle + facing * half)) >= LYING_R:
				continue
			now_in[w.id] = int(inside.get(w.id, 0)) + SAMPLE_TICKS
			longest = maxi(longest, now_in[w.id])
		inside = now_in
	t.check(longest <= GRACE_TICKS, "nobody stands in a dead body longer than a step off it takes (%d ticks at most)" % longest)


## A blast beside walkers until it takes somebody's limb: they are down for good, bleeding, the limb gone (Pwalkers-13, 16: the blast took
## the walker away, or left the limb); the street under them demolished, they lie on (Pwalkers-17: a street gone took everybody on it);
## they bleed out where the body lies, kept dead. The blasts' draws are the blow's and the walker's: several walkers are tried.
func maimed_by_blast() -> void:
	var maimed := {}
	for w: Dictionary in CityStore.query(&"shot_targets", [LIMIT]).walkers.slice(0, MAIM_TRIES):
		if w.fallen:
			continue
		var before := {} # those a round maimed already: not the blast's
		for o: Dictionary in CityStore.query(&"shot_targets", [LookAim.WALKERS_READ]).walkers:
			if int(o.lost) != 0:
				before[o.id] = true
		CityStore.dispatch(&"strike", [w.position + Vector3(MAIM_OFF_M, 0, 0), MAIM_FORCE])
		for o: Dictionary in CityStore.query(&"shot_targets", [LookAim.WALKERS_READ]).walkers:
			if not before.has(o.id) and o.fallen and int(o.lost) != 0 and o.bleed > 0.0 and o.hp > 0.0:
				maimed = o
		if not maimed.is_empty():
			break
	if not t.check(not maimed.is_empty(), "a blast takes somebody's limb: down for good, bleeding (%s)" % [maimed]):
		return
	var kept: int = _census.call().kept
	var gone: Dictionary = CityStore.dispatch(&"demolish", [_carriageway_of(maimed)])
	t.check(gone.get("ok", false), "the street under them demolished (%s)" % [gone])
	t.ticks(SAMPLE_TICKS)
	var after: Dictionary = _walker_of.call(maimed.id)
	t.check(not after.is_empty() and after.fallen and after.position.distance_to(maimed.position) <= SAME_SPOT_M, "the street under them gone, they lie on (%s, %s before)" % [after, maimed])
	t.ticks(BLEED_TICKS)
	await _frames(BODY_SETTLE_FRAMES)
	t.check(_walker_of.call(maimed.id).get("position", Vector3.INF).distance_to(maimed.position) > SAME_SPOT_M and _census.call().kept > kept, "who lost a limb to the blast bleeds out where they lay, kept dead (%d kept, %d before)" % [_census.call().kept, kept])


## The middle of the carriageway of the street walker `w` lies on (walker_bodies: its street, row 5), where a click takes that street
## (one on the footway takes the street piece there first, and by a junction the nearest street may be the other).
func _carriageway_of(w: Dictionary) -> Vector3:
	var seg := -1
	for row: Array in CityStore.query(&"walker_bodies", [w.position, STREET_LOOK_M]).bodies:
		if row[0] == w.id:
			seg = row[5]
	var path: PackedVector3Array = CityStore.query(&"segment_path", [seg]) if seg >= 0 else PackedVector3Array()
	print("their street %d, its middle at %s" % [seg, path[path.size() / 2] if not path.is_empty() else w.position])
	return path[path.size() / 2] if not path.is_empty() else Vector3(w.position)


## The camera far off, the walking window round it leaves the wounded `down` lying there: they stay till they bleed out, and their body
## stays, dead (Pwalkers-14: the window, or the one they walked with leaving it, once took them away, their body left lying with nobody
## to die in it). A level camera's ground is its own place (city_core.cpp ground_seen), which the core makes the window's middle each
## frame. The last case: the town's crowd and traffic go with the window.
func bleeds_out_away(down: Dictionary) -> void:
	if down.is_empty():
		return
	var kept: int = _census.call().kept
	var cam := Camera3D.new()
	t.add_child(cam)
	cam.make_current()
	cam.global_position = down.position + Vector3(AWAY_M, 0, 0)
	await _frames(1)
	t.ticks(AWAY_TICKS)
	var away: Dictionary = _walker_of.call(down.id)
	t.check(not away.is_empty() and away.position.distance_to(down.position) <= SAME_SPOT_M and away.bleed > 0.0, "the walking window gone from them, the wounded still lie there bleeding (%s)" % [away])
	t.ticks(BLEED_TICKS)
	await _frames(BODY_SETTLE_FRAMES)
	t.check(_census.call().kept > kept, "who bled out stays where they lay, dead (%d kept, %d before)" % [_census.call().kept, kept])
	cam.global_position = Vector3.ZERO # the window back at the town's middle; the camera kept, which the terrain now follows
