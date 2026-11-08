// The body under the pointer before it is set down: one see-through tint, in either renderer, so it never reads as built.
shader_type spatial;
render_mode blend_mix, depth_draw_never, cull_back;

uniform float ghost_rough; // materials.json building_ghost
uniform vec3 tint : source_color;
uniform vec3 glow : source_color;
uniform float opacity;

void fragment() {
	ALBEDO = tint;
	EMISSION = glow;
	ROUGHNESS = ghost_rough;
	ALPHA = opacity;
}
