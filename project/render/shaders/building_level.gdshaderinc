// A building's near level handing over to its mid level (building_renderer_reach.cpp fade_levels). The near level dissolves by screen noise over the
// band, by its instance's level_keep, in its own material: a material swapped in play is a pipeline the engine makes on the frame it is first drawn
// (Godot docs, pipeline compilations: overrides are not compiled with the mesh), the stall of log/owner-review.md §9. The discard runs first in the
// fragment, so the depth prepass computes only the noise and the share (what nothing else of the fragment feeds); the mid level stands whole behind
// it, `level_back` metres further along the eye's ray, the same on screen, so the two never fight over a depth.
global uniform float dither_clock; // a new pattern a frame for the temporal filter, and one that stands still in a frozen picture (TIME stops wherever it was)
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
// Instance uniforms live in one buffer of 4096 items there (a UBO's size, whatever the project asks), a city's near level is more: the levels change at once.
const float level_keep = 1.0;
const float level_back = 0.0;
#else
instance uniform float level_keep : instance_index(0) = 1.0; // the share of the near level still drawn
instance uniform float level_back : instance_index(1) = 0.0;
#endif

// Interleaved gradient noise (J. Jimenez, "Next Generation Post Processing in Call of Duty: Advanced Warfare", SIGGRAPH 2014):
// its three constants; the pattern moves this many pixels a frame of the clock (any step off its period).
const float IGN_K = 52.9829189, IGN_X = 0.06711056, IGN_Y = 0.00583715, IGN_STEP = 61.0;
const float LEAST_AXIS = 1e-8; // an axis's squared length divided by never under this

float level_noise(vec2 px) {
	return fract(IGN_K * fract(dot(px + dither_clock * IGN_STEP, vec2(IGN_X, IGN_Y))));
}

// `d` (world) in the frame of `model`, whose axes are square to each other at any scale.
vec3 level_local(mat4 model, vec3 d) {
	vec3 a = model[0].xyz, b = model[1].xyz, c = model[2].xyz;
	return vec3(dot(a, d) / max(dot(a, a), LEAST_AXIS), dot(b, d) / max(dot(b, b), LEAST_AXIS), dot(c, d) / max(dot(c, c), LEAST_AXIS));
}
