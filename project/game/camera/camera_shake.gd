## A blast shakes the camera for a moment: the effects' own clock (CityStore query `effects_shake`) says how many
## metres it moves now. The rig puts the camera where it wants it (`rest`, seen again whenever the camera is not
## the one this shook); this moves it off that place and back. A child of the rig, it runs after the rig's own frame.
class_name CameraShake
extends Node

var camera: Camera3D
var _rest := Transform3D()
var _shaken := Transform3D()
var _shaking := false


func _init(target: Camera3D) -> void:
	camera = target


func _process(_delta: float) -> void:
	if not _shaking or camera.global_transform != _shaken:
		_rest = camera.global_transform # the rig moved it (or nothing shook it): this is where it belongs
	var metres: float = CityStore.query(&"effects_shake")
	if metres <= 0.0:
		if _shaking:
			camera.global_transform = _rest
			_shaking = false
		return
	_shaking = true
	var jolt := Vector3(randf_range(-1, 1), randf_range(-1, 1), randf_range(-1, 1))
	_shaken = Transform3D(_rest.basis, _rest.origin + jolt * metres)
	camera.global_transform = _shaken
