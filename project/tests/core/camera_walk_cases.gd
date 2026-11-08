## Moving over the buildings at the city's view, in camera_input_test: the zoom holds. The pull is for the street, and the eye never jumps towards what it crosses.
## The rig is stepped by hand at a fixed step (its frame process off): what is measured is the rig, not how long the machine's frames took.
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
const STEP_S := 1.0 / 60.0

var t: Node


func _init(test: Node) -> void:
	t = test


func run() -> void:
	t.rig.set_process(false)
	var b: Dictionary = CityStore.query(&"city_bounds")
	t.rig.jump((b.min + b.max) * 0.5, 35.0, 45.0, VIEW_M)
	var from: float = t.rig.camera.global_position.distance_to(t.rig.focus)
	var worst := 0.0
	for i in STEPS:
		t.rig.slide(RIGHT_M, AHEAD_M)
		t.rig.settle(STEP_S)
		worst = maxf(worst, absf(t.rig.camera.global_position.distance_to(t.rig.focus) - from))
	t.check(t.rig.pull.shown() < t.EPS, "walking over the city: the eye is not pulled (%.3f)" % t.rig.pull.shown())
	t.check(worst < HOLD_M, "walking over the city: the zoom stays (moved %.2f m of %.0f m)" % [worst, from])
	_no_jolt(STREET_M)
	_no_jolt(ROOFS_M)
	t.rig.set_process(true)
	await t.frames(1)


## Walking along at a steady pace past roads, kerbs, walls and roofs, the eye's path has no step: what changes from one frame's move to the
## next stays a small share of the move.
func _no_jolt(view_m: float) -> void:
	var b: Dictionary = CityStore.query(&"city_bounds")
	t.rig.jump((b.min + b.max) * 0.5, 35.0, 25.0, view_m)
	var last: Vector3 = t.rig.camera.global_position
	var move := Vector3.ZERO
	var worst := 0.0
	for i in JOLT_STEPS:
		t.rig.slide(JOLT_STEP_M, JOLT_STEP_M * 0.5)
		t.rig.settle(STEP_S)
		var now: Vector3 = t.rig.camera.global_position
		if i > 0:
			worst = maxf(worst, (now - last - move).length())
		move = now - last
		last = now
	print("a jolt at %.0f m: %.3f m" % [view_m, worst])
	t.check(worst < JOLT_MAX_M, "walking at %.0f m: no jolt (%.3f m in a frame)" % [view_m, worst])
