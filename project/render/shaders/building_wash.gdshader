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
#include "res://render/shaders/hash.gdshaderinc"
#include "res://render/shaders/lamp_pool.gdshaderinc"

void fragment() {
	float lit = step(h01(uint(floor(UV2.x * 0.1) * 31.0 + floor(UV2.y * 0.1) * 977.0 + 65536.0)), outside_on);
	ALBEDO = linear_out(display_to_radiance(display_colour * pool_fall(length(UV * 2.0 - 1.0)) * opacity * night * lit));
}
