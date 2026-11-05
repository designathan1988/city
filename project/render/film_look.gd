## The picture's own look, over the tone mapping: the film grade as the Environment's lookup table and the fall-off to
## the corners under the interface (data/game/daylight.json "grade" and "vignette").
class_name FilmLook
extends RefCounted

const VIGNETTE_SHADER := "res://render/shaders/vignette.gdshader"
const UNDER_THE_HUD := -1


static func grade(environment: Environment, look: Dictionary) -> void:
	var table := ImageTexture3D.new()
	var side := int(look.lut_side)
	table.create(Image.FORMAT_RGB8, side, side, side, false, CityStore.query(&"film_grade", [look]))
	environment.adjustment_enabled = true
	environment.adjustment_color_correction = table


static func vignette(parent: Node, look: Dictionary) -> void:
	var rect := ColorRect.new()
	rect.set_anchors_preset(Control.PRESET_FULL_RECT)
	rect.mouse_filter = Control.MOUSE_FILTER_IGNORE
	var m := ShaderMaterial.new()
	m.shader = load(VIGNETTE_SHADER)
	for key: String in look:
		if key != "_doc":
			m.set_shader_parameter(key, look[key])
	rect.material = m
	var layer := CanvasLayer.new()
	layer.layer = UNDER_THE_HUD
	layer.add_child(rect)
	parent.add_child(layer)
