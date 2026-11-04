// Buildings: one material for every building surface. COLOR.a is the
// surface kind written by native/src/render/buildings/shell.h: 0 shell face (UV2.x = cooked finish layer + 1, 0 = no
// finish), 1/2 glass pane (dark), 3 far wall with painted windows (UV in bays/storeys, CUSTOM0 = window shares and flags),
// 4 lamp, 5 lawn. Finishes: world-planar UVs in metres, map normalised by its layer mean, normal map, roughness and metal
// mask. Panes and far windows: building_pane.gdshaderinc (CUSTOM0..3 written by shell.cpp).
shader_type spatial;
render_mode cull_back;

global uniform float night;
global uniform float rooms_awake; // the share of people awake at this hour (daylight.gd)
global uniform float room_daylight; // how bright a room seen through its glass is by day, as light it sends out
global uniform float outside_on; // the share of outside lights lit at this hour
uniform sampler2DArray finish_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2DArray finish_normal : filter_linear_mipmap, repeat_enable;
uniform sampler2DArray finish_rough : filter_linear_mipmap, repeat_enable;
uniform float finish_m[13];
uniform float finish_mean[13];
uniform float finish_normal_k[13];
uniform float finish_metal[13];
uniform float wall_gain = 1.0;
uniform float lamp_glow = 4.0;
uniform float window_glow = 1.5;
uniform float glass_reflect = 0.08;
uniform vec3 glass_sky : source_color = vec3(0.45, 0.55, 0.66);
uniform float glass_sky_floor = 0.15;
uniform float furniture_tone[120]; // linear tone of each furniture kind, three floats a kind (materials.json)

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_room0;
varying flat vec4 v_room1;
varying flat vec4 v_room2;
varying flat vec4 v_trim;

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
	v_room0 = CUSTOM0;
	v_room1 = CUSTOM1;
	v_room2 = CUSTOM2;
	v_trim = CUSTOM3;
}

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/building_common.gdshaderinc"
#include "res://render/shaders/building_lamps.gdshaderinc"
const vec3 PANE_MEAN_SEEN = vec3(0.42, 0.45, 0.47); // a room under a pixel: its mean by day
#include "res://render/shaders/building_pane.gdshaderinc"

void fragment() {
	int kind = int(COLOR.a * 255.0 + 0.5);
	vec3 n = normalize(v_wn);
	vec3 V = normalize(v_world - CAMERA_POSITION_WORLD);
	vec3 albedo = to_linear(COLOR.rgb), emit = vec3(0.0), nn = n;
	float rough = 0.86, metal = 0.0;
	if (kind == 1 || kind == 2) {
		uint code = uint(COLOR.r * 255.0 + 0.5) | (uint(COLOR.g * 255.0 + 0.5) << 8u);
		pane(UV, max(UV2, vec2(0.3)), code, uint(COLOR.b * 255.0 + 0.5), kind == 2, n, V, albedo, emit, nn, rough, metal);
	} else if (kind == 4) {
		emit = albedo * night * lamp_glow * step(h01(uint(floor(v_world.x * 0.1) * 31.0 + floor(v_world.z * 0.1) * 977.0 + 65536.0)), outside_on);
	} else if (kind == 5) {
		albedo *= 0.8 + 0.25 * vnoise(v_world.xz * 0.7) + 0.15 * vnoise(v_world.xz * 3.1);
		rough = 0.95;
	} else {
		int fin = int(floor(UV2.x + 0.001)) - 1;
		if (fin >= 0 && fin < 13) {
			vec3 tone;
			finish_layer(fin, n, tone, nn, rough, metal);
			albedo *= tone;
			if (fin != 8 && fin != 7) albedo *= weather(n);
		}
		albedo *= wall_gain;
		if (kind == 3) {
			float glass;
			far_window(UV, v_room0, uint(fract(UV2.x) * 256.0 + 0.5), n, V, albedo, emit, glass);
			rough = mix(rough, 0.15, glass);
			nn = normalize(mix(nn, n, glass));
		}
	}
	ALBEDO = surface_out(albedo);
	EMISSION = surface_out(emit);
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
}
