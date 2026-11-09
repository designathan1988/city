// A railing as one face, from where its bars as boxes are under a pixel: the bars drawn by the fragment, each by the share of the pixel it really
// covers, so a railing thins into a veil at a distance and neither shimmers, makes moire nor vanishes. UV over the piece, UV2.x what it draws
// (shell.h CardKind), COLOR its tint. UV2.y of a railing: how far its boxes stand from any of its points at most, 0 when it has none (the mid
// level); of a brise, a fin's share of its pitch. Seen from both sides. It casts no shadow: its bars are under a shadow texel, and the one-pixel
// rule in that pass would draw them a texel wide, stripes on what stands beside.
shader_type spatial;
render_mode cull_disabled;
#include "res://render/shaders/contracts/buildings.gdshaderinc"

uniform float bars_end_m; // the bars and blades a card stands for, as boxes, end here (building_renderer_reach.cpp: the first part band's end)
uniform float roughness; // materials.json building_card, as the next
uniform float metalness;

const float BRISE_PITCH = 1.0 / float(BRISE_FINS); // a fin's pitch on the card
// The railings as a card draws them are the near level's boxes (contracts/buildings.gdshaderinc RAIL_*, part_models.cpp): the bars' pitch
// in front and on a side is one over their gaps.
const float FRONT_BARS = 1.0 / float(RAIL_FRONT_BARS), SIDE_BARS = 1.0 / float(RAIL_SIDE_BARS);
const float LEAST_FW = 1e-5; // a footprint never under this
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
	if (kind == CARD_BRISE) return every(q.x + 0.5 * BRISE_PITCH, BRISE_PITCH, share, fw.x * BRISE_FILTER_PX);
	if (kind == CARD_ROOF_RAILING) return either(either(bar(q.y, RAIL_TOP_AT, RAIL_TOP_H, fw.y), bar(q.y, ROOF_RAIL_MID_AT, RAIL_LOW_H, fw.y)), every(q.x, ROOF_RAIL_POSTS, ROOF_RAIL_POST_W, fw.x));
	float rails = either(bar(q.y, RAIL_TOP_AT, RAIL_TOP_H, fw.y), bar(q.y, RAIL_LOW_AT, RAIL_LOW_H, fw.y));
	return either(rails, every(q.x, kind == CARD_RAILING_FRONT ? FRONT_BARS : SIDE_BARS, RAIL_BAR, fw.x));
}

void fragment() {
	int kind = int(UV2.x + 0.5);
	if (level_noise(FRAGCOORD.xy) >= level_keep) discard;
	// nearer than its boxes' end the boxes draw it: a brise by its own points, a railing once every box of it stands (UV2.y)
	float eye_m = length(v_world - CAMERA_POSITION_WORLD);
	if (kind == CARD_BRISE ? eye_m < bars_end_m : (UV2.y > 0.0 && eye_m < bars_end_m - UV2.y)) discard;
	vec2 fw = max(fwidth(UV), vec2(LEAST_FW));
	// the share of the pixel kept as a share of the pixels that stay: a screen of bars under a pixel is a veil and no moire; the frames resolve the grain
	float grain = level_noise(FRAGCOORD.xy);
	if (cover(kind, UV, fw, UV2.y) <= grain) discard;
	ALBEDO = surface_out(srgb_to_linear(COLOR.rgb));
	ROUGHNESS = roughness;
	METALLIC = metalness;
}
