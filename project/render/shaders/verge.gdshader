// The ground between a footway's edge and the terrain (render/roads/road_verge.cpp): a ribbon in the terrain's own grass,
// so the land runs up to the paving with no wall and no seam. Its uniforms are the terrain's (render/city_world.gd). It
// takes the map's biome and not what was painted on the ground: the ribbon is a metre wide and paint stays on the terrain.
shader_type spatial;
render_mode blend_mix, depth_draw_opaque, cull_back;

global uniform float rain_wet;
uniform sampler2D grass_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_m = 14.0;
uniform float patch_scale;
uniform sampler2DArray land_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D grass_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform float detail_m = 1.6;
uniform float detail_near_px;
uniform float detail_far_px;
uniform float detail_wide_from_px; // where the blades' colour starts to thin, gone at the next (as terrain.gdshader)
uniform float detail_wide_to_px;
uniform float detail_wide_share;
uniform float detail_albedo = 0.9;
uniform vec3 biome_tint[7] : source_color;
uniform int map_biome = 0;

#include "res://render/shaders/terrain_layers.gdshaderinc"

varying vec3 v_world;

#include "res://render/shaders/canyon_light.gdshaderinc"

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
}

void fragment() {
	float detail = 1.0 - smoothstep(DETAIL_NEAR_M, DETAIL_FAR_M, length(v_world.xz - CAMERA_POSITION_WORLD.xz));
	float per_pixel = max(fwidth(v_world.x), fwidth(v_world.z));
	vec3 g;
	float n;
	ALBEDO = grass_ground(v_world.xz, detail, biome_tint[map_biome], map_biome, per_pixel, g, n);
	vec2 tilt = grass_tilt(v_world.xz, per_pixel, 1.0);
	vec3 up = (INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz;
	NORMAL = mat3(VIEW_MATRIX) * normalize(up + vec3(tilt.x, 0.0, tilt.y));
	ROUGHNESS = mix(0.95, 0.6, rain_wet);
	AO = canyon_sky_seen(v_world, vec3(0.0, 1.0, 0.0)); // the sky the buildings leave the ground
}
