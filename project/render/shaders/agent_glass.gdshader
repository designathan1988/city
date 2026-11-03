// A car's glass (native/src/render/agents: the glass surface of its mesh, over its cabin and the people in it): tinted,
// glossy and see-through, more so looked at square than at a grazing angle (Fresnel), as the Roadcraft 4.0 car glass.
shader_type spatial;
render_mode blend_mix, depth_draw_opaque, cull_back;

uniform vec3 tint : source_color;
uniform float opacity = 0.35;
uniform float edge_opacity = 0.9;
uniform float edge_power = 3.0;
uniform float roughness = 0.05;
uniform float specular = 0.8;

void fragment() {
	float grazing = pow(1.0 - clamp(dot(NORMAL, VIEW), 0.0, 1.0), edge_power);
	ALBEDO = tint;
	ALPHA = mix(opacity, edge_opacity, grazing);
	ROUGHNESS = roughness;
	SPECULAR = specular;
}
