// Roads: the mesher (native/src/render/roads/road_look.h) writes the sRGB strip colour in
// COLOR.rgb, the material id in COLOR.a * 16, texture metres in UV ((u, v) on segments, world xz on plates and
// corners) and the lamp pool coordinates in UV2 (|u - lamp| and metres from the first lamp; -1000 = junction plate,
// lit evenly; 1000 = no lamp). The cooked maps are tinted so their mean becomes the strip colour. Everything in linear
// on both renderers (color_space.gdshaderinc).
shader_type spatial;

global uniform float night;

uniform sampler2D asphalt_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D asphalt_normal : hint_normal, filter_linear_mipmap, repeat_enable;
uniform sampler2D footway_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D footway_normal : hint_normal, filter_linear_mipmap, repeat_enable;
uniform sampler2D kerb_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D verge_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
// Mean colour of each map, computed once in project/render/city_world.gd (one read instead of a smallest-mip fetch per
// pixel; the strip colour stays the mean albedo).
uniform vec3 mean_asphalt : source_color = vec3(1.0);
uniform vec3 mean_footway : source_color = vec3(1.0);
uniform vec3 mean_kerb : source_color = vec3(1.0);
uniform vec3 mean_verge : source_color = vec3(1.0);
uniform vec3 asphalt_reference : source_color = vec3(1.0); // the colour the asphalt map is authored against
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
uniform float pool_step = 1.0; // the lamp step, set by the road renderer from the street furniture catalog
uniform float pool_gain = 1.6;
uniform float plate_glow = 0.55;
uniform float pool_floor = 0.08; // pools light dark asphalt as if it were this much brighter

const int M_ROAD = 0;
const int M_PLATE = 1;
const int M_FOOTPATH = 2;
const int M_KERB_FACE = 3;
const int M_LAWN = 4;
const int M_CONCRETE = 5;
const int M_GUTTER = 6;
const int M_PAINT = 7;
const int M_PAVERS = 8;
const int M_STONE = 9;
const int M_COBBLE = 10;
const int M_BALLAST = 11;
const int M_SHOULDER_EDGE = 12;

varying flat int v_mat;
varying vec3 v_world;
varying vec2 v_pool;

const float MEAN_FLOOR = 0.001;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/noise2d.gdshaderinc"

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
	vec3 base = srgb_to_linear(COLOR.rgb); // the strip colour, sRGB in the mesh; everything below in linear
	float pix = max(max(fwidth(v_world.x), fwidth(v_world.z)), 0.001);
	vec2 uv = UV;
	vec3 albedo = base;
	vec2 bend = vec2(0.0);
	float rough = 0.88;
	float blotch = 1.0 + (noise(v_world.xz * 0.08) - 0.5) * 0.12;
	vec3 nw = (INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz;
	bool up = nw.y > 0.7;
	if (v_mat == M_ROAD || v_mat == M_PLATE || v_mat == M_PAINT) {
		vec3 t = source_linear(texture(asphalt_map, uv / asphalt_m).rgb);
		vec3 asphalt_avg = max(source_linear(mean_asphalt), vec3(MEAN_FLOOR));
		albedo = (v_mat == M_PAINT) ? base * mix(1.0, dot(t / asphalt_avg, vec3(0.333)), 0.25) : t * base / max(source_linear(asphalt_reference), vec3(MEAN_FLOOR)) * blotch;
		bend = (texture(asphalt_normal, uv / asphalt_m).xy - 0.5) * 2.0;
		rough = v_mat == M_PAINT ? 0.62 : 0.92;
	} else if (v_mat == M_FOOTPATH || v_mat == M_STONE) {
		vec3 t = source_linear(texture(footway_map, uv.yx / footway_m).rgb);
		albedo = t * base / max(source_linear(mean_footway), vec3(MEAN_FLOOR));
		bend = (texture(footway_normal, uv.yx / footway_m).xy - 0.5) * 2.0;
	} else if (v_mat == M_PAVERS) {
		vec2 q = vec2(uv.y, uv.x);
		q.x += step(1.0, mod(floor(q.y / paver_h), 2.0)) * paver_w * 0.5;
		albedo = base * (1.0 - 0.3 * joints(q, vec2(paver_w, paver_h), vec2(0.016), pix)) * blotch;
	} else if (v_mat == M_KERB_FACE || v_mat == M_SHOULDER_EDGE || v_mat == M_GUTTER) {
		vec2 kq = (v_mat == M_KERB_FACE) ? uv.yx : uv;
		albedo = source_linear(texture(kerb_map, kq / kerb_m).rgb)
				* base / max(source_linear(mean_kerb), vec3(MEAN_FLOOR));
		float cell = v_mat == M_GUTTER ? gutter_joint_m : kerb_block_m;
		float along = v_mat == M_GUTTER ? uv.y : kq.x;
		float tone = 1.0 + (hash(vec2(floor(along / cell), 7.0)) - 0.5) * 0.1;
		float joint = 1.0 - smoothstep(0.008, 0.008 + pix, abs(fract(along / cell + 0.5) - 0.5) * cell);
		albedo *= tone * (1.0 - 0.35 * joint * step(pix, 0.2));
		rough = 0.82;
	} else if (v_mat == M_LAWN) {
		albedo = source_linear(texture(verge_map, v_world.xz / verge_m).rgb) * base
				/ max(source_linear(mean_verge), vec3(MEAN_FLOOR));
		rough = 0.95;
	} else if (v_mat == M_COBBLE) {
		vec2 c = uv * cobble_per_m;
		float edge = joints(c, vec2(1.0), vec2(0.18), pix * cobble_per_m);
		albedo = base * (1.0 - 0.4 * edge) * (0.9 + 0.2 * hash(floor(c)));
	} else if (v_mat == M_CONCRETE || v_mat == M_BALLAST) {
		albedo = base * blotch * (0.94 + 0.12 * noise(v_world.xz * 3.0));
	}
	ALBEDO = linear_out(albedo);
	ROUGHNESS = rough;
	SPECULAR = 0.35;
	if (up) {
		float fade = (1.0 - smoothstep(0.02, 0.5, pix)) * normal_strength;
		vec3 n = normalize(nw + vec3(-bend.x * fade, 0.0, -bend.y * fade));
		NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
	}
	float pools = night > 0.0 ? lamp_pools() : 0.0; // daylight: skip the two exp() per pixel
	EMISSION = linear_out((albedo + vec3(pool_floor)) * source_linear(pool_color) * pools * night * pool_gain);
}
