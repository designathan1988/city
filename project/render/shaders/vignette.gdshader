// The picture falls off towards the corners. Multiplied over the 3D image, under the interface (render/city_world.gd).
shader_type canvas_item;
render_mode blend_mul;

uniform float floor_share;
uniform float from_r2;
uniform float to_r2;
uniform float span;

void fragment() {
	vec2 d = UV - 0.5;
	COLOR = vec4(vec3(mix(1.0, floor_share, smoothstep(from_r2, to_r2, dot(d, d) * span))), 1.0);
}
