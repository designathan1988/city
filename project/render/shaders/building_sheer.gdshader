// A glass guard of the near level, a surface of its own on the building's mesh: laminated glass, half seen through. COLOR its
// tint. The alpha pass draws no shadow of it (a guard's is the faint one of glass).
shader_type spatial;
render_mode blend_mix, cull_back, depth_draw_never;

uniform float opacity = 0.42;
uniform float roughness = 0.05;
uniform float metalness = 0.2;

#include "res://render/shaders/building_colour.gdshaderinc"

void fragment() {
	ALBEDO = surface_out(to_linear(COLOR.rgb));
	ALPHA = opacity;
	ROUGHNESS = roughness;
	METALLIC = metalness;
}
