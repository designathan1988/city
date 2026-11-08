// What lights a cloud, a fog bank and the mist (render/shaders/atmosphere.gdshader): the sun's or the moon's own light (the pass's
// sun_light and sun_dir), the sky's level light, the ground's and the town's light from below. Needs emission_light.gdshaderinc,
// sky_level_map.gdshaderinc and air_globals.gdshaderinc.
#ifndef CLOUD_LIGHT_INC
#define CLOUD_LIGHT_INC

uniform float cloud_albedo; // daylight.json sky_glow.cloud_reflect: what a cloud sends back of the light on it
global uniform vec3 town_light; // the town's light sent up while its lamps are on, a radiance (render/daylight.gd)

// A cloud's light, white and diffuse (its droplets scatter all colours alike, many times over): cloud_albedo of what falls on it,
// the sun's (or the moon's) light where it reaches the point (`lit`), the sky's level light from above (render/sky_light_probe.gd:
// the sun's, the moon's and the town's skies) and from below the ground's (its albedo under the sun and the sky) and the town's
// own light, shared by how high in the cloud the point is (its upper half sees more sky, its base more ground). A radiance in the
// scene's unit, pre-exposed as the scene is.
vec3 cloud_light(float lit, float up) {
	vec3 level = sky_level();
	vec3 below = air_ground * (sun_light * max(sun_dir.y, 0.0) + level) + town_light;
	return unshaded_light(cloud_albedo * (sun_light * lit + mix(below, level, 0.5 + 0.5 * up)));
}

#endif
