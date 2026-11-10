// A person far off: a figure a few pixels tall is a card turned to the eye, showing the one of the yaw views in its atlas (the strip each
// atlas holds, people_asset.h) nearest the direction it is seen from. Standing, as a figure a few pixels tall is read; it casts nothing.
// INSTANCE_CUSTOM = (body, height, seed, 0). The strip's texels round a figure are black with no coverage, so a coarser mip is the figure's
// colour times its coverage: the level stays two short of a frame's own size (finer than that, the next frame and the black round it
// bleed in) and the colour is taken back from its coverage.
shader_type spatial;
render_mode cull_disabled;

uniform sampler2DArray atlas : source_color, filter_linear_mipmap_anisotropic, repeat_disable;
uniform vec4 strip; // x, y, a frame's width and height, in uv
uniform float frames;

const float TAU_RAD = 6.28318530718;
const float CARD_ASPECT = 0.5; // a frame is twice as tall as wide
const float CUT = 0.5; // the texels of the picture that are drawn
const float FAR_ROUGH = 0.85;
const float FRAME_LEVELS_KEPT = 2.0; // a frame never coarser than a quarter of its own texels across
const float COVER_LEAST = 1e-3; // under this a texel has no colour to take back

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
	vec2 texels = vec2(textureSize(atlas, 0).xy);
	vec2 across = dFdx(v_uv) * texels, down = dFdy(v_uv) * texels;
	float frame_texels = strip.w * texels.y;
	float level = clamp(0.5 * log2(max(dot(across, across), dot(down, down))), 0.0, log2(frame_texels) - FRAME_LEVELS_KEPT);
	vec4 texel = textureLod(atlas, vec3(v_uv, v_layer), level);
	ALBEDO = texel.rgb / max(texel.a, COVER_LEAST);
	ALPHA = texel.a;
	ALPHA_SCISSOR_THRESHOLD = CUT;
	ROUGHNESS = FAR_ROUGH;
}
