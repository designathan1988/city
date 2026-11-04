// The scene radiance the Environment's ACES shows as a given display colour: for what is drawn unlit and was authored as the colour
// the screen should show (the reference game writes such colours straight to its frame buffer, clamped to 0..1 per layer). Godot 4.7
// servers/rendering/renderer_rd/shaders/effects/tonemap.glsl tonemap_aces, mirrored by render/tone_map.gd (ToneMap.radiance is the
// same inverse, with a search for the nearest colour where a channel would need negative radiance).
// Needs color_space.gdshaderinc (srgb_to_linear); the material sets tone_exposure and tone_white to the Environment's.
#ifndef TONE_INVERSE_INC
#define TONE_INVERSE_INC

uniform float tone_exposure = 1.0;
uniform float tone_white = 1.0;

const float ACES_BIAS = 1.8; // tonemap_aces exposure_bias
const float ACES_A = 0.0245786;
const float ACES_B = 0.000090537;
const float ACES_C = 0.983729;
const float ACES_D = 0.432951;
const float ACES_E = 0.238081;
const float ACES_TOP = 0.999; // of the curve's asymptote 1 / C: the highest output asked for
// rgb_to_rrt and odt_to_rgb, column by column (GLSL's mat3 constructor takes columns).
const mat3 ACES_RRT = mat3(vec3(0.59719, 0.07600, 0.02840), vec3(0.35458, 0.90834, 0.13383), vec3(0.04823, 0.01566, 0.83777));
const mat3 ACES_ODT = mat3(vec3(1.60475, -0.10208, -0.00327), vec3(-0.53108, 1.10813, -0.07276), vec3(-0.07367, -0.00605, 1.07602));

float aces_curve(float x) {
	return (x * (x + ACES_A) - ACES_B) / (x * (ACES_C * x + ACES_D) + ACES_E);
}

// x >= 0 with aces_curve(x) = y, 0 <= y < 1 / C: the root of (yC - 1) x^2 + (yD - A) x + (yE + B) = 0.
float aces_curve_inverse(float y) {
	float a = y * ACES_C - 1.0;
	float b = y * ACES_D - ACES_A;
	float c = y * ACES_E + ACES_B;
	return (-b - sqrt(b * b - 4.0 * a * c)) / (2.0 * a);
}

// Where a channel would need negative radiance (or more than the curve reaches), the colour keeps as much of its chroma as the
// range allows: the curve's input y is linear in the display colour, so the share is found directly, not searched for.
vec3 display_to_radiance(vec3 shown) {
	vec3 goal = srgb_to_linear(clamp(shown, vec3(0.0), vec3(1.0)));
	float scale = aces_curve(max(tone_white, 1.0) * ACES_BIAS);
	mat3 odt_inverse = inverse(ACES_ODT);
	vec3 y_goal = odt_inverse * (goal * scale);
	vec3 y_grey = odt_inverse * (vec3(dot(goal, vec3(0.2126, 0.7152, 0.0722))) * scale);
	vec3 step_y = y_goal - y_grey;
	float top = ACES_TOP / ACES_C;
	float keep = 1.0;
	for (int i = 0; i < 3; i++) {
		if (y_goal[i] < 0.0) keep = min(keep, y_grey[i] / (y_grey[i] - y_goal[i]));
		else if (y_goal[i] > top) keep = min(keep, (top - y_grey[i]) / step_y[i]);
	}
	vec3 y = clamp(y_grey + step_y * max(keep, 0.0), vec3(0.0), vec3(top));
	vec3 x = vec3(aces_curve_inverse(y.x), aces_curve_inverse(y.y), aces_curve_inverse(y.z));
	return max(inverse(ACES_RRT) * x / (tone_exposure * ACES_BIAS), vec3(0.0));
}

#endif
