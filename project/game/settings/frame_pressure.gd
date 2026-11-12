## Main SceneTree process interval used by the automatic governor. This is wall work including lock waits,
## excluding frame-cap sleep, rendering after the tree, input before process_frame, and the parallel simulation thread.
class_name FramePressure
extends Node

const LAST_PROCESS := 2147483647
const US_PER_MS := 1000.0

var scene_ms := 0.0
var _begin_us := 0


func _ready() -> void:
	process_mode = Node.PROCESS_MODE_ALWAYS
	process_priority = LAST_PROCESS
	get_tree().process_frame.connect(_begin_frame)


func _begin_frame() -> void:
	_begin_us = Time.get_ticks_usec()


func _process(_delta: float) -> void:
	if _begin_us > 0:
		scene_ms = float(Time.get_ticks_usec() - _begin_us) / US_PER_MS
