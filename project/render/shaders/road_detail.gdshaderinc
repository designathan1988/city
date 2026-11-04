// The close-up layer of the street surfaces: a second, finer map read where a pixel is smaller than the macro map's grain,
// twice at two scales and turned so that no tile shows, and the macro map softened under it. Past a few centimetres a
// pixel it weighs nothing and nothing is read.
uniform sampler2D asphalt_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D asphalt_detail_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D concrete_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D concrete_detail_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float asphalt_detail_m = 1.28;
uniform float concrete_detail_m = 1.04;
uniform float detail_near_m = 0.018; // metres a pixel: fully shown below, gone above detail_far_m
uniform float detail_far_m = 0.064;

const mat2 DETAIL_TURN = mat2(vec2(0.5403, 0.8415), vec2(-0.8415, 0.5403));
const float DETAIL_SECOND = 0.4137; // scale of the second read
const float DETAIL_MIX = 0.55; // its share
const vec2 DETAIL_SHIFT = vec2(0.31);

float detail_weight(float pix) {
	return 1.0 - smoothstep(detail_near_m, detail_far_m, pix);
}

vec3 detail_albedo(sampler2D map, vec2 uv, vec2 dx, vec2 dy) {
	vec3 a = textureGrad(map, uv, dx, dy).rgb * 2.0;
	vec3 b = textureGrad(map, DETAIL_TURN * uv * DETAIL_SECOND + DETAIL_SHIFT, DETAIL_TURN * dx * DETAIL_SECOND, DETAIL_TURN * dy * DETAIL_SECOND).rgb * 2.0;
	return a * mix(vec3(1.0), b, DETAIL_MIX);
}

vec2 detail_bend(sampler2D map, vec2 uv, vec2 dx, vec2 dy) {
	vec2 a = textureGrad(map, uv, dx, dy).xy * 2.0 - 1.0;
	vec2 b = textureGrad(map, DETAIL_TURN * uv * DETAIL_SECOND + DETAIL_SHIFT, DETAIL_TURN * dx * DETAIL_SECOND, DETAIL_TURN * dy * DETAIL_SECOND).xy * 2.0 - 1.0;
	return a + (b * DETAIL_TURN) * DETAIL_MIX;
}
