// What data/render/palette.json terrain sets on the ground's shader (render/city_world.gd sets every key once the terrain is in
// the scene).
// Public uniforms
group_uniforms ground;
uniform float grid_m; // the Layers popover's grid over the ground, its cell
uniform float grid_alpha;
uniform sampler2D grass_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float grass_m; // metres per repeat of the grass map
uniform vec3 rock : source_color;
uniform float rock_slope; // 1 - normal.y; data/render/palette.json terrain (angle of repose)
uniform float patch_scale;
uniform sampler2DArray land_map : source_color, filter_linear_mipmap_anisotropic, repeat_enable;
uniform float dirt_layer; // the land map's layers of soil and of rock, and the metres a repeat of each covers
uniform float rock_layer; // granite's face, the land's rock where no other was painted (rock_strata.gdshaderinc)
uniform float rock_flat_share; // the rock's flat tone over its texture
uniform float dirt_m;
uniform float rock_m;
uniform float dirt_from; // 1 - normal.y where soil starts to show; data/render/palette.json terrain (angle of repose)
uniform float dirt_to; // ... and where it has replaced the grass
uniform float map_half; // the map's half side: the core's, at its first read (render/city_world.gd)
uniform float past_map_y_m; // past the map the land lies under the sea
uniform sampler2D grass_detail : filter_linear_mipmap_anisotropic, repeat_enable; // data, not colour: mean 0.5, doubled in the shader
uniform float detail_m; // metres one tile of the grain covers
uniform float detail_near_px; // metres a pixel under which the grain is whole, and over which it is gone
uniform float detail_far_px;
uniform float detail_wide_from_px; // metres a pixel where the blades' colour starts to thin, gone at the next
uniform float detail_wide_to_px;
uniform float detail_wide_share;
uniform float detail_albedo;
uniform float slope_wander; // how far the patches move the slope's bands
uniform float rock_edge; // the rock's band, over its slope
uniform float wall_dim; // what the wall at the end of the map loses
global uniform float night;
group_uniforms;
