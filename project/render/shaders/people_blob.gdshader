// The soft disc under a person too far for a real shadow (native/src/render/agents/people_renderer.cpp): dark in the middle,
// nothing at the rim, laid on the ground and never written to the depth.
shader_type spatial;
render_mode unshaded, blend_mix, depth_draw_never, cull_disabled, shadows_disabled;

uniform float opacity;

varying float v_out; // 0 in the middle, 1 at the rim

void vertex() {
	v_out = length(VERTEX.xz);
}

void fragment() {
	ALBEDO = vec3(0.0);
	ALPHA = opacity * (1.0 - v_out);
}
