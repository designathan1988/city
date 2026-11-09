// A lot's lawn (render/buildings/building_renderer.cpp, the "verge" material): the terrain's own grass on a lot's ground. Its
// uniforms are the terrain's (render/world_materials.gd). It takes the map's biome and not what was painted on the ground: paint
// stays on the terrain; its straw is the ecosystem's dry herbs at the point (ecology.gdshaderinc), as the land round it.
// (The roads' verge it was first made for is gone: render/roads/ground_seam.h meets the terrain instead.)
shader_type spatial;
render_mode blend_mix, depth_draw_opaque, cull_back;

global uniform float rain_wet;
uniform sampler2D grass_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_m;
uniform float patch_scale;
uniform sampler2DArray land_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform sampler2D grass_detail : filter_linear_mipmap_anisotropic, repeat_enable;
uniform float detail_m;
uniform float detail_near_px;
uniform float detail_far_px;
uniform float detail_wide_from_px; // where the blades' colour starts to thin, gone at the next (as terrain.gdshader)
uniform float detail_wide_to_px;
uniform float detail_wide_share;
uniform float detail_albedo;
#include "res://render/shaders/contracts/land.gdshaderinc"
uniform vec3 biome_tint[1 << PAINT_ID_BITS] : source_color; // by the overlay id, as the terrain's
uniform int map_biome = 0;

#include "res://render/shaders/terrain_layers.gdshaderinc"
#include "res://render/shaders/ecology.gdshaderinc"

varying vec3 v_world;

void vertex() {
	v_world = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
}

void fragment() {
	float detail = 1.0 - smoothstep(detail_fade_m.x, detail_fade_m.y, length(v_world.xz - CAMERA_POSITION_WORLD.xz));
	float per_pixel = max(fwidth(v_world.x), fwidth(v_world.z));
	vec3 g;
	float n;
	ALBEDO = grass_ground(v_world.xz, detail, biome_tint[map_biome], ecology_at(v_world.xz).x, per_pixel, g, n);
	vec2 tilt = grass_tilt(v_world.xz, per_pixel, 1.0);
	vec3 up = (INV_VIEW_MATRIX * vec4(NORMAL, 0.0)).xyz;
	NORMAL = mat3(VIEW_MATRIX) * normalize(up + vec3(tilt.x, 0.0, tilt.y));
	ROUGHNESS = mix(ground_rough.x, ground_rough.y, rain_wet);
}
