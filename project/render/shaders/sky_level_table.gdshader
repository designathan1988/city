// The clear sky's light on level ground over the sun's heights, and the town's light in a sky with no sun, rendered once and read
// back once by render/sky_level_table.gd: what render/daylight.gd meters the eye's adaptation by (render/eye.gd). The level light does
// not hang on the sun's azimuth, so one row holds it. Texel x < steps: the sun at sin(elevation) from mu_low up to 1, the weather
// clear, no moon (the moon's sky is the same row times its light over the sun's); texel steps: the level light of the town's light at
// radiance 1, under a clear sky with no sun; texel steps + 1: that light's zenith.
shader_type canvas_item;

#include "res://render/shaders/sky_model.gdshaderinc"
#include "res://render/shaders/air_globals.gdshaderinc"

uniform int steps = 2;
uniform float mu_low = 0.0;

void fragment() {
	int x = int(FRAGCOORD.x);
	Air a = air_now();
	SkyWeather clear = SkyWeather(vec3(0.0, 1.0, 0.0), vec3(0.0), vec3(0.0), 0.0, 0.0, 0.0, 0.0);
	vec3 c;
	if (x < steps) {
		float mu = mix(mu_low, 1.0, float(x) / float(steps - 1));
		a.sun_dir = vec3(sqrt(1.0 - mu * mu), mu, 0.0);
		c = sky_level_seen(a, clear, 0.0);
	} else {
		a.sun_top = vec3(0.0);
		SkyWeather town = clear;
		town.city_up = vec3(1.0);
		c = x == steps ? sky_level_seen(a, town, 0.0) : sky_seen(a, town, vec3(0.0, 1.0, 0.0), 0.0);
	}
	COLOR = vec4(c, 1.0);
}
