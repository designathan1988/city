// The weather of sky_model.gdshaderinc as the uniforms render/daylight.gd sets on a material that draws the sky (the sky's own and
// render/sky_light_probe.gd's): the moon, the town's light, the overcast and its light, a bolt's flash.
#ifndef SKY_WEATHER_INC
#define SKY_WEATHER_INC

uniform vec3 moon_dir = vec3(0.0, 1.0, 0.0); // towards the moon
uniform vec3 moon_top = vec3(0.0); // its light over the air, none while the sun is up
uniform vec3 city_up = vec3(0.0); // the town's light sent up, the radiance of the ground under the air
uniform float cloud_reflect = 0.0; // what an overcast deck sends back of it
uniform float overcast = 0.0; // the share of the sky the deck covers
uniform float overcast_pass = 0.25; // what a full deck lets down of the clear sky's light
uniform float flash = 0.0; // a bolt's light over the sky, a share of it

SkyWeather weather_now() {
	return SkyWeather(moon_dir, moon_top, city_up, cloud_reflect, overcast, overcast_pass, flash);
}

#endif
