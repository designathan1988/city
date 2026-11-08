// The weather of sky_model.gdshaderinc as the uniforms render/daylight.gd sets on a material that draws the sky (the sky's own and
// render/sky_light_probe.gd's): the night's colours and gain, how dark it is, the overcast and its light, a bolt's flash.
#ifndef SKY_WEATHER_INC
#define SKY_WEATHER_INC

uniform vec3 night_zenith : source_color = vec3(0.04, 0.08, 0.14);
uniform vec3 night_horizon : source_color = vec3(0.11, 0.15, 0.22);
uniform float night_gain = 1.0; // the night's colours as light (daylight.json sky_gain)
uniform float dark = 0.0; // 1 at night: the night's sky over the day's
uniform float overcast = 0.0; // the share of the sky the deck covers
uniform float overcast_pass = 0.25; // what a full deck lets down of the clear sky's light
uniform float flash = 0.0; // a bolt's light over the sky, a share of it

SkyWeather weather_now() {
	return SkyWeather(night_zenith, night_horizon, night_gain, dark, overcast, overcast_pass, flash);
}

#endif
