// The sky's light on level ground as render/sky_light_probe.gd renders it (one texel, sky_level.gdshader), and the daylight a room
// seen through its glass takes of it: the daylight factor, the share of the unobstructed sky's level light that reaches a room's
// working plane (render/daylight.gd sets it from data/game/daylight.json). By its definition the direct sun is not in it.
#ifndef SKY_LEVEL_MAP_INC
#define SKY_LEVEL_MAP_INC

global uniform sampler2D sky_level_map : filter_nearest;
global uniform float daylight_factor;

vec3 sky_level() {
	return texelFetch(sky_level_map, ivec2(0), 0).rgb;
}

vec3 room_daylight() {
	return daylight_factor * sky_level();
}

#endif
