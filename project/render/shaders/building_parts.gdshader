// Instanced facade parts (native/src/render/buildings/shell.h BoxInstance): frames, sills, railings, shutters, brise
// fins, roof rails and fence panels drawn as unit-cube MultiMesh instances. INSTANCE_CUSTOM = (sRGB tone rgb, finish
// layer); the geometry carries no finish vertex attribute, so this is building.gdshader with the finish and tone read
// from the instance instead of COLOR/UV2.
shader_type spatial;
render_mode cull_back;

global uniform float night;

uniform sampler2DArray finish_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2DArray finish_normal : filter_linear_mipmap, repeat_enable;
uniform sampler2DArray finish_rough : filter_linear_mipmap, repeat_enable;
uniform float finish_m[13];
uniform float finish_mean[13];
uniform float finish_normal_k[13];
uniform float finish_metal[13];
uniform float wall_gain = 1.0;

varying vec3 v_world;
varying vec3 v_wn;
varying flat vec4 v_custom;

#include "res://render/shaders/building_colour.gdshaderinc"
#include "res://render/shaders/building_common.gdshaderinc"

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_wn = normalize((MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz);
	v_custom = INSTANCE_CUSTOM;
}

void fragment() {
	vec3 n = normalize(v_wn);
	vec3 albedo = to_linear(v_custom.rgb), nn = n;
	float rough = 0.86, metal = 0.0;
	int fin = int(floor(v_custom.a + 0.5)); // -1: no finish (int() would truncate -0.5 to layer 0)
	if (fin >= 0 && fin < 13) {
		vec3 tone;
		finish_layer(fin, n, tone, nn, rough, metal);
		albedo *= tone;
		if (fin != 8 && fin != 7) albedo *= weather(n);
	}
	ALBEDO = surface_out(albedo * wall_gain);
	ROUGHNESS = rough;
	METALLIC = metal;
	NORMAL = normalize((VIEW_MATRIX * vec4(nn, 0.0)).xyz);
}
