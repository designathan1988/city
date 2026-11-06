## A value that follows its target as a critically damped spring: it leaves and arrives with no jerk and never overshoots. The step is the
## exact solution of the spring over `dt`, so the path is the same at any frame rate (Lowe, Critically Damped Ease-In/Ease-Out Smoothing,
## Game Programming Gems 4; Unity's SmoothDamp). `smooth_s` is about the time it takes to get there.
class_name CameraDamp
extends RefCounted

const LEAST_S := 1e-4

var x := 0.0
var v := 0.0


func snap(value: float) -> void:
	x = value
	v = 0.0


func to(target: float, dt: float, smooth_s: float) -> float:
	var w := 2.0 / maxf(smooth_s, LEAST_S)
	var e := exp(-w * dt)
	var d := x - target
	var t := (v + w * d) * dt
	x = target + (d + t) * e
	v = (v - w * t) * e
	return x
