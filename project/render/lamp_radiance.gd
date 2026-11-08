## The signal lenses lit, as lights: each lens's colour (data/render/streets.json props.signal, the regulated colour as near as the
## screen shows it), of luminance 1, times its luminance (streets.json props.lens_cd_m2), a radiance in the scene's unit (render/
## daylight.gd lux): bright in the sun, blinding in the night, as a signal is.
class_name LampRadiance
extends RefCounted


## Each lens lit (props.gdshader lit_radiance).
static func lenses(m: ShaderMaterial) -> void:
	var lenses := int(CityStore.constants.shader.LENS_COUNT)
	var lit := PackedVector3Array()
	lit.resize(lenses)
	var props: Dictionary = DataTables.render_table("streets").props
	var unit: float = PI / DataTables.tuning("render_light_unit_lx")
	var done := {}
	for part: Dictionary in props.signal:
		if part.has("lens") and not done.has(int(part.lens)):
			var k := int(part.lens)
			done[k] = true
			var c := Color.html(part.color).srgb_to_linear()
			var colour := Vector3(c.r, c.g, c.b)
			lit[k] = colour / ToneMap.luminance(colour) * float(props.lens_cd_m2[k]) * unit
	m.set_shader_parameter("lit_radiance", lit)
