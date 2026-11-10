// The ground seams beside the roads (native/src/render/roads/ground_seam.h): the terrain's cells the paving's edge crosses, cut along it
// and drawn here in the terrain's own surface. A vertex that is a corner of the terrain's grid (UV2.x 1) takes its height and normal
// from the terrain's maps, as the terrain's nearest level draws that corner, so a seam meets the cells round it exactly; a point of
// the paving's edge keeps its own. The terrain under a seam is left whole and stands lower than it (sim/terrain/road_shape.cpp), so
// the seam covers it at every level of detail. Its uniforms are the terrain material's, fed by render/city_world.gd.
shader_type spatial;
render_mode blend_mix,depth_draw_opaque,cull_back,skip_vertex_transform;

#include "res://render/shaders/terrain_common.gdshaderinc"

void vertex() {
	v_camera_pos = MAIN_CAM_INV_VIEW_MATRIX[3].xyz;
	v_vertex = (MODEL_MATRIX * vec4(VERTEX, 1.0)).xyz;
	v_vertex_xz_dist = length(v_vertex.xz - _target_pos.xz);
	UV = v_vertex.xz * _vertex_density;
	v_ground_n = (MODEL_MATRIX * vec4(NORMAL, 0.0)).xyz;
	if (UV2.x > 0.5) { // a corner of the grid: the terrain's own height and normal there
		vec2 at = round(UV);
		if (!past_map(v_vertex.xz)) {
			v_vertex.y = ground_height(at);
			v_ground_n = ground_normal_at(at, 1.0);
		}
	}
	VERTEX = (VIEW_MATRIX * vec4(v_vertex, 1.0)).xyz;
	NORMAL = normalize(mat3(VIEW_MATRIX) * v_ground_n);
}

void fragment() {
#include "res://render/shaders/terrain_ground.gdshaderinc"
}
