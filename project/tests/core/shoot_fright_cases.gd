## The shoot_test's cases of who hears: a round that lands near a crowd, a blast among it.
class_name ShootFrightCases
extends RefCounted

const BLAST_OFF_M := 5.0
const CROWD_M := 25.0 # a crowd round a walker: those this near
const BLAST_FORCE := 2
const BLAST_LOOKS := 12
const BLAST_TICKS := 40
const HEARD_M := 40.0
const CROWD := 2000
const HEARD_OFF_M := 1.5
const REACT_TICKS := 30
const FLEE_TICKS := 200 # ten seconds: inside the shortest fright, 18 s by the person, and its hold
const AWAY_SHARE := 0.7

var t: Node # the shoot_test (TestCase): its check, ticks and aim
var _shoot: Callable
var _walker_of: Callable


func _init(test: Node, shoot: Callable, walker_of: Callable) -> void:
	t = test
	_shoot = shoot
	_walker_of = walker_of


## A shot that hits nobody: those near stop, then go away from it.
func reactions() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [CROWD])
	if found.walkers.is_empty():
		t.check(false, "there are pedestrians to hear the shot")
		return
	var at: Vector3 = found.walkers[0].position
	var near := []
	for w in found.walkers:
		if w.position.distance_to(at) < HEARD_M:
			near.append(w)
	var round: Dictionary = _shoot.call(at + Vector3(HEARD_OFF_M, 0, HEARD_OFF_M))
	t.check(round.get("hit", 1) == 0, "the shot lands on the ground, hitting nobody %s" % [round])
	t.ticks(REACT_TICKS)
	var acts: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0]).acts
	var reacting: int = int(acts.get("crouch", 0)) + int(acts.get("photo", 0)) + int(acts.get("look", 0))
	print("who heard: ", near.size(), " near; ", acts)
	t.check(reacting > 0, "who hears the shot stops: %s" % [acts])
	t.ticks(FLEE_TICKS)
	var away := 0
	var left := 0
	for w in near:
		var now: Dictionary = _walker_of.call(w.id)
		if now.is_empty():
			continue
		left += 1
		if now.position.distance_to(at) > w.position.distance_to(at):
			away += 1
	print("far from the shot after: ", away, " of ", left)
	t.check(left == 0 or float(away) / left >= AWAY_SHARE, "who heard runs far away (%d of %d)" % [away, left])


# A blast is a stampede: those near go down or crouch where they are, and none stays on its way toward it.
func stampede() -> void:
	var found: Array = CityStore.query(&"shot_targets", [CROWD]).walkers
	if found.is_empty():
		t.check(false, "there are pedestrians for the blast")
		return
	# by the busiest walker, within the stampede's trip reach (traffic.json ped_stampede_trip_m 30 m) of those round it; force 2 knocks
	# nobody down there (its eardrum odds are nil): who is on the ground after it has tripped
	var at := LookAim.busiest(found.map(func(w: Dictionary) -> Vector3: return w.position), CROWD_M) + Vector3(BLAST_OFF_M, 0, BLAST_OFF_M)
	CityStore.dispatch(&"strike", [at, BLAST_FORCE])
	var down := 0
	var fell := 0 # tripped in the stampede: on the ground, a body of their own (ped_fright.cpp trip_now), not crouched
	for i in BLAST_LOOKS:
		t.ticks(BLAST_TICKS)
		down = maxi(down, int(CityStore.query(&"people_census", [Vector3.ZERO, 0.0]).acts.get("crouch", 0)))
		fell = maxi(fell, CityStore.query(&"shot_targets", [CROWD]).walkers.filter(func(w: Dictionary) -> bool: return w.fallen).size())
	print("in the blast, crouching at most: ", down, ", on the ground at most: ", fell)
	t.check(down > 0, "in the blast someone crouches")
	t.check(fell > 0, "in the stampede someone trips and falls to the ground (%d at most)" % fell)
	CityStore.dispatch(&"undo")
