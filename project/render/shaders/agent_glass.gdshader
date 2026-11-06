// A vehicle's glass near the camera (native/src/render/agents: the glass surface of a car's, bus's or truck's near mesh,
// over its cabin and the people in it). What is seen is the light it reflects, its cool tint (materials.json) and what
// lies behind it, dimmed. Its surface reflects R(θ) = F0 + (1 - F0)(1 - cos θ)^5 (Schlick, "An Inexpensive BRDF
// Model for Physically-based Rendering", Computer Graphics Forum 13(3), 1994), F0 = ((n - 1) / (n + 1))^2 for its index
// of refraction n; what gets through the surface is then dimmed to `transmittance`. So the part of the view behind that
// is covered, the blend's alpha, is 1 - transmittance * (1 - R). Godot's SPECULAR is F0 / 0.08 (0.5: F0 = 4 %).
shader_type spatial;
render_mode blend_mix, depth_draw_opaque, cull_back;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/agent_door.gdshaderinc"

const float SPECULAR_PER_F0 = 12.5; // Godot: F0 = 0.08 * SPECULAR
const float SCHLICK_POWER = 5.0;

uniform vec3 tint : source_color;
uniform float transmittance = 0.7;
uniform float ior = 1.52;
uniform float roughness = 0.0;

void vertex() {
	if (COLOR.g > PANE_MARK) VERTEX.y = mix(VERTEX.y, COLOR.r * SILL_SPAN_M, pane_drop(COLOR.a, VERTEX.x));
	if (UV2.y > DOOR_MARK) door_swing(VERTEX, NORMAL, UV, INSTANCE_CUSTOM.g);
}

void fragment() {
	float f0 = pow((ior - 1.0) / (ior + 1.0), 2.0);
	float r = f0 + (1.0 - f0) * pow(1.0 - clamp(dot(NORMAL, VIEW), 0.0, 1.0), SCHLICK_POWER);
	ALBEDO = linear_out(source_linear(tint));
	ALPHA = 1.0 - transmittance * (1.0 - r);
	ROUGHNESS = roughness;
	SPECULAR = clamp(f0 * SPECULAR_PER_F0, 0.0, 1.0);
}
