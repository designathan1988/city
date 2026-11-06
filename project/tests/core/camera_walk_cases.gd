## Moving over the buildings at the city's view, in camera_input_test: the zoom holds. The pull is for the street, and the eye never jumps towards what it crosses.
class_name CameraWalkCases
extends RefCounted

const STEPS := 90
const VIEW_M := 250.0 # the view over the blocks: well past the street, where the pull ends
const RIGHT_M := 6.0
const AHEAD_M := 3.0
const HOLD_M := 1.0 # the ground under the point looked at moves it this much at most

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
	t._check(t.rig.pull.shown < t.EPS, "andar sobre a cidade: o olho não é puxado (%.3f)" % t.rig.pull.shown)
	t._check(worst < HOLD_M, "andar sobre a cidade: o zoom fica (variou %.2f m de %.0f m)" % [worst, from])
