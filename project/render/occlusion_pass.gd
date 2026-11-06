## The occlusion pass of render/shaders/occlusion.gdshader.
extends "res://render/screen_pass.gd"


func setup(table: Dictionary) -> void:
	var material := lay("res://render/shaders/occlusion.gdshader")
	for key in ["reach_m", "reach_curve", "thickness_m", "power", "strength"]:
		material.set_shader_parameter(key, table[key])


func turn(on: bool) -> void:
	visible = on and available()
