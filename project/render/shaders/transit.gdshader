// Tracks, stations, shelters and the cars of the trains (native/src/render/transit): each part its vertex colour, a car's times its line's
// (the instance colour, COLOR already multiplied).
shader_type spatial;

#include "res://render/shaders/color_space.gdshaderinc"

uniform float roughness = 0.7;

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb));
	ROUGHNESS = roughness;
}
