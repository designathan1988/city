// What data/render/palette.json terrain sets on the ground's shader.
// Public uniforms
group_uniforms ground;
uniform float grid_m = 10.0; // the zoning grid (world/lots.ts: the 10 m cell)
uniform float grid_alpha = 0.35;
uniform sampler2D grass_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_m = 14.0; // metres per repeat of the grass map
uniform vec3 rock : source_color;
uniform float rock_slope; // 1 - normal.y; data/render/palette.json terrain (angle of repose)
uniform float patch_scale;
uniform sampler2DArray land_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float dirt_layer = 8.0;
uniform float rock_layer = 2.0;
uniform float dirt_m = 13.6;
uniform float rock_m = 23.2;
uniform float dirt_from; // 1 - normal.y where soil starts to show; data/render/palette.json terrain (angle of repose)
uniform float dirt_to; // ... and where it has replaced the grass
uniform float map_half = 5120.0; // the open map's half side, set by CityWorld.set_map_half when a map is read
uniform float past_map_y_m; // past the map the land lies under the sea
uniform sampler2D grass_detail : filter_linear_mipmap_anisotropic, repeat_enable; // data, not colour: mean 0.5, doubled in the shader
uniform float detail_m = 1.6; // metres one tile of the grain covers
uniform float detail_near_px; // metres a pixel under which the grain is whole, and over which it is gone
uniform float detail_far_px;
uniform float detail_wide_from_px; // metres a pixel where the blades' colour starts to thin, gone at the next
uniform float detail_wide_to_px;
uniform float detail_wide_share;
uniform float detail_albedo = 0.9;
global uniform float night;
group_uniforms;
