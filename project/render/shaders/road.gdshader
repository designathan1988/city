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
uniform sampler2D verge_normal : hint_normal, filter_linear_mipmap, repeat_enable;
// Mean colour of each map, computed once in project/render/city_world.gd (one read instead of a smallest-mip fetch per
// pixel; the strip colour stays the mean albedo).
uniform vec3 mean_asphalt : source_color = vec3(1.0);
uniform vec3 mean_footway : source_color = vec3(1.0);
uniform vec3 mean_kerb : source_color = vec3(1.0);
uniform vec3 asphalt_reference : source_color = vec3(1.0); // the colour the asphalt map is authored against
uniform float asphalt_m = 10.4;
uniform float footway_m = 7.2;
uniform float kerb_m = 3.2;
uniform float verge_m = 8.8;
uniform vec3 lane_tone[8]; // linear, by road class (road_look.h wear_color)
uniform vec3 footway_tone;
uniform float normal_asphalt = 1.0;
uniform float normal_footway = 1.0;
uniform float normal_kerb = 0.85;
uniform float normal_verge = 0.9;
uniform vec3 detail_asphalt = vec3(0.6, 0.65, 1.6); // how hard the detail colours, how hard it bends, the macro map's blur
uniform vec3 detail_footway = vec3(0.7, 0.8, 0.6);
uniform vec3 detail_kerb = vec3(0.55, 0.7, 0.8);
uniform vec3 detail_verge = vec3(1.0, 1.2, 2.0);
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
const int M_LANE = 6;
const int M_PAINT = 7;
const int M_PAVERS = 8;
const int M_STONE = 9;
const int M_COBBLE = 10;
const int M_BALLAST = 11;
const int M_SHOULDER_EDGE = 12;

varying flat int v_mat;
varying float v_id;
varying vec3 v_world;
varying vec2 v_pool;

const float MEAN_FLOOR = 0.001;

#include "res://render/shaders/color_space.gdshaderinc"
#include "res://render/shaders/lattice_noise.gdshaderinc"
#include "res://render/shaders/road_detail.gdshaderinc"
#include "res://render/shaders/road_wear.gdshaderinc"
#include "res://render/shaders/wear.gdshaderinc"

// Coverage of the grout lines of a grid with cell size `cell` (fades out when a cell gets smaller than a few pixels).
float joints(vec2 p, vec2 cell, vec2 width, float pix) {
	vec2 f = fract(p / cell) * cell;
	vec2 d = min(f, cell - f);
	vec2 j = 1.0 - smoothstep(width * 0.5, width * 0.5 + pix, d);
	return max(j.x, j.y) * (1.0 - smoothstep(0.05, 0.25, pix / min(cell.x, cell.y)));
}

void vertex() {
	v_id = COLOR.a * 16.0;
	v_mat = int(floor(v_id));
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
	if (v_mat == M_LANE) {
		base = lane_tone[int(fract(v_id) * TONE_SLOTS)];
	} else if (v_mat == M_FOOTPATH) {
		base = footway_tone;
	}
	float pix = max(max(fwidth(v_world.x), fwidth(v_world.z)), 0.001);
	vec2 uv = UV;
	vec3 albedo = base;
	vec2 bend = vec2(0.0);
	vec2 fine = vec2(0.0);
	float bend_scale = normal_asphalt;
	float near = detail_weight(pix);
	float rough = 1.0;
	float blotch = 1.0 + (vnoise(v_world.xz * 0.08) - 0.5) * 0.12;
	vec3 nw = (INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz;
	bool up = nw.y > 0.7;
	if (v_mat == M_ROAD || v_mat == M_PLATE || v_mat == M_PAINT || v_mat == M_LANE) {
		vec3 t = source_linear(texture(asphalt_map, uv / asphalt_m, near * detail_asphalt.z).rgb);
		vec2 duv = uv / asphalt_detail_m, ddx = dFdx(duv), ddy = dFdy(duv);
		if (near > 0.0) {
			t *= mix(vec3(1.0), detail_albedo(asphalt_detail, duv, ddx, ddy), near * detail_asphalt.x);
			fine = detail_bend(asphalt_detail_normal, duv, ddx, ddy) * near * detail_asphalt.y;
		}
		vec3 asphalt_avg = max(source_linear(mean_asphalt), vec3(MEAN_FLOOR));
		albedo = (v_mat == M_PAINT) ? base * mix(1.0, dot(t / asphalt_avg, vec3(0.333)), 0.25) : t * base / max(source_linear(asphalt_reference), vec3(MEAN_FLOOR)) * blotch;
		if (v_mat == M_LANE) {
			float grain = source_linear(texture(asphalt_map, uv / asphalt_m * 2.7).rgb).r;
			albedo *= lane_wear(min(COLOR.r, COLOR.g) * WEAR_SPAN_M, COLOR.b * WEAR_LANE_M, grain);
		}
		bend = (texture(asphalt_normal, uv / asphalt_m, near * detail_asphalt.z).xy - 0.5) * 2.0;
		if (v_mat != M_PAINT) albedo = wear_asphalt(albedo, wear_at(v_world.xz).x, v_world.xz);
		rough = v_mat == M_PAINT ? 0.62 : 0.92;
	} else if (v_mat == M_FOOTPATH || v_mat == M_STONE) {
		vec3 t = source_linear(texture(footway_map, uv / footway_m, near * detail_footway.z).rgb);
		vec2 duv = uv / concrete_detail_m, ddx = dFdx(duv), ddy = dFdy(duv);
		if (near > 0.0) {
			t *= mix(vec3(1.0), detail_albedo(concrete_detail, duv, ddx, ddy), near * detail_footway.x);
			fine = detail_bend(concrete_detail_normal, duv, ddx, ddy) * near * detail_footway.y;
		}
		bend_scale = normal_footway;
		albedo = t * base / max(source_linear(mean_footway), vec3(MEAN_FLOOR));
		float grain = source_linear(texture(asphalt_map, uv / asphalt_m * 2.7).rgb).r;
		if (COLOR.r + COLOR.g > 0.0) { // a paved median has neither kerb nor lot to wear by
			albedo *= footway_wear(COLOR.r * WEAR_SPAN_M, (COLOR.r + COLOR.g) * WEAR_SPAN_M, grain);
		}
		bend = (texture(footway_normal, uv / footway_m, near * detail_footway.z).xy - 0.5) * 2.0;
		albedo = wear_paving(albedo, wear_at(v_world.xz).y, v_world.xz);
	} else if (v_mat == M_PAVERS) {
		vec2 q = vec2(uv.y, uv.x);
		q.x += step(1.0, mod(floor(q.y / paver_h), 2.0)) * paver_w * 0.5;
		albedo = base * (1.0 - 0.3 * joints(q, vec2(paver_w, paver_h), vec2(0.016), pix)) * blotch;
		albedo = wear_paving(albedo, wear_at(v_world.xz).y, v_world.xz);
	} else if (v_mat == M_KERB_FACE || v_mat == M_SHOULDER_EDGE) {
		// on top laid over the ground as the map tiles, along a road it runs through the stone and the grime; on a face, along it
		// and up it, or the ground's two axes would hold still up the whole height and draw stripes
		vec2 kq = up ? v_world.xz : uv;
		vec3 t = source_linear(texture(kerb_map, kq / kerb_m, near * detail_kerb.z).rgb);
		vec2 duv = kq / concrete_detail_m, ddx = dFdx(duv), ddy = dFdy(duv);
		if (near > 0.0) {
			t *= mix(vec3(1.0), detail_albedo(concrete_detail, duv, ddx, ddy), near * detail_kerb.x);
			fine = detail_bend(concrete_detail_normal, duv, ddx, ddy) * near * detail_kerb.y;
		}
		albedo = t * base / max(source_linear(mean_kerb), vec3(MEAN_FLOOR));
		bend_scale = normal_kerb;
		rough = 0.82;
	} else if (v_mat == M_LAWN) {
		vec2 gq = v_world.xz / verge_m;
		vec3 t = source_linear(texture(verge_map, gq, near * detail_verge.z).rgb);
		vec2 duv = v_world.xz / grass_detail_m, ddx = dFdx(duv), ddy = dFdy(duv);
		if (near > 0.0) {
			t *= mix(vec3(1.0), detail_albedo(grass_detail, duv, ddx, ddy), near * detail_verge.x);
			fine = detail_bend(grass_detail_normal, duv, ddx, ddy) * near * detail_verge.y;
		}
		albedo = t * base; // the map is the grass: the strip's colour only tints it
		bend = (texture(verge_normal, gq, near * detail_verge.z).xy - 0.5) * 2.0;
		bend_scale = normal_verge;
		rough = 0.95;
	} else if (v_mat == M_COBBLE) {
		vec2 c = uv * cobble_per_m;
		float edge = joints(c, vec2(1.0), vec2(0.18), pix * cobble_per_m);
		albedo = base * (1.0 - 0.4 * edge) * (0.9 + 0.2 * hash2(floor(c)));
	} else if (v_mat == M_CONCRETE || v_mat == M_BALLAST) {
		albedo = base * blotch * (0.94 + 0.12 * vnoise(v_world.xz * 3.0));
	}
	ALBEDO = linear_out(albedo);
	ROUGHNESS = rough;
	SPECULAR = 0.5; // the F0 of 0.04 of the roads of the Roadcraft (three: dielectric default; Godot: 0.08 * specular)
	if (up) {
		float fade = 1.0 - smoothstep(0.02, 0.5, pix);
		bend = bend * bend_scale * fade + fine;
		vec3 n = normalize(nw + vec3(-bend.x, 0.0, -bend.y));
		NORMAL = normalize((VIEW_MATRIX * vec4(n, 0.0)).xyz);
	}
	float pools = night > 0.0 ? lamp_pools() : 0.0; // daylight: skip the two exp() per pixel
	EMISSION = linear_out((albedo + vec3(pool_floor)) * source_linear(pool_color) * pools * night * pool_gain);
}
