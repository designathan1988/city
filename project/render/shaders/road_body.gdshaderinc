// Roads: the mesher (native/src/render/roads/road_look.h) writes the sRGB strip colour in
// COLOR.rgb, the material id in COLOR.a * 16, texture metres in UV ((u, v) on segments, world xz on plates and
// corners, (across, along) on a kerb's top, the run along it on any face) and the lamp pool coordinates in UV2 (|u - lamp| and metres from the first lamp; -1000 = junction plate,
// lit evenly; 1000 = no lamp). The cooked maps are tinted so their mean becomes the strip colour. Everything in linear
// on both renderers (color_space.gdshaderinc).
#include "res://render/shaders/shader_contracts.gdshaderinc"

global uniform float night;
global uniform float road_paint; // 0: the detail cut-off of the graphics tier leaves the paint out

uniform sampler2D asphalt_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D asphalt_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D footway_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D footway_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D kerb_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D kerb_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D verge_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D verge_normal : hint_normal, filter_linear_mipmap_anisotropic, repeat_enable;
// Mean colour of each map, computed once in project/render/surface_means.gd (one read instead of a smallest-mip fetch per
// pixel; the strip colour stays the mean albedo).
uniform vec3 mean_asphalt : source_color = vec3(1.0);
uniform vec3 mean_footway : source_color = vec3(1.0);
uniform vec3 mean_kerb : source_color = vec3(1.0);
uniform vec3 asphalt_reference : source_color = vec3(1.0); // the colour the asphalt map is authored against
uniform float asphalt_m = 10.4;
uniform float footway_m = 7.2;
uniform float kerb_m = 3.2;
uniform float verge_m = 8.8;
uniform vec3 lane_tone[ROAD_TONE_SLOTS]; // linear, by road class (road_look.h wear_color)
uniform vec3 footway_tone;
uniform float plate_blend = 2.26; // metres of that difference over which two arms share a plate
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
uniform float pool_along_m = 11.5; // a lamp's pool along the road: where it ends; across, the street furniture owner sets it
uniform float pool_step = 1.0; // the lamp step, set by the road renderer from the street furniture catalog
uniform float pool_gain = 0.32;
uniform float plate_glow = 0.55;
global uniform vec2 lamp_lit; // where the lamps with a light of their own give way to these pools (render/props/lamp_lights.h)

// The far level (render/roads/far_road_renderer.h): a superchunk's mesh, whose vertices carry their chunk in CUSTOM0; the chunks drawn in detail fold
// away, and the rest stands `far_pull` metres towards the eye, along its ray, so the coarser ground out there never comes through.
#ifdef ROAD_FAR
instance uniform int chunk_mask = 0;
instance uniform float far_pull = 0.0;
#endif

const float FAR_LINE_M = 0.12; // a lane line's width: from afar it is the share of the pixel it covers
const float FAR_LINE_DUTY = 0.6; // of a line's length, painted: dashes and solid lines together
const vec3 FAR_LINE_TONE = vec3(0.78);

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
const int M_ARM_MEET = 13; // a plate between two arms: tones in the alpha and in red, how far from the one to the other in UV2.y

varying flat int v_mat;
varying float v_id;
varying vec3 v_world;
varying vec2 v_pool;
varying float v_eye_m;

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
#ifdef ROAD_FAR
	if (far_pull > 0.0) {
		vec3 to_eye = CAMERA_POSITION_WORLD - v_world;
		VERTEX = ((chunk_mask >> int(CUSTOM0.r * 255.0 + 0.5)) & 1) != 0 ? vec3(0.0) : VERTEX + to_eye * (far_pull / max(length(to_eye), far_pull));
	}
#endif
	v_pool = UV2;
	v_eye_m = length(v_world.xz - CAMERA_POSITION_WORLD.xz);
}

#include "res://render/shaders/lamp_pool.gdshaderinc"

float lamp_pools() {
	if (v_pool.x < -500.0) {
		return plate_glow;
	}
	if (v_pool.x > 500.0) {
		return 0.0;
	}
	float dv = v_pool.y - pool_step * round(v_pool.y / pool_step);
	float du = abs(v_pool.x);
	float far = pool_step - abs(dv);
	return pool_fall(length(vec2(du, dv / pool_along_m))) + pool_fall(length(vec2(du, far / pool_along_m)));
}

void fragment() {
	vec3 base = srgb_to_linear(COLOR.rgb); // the strip colour, sRGB in the mesh; everything below in linear
	if (v_mat == M_LANE) {
		base = lane_tone[int(fract(v_id) * TONE_SLOTS)];
	} else if (v_mat == M_ARM_MEET) {
		base = mix(lane_tone[int(fract(v_id) * TONE_SLOTS)], lane_tone[int(COLOR.r * TONE_SLOTS)], clamp(0.5 + v_pool.y / (2.0 * plate_blend), 0.0, 1.0));
	} else if (v_mat == M_FOOTPATH) {
		base = footway_tone;
	}
	float pix = max(max(fwidth(v_world.x), fwidth(v_world.z)), 0.001);
	vec2 uv = UV;
	vec2 frame = uv; // what the normal map's axes run along
	vec3 albedo = base;
	vec2 bend = vec2(0.0);
	vec2 fine = vec2(0.0);
	float bend_scale = normal_asphalt;
	float near = detail_weight(pix);
	float rough = 1.0;
	float blotch = 1.0 + (vnoise(v_world.xz * 0.08) - 0.5) * 0.12;
	vec3 nw = (INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz;
	bool up = nw.y > 0.7;
	if (v_mat == M_PAINT && road_paint < 0.5) discard;
	if (v_mat == M_ROAD || v_mat == M_PLATE || v_mat == M_ARM_MEET || v_mat == M_PAINT || v_mat == M_LANE) {
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
#ifdef ROAD_FAR
			if (far_pull > 0.0) { // the lines between the lanes, filtered: a box of the pixel's width over the line's
				float edge = min(COLOR.r, COLOR.g) * WEAR_SPAN_M, lane = max(COLOR.b * WEAR_LANE_M, MEAN_FLOOR);
				float off = abs(edge - max(round(edge / lane), 1.0) * lane), wide = max(fwidth(edge), FAR_LINE_M);
				float seen = clamp(min(FAR_LINE_M * 0.5, off + wide * 0.5) - max(-FAR_LINE_M * 0.5, off - wide * 0.5), 0.0, FAR_LINE_M) / wide;
				albedo = mix(albedo, FAR_LINE_TONE, seen * FAR_LINE_DUTY);
			}
#endif
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
		// on top across and along the road, so the stone and its grime pass by along it; on a face along it and at its height in
		// the world: the Roadcraft's skirt, whose rows are whichever the road's altitude falls on
		vec2 kq = up ? uv : vec2(uv.x, v_world.y);
		frame = kq;
		vec3 t = source_linear(texture(kerb_map, kq / kerb_m, near * detail_kerb.z).rgb);
		vec2 duv = kq / concrete_detail_m, ddx = dFdx(duv), ddy = dFdy(duv);
		if (near > 0.0) {
			t *= mix(vec3(1.0), detail_albedo(concrete_detail, duv, ddx, ddy), near * detail_kerb.x);
			fine = detail_bend(concrete_detail_normal, duv, ddx, ddy) * near * detail_kerb.y;
		}
		albedo = t * base / max(source_linear(mean_kerb), vec3(MEAN_FLOOR));
		bend = (texture(kerb_normal, kq / kerb_m, near * detail_kerb.z).xy - 0.5) * 2.0;
		bend_scale = normal_kerb;
		rough = 0.82;
	} else if (v_mat == M_LAWN) {
		vec2 gq = v_world.xz / verge_m;
		frame = v_world.xz;
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
	float fade = 1.0 - smoothstep(0.02, 0.5, pix);
	NORMAL = bent_normal(NORMAL, VERTEX, frame, bend * bend_scale * fade + fine);
	float far = pool_handed_over(v_eye_m, lamp_lit);
	float pools = night > 0.0 && far > 0.0 ? lamp_pools() * far : 0.0; // daylight and near: skip the two falls per pixel
	EMISSION = linear_out(albedo * source_linear(pool_color) * pools * night * pool_gain); // the surface's own colour: dark asphalt stays dark, the paint shows
}
