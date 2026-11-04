// A railing as one face: its bars drawn by the fragment, each at least a pixel wide and its colour mixed with what stands behind it by
// the share of the pixel a bar really covers, so a railing holds at any distance and neither shimmers nor vanishes. UV over the
// piece, UV2.x what it draws (shell.h CardKind), COLOR its tint. Seen from both sides.
shader_type spatial;
render_mode cull_disabled;

uniform float thin_bar_m = 0.02; // a bar's width
uniform float thin_behind = 0.16; // the tone behind a bar under a pixel
uniform float roughness = 0.45;
uniform float metalness = 0.55;

const int ROOF_RAILING = 2;
const int RAILING_FRONT = 3;
const float TOP_RAIL = 0.97, TOP_RAIL_H = 0.06;

varying vec3 v_world;

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
}

#include "res://render/shaders/building_colour.gdshaderinc"

float bar(float x, float c, float w, float fw) { return step(abs(x - c), 0.5 * max(w, fw)); }
float every(float x, float pitch, float w, float fw) { return step(abs(fract(x / pitch + 0.5) - 0.5) * pitch, 0.5 * max(w, fw)); }

bool on_bar(int kind, vec2 q, vec2 fw) {
	if (kind == ROOF_RAILING) return max(max(bar(q.y, TOP_RAIL, TOP_RAIL_H, fw.y), bar(q.y, 0.45, 0.04, fw.y)), every(q.x, 0.25, 0.02, fw.x)) > 0.5;
	float rails = max(bar(q.y, TOP_RAIL, TOP_RAIL_H, fw.y), bar(q.y, 0.12, 0.04, fw.y));
	return max(rails, every(q.x, kind == RAILING_FRONT ? 0.1 : 0.25, 0.015, fw.x)) > 0.5;
}

void fragment() {
	vec2 fw = fwidth(UV);
	float pixel_m = max(length(fwidth(v_world)) * 0.7071, 1e-4);
	if (!on_bar(int(UV2.x + 0.5), UV, fw)) discard;
	ALBEDO = surface_out(mix(vec3(thin_behind), to_linear(COLOR.rgb), clamp(thin_bar_m / pixel_m, 0.0, 1.0)));
	ROUGHNESS = roughness;
	METALLIC = metalness;
}
