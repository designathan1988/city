// Colour spaces of the building shaders (building, building_far, building_parts): every sum and product in linear, and
// each colour taken in and handed out the way the renderer in use passes it (checked on screen by
// tests/core/building_light_test.gd on both renderers).
// - In: a vertex colour or an instance's custom tone is the sRGB value the mesh carries, on both renderers. A colour
//   uniform (source_color) arrives linear on Forward+ (its material storage converts a Color) and as set, in sRGB, on
//   Compatibility (its material storage converts none).
// - Out: Forward+ takes ALBEDO and EMISSION linear; Compatibility (OUTPUT_IS_SRGB, lighting in sRGB space,
//   godotengine/godot#88214) decodes both from sRGB before lighting and tone mapping (drivers/gles3/shaders/scene.glsl),
//   so there they go in encoded, the exact inverse.
// The transfer functions are sRGB's (IEC 61966-2-1): the linear toe's slope and its ends, the curve's offset, scale and power.
const float SRGB_TOE = 12.92, SRGB_TOE_IN = 0.04045, SRGB_TOE_OUT = 0.0031308, SRGB_OFFSET = 0.055, SRGB_SCALE = 1.055, SRGB_GAMMA = 2.4;

vec3 to_linear(vec3 c) {
	return mix(c / SRGB_TOE, pow((c + SRGB_OFFSET) / SRGB_SCALE, vec3(SRGB_GAMMA)), step(SRGB_TOE_IN, c));
}

vec3 to_srgb(vec3 c) {
	return mix(c * SRGB_TOE, SRGB_SCALE * pow(c, vec3(1.0 / SRGB_GAMMA)) - SRGB_OFFSET, step(SRGB_TOE_OUT, c));
}

vec3 uniform_linear(vec3 c) {
	return OUTPUT_IS_SRGB ? to_linear(c) : c;
}

vec3 surface_out(vec3 c) {
	return OUTPUT_IS_SRGB ? to_srgb(c) : c;
}
