// The sky: the clear air's light (sky_model.gdshaderinc) from the eye's height, the sun's disc in the picture only, an overcast deck
// over it as the weather asks, and the night's own sky as the sun goes. render/air.gd sets the air, render/daylight.gd the rest.
// What the radiance map takes of it is the light every surface gets from the sky (the Environment's ambient and reflections).
shader_type sky;

#include "res://render/shaders/sky_model.gdshaderinc"

uniform vec3 solar_direction = vec3(0.0, 1.0, 0.0); // towards the sun, wherever the light of the night points
uniform vec3 solar_top = vec3(1.0);
uniform vec4 air_rayleigh;
uniform vec4 air_mie;
uniform vec4 air_ozone;
uniform vec3 air_ground;
uniform vec3 night_zenith : source_color = vec3(0.04, 0.08, 0.14);
uniform vec3 night_horizon : source_color = vec3(0.11, 0.15, 0.22);
uniform float dark = 0.0; // 1 at night: the night's sky in place of the day's
uniform float night_gain = 1.0; // the night's colours as light (daylight.json sky_gain)
uniform float overcast = 0.0; // the share of the sky the deck covers, as the weather has it
uniform float overcast_pass = 0.25; // what an overcast deck lets down of the clear sky's light (render/daylight.gd)
uniform float flash = 0.0; // a bolt's light over the sky, a share of it

const float NIGHT_CURVE = 0.62; // how fast the night's sky darkens from the horizon up

void sky() {
	vec3 dir = normalize(EYEDIR);
	float h = max(POSITION.y, 1.0);
	Air a = Air(solar_direction, solar_top, air_rayleigh, air_mie, air_ozone, air_ground);
	vec3 col = sky_radiance(a, dir, h);
	if (!AT_CUBEMAP_PASS) col += sky_sun_disc(a, dir, h);
	// An overcast deck: the CIE overcast sky (CIE S 011/E:2003 type 1), L = Lz (1 + 2 sin(elevation)) / 3, its level light E = 7/9 pi Lz
	// a share of the clear sky's direct light on level ground (render/daylight.gd overcast_pass).
	float lz = 9.0 / 7.0 * overcast_pass * dot(sky_sun_at(a, 0.0), vec3(0.2126, 0.7152, 0.0722)) * max(solar_direction.y, 0.0);
	vec3 deck = vec3(lz * (1.0 + 2.0 * max(dir.y, 0.0)) / 3.0);
	col = mix(col, deck, overcast);
	vec3 night = mix(night_horizon, night_zenith, pow(clamp(dir.y, 0.0, 1.0), NIGHT_CURVE));
	COLOR = (col + night * dark * night_gain) * (1.0 + flash);
}
