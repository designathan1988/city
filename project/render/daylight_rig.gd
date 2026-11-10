## The lights of the day, made once for the world (render/city_world.gd): the sun with its shadows and the flat lights of the
## sky and the ground that Daylight (render/daylight.gd) then drives by the hour.
class_name DaylightRig
extends RefCounted

const RIGHT_ANGLE := 90.0


## A light without shadows.
static func plain() -> DirectionalLight3D:
	var light := DirectionalLight3D.new()
	light.shadow_enabled = false
	return light


## What the sky adds to what faces it, and to nothing else: walls take the flat ambient, the ground and the roofs this
## as well, so a shaded wall and a shaded street do not get the same light (the 4.0's hemisphere light and sky map).
static func top() -> DirectionalLight3D:
	var light := plain()
	light.light_specular = 0.0
	light.rotation_degrees = Vector3(-RIGHT_ANGLE, 0.0, 0.0)
	return light


## What the ground does not give back to what faces it: the same lobe as the sky's, taken away, so a flat ambient and the two
## make a hemisphere, the sky over and the ground under (a soffit, the underside of a crown).
static func under() -> DirectionalLight3D:
	var light := plain()
	light.light_specular = 0.0
	light.light_negative = true
	light.rotation_degrees = Vector3(RIGHT_ANGLE, 0.0, 0.0)
	return light


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
