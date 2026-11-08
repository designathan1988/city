// Shared by vegetation.gdshader (tree bodies) and vegetation_cards.gdshader (leaf cards, palm fronds, far cards); the
// vertex layout is native/src/render/trees/tree_kit.h's (UV2.x part, UV2.y a card's half size, UV a card's corner,
// COLOR.rgb the sRGB tone, COLOR.a a card's roll), the instance custom data the tint and the
// species index. Vertices are placed here (skip_vertex_transform): the wind bends the tree, cards turn to the view
// (the light's, in the shadow pass) and each tree fades between its near model and its far card by its own distance to
// the main camera.
//
// Wind: a tree bends quasi-statically under the drag of the wind it stands in, and sways at its own natural frequency
// under the turbulent part of that drag (Sousa, "Vegetation Procedural Animation and Shading in Crysis", GPU Gems 3
// ch. 16: the bend grows with the square of the height and the bent point keeps its distance from the foot).
//   static bend at the top, as a share of the height, per (m/s) squared of wind: sway_per_ms2 (drag goes with the
//   square of the speed; measured bending of 9.5 and 13.5 m pines under 4-8 m/s, Peltola et al. 1993);
//   gusts: the turbulence frozen in the mean wind and carried along by it (Taylor 1938), one gust a gust_m (the peak
//   of Kaimal et al. 1972's longitudinal spectrum, n z / U = 1/22 at z = 10 m), the speed varying by twice the
//   turbulence intensity either way (EN 1991-1-4 eq. 4.7, I = 1 / ln(z / z0), terrain category II);
//   sway: the turbulent share of the drag (2 I of it) at the natural frequency sway_hz (Jackson et al. 2019; 0.2-0.3 Hz
//   for trees of this size with leaves).
// Leaf flutter is not drawn: no measured amplitude for it has been found.
#include "res://render/shaders/contracts/trees.gdshaderinc"

const float FAR_TINT_SPAN = 1.0 / FAR_TINT_SHARE; // a vertex colour stops at 1: the tint packed at its share, either side of it

uniform float near_m; // set by the tree and cover renderers, as fade_m and the picture's frames
uniform float fade_m;
global uniform float wind_x; // the way the wind blows, x and z of a unit vector (render/sky_view.gd)
global uniform float wind_z;
global uniform float wind_ms;
uniform float sway_per_ms2; // materials.json: the gust set, the sway or stone set, each row's roughness and sheen
uniform float gust_m;
uniform float turbulence;
uniform float sway_hz;
uniform float roughness;
uniform float leaf_specular;
uniform float bend_most; // a point bends by its height up the tree, at most this many tree heights (materials.json gust)
// Each tree its own switch distance and swing phase: its cell's dot with two irrational-ish steps, scaled off the integers.
const vec2 SWITCH_STEP = vec2(0.0973, 0.1031), PHASE_STEP = vec2(0.1031, 0.0973);
const float SWITCH_SCALE = 47.53, PHASE_SCALE = 43.17;
const float LEAST_ARM = 1e-4; // a point this near the foot is moved, not turned
const float SHEEN_WHOLE_FROM = 0.6; // of the cosine of the view on the crown's normal

// A crown's normal is the bulk's, not a leaf's: where the eye grazes it, against the sun, the Fresnel of one smooth surface would
// light as white what is hundreds of leaves facing every way. Only a crown seen from the front has a sheen.
float leaf_sheen(vec3 n, vec3 view) {
	return leaf_specular * smoothstep(0.0, SHEEN_WHOLE_FROM, abs(dot(n, view)));
}

uniform int picture_frames;
uniform int picture_rows;
uniform float picture_top_deg;

varying flat vec3 v_tint;
varying flat vec2 v_cell; // far: the picture's frame (column, row)
varying flat float v_part;
varying vec3 v_model; // the point on the tree, metres from its foot

#include "res://render/shaders/color_space.gdshaderinc"

// COLOR as linear albedo: Compatibility (OUTPUT_IS_SRGB) hands it over already decoded (road.gdshader does the same).
vec3 vertex_albedo(vec3 c, bool srgb_out) {
	return srgb_out ? c : srgb_to_linear(c);
}

// Whether this level of the tree standing at `root` is drawn at distance `d`: its near level inside its own switch distance,
// its far card past it. Each tree switches whole at its own distance, spread over fade_m, so a wood changes level tree by tree
// and a still frame holds no tree between levels.
bool tree_shown(vec3 root, float d, bool far) {
	float at = near_m + fade_m * fract(dot(floor(root.xz), SWITCH_STEP) * SWITCH_SCALE);
	return (d >= at) == far;
}

// The wind's push on a point `local` (model units, the tree one unit tall) of a tree standing at `root`, `height`
// metres tall.
vec3 wind_offset(vec3 local, vec3 root, float height, float t) {
	vec2 wind_dir = normalize(vec2(wind_x, wind_z));
	float h = clamp(local.y, 0.0, bend_most);
	float bend = sway_per_ms2 * wind_ms * wind_ms * h * h * height;
	// The gust the tree stands in: a wave of the frozen turbulence moving downwind at the mean speed.
	float gust = sin(TAU * (dot(root.xz, wind_dir) - wind_ms * t) / gust_m);
	float phase = TAU * fract(dot(floor(root.xz), PHASE_STEP) * PHASE_SCALE); // each tree its own swing
	float sway = sin(TAU * sway_hz * t + phase);
	return vec3(wind_dir.x, 0.0, wind_dir.y) * bend * (1.0 + 2.0 * turbulence * gust + 2.0 * turbulence * sway);
}

// The point bent by `off` and put back at its distance from the foot (the length kept, GPU Gems 3 ch. 16).
vec3 keep_length(vec3 p, vec3 root, vec3 off) {
	vec3 a = p - root;
	float len = length(a);
	return len > LEAST_ARM ? root + normalize(a + off) * len : p + off;
}

// Places one vertex of a tree (see the header): `vertex` and `normal` come in model space and leave in view space;
// A level not shown has its vertices collapsed; `tint` and `part` go on to the
// fragment. `view` is the pass's (the light's in the shadow pass), `main_inv` the main camera's, for the distance the
// levels switch at. Two sources: a MultiMesh instance (near models: `inst`, the instance's custom data, is its tint and
// species), or a far-card mesh (render/trees/far_cards.h: CUSTOM0 is the card's width, the tree's height, the species
// and the card's half height; COLOR.a the card centre's height; the corner is the vertex's index modulo 4).
void tree_vertex(inout vec3 vertex, inout vec3 normal, inout vec2 uv, vec2 uv2, vec4 color, vec4 inst, vec4 custom0, int vertex_id,
		mat4 model, mat4 view, mat4 main_inv, float t, out vec3 tint, out float part, out vec2 cell) {
	bool far = custom0.w > 0.0;
	part = far ? PART_FAR : uv2.x;
	tint = far ? color.rgb * FAR_TINT_SPAN : inst.rgb;
	cell = vec2(0.0);
	vec3 root = (model * vec4(far ? vertex : vec3(0.0), 1.0)).xyz;
	if (!tree_shown(root, distance(root, main_inv[3].xyz), far)) {
		vertex = vec3(0.0);
		return;
	}
	bool card = part > PART_CARD - 0.5 && part < PART_CARD + 0.5;
	float sx = far ? custom0.x : length(model[0].xyz), sy = far ? custom0.y : length(model[1].xyz);
	vec3 local = far ? vec3(0.0, color.a, 0.0) : vertex;
	vec3 wp = far ? root + vec3(0.0, color.a * sy, 0.0) : (model * vec4(vertex, 1.0)).xyz;
	wp = keep_length(wp, root, wind_offset(local, root, sy, t));
	vec3 vp = (view * vec4(wp, 1.0)).xyz;
	if (far) {
		int k = vertex_id % 4;
		uv = vec2(float(k == 1 || k == 2), float(k >= 2));
		vp.xy += (uv - vec2(0.5)) * vec2(sx, 2.0 * custom0.w * sy);
		// The frame nearest the view: how far above the horizon this pass looks down.
		vec3 forward = -vec3(view[0][2], view[1][2], view[2][2]);
		float elev = degrees(asin(clamp(-forward.y, 0.0, 1.0)));
		cell = vec2(round(elev / picture_top_deg * float(picture_frames - 1)), custom0.z);
	} else if (card) {
		float a = color.a * TAU;
		vp.xy += mat2(vec2(cos(a), sin(a)), vec2(-sin(a), cos(a))) * (uv - vec2(0.5)) * (2.0 * uv2.y * sx);
	}
	vec3 n = far ? normalize(vec3(0.0, 1.0, 0.0) + normalize(main_inv[3].xyz - root)) : normalize(mat3(model) * normal);
	normal = normalize((view * vec4(n, 0.0)).xyz);
	vertex = vp;
}

// The vertex stage both vegetation shaders share.
void vertex() {
	v_model = VERTEX * length(MODEL_MATRIX[1].xyz);
	float part;
	vec3 tint;
	vec2 cell;
	tree_vertex(VERTEX, NORMAL, UV, UV2, COLOR, INSTANCE_CUSTOM, CUSTOM0, VERTEX_ID, MODEL_MATRIX, VIEW_MATRIX, MAIN_CAM_INV_VIEW_MATRIX, TIME, tint,
			part, cell);
	v_cell = cell;
	v_tint = tint;
	v_part = part;
}
