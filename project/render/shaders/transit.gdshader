// Tracks, stations, shelters and the cars of the trains (native/src/render/transit): each box its vertex colour, a car's times its line's
// (the instance colour, COLOR already multiplied); UV carries the kind of box's roughness and metal.
shader_type spatial;

#include "res://render/shaders/color_space.gdshaderinc"

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb));
	ROUGHNESS = UV.x;
	METALLIC = UV.y;
}
