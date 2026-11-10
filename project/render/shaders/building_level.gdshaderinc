// A building's near level handing over to its mid level (building_renderer_reach.cpp fade_levels). The near level dissolves by screen noise over the
// band, only on the sub-chunks in it: `discard` costs the depth prepass of whatever carries it (Godot's shading language docs), so the shaders that
// discard are variants, put on by instance (LEVEL_FADE); the mid level stands whole behind it, `level_back` metres further along the eye's ray, the same on
// screen, so the two never fight over a depth.
global uniform float dither_clock; // a new pattern a frame for the temporal filter, and one that stands still in a frozen picture (TIME stops wherever it was)
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
// Instance uniforms live in one buffer of 4096 items there (a UBO's size, whatever the project asks), a city's near level is more: the levels change at once.
const float level_keep = 1.0;
const float level_back = 0.0;
#else
instance uniform float level_keep : instance_index(0) = 1.0; // the share of the near level still drawn
instance uniform float level_back : instance_index(1) = 0.0;
#endif

float level_noise(vec2 px) {
	return fract(52.9829189 * fract(dot(px + dither_clock * 61.0, vec2(0.06711056, 0.00583715)))); // Jimenez 2014
}

// `d` (world) in the frame of `model`, whose axes are square to each other at any scale.
vec3 level_local(mat4 model, vec3 d) {
	vec3 a = model[0].xyz, b = model[1].xyz, c = model[2].xyz;
	return vec3(dot(a, d) / max(dot(a, a), 1e-8), dot(b, d) / max(dot(b, b), 1e-8), dot(c, d) / max(dot(c, c), 1e-8));
}
