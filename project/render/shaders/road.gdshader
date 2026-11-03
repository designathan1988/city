// Roads (Roadcraft render/roads.ts): the mesher (native/src/render/roads/road_look.h) writes the sRGB strip colour in
// COLOR.rgb, the material id in COLOR.a * 16, texture metres in UV ((u, v) on segments, world xz on plates and
// corners) and the lamp pool coordinates in UV2 (|u - lamp| and metres from the first lamp; -1000 = junction plate,
// lit evenly; 1000 = no lamp). The cooked maps are tinted so their mean becomes the strip colour.
shader_type spatial;

global uniform float night;

uniform sampler2D asphalt_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D asphalt_normal : hint_normal, filter_linear_mipmap, repeat_enable;
uniform sampler2D footway_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D footway_normal : hint_normal, filter_linear_mipmap, repeat_enable;
uniform sampler2D kerb_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D verge_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float asphalt_m = 10.4;
uniform float footway_m = 7.2;
uniform float kerb_m = 3.2;
uniform float verge_m = 8.8;
uniform float normal_strength = 0.8;
uniform float kerb_block_m = 1.0;
uniform float gutter_joint_m = 1.0;
uniform float paver_w = 0.2;
uniform float paver_h = 0.1;
uniform float cobble_per_m = 4.2;
uniform vec3 pool_color : source_color = vec3(1.0, 0.82, 0.55);
uniform float pool_sigma = 7.5;
uniform float pool_step = 32.0;
uniform float pool_gain = 1.6;
uniform float plate_glow = 0.55;
uniform float pool_floor = 0.08; // pools light dark asphalt as if it were this much brighter

const int M_ASPHALT = 0;
const int M_PLATE = 1;
const int M_FOOTWAY = 2;
const int M_KERB = 3;
const int M_GRASS = 4;
const int M_CONCRETE = 5;
const int M_GUTTER = 6;
const int M_PAINT = 7;
const int M_PAVERS = 8;
const int M_STONE = 9;
const int M_COBBLE = 10;
const int M_BALLAST = 11;
const int M_SKIRT = 12;

varying flat int v_mat;
varying vec3 v_world;
varying vec2 v_pool;

// A surface map's own mean colour: its smallest mip, read in the same colour space as the texels it divides, so the
// strip colour stays the mean albedo on every renderer (a hand-typed sRGB mean was decoded differently by
// Compatibility and Forward+ and turned the asphalt black).
const float MEAN_LOD = 16.0;
const float MEAN_FLOOR = 0.001;

vec3 mean_of_asphalt() {
	return max(textureLod(asphalt_map, vec2(0.5), MEAN_LOD).rgb, vec3(MEAN_FLOOR));
}

vec3 to_linear(vec3 c) {
	return mix(c / 12.92, pow((c + 0.055) / 1.055, vec3(2.4)), step(0.04045, c));
}

// The strip colour (sRGB in the mesh) as linear albedo. The Compatibility renderer (OUTPUT_IS_SRGB) already hands
// COLOR over decoded, and decoding it again crushed dark strips: asphalt #4a4d53 came out at 0.3 % albedo, black.
vec3 vertex_albedo(vec3 c) {
	return OUTPUT_IS_SRGB ? c : to_linear(c);
}

float hash(vec2 p) {
	return fract(sin(dot(p, vec2(127.1, 311.7))) * 43758.5453);
}

float noise(vec2 p) {
	vec2 i = floor(p);
	vec2 f = fract(p);
	vec2 u = f * f * (3.0 - 2.0 * f);
	return mix(mix(hash(i), hash(i + vec2(1, 0)), u.x), mix(hash(i + vec2(0, 1)), hash(i + vec2(1, 1)), u.x), u.y);
}

// Coverage of the grout lines of a grid with cell size `cell` (fades out when a cell gets smaller than a few pixels).
float joints(vec2 p, vec2 cell, vec2 width, float pix) {
	vec2 f = fract(p / cell) * cell;
	vec2 d = min(f, cell - f);
	vec2 j = 1.0 - smoothstep(width * 0.5, width * 0.5 + pix, d);
	return max(j.x, j.y) * (1.0 - smoothstep(0.05, 0.25, pix / min(cell.x, cell.y)));
}

void vertex() {
	v_mat = int(floor(COLOR.a * 16.0));
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_pool = UV2;
}

float lamp_pools() {
	if (v_pool.x < -500.0) {
		return plate_glow;
	}
	if (v_pool.x > 500.0) {
		return 0.0;
	}
	float dv = v_pool.y - pool_step * round(v_pool.y / pool_step);
	float k = 1.0 / (2.0 * pool_sigma * pool_sigma);
	float du2 = max(v_pool.x, 0.0) * max(v_pool.x, 0.0);
	float far = pool_step - abs(dv);
	return exp(-(du2 + dv * dv) * k) + exp(-(du2 + far * far) * k);
}

void fragment() {
	vec3 base = vertex_albedo(COLOR.rgb);
	float pix = max(max(fwidth(v_world.x), fwidth(v_world.z)), 0.001);
	vec2 uv = UV;
	vec3 albedo = base;
	vec2 bend = vec2(0.0);
	float rough = 0.88;
	float blotch = 1.0 + (noise(v_world.xz * 0.08) - 0.5) * 0.12;
	vec3 nw = (INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz;
	bool up = nw.y > 0.7;
	if (v_mat == M_ASPHALT || v_mat == M_PLATE || v_mat == M_PAINT) {
		vec3 t = texture(asphalt_map, uv / asphalt_m).rgb;
		vec3 asphalt_avg = mean_of_asphalt();
		albedo = (v_mat == M_PAINT) ? base * mix(1.0, dot(t / asphalt_avg, vec3(0.333)), 0.25) : t * base / asphalt_avg * blotch;
		bend = (texture(asphalt_normal, uv / asphalt_m).xy - 0.5) * 2.0;
		rough = v_mat == M_PAINT ? 0.62 : 0.92;
	} else if (v_mat == M_FOOTWAY || v_mat == M_STONE) {
		albedo = texture(footway_map, uv.yx / footway_m).rgb * base / max(textureLod(footway_map, vec2(0.5), MEAN_LOD).rgb, vec3(MEAN_FLOOR));
		bend = (texture(footway_normal, uv.yx / footway_m).xy - 0.5) * 2.0;
	} else if (v_mat == M_PAVERS) {
		vec2 q = vec2(uv.y, uv.x);
		q.x += step(1.0, mod(floor(q.y / paver_h), 2.0)) * paver_w * 0.5;
		albedo = base * (1.0 - 0.3 * joints(q, vec2(paver_w, paver_h), vec2(0.016), pix)) * blotch;
	} else if (v_mat == M_KERB || v_mat == M_SKIRT || v_mat == M_GUTTER) {
		vec2 kq = (v_mat == M_KERB) ? uv.yx : uv;
		albedo = texture(kerb_map, kq / kerb_m).rgb * base / max(textureLod(kerb_map, vec2(0.5), MEAN_LOD).rgb, vec3(MEAN_FLOOR));
		float cell = v_mat == M_GUTTER ? gutter_joint_m : kerb_block_m;
		float along = v_mat == M_GUTTER ? uv.y : kq.x;
		float tone = 1.0 + (hash(vec2(floor(along / cell), 7.0)) - 0.5) * 0.1;
		float joint = 1.0 - smoothstep(0.008, 0.008 + pix, abs(fract(along / cell + 0.5) - 0.5) * cell);
		albedo *= tone * (1.0 - 0.35 * joint * step(pix, 0.2));
		rough = 0.82;
	} else if (v_mat == M_GRASS) {
		albedo = texture(verge_map, v_world.xz / verge_m).rgb * base / max(textureLod(verge_map, vec2(0.5), MEAN_LOD).rgb, vec3(MEAN_FLOOR));
		rough = 0.95;
	} else if (v_mat == M_COBBLE) {
		vec2 c = uv * cobble_per_m;
		float edge = joints(c, vec2(1.0), vec2(0.18), pix * cobble_per_m);
		albedo = base * (1.0 - 0.4 * edge) * (0.9 + 0.2 * hash(floor(c)));
	} else if (v_mat == M_CONCRETE || v_mat == M_BALLAST) {
		albedo = base * blotch * (0.94 + 0.12 * noise(v_world.xz * 3.0));
	}
	ALBEDO = albedo;
	ROUGHNESS = rough;
	SPECULAR = 0.35;
	if (up) {
		float fade = (1.0 - smoothstep(0.02, 0.5, pix)) * normal_strength;
		vec3 n = normalize(nw + vec3(-bend.x * fade, 0.0, -bend.y * fade));
		NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
	}
	EMISSION = (albedo + vec3(pool_floor)) * pool_color * lamp_pools() * night * pool_gain;
}
