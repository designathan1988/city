// Colour spaces of a spatial shader on each renderer, so a shader can work in linear on both (Godot 4.7.2):
// - Forward+ hands COLOR over in sRGB, samples source_color textures in linear, converts a Color given to a uniform to
//   linear (renderer_rd/storage_rd/material_storage.cpp), and takes ALBEDO and EMISSION in linear;
// - Compatibility (OUTPUT_IS_SRGB) hands COLOR, source_color textures and uniforms over as they are, in sRGB (its texture
//   and material storage convert none), and decodes ALBEDO and EMISSION from sRGB itself (drivers/gles3/shaders/
//   scene.glsl), so on it they go out encoded, by the exact inverse of its decoding.

vec3 srgb_to_linear(vec3 c) {
	return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

vec3 linear_to_srgb(vec3 c) {
	return mix(c * 12.92, 1.055 * pow(c, vec3(1.0 / 2.4)) - 0.055, step(0.0031308, c));
}

// A source_color texture sample or a colour uniform, in linear.
vec3 source_linear(vec3 c) {
	return OUTPUT_IS_SRGB ? srgb_to_linear(c) : c;
}

// A linear colour as ALBEDO or EMISSION.
vec3 linear_out(vec3 c) {
	return OUTPUT_IS_SRGB ? linear_to_srgb(c) : c;
}
