// Shared by vegetation.gdshader (tree bodies) and vegetation_cards.gdshader (leaf cards, palm fronds, far cards); the
// vertex layout is native/src/render/trees/tree_kit.h's (UV2.x part, UV2.y a card's half size, UV a card's corner,
// COLOR.rgb the sRGB tone, COLOR.a a card's roll), the instance custom data the tint (near) or linear tone (far) and the
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

const float PART_CARD = 1.0, PART_FROND = 2.0, PART_FAR = 3.0; // tree_kit.h TreePart

uniform float near_m = 240.0;
uniform float fade_m = 30.0;
uniform float pop_m = 3.0;
uniform float wind_dir_x = 0.8; // the way the wind blows, x and z of a unit vector
uniform float wind_dir_z = -0.6;
uniform float wind_speed_ms = 3.0;
uniform float sway_per_ms2 = 0.00016;
uniform float gust_m = 220.0;
uniform float turbulence = 0.19;
uniform float sway_hz = 0.25;
uniform float roughness = 0.85;

varying flat vec3 v_tint;
varying flat float v_fade;
varying flat float v_part;

vec3 to_linear(vec3 c) {
	return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

// COLOR as linear albedo: Compatibility (OUTPUT_IS_SRGB) hands it over already decoded (road.gdshader does the same).
vec3 vertex_albedo(vec3 c, bool srgb_out) {
	return srgb_out ? c : to_linear(c);
}

// How much of the tree standing at `root` is drawn at distance `d`: its near level inside its own switch distance, its
// far card past it. Each tree switches at its own distance spread over fade_m, whole, crossing over only pop_m: a wood
// changes level tree by tree instead of every crown half dithered at once.
float tree_fade(vec3 root, float d, bool far) {
	float at = near_m + fade_m * fract(dot(floor(root.xz), vec2(0.0973, 0.1031)) * 47.53);
	float k = smoothstep(at, at + pop_m, d);
	return far ? k : 1.0 - k;
}

// The wind's push on a point `local` (model units, the tree one unit tall) of a tree standing at `root`, `height`
// metres tall.
vec3 wind_offset(vec3 local, vec3 root, float height, float t) {
	vec2 wind_dir = normalize(vec2(wind_dir_x, wind_dir_z));
	float h = clamp(local.y, 0.0, 1.4);
	float bend = sway_per_ms2 * wind_speed_ms * wind_speed_ms * h * h * height;
	// The gust the tree stands in: a wave of the frozen turbulence moving downwind at the mean speed.
	float gust = sin(TAU * (dot(root.xz, wind_dir) - wind_speed_ms * t) / gust_m);
	float phase = TAU * fract(dot(floor(root.xz), vec2(0.1031, 0.0973)) * 43.17); // each tree its own swing
	float sway = sin(TAU * sway_hz * t + phase);
	return vec3(wind_dir.x, 0.0, wind_dir.y) * bend * (1.0 + 2.0 * turbulence * gust + 2.0 * turbulence * sway);
}

// The point bent by `off` and put back at its distance from the foot (the length kept, GPU Gems 3 ch. 16).
vec3 keep_length(vec3 p, vec3 root, vec3 off) {
	vec3 a = p - root;
	float len = length(a);
	return len > 1e-4 ? root + normalize(a + off) * len : p + off;
}

// 4x4 ordered dither threshold of a pixel, for the cross-fade between a tree's levels.
float dither(vec2 frag) {
	ivec2 c = ivec2(mod(frag, 4.0));
	int k = c.x + c.y * 4;
	int bayer[16] = int[](0, 8, 2, 10, 12, 4, 14, 6, 3, 11, 1, 9, 15, 7, 13, 5);
	return (float(bayer[k]) + 0.5) / 16.0;
}

// Places one vertex of a tree (see the header): `vertex` and `normal` come in model space and leave in view space;
// `fade` is how much of the tree this level draws (0: none, its vertices collapsed); `tint` and `part` go on to the
// fragment. `view` is the pass's (the light's in the shadow pass), `main_inv` the main camera's, for the distance the
// levels switch at. Two sources: a MultiMesh instance (near models: `inst`, the instance's custom data, is its tint and
// species), or a far-card mesh (render/trees/far_cards.h: CUSTOM0 is the card's width, the tree's height, the species
// and the card's half height; COLOR.a the card centre's height; the corner is the vertex's index modulo 4).
void tree_vertex(inout vec3 vertex, inout vec3 normal, inout vec2 uv, vec2 uv2, vec4 color, vec4 inst, vec4 custom0, int vertex_id,
		mat4 model, mat4 view, mat4 main_inv, float t, out float fade, out vec3 tint, out float part) {
	bool far = custom0.w > 0.0;
	part = far ? PART_FAR : uv2.x;
	tint = far ? vec3(1.0) : inst.rgb;
	vec3 root = (model * vec4(far ? vertex : vec3(0.0), 1.0)).xyz;
	fade = tree_fade(root, distance(root, main_inv[3].xyz), far);
	if (fade <= 0.0) {
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
	float fade, part;
	vec3 tint;
	tree_vertex(VERTEX, NORMAL, UV, UV2, COLOR, INSTANCE_CUSTOM, CUSTOM0, VERTEX_ID, MODEL_MATRIX, VIEW_MATRIX, MAIN_CAM_INV_VIEW_MATRIX, TIME, fade, tint,
			part);
	v_tint = tint;
	v_fade = fade;
	v_part = part;
}
