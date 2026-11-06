## Moving over the buildings at the city's view, in camera_input_test: the zoom holds. The pull is for the street, and the eye never jumps towards what it crosses.
class_name CameraWalkCases
extends RefCounted

const STEPS := 90
const VIEW_M := 250.0 # the view over the blocks: well past the street, where the pull ends
const RIGHT_M := 6.0
const AHEAD_M := 3.0
const HOLD_M := 1.0 # the ground under the point looked at moves it this much at most
const STREET_M := 25.0
const ROOFS_M := 90.0
const JOLT_STEPS := 240
const JOLT_STEP_M := 0.5
const JOLT_MAX_M := 0.4

var t: Node


func _init(test: Node) -> void:
	t = test


func run() -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	t.rig.jump((b.min + b.max) * 0.5, 35.0, 45.0, VIEW_M)
	await t._frames(1)
	var from: float = t.rig.camera.global_position.distance_to(t.rig.focus)
	var worst := 0.0
	for i in STEPS:
		t.rig.slide(RIGHT_M, AHEAD_M)
		await t._frames(1)
		worst = maxf(worst, absf(t.rig.camera.global_position.distance_to(t.rig.focus) - from))
	t._check(t.rig.pull.shown() < t.EPS, "andar sobre a cidade: o olho não é puxado (%.3f)" % t.rig.pull.shown())
	t._check(worst < HOLD_M, "andar sobre a cidade: o zoom fica (variou %.2f m de %.0f m)" % [worst, from])
	await _no_jolt(STREET_M)
	await _no_jolt(ROOFS_M)


## Walking along at a steady pace past roads, kerbs, walls and roofs, the eye's path has no step: what changes from one frame's move to the
## next stays a small share of the move.
func _no_jolt(view_m: float) -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	t.rig.jump((b.min + b.max) * 0.5, 35.0, 25.0, view_m)
	await t._frames(1)
	var last: Vector3 = t.rig.camera.global_position
	var move := Vector3.ZERO
	var worst := 0.0
	for i in JOLT_STEPS:
		t.rig.slide(JOLT_STEP_M, JOLT_STEP_M * 0.5)
		await t._frames(1)
		var now: Vector3 = t.rig.camera.global_position
		if i > 0:
			worst = maxf(worst, (now - last - move).length())
		move = now - last
		last = now
	print("solavanco a %.0f m: %.3f m" % [view_m, worst])
	t._check(worst < JOLT_MAX_M, "andar a %.0f m: sem solavanco (%.3f m num quadro)" % [view_m, worst])
