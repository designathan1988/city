// A light written as EMISSION, on each renderer (Godot 4.7.2). Both pre-expose the lights, the sky and its ambient by the camera's
// exposure (CameraAttributes.exposure_multiplier: render/eye.gd, the eye's adaptation), so the picture keeps its precision at night;
// Forward+ pre-exposes EMISSION too (scene_forward_clustered.glsl, emission *= scene_data.emissive_exposure_normalization) and
// Compatibility does not (drivers/gles3/shaders/scene.glsl declares the factor and reads it nowhere). So a light in the scene's own
// unit is pre-exposed here on Compatibility, and a light authored as the screen shows it (render/tone_map.gd radiance: a lit signal
// lens, a vehicle's lamps) leaves the pre-exposure again on Forward+, so it shows its colour at any hour. The value goes out
// through the shader's own encoding (linear_out, surface_out). Neither renderer pre-exposes what is drawn unshaded.
#ifndef EMISSION_LIGHT_INC
#define EMISSION_LIGHT_INC

global uniform float pre_exposure;

// A light in the scene's unit (a radiance: render/lamp_photometry.h, render/daylight.gd lux).
vec3 emission_light(vec3 c) {
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	return c * pre_exposure;
#else
	return c;
#endif
}

// A light authored as the screen shows it under the Environment's own exposure.
vec3 emission_shown(vec3 c) {
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	return c;
#else
	return c / pre_exposure;
#endif
}

// A light in the scene's unit drawn unshaded: the physical air of a full-screen pass.
vec3 unshaded_light(vec3 c) {
	return c * pre_exposure;
}

#endif
