// Calibration case (LAB_COPY): the reference's glaze shader, held there in a string, copied into a Godot shader.
shader_type spatial;

uniform float glaze_depth;
varying vec3 v_normal;

void fragment() {
	float rim = pow(1.0 - abs(dot(normalize(v_normal), vec3(0.0, 0.0, 1.0))), 2.6);
	vec3 crust = mix(vec3(0.62, 0.38, 0.17), vec3(0.93, 0.71, 0.42), rim * glaze_depth);
	ALBEDO = crust + rim * 0.18;
}
