// Props, street furniture, trees and the signal lenses, in linear on both renderers (color_space.gdshaderinc).
// COLOR.rgb is sRGB; UV2.x tags each part (native/src/render/props/prop_kit.h PropTag): 0 fixed colour, 1 tint
// (INSTANCE_CUSTOM.rgb, trees and agents), 2 glow at night (lamp glass, windows), 3 a signal lens (render/props/
// signal_lamps.h): UV2.y which lens (prop_kit.h Lens), its post's INSTANCE_CUSTOM.r and .g the car lens and the
// walkers' lens lit (-1: none). A lit lens is a light: no albedo nor reflection, only lit_radiance, the scene radiance
// the Environment's tone mapping shows as the lens colour (render/tone_map.gd, set by city_world.gd). An unlit lens is
// its colour lit by the scene, its surface polished polycarbonate (lens_roughness, lens_ior). One shader for all of
// them: each spatial shader's variants cost ~3 MiB.
shader_type spatial;
#include "res://render/shaders/contracts/streets.gdshaderinc"

global uniform float night;
uniform vec3 glow_color : source_color;
uniform float glow_gain = 6.0;
uniform float roughness = 0.85;
uniform vec3 lit_radiance[LENS_COUNT]; // per lens
uniform vec3 dark_radiance[LENS_COUNT]; // an unlit lens's faint glow (render/lamp_radiance.gd)
uniform float lens_roughness = 0.0;
uniform float lens_ior = 1.5;
uniform float wire_min_px = 0.7; // a wire never narrower than this on the screen
uniform float wire_sway_m = 0.35; // how far a wire swings at mid-span in a full gust
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

// A span's phase and its poles' kinds share INSTANCE_CUSTOM.y (street_props.cpp push_span): phase + PROP_SPAN_KIND_STRIDE (kind_a +
// 2 kind_b), a kind 0 urban, 1 rural. A secondary wire carries WIRE_SECONDARY_MARK in its rural seat.

// A wire's vertex: VERTEX.x is t along the span's chord (the instance's x axis), y and z its ring, UV its height up the poles
// and its seat along an urban pole's arm, `rural` its seat along a rural pole's; the arms (turned toward the street) have the
// headings INSTANCE_CUSTOM.z (t = 0) and .w (t = 1). The arm between them is horizontal, and the chord may climb, so the offset
// goes into the instance's axes by parts. The wire hangs in a parabola of depth INSTANCE_CUSTOM.x, swings across in the wind by
// its phase, and keeps wire_min_px on the screen. A secondary wire with a rural pole at either end is not strung: nothing.
vec3 wire_vertex(vec3 v, vec2 place, float rural, vec2 headings, mat4 model, mat4 model_view, mat4 projection, vec2 viewport, float sag, float phase_kinds, float time, bool shadow) {
	float kinds = floor(phase_kinds / PROP_SPAN_KIND_STRIDE);
	float phase = phase_kinds - kinds * PROP_SPAN_KIND_STRIDE;
	bool rural_a = mod(kinds, 2.0) > 0.5, rural_b = kinds > 1.5;
	if (rural > 0.5 * WIRE_SECONDARY_MARK && (rural_a || rural_b)) {
		return vec3(0.0);
	}
	float seat_a = rural_a ? rural : place.y, seat_b = rural_b ? rural : place.y;
	float t = v.x, u = t * 2.0 - 1.0;
	vec3 chord = model[0].xyz;
	vec2 arm = mix(vec2(cos(headings.x), sin(headings.x)) * seat_a, vec2(cos(headings.y), sin(headings.y)) * seat_b, t);
	float ahead = dot(arm, chord.xz) / dot(chord.xz, chord.xz);
	vec3 hung = vec3(t + ahead, place.x - ahead * chord.y, dot(arm, model[2].xz));
	float depth = max(-(model_view * vec4(hung, 1.0)).z, 0.01);
	float pixel = depth / (projection[1][1] * viewport.y * 0.5);
	float radius = max(length(v.yz), 1e-4);
	// In the shadow pass the wire is at rest, at its own width: the sun's projection is no screen, and read as one it swelled the wires
	// into shadows tens of metres wide along every line (the 4.0: wireMaterials, the depth material).
	float keep = shadow ? 1.0 : max(1.0, wire_min_px * pixel * 0.5 / radius);
	vec2 ring = v.yz * keep;
	float swing = shadow ? 0.0 : 4.0 * t * (1.0 - t) * wire_sway_m * (0.55 + 0.45 * sin(time * 1.9 + phase)) * 0.5;
	return hung + vec3(0.0, ring.x - sag * (1.0 - u * u), ring.y + swing);
}

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
	if (abs(v_tag - PROP_WIRE) < 0.5) {
		VERTEX = wire_vertex(VERTEX, UV, UV2.y, INSTANCE_CUSTOM.zw, MODEL_MATRIX, MODELVIEW_MATRIX, PROJECTION_MATRIX, VIEWPORT_SIZE, INSTANCE_CUSTOM.x, INSTANCE_CUSTOM.y, TIME, IN_SHADOW_PASS);
	} else if (INSTANCE_CUSTOM.a > 0.5 && UV2.y > 0.5 && abs(v_tag - PROP_LENS) > 0.5) {
		if ((int(INSTANCE_CUSTOM.a + 0.5) & int(UV2.y + 0.5)) == 0) {
			VERTEX = vec3(0.0); // a part this pole does not have
		}
	}
	v_seen = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_faces = (MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz;
	if (abs(v_tag - PROP_LENS) < 0.5) {
		float lens = UV2.y;
		v_lit = abs(lens - INSTANCE_CUSTOM.r) < 0.5 || abs(lens - INSTANCE_CUSTOM.g) < 0.5 ? 1.0 : 0.0;
		int which = clamp(int(lens + 0.5), 0, LENS_COUNT - 1);
		v_light = mix(dark_radiance[which], lit_radiance[which], v_lit);
	}
}

void fragment() {
	vec3 c = srgb_to_linear(COLOR.rgb); // COLOR is sRGB on both renderers
	if (abs(v_tag - PROP_TINT) < 0.5) {
		c *= v_tint;
	}
	if (abs(v_tag - PROP_WOOD) < 0.5) {
		float around = atan(v_local.z, v_local.x) * 3.0;
		c *= 1.0 - wood_grain + 2.0 * wood_grain * vnoise(vec2(around, v_local.y * 0.7));
	}
	if (abs(v_tag - PROP_PLATE) < 0.5) {
		c = source_linear(texture(sign_atlas, vec3(UV, v_layer)).rgb);
	}
	float rough = roughness;
	if (abs(v_tag - PROP_FINISHED) < 0.5) {
		METALLIC = v_finish.x;
		rough = v_finish.y;
	}
	if (abs(v_tag - PROP_SIGN_POST) < 0.5) {
		METALLIC = post_metal;
		rough = post_roughness;
	}
	float near = 1.0 - smoothstep(SURFACE_END_M * SURFACE_FROM, SURFACE_END_M, length(VERTEX));
	if (near > 0.0) {
		if (abs(v_tag - PROP_PLANK) < 0.5) {
			plank(UV.x, UV.y, fwidth(UV.y), c, rough);
		} else if (abs(v_tag - PROP_FIXED) < 0.5 || abs(v_tag - PROP_WOOD) < 0.5 || abs(v_tag - PROP_FINISHED) < 0.5 || abs(v_tag - PROP_SIGN_POST) < 0.5) {
			weather(v_seen, v_faces, abs(v_tag - PROP_FINISHED) < 0.5 ? 1.0 : UV.y, near, c, rough);
		}
	}
	ALBEDO = linear_out(c);
	ROUGHNESS = rough;
	if (abs(v_tag - PROP_LENS) < 0.5) {
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
