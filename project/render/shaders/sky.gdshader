// The sky: the air and the weather of sky_model.gdshaderinc (sky_seen) from the eye's height, the sun's disc in the picture only.
// render/air.gd sets the air, render/daylight.gd the weather. What the radiance map takes of it is the light every surface gets from
// the sky (the Environment's ambient and reflections).
shader_type sky;

#include "res://render/shaders/sky_model.gdshaderinc"

uniform vec3 solar_direction = vec3(0.0, 1.0, 0.0); // towards the sun, wherever the light of the night points
uniform vec3 solar_top = vec3(1.0);
uniform vec4 air_rayleigh;
uniform vec4 air_mie;
uniform vec4 air_ozone;
uniform vec3 air_ground;
#include "res://render/shaders/sky_weather.gdshaderinc"

void sky() {
	vec3 dir = normalize(EYEDIR);
	float h = max(POSITION.y, 1.0);
	Air a = Air(solar_direction, solar_top, air_rayleigh, air_mie, air_ozone, air_ground);
	vec3 col = sky_seen(a, weather_now(), dir, h);
	if (!AT_CUBEMAP_PASS) col += sky_sun_disc(a, dir, h);
	COLOR = col;
}
