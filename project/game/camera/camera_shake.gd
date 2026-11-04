## A blast shakes the camera for a moment: the effects' own clock (CityStore query `effects_shake`) says how many
## metres it moves now. The shake is the camera's frustum offset (Camera3D h_offset / v_offset, a shear that never touches
## the camera's transform), so it cannot drift from where the rig put the camera: with no shake both offsets are 0.
## A child of the rig, it runs after the rig's own frame.
class_name CameraShake
extends Node

var camera: Camera3D


func _init(target: Camera3D) -> void:
	camera = target


func _process(_delta: float) -> void:
	var metres: float = CityStore.query(&"effects_shake")
	camera.h_offset = randf_range(-1, 1) * metres
	camera.v_offset = randf_range(-1, 1) * metres
