// Colour spaces of the building shaders (building, building_far, building_parts): every sum and product in linear, and
// each colour taken in and handed out the way the renderer in use passes it (checked on screen by
// tests/core/building_light_test.gd on both renderers).
// - In: a vertex colour or an instance's custom tone is the sRGB value the mesh carries, on both renderers. A colour
//   uniform (source_color) arrives linear on Forward+ (its material storage converts a Color) and as set, in sRGB, on
//   Compatibility (its material storage converts none).
// - Out: Forward+ takes ALBEDO and EMISSION linear; Compatibility (OUTPUT_IS_SRGB, lighting in sRGB space,
//   godotengine/godot#88214) decodes both from sRGB before lighting and tone mapping (drivers/gles3/shaders/scene.glsl),
//   so there they go in encoded, the exact inverse.
// The transfer functions are sRGB's, color_space.gdshaderinc's.
#include "res://render/shaders/color_space.gdshaderinc"

vec3 uniform_linear(vec3 c) {
	return OUTPUT_IS_SRGB ? srgb_to_linear(c) : c;
}

vec3 surface_out(vec3 c) {
	return OUTPUT_IS_SRGB ? linear_to_srgb(c) : c;
}
