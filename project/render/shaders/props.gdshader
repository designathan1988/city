// Props, street furniture, trees and the signal lenses, in linear on both renderers (color_space.gdshaderinc).
// COLOR.rgb is sRGB; UV2.x tags each part (native/src/render/props/prop_kit.h PropTag): 0 fixed colour, 1 tint
// (INSTANCE_CUSTOM.rgb, trees and agents), 2 glow at night (lamp glass, windows), 3 a signal lens (render/props/
// signal_lamps.h): UV2.y which lens (prop_kit.h Lens), its post's INSTANCE_CUSTOM.r and .g the car lens and the
// walkers' lens lit (-1: none). A lit lens is a light: no albedo nor reflection, only lit_radiance, the scene radiance
// the Environment's tone mapping shows as the lens colour (render/tone_map.gd, set by city_world.gd). An unlit lens is
// its colour lit by the scene, its surface polished polycarbonate (lens_roughness, lens_ior). One shader for all of
// them: each spatial shader's variants cost ~3 MiB.
shader_type spatial;

global uniform float night;
uniform vec3 glow_color : source_color;
uniform float glow_gain = 6.0;
uniform float roughness = 0.85;
uniform vec3 lit_radiance[5]; // per Lens (prop_kit.h Lens::COUNT)
uniform float lens_roughness = 0.0;
uniform float lens_ior = 1.5;

varying flat float v_tag;
varying flat vec3 v_tint;
varying flat float v_lit;
varying flat vec3 v_light;

#include "res://render/shaders/color_space.gdshaderinc"

void vertex() {
	v_tag = UV2.x;
	v_tint = INSTANCE_CUSTOM.rgb;
	v_lit = 0.0;
	v_light = vec3(0.0);
	if (v_tag > 2.5) {
		float lens = UV2.y;
		v_lit = abs(lens - INSTANCE_CUSTOM.r) < 0.5 || abs(lens - INSTANCE_CUSTOM.g) < 0.5 ? 1.0 : 0.0;
		v_light = lit_radiance[clamp(int(lens + 0.5), 0, 4)] * v_lit;
	}
}

void fragment() {
	vec3 c = srgb_to_linear(COLOR.rgb); // COLOR is sRGB on both renderers
	if (v_tag > 0.5 && v_tag < 1.5) {
		c *= v_tint;
	}
	ALBEDO = linear_out(c);
	ROUGHNESS = roughness;
	if (v_tag > 2.5) {
		ALBEDO = linear_out(c * (1.0 - v_lit));
		ROUGHNESS = lens_roughness;
		// Godot's dielectric reflectance at normal incidence is 0.16 SPECULAR^2 (scene_forward_lights_inc.glsl F0);
		// Fresnel's ((n - 1) / (n + 1))^2 for the lens's index of refraction. A lit lens reflects nothing.
		SPECULAR = sqrt(pow((lens_ior - 1.0) / (lens_ior + 1.0), 2.0) / 0.16) * (1.0 - v_lit);
		EMISSION = linear_out(v_light);
	} else if (v_tag > 1.5) {
		EMISSION = linear_out(source_linear(glow_color) * c * night * glow_gain);
	}
}
