// Toon terrain for Terrain3D (shader override). Vertex stage adapted from Terrain3D's minimum.gdshader
// (MIT, (c) Cory Petkovsek, Roope Palmroos and contributors); fragment paints flat stylised colours (terrain_ground.gdshaderinc,
// shared with the ground seams beside the roads, terrain_seam.gdshader, which lie over this terrain where a road's edge crosses it).
shader_type spatial;
render_mode blend_mix,depth_draw_opaque,cull_back,skip_vertex_transform;

#include "res://render/shaders/terrain_common.gdshaderinc"

////////////////////////
// Vertex
////////////////////////

// The land's height between the grid's points (the nearest level's sub-texel vertices).
float interpolated_height(vec2 pos) {
	vec2 at = floor(pos), f = fract(pos);
	float low = mix(ground_height(at), ground_height(at + vec2(1.0, 0.0)), f.x);
	float high = mix(ground_height(at + vec2(0.0, 1.0)), ground_height(at + vec2(1.0, 1.0)), f.x);
	return mix(low, high, f.y);
}

void vertex() {
	// Save Camera Position to varying for access in later functions
	v_camera_pos = MAIN_CAM_INV_VIEW_MATRIX[3].xyz;

	// Get vertex of flat plane in world coordinates and set world UV
	v_vertex = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;

	// Distance from target node to vertex on a flat plane
	v_vertex_xz_dist = length(v_vertex.xz - _target_pos.xz);

	// Geomorph vertex across clipmap LODs, set end and start for linear height interpolate
	float scale = MODEL_MATRIX[0][0];
	float inv_scale = 1.0 / scale;
	float max_xz = max(abs(v_vertex.x - _target_pos.x), abs(v_vertex.z - _target_pos.z));
	float vertex_lerp = smoothstep(0.0, 1.0, (max_xz * inv_scale - _mesh_size - T3D_MORPH_FROM_CELLS) / (_mesh_size - T3D_MORPH_FROM_CELLS));
	vec2 vertex_fract = fract(VERTEX.xz * 0.5) * 2.0;
	// For LOD0 morph from a regular grid to an alternating grid to align with LOD1+
	bool lod0 = scale < _vertex_spacing / _subdiv + T3D_LOD0_SLACK;
	vec2 shift = lod0 ?
		// Shift from regular to symmetric
		mix(vertex_fract, vec2(vertex_fract.x, -vertex_fract.y),
			round(fract(round(mod(v_vertex.z * inv_scale, 4.0)) *
			round(mod(v_vertex.x * inv_scale, 4.0)) * 0.25))) :
		// Symmetric shift
		vertex_fract * round((fract(v_vertex.xz * 0.25 * inv_scale) - 0.5) * 4.0);
	vec2 start_pos = v_vertex.xz * _vertex_density;
	vec2 end_pos = (v_vertex.xz - shift * scale) * _vertex_density;
	v_vertex.xz -= shift * scale * vertex_lerp;

	// UV coordinates in region space. 0-1 covers 1 region, 1-2 is the next region, etc.
	UV = v_vertex.xz * _vertex_density;

	// UV coordinates in region space + texel offset. Values are 0 to 1 within regions
	UV2 = fma(UV, vec2(_region_texel_size), vec2(0.5 * _region_texel_size));

	// Terrain3D's own hole bit, as its minimum.gdshader reads it. The game holes nothing under the roads: a hole follows the
	// mesher's levels and transition strips (terrain_3d_mesher.cpp snap), which a seam cannot match, and opened gaps onto the water
	// below; the ground seams lie over this terrain instead, which stands lower than them there (road_shape.cpp). 1 lookup
	uint control = ground_control(start_pos);
	bool hole = bool(control >>2u & 0x1u);
	bool past = past_map(v_vertex.xz);

	// Show holes to all cameras except mouse camera (on exactly 1 layer)
	if ( !(CAMERA_VISIBLE_LAYERS == _mouse_layer) && hole) {
		v_vertex.x = 0. / 0.;
	} else {
		// Set final vertex height.
		float h;
		// This branch is static for each of the clipmap segments
		// Interpolated reads only occur where sub-texel values are required.
		if (scale < _vertex_spacing) {
			h = interpolated_height(UV);
		} else {
			h = mix(ground_height(start_pos), ground_height(end_pos), vertex_lerp);
		}
		v_vertex.y = past ? past_map_y_m : h;
		// past the map a vertex stands at its end: the clipmap follows the camera, and the wall it makes must not
		if (past) v_vertex.xz = clamp(v_vertex.xz, vec2(-map_half), vec2(map_half));
		v_ground_n = ground_normal_at(round(UV), max(1.0, scale * _vertex_density));
	}

	// Convert model space to view space w/ skip_vertex_transform render mode
	VERTEX = (VIEW_MATRIX * vec4(v_vertex, 1.0)).xyz;
	NORMAL = normalize((MODELVIEW_MATRIX * vec4(NORMAL, 0.0)).xyz);
	BINORMAL = normalize((MODELVIEW_MATRIX * vec4(BINORMAL, 0.0)).xyz);
	TANGENT = normalize((MODELVIEW_MATRIX * vec4(TANGENT, 0.0)).xyz);
}

////////////////////////
// Fragment (terrain_ground.gdshaderinc)
////////////////////////

void fragment() {
#include "res://render/shaders/terrain_ground.gdshaderinc"
}
