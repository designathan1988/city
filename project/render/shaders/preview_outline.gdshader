// The outlines of the lots while the Lots tool is up (render/preview_line.gd show_outlines): each ring a band half_px screen pixels to
// each side of its line at whatever distance, the width the other previews give a ribbon at its own point (ToolController.px_at: the
// metres a pixel covers there, from the vertical angle of view and the screen's height). As their material: unshaded, over everything.
shader_type spatial;
render_mode world_vertex_coords, unshaded, cull_disabled, depth_test_disabled, depth_draw_never, shadows_disabled;

uniform vec4 colour : source_color;
uniform float half_px = 2.0;
uniform float screen_h; // the viewport's visible height, pixels (what px_at divides by)

void vertex() {
	// UV: the unit vector to the line's left at this point, signed by the side the vertex stands on.
	float metres_per_px = length(VERTEX - CAMERA_POSITION_WORLD) * 2.0 / (PROJECTION_MATRIX[1][1] * screen_h);
	VERTEX.xz += UV * half_px * metres_per_px;
}

void fragment() {
	ALBEDO = colour.rgb;
	ALPHA = colour.a;
}
