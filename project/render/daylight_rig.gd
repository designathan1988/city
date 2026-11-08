## The light of the day, made once for the world (render/city_world.gd): the sun with its shadows, that Daylight
## (render/daylight.gd) then drives by the hour. The sky's light is the Environment's, read from the sky itself.
class_name DaylightRig
extends RefCounted

## The sun with its shadows: four cascades, blended where they meet.
static func sun(day: Dictionary) -> DirectionalLight3D:
	var light := DirectionalLight3D.new()
	light.shadow_enabled = true
	light.directional_shadow_mode = DirectionalLight3D.SHADOW_PARALLEL_4_SPLITS
	light.directional_shadow_max_distance = day.shadow_max_distance_m
	light.directional_shadow_split_1 = day.shadow_split_1
	light.directional_shadow_split_2 = day.shadow_split_2
	light.directional_shadow_split_3 = day.shadow_split_3
	light.directional_shadow_blend_splits = true
	light.directional_shadow_fade_start = day.shadow_fade_start
	light.shadow_normal_bias = day.shadow_normal_bias
	light.shadow_blur = day.shadow_blur
	light.directional_shadow_pancake_size = day.shadow_pancake_m
	return light
