// The close-up layer of the street surfaces: a second, finer map read where a pixel is smaller than the macro map's grain,
// twice at two scales and turned so that no tile shows, and the macro map softened under it. Past a few centimetres a
// pixel it weighs nothing and nothing is read.
uniform sampler2D asphalt_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D asphalt_detail_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D concrete_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D concrete_detail_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D grass_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D grass_detail_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float asphalt_detail_m = 1.28;
uniform float concrete_detail_m = 1.04;
uniform float grass_detail_m = 1.6;
uniform float detail_near_m = 0.018; // metres a pixel: fully shown below, gone above detail_far_m
uniform float detail_far_m = 0.064;

#include "res://render/shaders/detail_pair.gdshaderinc"

float detail_weight(float pix) {
	return 1.0 - smoothstep(detail_near_m, detail_far_m, pix);
}

// A normal map's bend laid on a surface along the axes of its coordinates `st`, read from how they run across the screen
// (Schueler, "Normal Mapping Without Precomputed Tangents"), so a road bends the same whichever way it runs and a kerb's face
// bends as its top. The sign of the screen's own area keeps it right on a renderer whose y runs down.
vec3 bent_normal(vec3 n, vec3 p, vec2 st, vec2 bend) {
	vec3 q0 = dFdx(p), q1 = dFdy(p);
	vec2 s0 = dFdx(st), s1 = dFdy(st);
	vec3 a = cross(q1, n), b = cross(n, q0);
	vec3 t = a * s0.x + b * s1.x, u = a * s0.y + b * s1.y;
	float len = max(dot(t, t), dot(u, u));
	float k = len > 0.0 ? inversesqrt(len) * sign(dot(cross(q0, q1), n)) : 0.0;
	return normalize(n + (t * bend.x + u * bend.y) * k);
}
