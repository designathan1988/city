// Colour spaces of a spatial shader on each renderer, so a shader can work in linear on both (Godot 4.7.2):
// - Forward+ hands COLOR over in sRGB, samples source_color textures in linear, converts a Color given to a uniform to
//   linear (renderer_rd/storage_rd/material_storage.cpp), and takes ALBEDO and EMISSION in linear;
// - Compatibility (OUTPUT_IS_SRGB) hands COLOR, source_color textures and uniforms over as they are, in sRGB (its texture
//   and material storage convert none), and decodes ALBEDO and EMISSION from sRGB itself (drivers/gles3/shaders/
//   scene.glsl), so on it they go out encoded, by the exact inverse of its decoding.
// Compatibility's own transfer functions are fits of sRGB, not sRGB (drivers/gles3/shaders/tonemap_inc.glsl): it decodes
// with a cubic and encodes with the power segment alone (no linear toe), which is what the gles_* functions mirror.
#ifndef COLOR_SPACE_INC
#define COLOR_SPACE_INC

// IEC 61966-2-1 (sRGB), both ways: the linear toe's slope and its ends, the curve's offset, scale and power.
const float SRGB_TOE = 12.92, SRGB_TOE_IN = 0.04045, SRGB_TOE_OUT = 0.0031308, SRGB_OFFSET = 0.055, SRGB_SCALE = 1.055, SRGB_GAMMA = 2.4;

vec3 srgb_to_linear(vec3 c) {
	return mix(c / SRGB_TOE, pow((c + SRGB_OFFSET) / SRGB_SCALE, vec3(SRGB_GAMMA)), step(SRGB_TOE_IN, c));
}

vec3 linear_to_srgb(vec3 c) {
	return mix(c * SRGB_TOE, SRGB_SCALE * pow(c, vec3(1.0 / SRGB_GAMMA)) - SRGB_OFFSET, step(SRGB_TOE_OUT, c));
}

// Compatibility's decoding: c (c (0.305306011 c + 0.682171111) + 0.012522878) (tonemap_inc.glsl srgb_to_linear).
const vec3 GLES_DECODE = vec3(0.305306011, 0.682171111, 0.012522878);
const int GLES_ENCODE_STEPS = 4; // Newton steps from the exact encoding: the cubic is monotonic (every coefficient > 0)

vec3 gles_decode(vec3 c) {
	return c * (c * (c * GLES_DECODE.x + GLES_DECODE.y) + GLES_DECODE.z);
}

// The value Compatibility decodes to linear `l` (l >= 0): the root of its cubic.
vec3 gles_encode(vec3 l) {
	vec3 c = linear_to_srgb(max(l, vec3(0.0)));
	for (int i = 0; i < GLES_ENCODE_STEPS; i++) {
		vec3 slope = c * (3.0 * GLES_DECODE.x * c + 2.0 * GLES_DECODE.y) + GLES_DECODE.z;
		c -= (gles_decode(c) - l) / slope;
	}
	return c;
}

// Compatibility's encoding of its output, the sRGB power segment (tonemap_inc.glsl linear_to_srgb), and its inverse.
vec3 gles_output(vec3 l) {
	return max(SRGB_SCALE * pow(max(l, vec3(0.0)), vec3(1.0 / SRGB_GAMMA)) - SRGB_OFFSET, vec3(0.0));
}

vec3 gles_output_inverse(vec3 e) {
	return pow((max(e, vec3(0.0)) + SRGB_OFFSET) / SRGB_SCALE, vec3(SRGB_GAMMA));
}

// A linear colour's luminance: ITU-R BT.709's weights of its primaries (the sRGB primaries, IEC 61966-2-1).
const vec3 LUMA_709 = vec3(0.2126, 0.7152, 0.0722);

float luminance(vec3 c) {
	return dot(c, LUMA_709);
}

// A linear colour with `keep` of its chroma, the rest towards its own luminance.
vec3 keep_chroma(vec3 c, float keep) {
	return mix(vec3(luminance(c)), c, keep);
}

// A source_color texture sample or a colour uniform, in linear. (The renderer test is the preprocessor's, the same as
// OUTPUT_IS_SRGB, so that sky shaders, which have no OUTPUT_IS_SRGB, can include this file too.)
vec3 source_linear(vec3 c) {
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	return srgb_to_linear(c);
#else
	return c;
#endif
}

// A linear colour as ALBEDO or EMISSION (and as a sky's COLOR, which Compatibility decodes alike: drivers/gles3/shaders/
// sky.glsl).
vec3 linear_out(vec3 c) {
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	return gles_encode(c);
#else
	return c;
#endif
}

#endif
