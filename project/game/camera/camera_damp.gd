## A value that follows its target as a critically damped spring: it leaves and arrives with no jerk and never overshoots. The step is the
## exact solution of the spring over `dt`, so the path is the same at any frame rate (Lowe, Critically Damped Ease-In/Ease-Out Smoothing,
## Game Programming Gems 4; Unity's SmoothDamp). `smooth_s` is about the time it takes to get there.
class_name CameraDamp
extends RefCounted

const SHORTEST_S := 1e-4 # a frame's time divides: never under this
const HALF := 0.5

var x := 0.0
var v := 0.0
var _goal: CameraDamp # the spring ahead of this one, for a target that jumps


func snap(value: float) -> void:
	x = value
	v = 0.0
	if _goal != null:
		_goal.snap(value)


func to(target: float, dt: float, smooth_s: float) -> float:
	var w := 2.0 / maxf(smooth_s, SHORTEST_S)
	var e := exp(-w * dt)
	var d := x - target
	var t := (v + w * d) * dt
	x = target + (d + t) * e
	v = (v - w * t) * e
	return x


## For a target that jumps, as a roof does under a moving view: one spring takes a step with its whole acceleration in the first frame, and that is the jolt. A second ahead
## of it, each with half the time, arrives when one would, and starts and ends from rest.
func ease_to(target: float, dt: float, smooth_s: float) -> float:
	if _goal == null:
		_goal = CameraDamp.new()
		_goal.snap(x)
	return to(_goal.to(target, dt, smooth_s * HALF), dt, smooth_s * HALF)
