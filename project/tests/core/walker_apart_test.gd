## The crowd's one rule for the room between two walkers, on bodies laid out here (probe `walkers_apart`: each body's place, the step
## it would take this tick in the world and how far either side of that step's line it may give way; back comes the step each takes):
## whatever their ways, no two end the tick nearer than two bodies, nor nearer than they stood when they stood nearer already; a
## walker nobody is in the way of takes the step it chose; no step goes further ahead than chosen, nor back, nor further aside than it
## may; and two abreast whose ways converge on one point walk on side by side and both come to it.
## Run: godot --path project res://tests/core/walker_apart_test.tscn -- --no-flow
extends TestCase

const ROUNDING_M := 0.0001 # places are float metres
const ASIDE_M := 0.026 # how far a walker may give way in a tick: half its step ahead (traffic.json ped_lateral_share)
## Each scene: its bodies' places and steps (metres, x and z), those that must take the step they chose (`free`) and those that must
## still move (`moves`).
const SCENES := [
	{"name": "two abreast whose lines converge on one approach point (walker_motion_test's 568 and 569)",
		"at": [Vector2(-47.755, 102.839), Vector2(-47.750, 103.315)], "step": [Vector2(0.0498, 0.0127), Vector2(0.0512, -0.0063)],
		"free": [], "moves": [0, 1]},
	{"name": "two walking straight at each other",
		"at": [Vector2(0, 0), Vector2(1, 0)], "step": [Vector2(0.3, 0), Vector2(-0.3, 0)], "free": [], "moves": [0, 1]},
	{"name": "two whose ways cross at a right angle",
		"at": [Vector2(0, 0), Vector2(0.5, 0.5)], "step": [Vector2(0.2, 0), Vector2(0, -0.2)], "free": [], "moves": [0, 1]},
	{"name": "one walking into one who stands",
		"at": [Vector2(0, 0), Vector2(0.6, 0)], "step": [Vector2(0.2, 0), Vector2(0, 0)], "free": [1], "moves": [0]},
	{"name": "two walking away from each other",
		"at": [Vector2(0, 0), Vector2(0.5, 0)], "step": [Vector2(-0.1, 0), Vector2(0.1, 0)], "free": [0, 1], "moves": []},
	{"name": "two far apart",
		"at": [Vector2(0, 0), Vector2(5, 0)], "step": [Vector2(0.07, 0), Vector2(-0.07, 0)], "free": [0, 1], "moves": []},
	{"name": "two closing on one who stands between them",
		"at": [Vector2(0, 0), Vector2(0.6, 0), Vector2(1.2, 0)], "step": [Vector2(0.2, 0), Vector2(0, 0), Vector2(-0.2, 0)], "free": [1], "moves": [0, 2]},
	{"name": "two in one another already: the one walking on is not held, the one behind takes no step toward it",
		"at": [Vector2(0, 0), Vector2(0.4, 0)], "step": [Vector2(0.05, 0), Vector2(0.05, 0)], "free": [1], "moves": []},
	{"name": "two abreast walking the same way",
		"at": [Vector2(0, 0), Vector2(0, 0.5)], "step": [Vector2(0.07, 0), Vector2(0.07, 0)], "free": [0, 1], "moves": []},
	{"name": "two passing each other, a body's width clear between them",
		"at": [Vector2(0, 0), Vector2(0.3, 0.7)], "step": [Vector2(0.07, 0), Vector2(-0.07, 0)], "free": [0, 1], "moves": []},
]
## The pair of walker_motion_test walked on tick by tick: each on its own straight line to the one point both were given, at its pace.
const WALK_FROM := [Vector2(-47.755, 102.839), Vector2(-47.750, 103.315)]
const WALK_TO := Vector2(-46.5, 103.16)
const WALK_PACE_M := 0.0515 # a tick's step ahead
const WALK_TICKS := 60 # twice the ticks either needs alone


func _ready() -> void:
	if not boot():
		finish()
		return
	for scene: Dictionary in SCENES:
		_play(scene)
	_walk_on()
	finish()


## The steps the bodies take, each allowed ASIDE_M either side of its step's line.
func _taken(at: PackedVector2Array, steps: PackedVector2Array) -> Dictionary:
	var aside := PackedFloat32Array()
	aside.resize(at.size())
	aside.fill(ASIDE_M)
	return CityStore.query(&"walkers_apart", [at, steps, aside])


func _play(scene: Dictionary) -> void:
	var at := PackedVector2Array(scene.at)
	var steps := PackedVector2Array(scene.step)
	var held := _taken(at, steps)
	var touch: float = held.touch
	var taken: PackedVector2Array = held.taken
	check(taken.size() == at.size(), "%s: a step for every body (%d of %d)" % [scene.name, taken.size(), at.size()])
	if taken.size() != at.size():
		return
	for i in at.size():
		var way := steps[i].normalized()
		var ahead := taken[i].dot(way)
		var aside := absf(taken[i].dot(way.orthogonal()))
		check(ahead >= -ROUNDING_M and ahead <= steps[i].length() + ROUNDING_M, "%s: body %d steps no further ahead than it chose, and not back (%.4f of %.4f m)" % [scene.name, i, ahead, steps[i].length()])
		check(aside <= ASIDE_M + ROUNDING_M, "%s: body %d gives way no further than it may (%.4f m)" % [scene.name, i, aside])
	for i in at.size():
		for j in range(i + 1, at.size()):
			var before := at[i].distance_to(at[j])
			var now := (at[i] + taken[i]).distance_to(at[j] + taken[j])
			check(now >= minf(touch, before) - ROUNDING_M, "%s: bodies %d and %d end the tick %.4f m apart (two bodies %.2f m, before %.4f m)" % [
					scene.name, i, j, now, touch, before])
	for i: int in scene.free:
		check(taken[i].distance_to(steps[i]) <= ROUNDING_M, "%s: body %d takes the step it chose (%s of %s)" % [scene.name, i, taken[i], steps[i]])
	for i: int in scene.moves:
		check(taken[i].length() > 0.0, "%s: body %d still moves (%s)" % [scene.name, i, taken[i]])


## Two abreast, each on its own line to the same point: they walk on side by side, never in one another, and both come to the end of
## their lines (each at its place beside the point), in no more than twice the time either needs alone.
func _walk_on() -> void:
	var at := PackedVector2Array(WALK_FROM)
	var way: Array[Vector2] = []
	var length: Array[float] = []
	var walked: Array[float] = [0.0, 0.0]
	for p: Vector2 in WALK_FROM:
		way.append((WALK_TO - p).normalized())
		length.append(p.distance_to(WALK_TO))
	var nearest := INF
	var touch := 0.0
	var ticks_taken := 0
	while ticks_taken < WALK_TICKS and (walked[0] < length[0] - ROUNDING_M or walked[1] < length[1] - ROUNDING_M):
		var steps := PackedVector2Array()
		for i in at.size():
			steps.append(way[i] * minf(WALK_PACE_M, maxf(0.0, length[i] - walked[i])))
		var held := _taken(at, steps)
		touch = held.touch
		for i in at.size():
			at[i] += held.taken[i]
			walked[i] += held.taken[i].dot(way[i])
		nearest = minf(nearest, at[0].distance_to(at[1]))
		ticks_taken += 1
	print("two abreast to one point: %d ticks, walked %.3f of %.3f m and %.3f of %.3f m, never nearer than %.4f m, they end at %s and %s" % [
			ticks_taken, walked[0], length[0], walked[1], length[1], nearest, at[0], at[1]])
	check(nearest >= touch - ROUNDING_M, "two abreast to one point are never in one another on the way: nearest %.4f m (two bodies %.2f m)" % [nearest, touch])
	for i in at.size():
		check(walked[i] >= length[i] - ROUNDING_M, "two abreast to one point: body %d comes to the end of its line (%.3f of %.3f m in %d ticks)" % [i, walked[i], length[i], ticks_taken])
