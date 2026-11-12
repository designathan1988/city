// The overhead wires of the streets' lines (render/props/prop_renderer.cpp draws each chunk's spans with this material, one
// MultiMesh a level): a cable bent from its template (native/src/render/props/prop_kit.h wire: a tube round x from t = 0 to 1)
// into its span's parabola, swung by the wind, its finish a dark cable's sheen (as the reference game draws its wires). On the
// screen a cable is never narrower than wire_clamp_px: a thinner one is drawn that wide, so it covers whole pixels and reads as
// a line over the street instead of a fraction of a pixel lost in the asphalt behind it. In linear on both renderers
// (color_space.gdshaderinc).
shader_type spatial;
render_mode cull_back;

uniform float wire_clamp_px = 1.0; // the least width a cable is drawn on the screen, in pixels
uniform float wire_sway_m = 0.35; // how far a wire swings at mid-span in a full gust
uniform float wire_metal = 0.3; // a cable's finish
uniform float wire_roughness = 0.55;

#include "res://render/shaders/color_space.gdshaderinc"

// A span's phase and its poles' kinds share INSTANCE_CUSTOM.y (street_props.cpp push_span): phase + KIND_STRIDE (kind_a + 2 kind_b),
// a kind 0 urban, 1 rural. A secondary wire carries SECONDARY_MARK in its rural seat (prop_kit.h WIRE_SECONDARY_MARK).
const float KIND_STRIDE = 8.0;
const float SECONDARY_MARK = 10000.0;
const float NEAREST_DEPTH_M = 0.01;
const float RADIUS_FLOOR_M = 1e-4;
const float GUST_HZ = 1.9, GUST_BASE = 0.55, GUST_SWING = 0.45;

// A wire's vertex: VERTEX.x is t along the span's chord (the instance's x axis), y and z its ring, UV its height up the poles
// and its seat along an urban pole's arm, `rural` its seat along a rural pole's; the arms (turned toward the street) have the
// headings INSTANCE_CUSTOM.z (t = 0) and .w (t = 1). The arm between them is horizontal, and the chord may climb, so the offset
// goes into the instance's axes by parts. The wire hangs in a parabola of depth INSTANCE_CUSTOM.x and swings across in the wind
// by its phase. A secondary wire with a rural pole at either end is not strung: nothing.
vec3 wire_vertex(vec3 v, vec2 place, float rural, vec2 headings, mat4 model, mat4 model_view, mat4 projection, vec2 viewport, float sag,
		float phase_kinds, float time, bool shadow) {
	float kinds = floor(phase_kinds / KIND_STRIDE);
	float phase = phase_kinds - kinds * KIND_STRIDE;
	bool rural_a = mod(kinds, 2.0) > 0.5, rural_b = kinds > 1.5;
	if (rural > 0.5 * SECONDARY_MARK && (rural_a || rural_b)) {
		return vec3(0.0);
	}
	float seat_a = rural_a ? rural : place.y, seat_b = rural_b ? rural : place.y;
	float t = v.x, u = t * 2.0 - 1.0;
	vec3 chord = model[0].xyz;
	vec2 arm = mix(vec2(cos(headings.x), sin(headings.x)) * seat_a, vec2(cos(headings.y), sin(headings.y)) * seat_b, t);
	float ahead = dot(arm, chord.xz) / dot(chord.xz, chord.xz);
	vec3 hung = vec3(t + ahead, place.x - ahead * chord.y, dot(arm, model[2].xz));
	float radius = max(length(v.yz), RADIUS_FLOOR_M);
	// In the shadow pass the wire is at rest, at its own width: the sun's projection is no screen, and read as one it swelled the
	// wires into shadows tens of metres wide along every line.
	float keep = 1.0;
	float swing = 0.0;
	if (!shadow) {
		float depth = max(-(model_view * vec4(hung, 1.0)).z, NEAREST_DEPTH_M);
		float wide = 2.0 * radius * projection[1][1] * viewport.y * 0.5 / depth; // its width on the screen, in pixels
		keep = max(1.0, wire_clamp_px / wide);
		swing = 4.0 * t * (1.0 - t) * wire_sway_m * (GUST_BASE + GUST_SWING * sin(time * GUST_HZ + phase)) * 0.5;
	}
	vec2 ring = v.yz * keep;
	return hung + vec3(0.0, ring.x - sag * (1.0 - u * u), ring.y + swing);
}

void vertex() {
	VERTEX = wire_vertex(VERTEX, UV, UV2.y, INSTANCE_CUSTOM.zw, MODEL_MATRIX, MODELVIEW_MATRIX, PROJECTION_MATRIX, VIEWPORT_SIZE,
			INSTANCE_CUSTOM.x, INSTANCE_CUSTOM.y, TIME, IN_SHADOW_PASS);
}

void fragment() {
	ALBEDO = linear_out(srgb_to_linear(COLOR.rgb)); // COLOR is sRGB on both renderers
	METALLIC = wire_metal;
	ROUGHNESS = wire_roughness;
}
