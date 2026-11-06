// A person far off: past the meshes a figure is a card turned to the eye, showing the one of the yaw views in its atlas (the strip each
// atlas holds, people_asset.h) nearest the direction it is seen from. Standing, as a figure a few pixels tall is read; it casts nothing.
// INSTANCE_CUSTOM = (body, height, seed, 0).
shader_type spatial;
render_mode cull_disabled;

uniform sampler2DArray atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform vec4 strip; // x, y, a frame's width and height, in uv
uniform float frames;

const float TAU_RAD = 6.28318530718;
const float CARD_ASPECT = 0.5; // a frame is twice as tall as wide
const float CUT = 0.5; // the texels of the picture that are drawn
const float ROUGH = 0.85;

varying flat float v_layer;
varying vec2 v_uv;

void vertex() {
	float height = INSTANCE_CUSTOM.g;
	vec3 foot = MODEL_MATRIX[3].xyz;
	mat3 to_model = inverse(mat3(MODEL_MATRIX));
	vec3 eye = to_model * (CAMERA_POSITION_WORLD - foot);
	vec3 level = normalize(vec3(eye.x, 0.0, eye.z) + vec3(1e-6, 0.0, 0.0));
	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), level));
	vec2 corner = VERTEX.xy;
	VERTEX = right * corner.x * height * CARD_ASPECT + vec3(0.0, (corner.y + 0.5) * height, 0.0);
	NORMAL = level;
	float frame = mod(floor(atan(level.x, level.z) / TAU_RAD * frames + 0.5), frames);
	v_uv = vec2(strip.x + (frame + corner.x + 0.5) * strip.z, strip.y + (corner.y + 0.5) * strip.w);
	v_layer = INSTANCE_CUSTOM.r;
}

void fragment() {
	vec4 texel = texture(atlas, vec3(v_uv, v_layer));
	ALBEDO = texel.rgb;
	ALPHA = texel.a;
	ALPHA_SCISSOR_THRESHOLD = CUT;
	ROUGHNESS = ROUGH;
}
