## The shoot_test's cases of who hears: a round that lands near a crowd, a blast among it.
class_name ShootFrightCases
extends RefCounted

const BLAST_OFF_M := 30.0
const BLAST_FORCE := 2
const BLAST_LOOKS := 12
const BLAST_TICKS := 40
const HEARD_M := 40.0
const CROWD := 2000
const HEARD_OFF_M := 1.5
const REACT_TICKS := 30
const FLEE_TICKS := 200 # ten seconds: inside the shortest fright, 18 s by the person, and its hold
const AWAY_SHARE := 0.7

var _check: Callable
var _shoot: Callable
var _walker_of: Callable


func _init(check: Callable, shoot: Callable, walker_of: Callable) -> void:
	_check = check
	_shoot = shoot
	_walker_of = walker_of


## A shot that hits nobody: those near stop, then go away from it.
func reactions() -> void:
	var found: Dictionary = CityStore.query(&"shot_targets", [CROWD])
	if found.walkers.is_empty():
		_check.call(false, "there are pedestrians to hear the shot")
		return
	var at: Vector3 = found.walkers[0].position
	var near := []
	for w in found.walkers:
		if w.position.distance_to(at) < HEARD_M:
			near.append(w)
	var round: Dictionary = _shoot.call(at + Vector3(HEARD_OFF_M, 0, HEARD_OFF_M))
	_check.call(round.get("hit", 1) == 0, "the shot lands on the ground, hitting nobody %s" % [round])
	CityStore.step_ticks(REACT_TICKS)
	var acts: Dictionary = CityStore.query(&"people_census", [Vector3.ZERO, 0.0]).acts
	var reacting: int = int(acts.get("crouch", 0)) + int(acts.get("photo", 0)) + int(acts.get("look", 0))
	print("who heard: ", near.size(), " perto; ", acts)
	_check.call(reacting > 0, "who hears the shot stops: %s" % [acts])
	CityStore.step_ticks(FLEE_TICKS)
	var away := 0
	var left := 0
	for w in near:
		var now: Dictionary = _walker_of.call(w.id)
		if now.is_empty():
			continue
		left += 1
		if now.position.distance_to(at) > w.position.distance_to(at):
			away += 1
	print("far from the shot after: ", away, " de ", left)
	_check.call(left == 0 or float(away) / left >= AWAY_SHARE, "who heard runs far away (%d of %d)" % [away, left])


# A blast is a stampede: those near go down or crouch where they are, and none stays on its way toward it.
func stampede() -> void:
	var found: Array = CityStore.query(&"shot_targets", [CROWD]).walkers
	if found.is_empty():
		_check.call(false, "there are pedestrians for the blast")
		return
	var at: Vector3 = found[0].position + Vector3(BLAST_OFF_M, 0, BLAST_OFF_M)
	CityStore.dispatch(&"strike", [at, BLAST_FORCE])
	var down := 0
	for i in BLAST_LOOKS:
		CityStore.step_ticks(BLAST_TICKS)
		down = maxi(down, int(CityStore.query(&"people_census", [Vector3.ZERO, 0.0]).acts.get("crouch", 0)))
	print("in the blast, crouching at most: ", down)
	_check.call(down > 0, "in the blast someone falls or crouches")
	CityStore.dispatch(&"undo")
