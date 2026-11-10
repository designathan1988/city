## What a lit lamp must radiate to show as its own colour through the tone mapping: the signal lenses and the lamps of the vehicles.
class_name LampRadiance
extends RefCounted

const LENSES := 5 # props.gdshader lit_radiance (prop_kit.h Lens::COUNT)

## Each lens lit (data/render/streets.json props.signal): the scene radiance the Environment's tone mapping shows as its
## colour (render/tone_map.gd mirrors Godot's ACES).
static func lenses(m: ShaderMaterial, environment: Environment) -> void:
	if environment.tonemap_mode != Environment.TONE_MAPPER_ACES:
		push_error("CityWorld: ToneMap só espelha o ACES; as lentes não mostrariam a sua cor")
	var lit := PackedVector3Array()
	lit.resize(LENSES)
	var done := {}
	for part: Dictionary in DataTables.render_table("streets").props.signal:
		if part.has("lens") and not done.has(int(part.lens)):
			done[int(part.lens)] = true
			lit[int(part.lens)] = ToneMap.radiance(Color.html(part.color), environment.tonemap_exposure,
					environment.tonemap_white)
	m.set_shader_parameter("lit_radiance", lit)
	var dark := PackedVector3Array()
	dark.resize(LENSES)
	var gain: float = DataTables.game("daylight").sky_gain
	var rows: Array = DataTables.render_table("streets").props.lens_dark
	for k in LENSES: # the 4.0's faint glow of an unlit lens, a radiance of its scale in ours
		var glow := Color.html(rows[k][1]).srgb_to_linear()
		dark[k] = Vector3(glow.r, glow.g, glow.b) * gain
	m.set_shader_parameter("dark_radiance", dark)


## A vehicle's lamps burning by day (data/render/agents.json trim): shown as their colour, like the signal lenses.
static func vehicles(m: ShaderMaterial, environment: Environment) -> void:
	var trim: Dictionary = DataTables.render_table("agents").trim
	for name: String in ["head_lit", "tail_lit", "brake_lit"]:
		m.set_shader_parameter(name, ToneMap.radiance(Color.html(trim[name]), environment.tonemap_exposure, environment.tonemap_white))
