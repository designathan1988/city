// The sky's light on level ground, one texel (render/sky_light_probe.gd renders it when the hour or the weather moves): the same air
// and weather as the sky (sky_model.gdshaderinc sky_level_seen), for what is lit by the sky without the engine's ambient, the
// rooms seen through glass and the contact shade's share.
shader_type canvas_item;

#include "res://render/shaders/sky_model.gdshaderinc"
#include "res://render/shaders/air_globals.gdshaderinc"
#include "res://render/shaders/sky_weather.gdshaderinc"

void fragment() {
	COLOR = vec4(sky_level_seen(air_now(), weather_now(), 0.0), 1.0);
}
