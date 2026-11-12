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
uniform vec3 dark_radiance[5]; // an unlit lens's faint glow (render/lamp_radiance.gd)
uniform float lens_roughness = 0.0;
uniform float lens_ior = 1.5;
uniform float wood_grain = 0.5;
uniform float post_metal = 0.6; // a sign post, galvanised
uniform float post_roughness = 0.45;
uniform sampler2DArray sign_atlas : source_color, filter_linear;

varying flat float v_tag;
varying flat vec3 v_tint;
varying flat float v_lit;
varying flat vec3 v_light;
varying flat float v_layer; // a plate's layer of the sign sheets, UV2.y (sign_atlas.h)
varying flat vec2 v_finish; // a barrier's metalness and roughness, INSTANCE_CUSTOM.b and .a (barrier_boxes.cpp)
varying vec3 v_local;
varying vec3 v_seen; // where it is in the world, and which way it faces
varying vec3 v_faces;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/lattice_noise.gdshaderinc"

// The surface of a thing out in the street, in world metres, for what the eye can tell apart: paint gone blotchy, dirt in the grain of it, and grime rising from the ground, as
// far as UV.y, its height. Near only: past `SURFACE_END_M` a pixel holds more than the noise does.
const float SURFACE_END_M = 90.0, SURFACE_FROM = 0.6;
const vec3 GRIME = vec3(0.62, 0.58, 0.52);
const float GRIME_REACH_M = 0.5;
const float WEAR_LOW = 0.82, WEAR_HIGH = 1.1;
const float WEAR_BLOTCH = 1.7, WEAR_SMUDGE = 7.1, WEAR_GRAIN = 37.0;
const float ROUGH_BLOTCH = 0.22, ROUGH_GRAIN = 0.1;

vec2 on_face(vec3 p, vec3 n) {
	vec3 a = abs(n);
	return a.y > a.x && a.y > a.z ? p.xz : a.x > a.z ? p.zy : p.xy;
}

void weather(vec3 p, vec3 n, float high, float near, inout vec3 c, inout float rough) {
	vec2 at = on_face(p, n);
	float blotch = vnoise(at * WEAR_BLOTCH) * 0.6 + vnoise(at * WEAR_SMUDGE) * 0.4, grain = vnoise(at * WEAR_GRAIN);
	vec3 worn = c * mix(WEAR_LOW, WEAR_HIGH, blotch) * mix(GRIME, vec3(1.0), smoothstep(0.0, GRIME_REACH_M, high));
	c = mix(c, worn, near);
	rough = clamp(rough + near * ((blotch - 0.5) * ROUGH_BLOTCH + (grain - 0.5) * ROUGH_GRAIN), 0.05, 1.0);
}

// A plank: `along` its length, `across` it (Modo's wood: rings distorted by waves, and a layer of noise for the grain), the rings fading before they alias.
const float PLANK_RINGS = 26.0, PLANK_WAVE = 7.0, PLANK_FIBRE = 110.0;
void plank(float along, float across, float width, inout vec3 c, inout float rough) {
	float wave = vnoise(vec2(along * 0.8, across * 5.0));
	float rings = 0.5 + 0.5 * sin(across * PLANK_RINGS + wave * PLANK_WAVE);
	float fibre = vnoise(vec2(along * 4.0, across * PLANK_FIBRE));
	float keep = clamp(1.0 - width * PLANK_RINGS * 0.3, 0.0, 1.0);
	c *= mix(1.0, mix(0.7, 1.12, rings) * (0.86 + 0.28 * fibre), keep);
	rough = mix(rough, 0.5 + 0.3 * fibre, keep);
}

void vertex() {
	v_tag = UV2.x;
	v_tint = INSTANCE_CUSTOM.rgb;
	v_lit = 0.0;
	v_light = vec3(0.0);
	v_finish = INSTANCE_CUSTOM.ba;
	v_layer = UV2.y;
	v_local = VERTEX;
	if (INSTANCE_CUSTOM.a > 0.5 && UV2.y > 0.5 && abs(v_tag - 3.0) > 0.5) {
		if ((int(INSTANCE_CUSTOM.a + 0.5) & int(UV2.y + 0.5)) == 0) {
			VERTEX = vec3(0.0); // a part this pole does not have
		}
	}
	v_seen = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_faces = (MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz;
	if (abs(v_tag - 3.0) < 0.5) {
		float lens = UV2.y;
		v_lit = abs(lens - INSTANCE_CUSTOM.r) < 0.5 || abs(lens - INSTANCE_CUSTOM.g) < 0.5 ? 1.0 : 0.0;
		int which = clamp(int(lens + 0.5), 0, 4);
		v_light = mix(dark_radiance[which], lit_radiance[which], v_lit);
	}
}

void fragment() {
	vec3 c = srgb_to_linear(COLOR.rgb); // COLOR is sRGB on both renderers
	if (v_tag > 0.5 && v_tag < 1.5) {
		c *= v_tint;
	}
	if (abs(v_tag - 5.0) < 0.5) {
		float around = atan(v_local.z, v_local.x) * 3.0;
		c *= 1.0 - wood_grain + 2.0 * wood_grain * vnoise(vec2(around, v_local.y * 0.7));
	}
	if (abs(v_tag - 6.0) < 0.5) {
		c = source_linear(texture(sign_atlas, vec3(UV, v_layer)).rgb);
	}
	float rough = roughness;
	if (abs(v_tag - 7.0) < 0.5) {
		METALLIC = v_finish.x;
		rough = v_finish.y;
	}
	if (abs(v_tag - 8.0) < 0.5) {
		METALLIC = post_metal;
		rough = post_roughness;
	}
	float near = 1.0 - smoothstep(SURFACE_END_M * SURFACE_FROM, SURFACE_END_M, length(VERTEX));
	if (near > 0.0) {
		if (abs(v_tag - 9.0) < 0.5) {
			plank(UV.x, UV.y, fwidth(UV.y), c, rough);
		} else if (v_tag < 0.5 || abs(v_tag - 5.0) < 0.5 || abs(v_tag - 7.0) < 0.5 || abs(v_tag - 8.0) < 0.5) {
			weather(v_seen, v_faces, abs(v_tag - 7.0) < 0.5 ? 1.0 : UV.y, near, c, rough);
		}
	}
	ALBEDO = linear_out(c);
	ROUGHNESS = rough;
	if (abs(v_tag - 3.0) < 0.5) {
		ALBEDO = linear_out(c * (1.0 - v_lit));
		ROUGHNESS = lens_roughness;
		// Godot's dielectric reflectance at normal incidence is 0.16 SPECULAR^2 (scene_forward_lights_inc.glsl F0);
		// Fresnel's ((n - 1) / (n + 1))^2 for the lens's index of refraction. A lit lens reflects nothing.
		SPECULAR = sqrt(pow((lens_ior - 1.0) / (lens_ior + 1.0), 2.0) / 0.16) * (1.0 - v_lit);
		EMISSION = linear_out(v_light);
	} else if (abs(v_tag - 2.0) < 0.5) {
		EMISSION = linear_out(source_linear(glow_color) * c * night * glow_gain);
	}
}
