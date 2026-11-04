// The water in one colour: what the Optimisations tab puts on the water's own material to see what its shading costs.
shader_type spatial;
render_mode cull_disabled;

uniform vec3 deep : source_color;

void fragment() {
	ALBEDO = deep;
}
