// The sky: the air and the weather of sky_model.gdshaderinc (sky_seen) from the eye's height, the sun's and the moon's discs in the
// picture only.
// render/air.gd sets the air, render/daylight.gd the weather. What the radiance map takes of it is the light every surface gets from
// the sky (the Environment's ambient and reflections).
shader_type sky;

#include "res://render/shaders/sky_model.gdshaderinc"
#include "res://render/shaders/color_space.gdshaderinc"

uniform vec3 solar_direction = vec3(0.0, 1.0, 0.0); // towards the sun, wherever the light of the night points
uniform vec3 solar_top = vec3(1.0);
uniform vec4 air_rayleigh;
uniform vec4 air_mie;
uniform vec4 air_ozone;
uniform vec3 air_ground;
uniform float pre_exposure = 1.0; // the eye's adaptation: the sky pre-exposes itself (render/light_meter.gd expose says why)
uniform float tone_exposure = 1.0; // the Environment's exposure, which Compatibility puts into the radiance map too (below)
#include "res://render/shaders/sky_weather.gdshaderinc"

void sky() {
	vec3 dir = normalize(EYEDIR);
	float h = max(POSITION.y, 1.0);
	Air a = Air(solar_direction, solar_top, air_rayleigh, air_mie, air_ozone, air_ground);
	SkyWeather w = weather_now();
	vec3 col = sky_seen(a, w, dir, h);
	if (!AT_CUBEMAP_PASS) col += sky_sun_disc(a, dir, h) + sky_moon_disc(a, w, dir, h);
#if CURRENT_RENDERER == RENDERER_COMPATIBILITY
	// Compatibility multiplies a sky's COLOR by the sky's energy (the camera's exposure with it) and then decodes it from sRGB
	// (drivers/gles3/shaders/sky.glsl): the code that decodes, under that multiplier, to the light pre-exposed. Its radiance map
	// takes that light times the Environment's exposure, which the scene applies once more to what it lights, encoded by the sRGB
	// power segment; the map's filter decodes it with the cubic, takes the mean and encodes it with the power segment again, and the
	// scene reads it with the cubic (tonemap_inc.glsl, effects/cubemap_filter.glsl), which are not inverses in the dark. So the
	// map's light passes that step twice backwards (power segment over the cubic's inverse): the filtered levels, the ambient and
	// every rough reflection, read the light pre-exposed; the unfiltered level, a mirror's, reads it a little brighter in the
	// darkest tones (the step is above the identity there). Its exposure is taken out.
	vec3 lit = col * pre_exposure;
	if (AT_CUBEMAP_PASS) lit = gles_output_inverse(gles_encode(gles_output_inverse(gles_encode(lit)))) / tone_exposure;
	COLOR = gles_encode(lit) / pre_exposure;
#else
	COLOR = col * pre_exposure;
#endif
}
