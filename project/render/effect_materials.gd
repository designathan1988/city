## A material of data/render/materials.json as a ShaderMaterial, with what the blast effects ask of the Environment's tone mapping
## (render/tone_map.gd): the unlit puffs draw smoke and dust as the colours the screen should show (puffs.gdshader,
## tone_inverse.gdshaderinc), and the crater's colours (display_colors) become the scene radiance the AgX shows as each.
class_name EffectMaterials
extends RefCounted

const TONE_INVERTED := ["puffs", "building_wash"] ## what tone_inverse.gdshaderinc reads


## `row`: the material's row of data/render/materials.json; render_priority orders the transparent layers (the higher is drawn over).
static func make(key: String, row: Dictionary, environment: Environment) -> ShaderMaterial:
	var m := ShaderMaterial.new()
	m.shader = load(row.shader)
	m.render_priority = int(row.get("render_priority", 0))
	if key in TONE_INVERTED:
		m.set_shader_parameter("tone_exposure", environment.tonemap_exposure)
		m.set_shader_parameter("tone_curve", ToneMap.params_of(environment))
	elif key == "marks":
		var rad := PackedVector3Array()
		for html: String in row.display_colors.crater:
			rad.append(ToneMap.radiance(Color.html(html), environment))
		m.set_shader_parameter("crater_rad", rad)
	return m
