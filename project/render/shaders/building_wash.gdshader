// The light of a lantern on its wall, added to what is behind it, a surface of its own on the near level's mesh: warm, falling off round
// the lamp, only at night and only where the lantern is on (the same lot as its glass, building.gdshader kind 4). UV over the quad, UV2 the
// lantern's place. It scatters nothing: no shadow, no depth.
// The reference game adds it after the tone mapping, to the colours on the screen; here the sum comes before, so what is added is the radiance
// the Environment's ACES shows as that colour (added to radiance, a saturated light would come out orange where the screen only warms).
shader_type spatial;
render_mode blend_add, unshaded, cull_back, depth_draw_never, shadows_disabled;

global uniform float night;
global uniform float outside_on;

uniform vec3 display_colour = vec3(0.89, 0.83, 0.7); // as the screen shows it: not a source_color, it stays encoded
uniform float opacity = 0.55;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/tone_inverse.gdshaderinc"
#include "res://render/shaders/building_hash.gdshaderinc"

// The pool's fall from its middle to its rim (the street lamps' stops).
float pool(float r) {
	if (r < 0.35) return mix(1.0, 0.55, r / 0.35);
	if (r < 0.7) return mix(0.55, 0.15, (r - 0.35) / 0.35);
	return mix(0.15, 0.0, clamp((r - 0.7) / 0.3, 0.0, 1.0));
}

void vertex() {
	// Inactive additive geometry has no screen area; retain the mesh for the next night.
	if (night == 0.0 || opacity == 0.0) VERTEX = vec3(0.0);
}

void fragment() {
	float lit = step(h01(uint(floor(UV2.x * 0.1) * 31.0 + floor(UV2.y * 0.1) * 977.0 + 65536.0)), outside_on);
	if (lit == 0.0 || night == 0.0 || opacity == 0.0) discard;
	ALBEDO = linear_out(display_to_radiance(display_colour * pool(length(UV * 2.0 - 1.0)) * opacity * night * lit));
}
