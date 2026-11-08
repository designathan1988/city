// The scene radiance the Environment's AgX shows as a given display colour: for what is drawn unlit and was authored as the colour
// the screen should show. Godot 4.7 servers/rendering/renderer_rd/shaders/effects/tonemap.glsl tonemap_agx on an SDR output,
// mirrored by render/tone_map.gd (ToneMap.radiance is the same inverse, with a search for the nearest colour where a channel would
// need negative radiance).
// Needs color_space.gdshaderinc (srgb_to_linear); the material sets tone_exposure to the Environment's and tone_curve to
// ToneMap.params_of(environment): the allenwp curve's contrast, toe a, slope and w.
#ifndef TONE_INVERSE_INC
#define TONE_INVERSE_INC
#include "res://render/shaders/contracts/tone.gdshaderinc"

uniform float tone_exposure = 1.0;
uniform vec4 tone_curve = vec4(1.0);

// The matrices' columns are the table's (AGX_INSET_*, AGX_OUTSET_*), which render/tone_map.gd reads too.
const mat3 AGX_INSET = mat3(AGX_INSET_0, AGX_INSET_1, AGX_INSET_2);
const mat3 AGX_OUTSET = mat3(AGX_OUTSET_0, AGX_OUTSET_1, AGX_OUTSET_2);
const float AGX_OUTPUT_MAX = 1.0; // an SDR output's top (render/tone_map.gd OUTPUT_MAX)

// x >= 0 with allenwp_curve(x) = y, 0 <= y < 1: the toe's power sigmoid or the shoulder's quadratic, as tone_map.gd _curve_inverse.
float agx_curve_inverse(float y) {
	if (y < AGX_CROSSOVER) return pow(tone_curve.y * y / (1.0 - y), 1.0 / tone_curve.x);
	float u = y - AGX_CROSSOVER;
	float a = tone_curve.z / tone_curve.w;
	float b = tone_curve.z * (1.0 - u / (AGX_OUTPUT_MAX - AGX_CROSSOVER));
	return (-b + sqrt(b * b + 4.0 * a * u)) / (2.0 * a) + AGX_CROSSOVER;
}

// Where a channel would need negative radiance (or more than the curve reaches), the colour keeps as much of its chroma as the
// range allows: the curve's output y is linear in the display colour, so the share is found directly, not searched for.
vec3 display_to_radiance(vec3 shown) {
	vec3 goal = srgb_to_linear(clamp(shown, vec3(0.0), vec3(1.0)));
	mat3 outset_inverse = inverse(AGX_OUTSET);
	vec3 y_goal = outset_inverse * goal;
	vec3 y_grey = outset_inverse * vec3(dot(goal, REC709_LUMA));
	vec3 step_y = y_goal - y_grey;
	float top = AGX_TOP * AGX_OUTPUT_MAX;
	float keep = 1.0;
	for (int i = 0; i < 3; i++) {
		if (y_goal[i] < 0.0) keep = min(keep, y_grey[i] / (y_grey[i] - y_goal[i]));
		else if (y_goal[i] > top) keep = min(keep, (top - y_grey[i]) / step_y[i]);
	}
	vec3 y = clamp(y_grey + step_y * max(keep, 0.0), vec3(0.0), vec3(top));
	vec3 x = vec3(agx_curve_inverse(y.x), agx_curve_inverse(y.y), agx_curve_inverse(y.z));
	return max(inverse(AGX_INSET) * x / tone_exposure, vec3(0.0));
}

#endif
