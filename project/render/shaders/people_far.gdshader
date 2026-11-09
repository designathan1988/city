// A person far off: past the meshes a figure is a card turned to the eye, showing the one of the yaw views in its atlas (the strip each
// atlas holds, people_asset.h) nearest the direction it is seen from. Standing, as a figure a few pixels tall is read; it casts nothing.
// INSTANCE_CUSTOM = (body, height, seed, 0).
shader_type spatial;
render_mode cull_disabled;

uniform sampler2DArray atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform vec4 strip; // x, y, a frame's width and height, in uv
uniform float frames;

const float CUT = 0.5; // the texels of the picture that are drawn
const float COARSEST_SHORT = 2.0; // the coarsest mip keeps a frame 2^2 texels tall, so the next frame never bleeds in (personImpostor.ts MAP_MAIN)
const float LEAST_LEVEL = 1e-6; // keeps the card's level direction off zero right over it
uniform float card_rough; // materials.json people_far

varying flat float v_layer;
varying vec2 v_uv;

void vertex() {
	float height = INSTANCE_CUSTOM.g;
	vec3 foot = MODEL_MATRIX[3].xyz;
	mat3 to_model = inverse(mat3(MODEL_MATRIX));
	vec3 eye = to_model * (CAMERA_POSITION_WORLD - foot);
	vec3 level = normalize(vec3(eye.x, 0.0, eye.z) + vec3(LEAST_LEVEL, 0.0, 0.0));
	vec3 right = normalize(cross(vec3(0.0, 1.0, 0.0), level));
	vec2 corner = VERTEX.xy;
	// as wide as a frame's own aspect (the atlas is square, so its uv width over its height)
	VERTEX = right * corner.x * height * strip.z / strip.w + vec3(0.0, (corner.y + 0.5) * height, 0.0);
	NORMAL = level;
	float frame = mod(floor(atan(level.x, level.z) / TAU * frames + 0.5), frames);
	v_uv = vec2(strip.x + (frame + corner.x + 0.5) * strip.z, strip.y + (corner.y + 0.5) * strip.w);
	v_layer = INSTANCE_CUSTOM.r;
}

void fragment() {
	// the frame's texels, never at a mip so coarse its neighbour frame bleeds in (personImpostor.ts MAP_MAIN)
	vec2 texels = vec2(textureSize(atlas, 0).xy);
	vec2 dx = dFdx(v_uv) * texels, dy = dFdy(v_uv) * texels;
	float coarsest = log2(strip.w * texels.y) - COARSEST_SHORT;
	float lod = clamp(0.5 * log2(max(dot(dx, dx), dot(dy, dy))), 0.0, coarsest);
	vec4 texel = textureLod(atlas, vec3(v_uv, v_layer), lod);
	ALBEDO = texel.rgb;
	ALPHA = texel.a;
	ALPHA_SCISSOR_THRESHOLD = CUT;
	ROUGHNESS = card_rough;
}
