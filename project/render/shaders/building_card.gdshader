// A railing as one face: its bars drawn by the fragment, each by the share of the pixel it really covers, so a railing thins into a veil at a
// distance and neither shimmers, makes moire nor vanishes. UV over the piece, UV2.x what it draws (shell.h CardKind), COLOR its tint. Seen from both sides. It casts no shadow: its bars are under a
// shadow texel, and the one-pixel rule in that pass would draw them a texel wide, stripes on what stands beside.
shader_type spatial;
render_mode cull_disabled;
#include "res://render/shaders/shader_contracts.gdshaderinc"

uniform float veil_from_m = 100.0; // the blades of a brise, as boxes, end here (buildings.json lod.part_bands, the first band)
uniform float roughness = 0.45;
uniform float metalness = 0.55;

const int ROOF_RAILING = 2;
const int RAILING_FRONT = 3;
const int BRISE_KIND = 5;
const float BRISE_PITCH = 1.0 / float(BRISE_FINS); // a fin's pitch on the card
const float TOP_RAIL = 0.97, TOP_RAIL_H = 0.06;
const float BRISE_FILTER_PX = 2.0;

#include "res://render/shaders/building_level.gdshaderinc"

varying vec3 v_world;

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	if (level_back > 0.0) VERTEX += level_local(MODEL_MATRIX, normalize(v_world - CAMERA_POSITION_WORLD) * level_back);
	if (IN_SHADOW_PASS) VERTEX = vec3(0.0); // every corner on one point: nothing is drawn
}

#include "res://render/shaders/building_colour.gdshaderinc"

// How much of a pixel `fw` wide a bar covers: the box filter of the pulse, exact (Quilez, Filtering the checkerboard; Advanced RenderMan).
float bar(float x, float c, float w, float fw) { return clamp((0.5 * (w + fw) - abs(x - c)) / fw, 0.0, 1.0); }
// The integral of a train of pulses `w` wide every `pitch`, from 0 to x.
float train(float x, float pitch, float w) {
	float k = floor(x / pitch);
	return k * w + min(x - k * pitch, w);
}
float every(float x, float pitch, float w, float fw) {
	float lo = x + 0.5 * (w - fw), hi = x + 0.5 * (w + fw);
	return (train(hi, pitch, w) - train(lo, pitch, w)) / fw;
}
float either(float a, float b) { return 1.0 - (1.0 - a) * (1.0 - b); }

float cover(int kind, vec2 q, vec2 fw, float share) {
	// Fins a pixel or two apart: a box one pixel wide leaves their beat with the pixels as arcs across a tower; two pixels wide its first null falls on the
	// pitch where the beat is strongest.
	if (kind == BRISE_KIND) return every(q.x + 0.5 * BRISE_PITCH, BRISE_PITCH, share, fw.x * BRISE_FILTER_PX);
	if (kind == ROOF_RAILING) return either(either(bar(q.y, TOP_RAIL, TOP_RAIL_H, fw.y), bar(q.y, 0.45, 0.04, fw.y)), every(q.x, 0.25, 0.02, fw.x));
	float rails = either(bar(q.y, TOP_RAIL, TOP_RAIL_H, fw.y), bar(q.y, 0.12, 0.04, fw.y));
	return either(rails, every(q.x, kind == RAILING_FRONT ? 0.1 : 0.25, 0.015, fw.x));
}

void fragment() {
	int kind = int(UV2.x + 0.5);
	if (level_noise(FRAGCOORD.xy) >= level_keep) discard;
	if (kind == BRISE_KIND && length(v_world - CAMERA_POSITION_WORLD) < veil_from_m) discard;
	vec2 fw = max(fwidth(UV), vec2(1e-5));
	// the share of the pixel kept as a share of the pixels that stay: a screen of bars under a pixel is a veil and no moire; the frames resolve the grain
	float grain = level_noise(FRAGCOORD.xy);
	if (cover(kind, UV, fw, UV2.y) <= grain) discard;
	ALBEDO = surface_out(to_linear(COLOR.rgb));
	ROUGHNESS = roughness;
	METALLIC = metalness;
}
