// How a cooked person is skinned, shared by every shader that poses one: 4 bones per vertex (BONE_INDICES / BONE_WEIGHTS, plain
// data, no skeleton is bound), each bone's pose as the three rows of its 3x4 bind -> pose matrix, linear blend of the four.
// What differs between shaders is only where the rows come from: the walk reads them from the palette (people.gdshader, two frames
// mixed), a falling body from its bone-matrix texture (one pose: a = b, t = 0).

const int LAST_BONE = 22; // 23 bones (people.json "bones")
const float FIXED_BYTE = 255.0;

// One value of a texture of 16-bit fixed point over [-span, span]: its high bytes in texel 2x, its low bytes in texel 2x + 1 of the row
// (the Compatibility renderer does not sample float textures reliably; people_asset.cpp and ragdoll_world.cpp write them).
vec4 fixed_point_row(sampler2D tex, int texel, int row, float span) {
	vec4 hi = texelFetch(tex, ivec2(texel * 2, row), 0);
	vec4 lo = texelFetch(tex, ivec2(texel * 2 + 1, row), 0);
	return (hi * 256.0 + lo) * FIXED_BYTE / 65535.0 * 2.0 * span - span;
}

struct BonePose {
	vec4 row_x;
	vec4 row_y;
	vec4 row_z;
};

// Unused slots may name any byte: keep every fetch inside the bone tables (out-of-range reads can be NaN).
ivec4 skin_bones(uvec4 indices) {
	return min(ivec4(indices), ivec4(LAST_BONE));
}

vec3 pose_apply(BonePose pose, vec4 p) {
	return vec3(dot(pose.row_x, p), dot(pose.row_y, p), dot(pose.row_z, p));
}

// One bone's share of the vertex and its normal, the pose being `a` mixed to `b` by t.
void skin_add(inout vec3 pos, inout vec3 nor, float weight, BonePose a, BonePose b, float t, vec3 vertex, vec3 normal) {
	vec4 p = vec4(vertex, 1.0);
	vec4 n = vec4(normal, 0.0);
	pos += weight * mix(pose_apply(a, p), pose_apply(b, p), t);
	nor += weight * mix(pose_apply(a, n), pose_apply(b, n), t);
}
