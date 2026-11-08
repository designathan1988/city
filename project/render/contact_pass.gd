## The sun's contact shadows of render/shaders/contact.gdshader. Without sun there is nothing to draw: the pass goes away with it.
extends "res://render/screen_pass.gd"

var _material: ShaderMaterial
var _allowed := false
var _strength := 0.0


func setup() -> void:
	_material = lay("res://render/shaders/contact.gdshader")
	var look: Dictionary = DataTables.game("sky").contact_shader
	var kinds := WorldMaterials.uniform_kinds(_material.shader)
	for key: String in look:
		if not key.begins_with("_"):
			_material.set_shader_parameter(key, WorldMaterials.typed(kinds, key, look[key]))


func turn(on: bool) -> void:
	_allowed = on
	_show()


## Each frame: where the sun is, how deep its contact shade may go and its light on level ground (the pass weighs that against
## the sky's light, render/shaders/contact.gdshader).
func follow(sun_direction: Vector3, strength: float, sun_level: float) -> void:
	_strength = strength * sun_level
	_material.set_shader_parameter("sun_direction", sun_direction)
	_material.set_shader_parameter("strength", strength)
	_material.set_shader_parameter("sun_level", sun_level)
	_show()


func _show() -> void:
	visible = _allowed and _strength > 0.0 and available()
