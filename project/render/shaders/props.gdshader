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
uniform float wire_min_px = 0.7; // a wire never narrower than this on the screen
uniform float wire_sway_m = 0.35; // how far a wire swings at mid-span in a full gust
uniform float wood_grain = 0.5;
uniform float post_metal = 0.6; // a sign post, galvanised
uniform float post_roughness = 0.45;
uniform sampler2D sign_atlas : source_color, filter_linear;

varying flat float v_tag;
varying flat vec3 v_tint;
varying flat float v_lit;
varying flat vec3 v_light;
varying flat vec2 v_finish; // a barrier's metalness and roughness, INSTANCE_CUSTOM.b and .a (barrier_boxes.cpp)
varying vec3 v_local;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/lattice_noise.gdshaderinc"

// A wire's vertex: VERTEX.x is t along the span's chord (the instance's x axis), y and z its ring, UV its place: metres up the
// poles and along the arms, whose headings are INSTANCE_CUSTOM.z (t = 0) and .w (t = 1). The arm between them is horizontal,
// and the chord may climb, so the offset goes into the instance's axes by parts. The wire hangs in a parabola of depth
// INSTANCE_CUSTOM.x, swings across in the wind by its phase INSTANCE_CUSTOM.y, and keeps wire_min_px on the screen.
vec3 wire_vertex(vec3 v, vec2 place, vec2 headings, mat4 model, mat4 model_view, mat4 projection, vec2 viewport, float sag, float phase, float time) {
	float t = v.x, u = t * 2.0 - 1.0;
	vec3 chord = model[0].xyz;
	vec2 arm = mix(vec2(cos(headings.x), sin(headings.x)), vec2(cos(headings.y), sin(headings.y)), t) * place.y;
	float ahead = dot(arm, chord.xz) / dot(chord.xz, chord.xz);
	vec3 hung = vec3(t + ahead, place.x - ahead * chord.y, dot(arm, model[2].xz));
	float depth = max(-(model_view * vec4(hung, 1.0)).z, 0.01);
	float pixel = depth / (projection[1][1] * viewport.y * 0.5);
	float radius = max(length(v.yz), 1e-4);
	float keep = max(1.0, wire_min_px * pixel * 0.5 / radius);
	vec2 ring = v.yz * keep;
	float swing = 4.0 * t * (1.0 - t) * wire_sway_m * (0.55 + 0.45 * sin(time * 1.9 + phase)) * 0.5;
	return hung + vec3(0.0, ring.x - sag * (1.0 - u * u), ring.y + swing);
}

void vertex() {
	v_tag = UV2.x;
	v_tint = INSTANCE_CUSTOM.rgb;
	v_lit = 0.0;
	v_light = vec3(0.0);
	v_finish = INSTANCE_CUSTOM.ba;
	v_local = VERTEX;
	if (abs(v_tag - 4.0) < 0.5) {
		VERTEX = wire_vertex(VERTEX, UV, INSTANCE_CUSTOM.zw, MODEL_MATRIX, MODELVIEW_MATRIX, PROJECTION_MATRIX, VIEWPORT_SIZE, INSTANCE_CUSTOM.x, INSTANCE_CUSTOM.y, TIME);
	} else if (INSTANCE_CUSTOM.a > 0.5 && UV2.y > 0.5 && abs(v_tag - 3.0) > 0.5) {
		if ((int(INSTANCE_CUSTOM.a + 0.5) & int(UV2.y + 0.5)) == 0) {
			VERTEX = vec3(0.0); // a part this pole does not have
		}
	}
	if (abs(v_tag - 3.0) < 0.5) {
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
	if (abs(v_tag - 5.0) < 0.5) {
		float around = atan(v_local.z, v_local.x) * 3.0;
		c *= 1.0 - wood_grain + 2.0 * wood_grain * vnoise(vec2(around, v_local.y * 0.7));
	}
	if (abs(v_tag - 6.0) < 0.5) {
		c = source_linear(texture(sign_atlas, UV).rgb);
	}
	ALBEDO = linear_out(c);
	ROUGHNESS = roughness;
	if (abs(v_tag - 7.0) < 0.5) {
		METALLIC = v_finish.x;
		ROUGHNESS = v_finish.y;
	}
	if (abs(v_tag - 8.0) < 0.5) {
		METALLIC = post_metal;
		ROUGHNESS = post_roughness;
	}
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
