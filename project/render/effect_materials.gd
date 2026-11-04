## A material of data/render/materials.json as a ShaderMaterial, with what the blast effects ask of the Environment's tone mapping
## (render/tone_map.gd): the unlit puffs draw smoke and dust as the colours the screen should show (puffs.gdshader,
## tone_inverse.gdshaderinc), and the crater's colours (display_colors) become the scene radiance the ACES shows as each.
class_name EffectMaterials
extends RefCounted

## Kept as text so the orphan gate sees the shaders the native effects load by name from materials.json.
const SHADERS := ["res://render/shaders/puffs.gdshader", "res://render/shaders/debris.gdshader", "res://render/shaders/marks.gdshader", "res://render/shaders/ragdoll.gdshader"]


## The parameters of a material: its own over those of the material it borrows from (`params_from`: the falling bodies wear the walkers' look).
static func params(table: Dictionary, key: String) -> Dictionary:
	var row: Dictionary = table[key]
	var out := {}
	if row.has("params_from"):
		out.merge(table[row.params_from].params)
	out.merge(row.params, true)
	return out


## `row`: the material's row of data/render/materials.json; render_priority orders the transparent layers (the higher is drawn over).
static func make(key: String, row: Dictionary, environment: Environment) -> ShaderMaterial:
	var m := ShaderMaterial.new()
	m.shader = load(row.shader)
	m.render_priority = int(row.get("render_priority", 0))
	if key == "puffs":
		m.set_shader_parameter("tone_exposure", environment.tonemap_exposure)
		m.set_shader_parameter("tone_white", environment.tonemap_white)
	elif key == "marks":
		var rad := PackedVector3Array()
		for html: String in row.display_colors.crater:
			rad.append(ToneMap.radiance(Color.html(html), environment.tonemap_exposure, environment.tonemap_white))
		m.set_shader_parameter("crater_rad", rad)
	return m
