## What a lit lamp must radiate to show as its own colour through the tone mapping: the signal lenses and the lamps of the vehicles.
class_name LampRadiance
extends RefCounted

## Each lens lit (data/render/streets.json props.signal): the scene radiance the Environment's tone mapping shows as its
## colour (render/tone_map.gd mirrors Godot's AgX).
static func lenses(m: ShaderMaterial, environment: Environment) -> void:
	if environment.tonemap_mode != Environment.TONE_MAPPER_AGX:
		push_error("CityWorld: ToneMap mirrors only AgX; the lenses would not show their colour")
	var lenses := int(CityStore.constants.shader.LENS_COUNT) # props.gdshader lit_radiance, dark_radiance
	var lit := PackedVector3Array()
	lit.resize(lenses)
	var done := {}
	for part: Dictionary in DataTables.render_table("streets").props.signal:
		if part.has("lens") and not done.has(int(part.lens)):
			done[int(part.lens)] = true
			lit[int(part.lens)] = ToneMap.radiance(Color.html(part.color), environment)
	m.set_shader_parameter("lit_radiance", lit)
	var dark := PackedVector3Array()
	dark.resize(lenses)
	var gain: float = DataTables.game("daylight").sky_gain
	var rows: Array = DataTables.render_table("streets").props.lens_dark
	for k in lenses: # the 4.0's faint glow of an unlit lens, a radiance of its scale in ours
		var glow := Color.html(rows[k][1]).srgb_to_linear()
		dark[k] = Vector3(glow.r, glow.g, glow.b) * gain
	m.set_shader_parameter("dark_radiance", dark)


## A vehicle's lamps burning by day (data/render/agents.json trim): shown as their colour, like the signal lenses.
static func vehicles(m: ShaderMaterial, environment: Environment) -> void:
	var trim: Dictionary = DataTables.render_table("agents").trim
	for name: String in ["head_lit", "tail_lit", "brake_lit"]:
		m.set_shader_parameter(name, ToneMap.radiance(Color.html(trim[name]), environment))
