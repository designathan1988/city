// What a shadow-only draw (MultiMeshDraw::shadow_only) casts with when nothing of its vertex() shows in a silhouette, as a vehicle's
// wheels and seats. With no vertex() it takes the engine's shared depth material (renderer_rd forward_clustered
// uses_shared_shadow_material), the cheapest of the shadow pass. Never seen. A draw whose vertex() places it (poles, wires) keeps its own.
shader_type spatial;
render_mode unshaded;

void fragment() {
	ALBEDO = vec3(0.0);
}
