// A bay of the mid level placed from its instance (native/src/render/buildings/bay_kit.cpp, shell.cpp Shell::bay). The mesh's x and y
// name the lines of the bay and z its plane; here they become the opening's own shares, its recess and its sill. From then on the
// bay is a face of building.gdshader like any other: the wall and its reveals by their finish, the pane by far_window.
const float BAY_MESH = 1.5; // UV2.y over this: the bay mesh (bay_kit.h BAY_MESH_MARK)
const int PIECE_REVEAL = 1, PIECE_PANE = 2, PIECE_SILL = 3;
const int BYTE_BITS = 8, BYTE_MASK = 255, FIN_SHIFT = 8, FIN_MASK = 15, FLAGS_SHIFT = 12, DEPTH_SHIFT = 20;
const float BYTE_MAX = 255.0, KIND_FAR_WALL = 3.0;
const float DEPTH_STEP_M = 0.04; // shell.cpp BAY_DEPTH_STEP_M
const float RECESS_M = 0.2; // how deep the pane of a far wall stands in it
const float SILL_PAST_M = 0.07, SILL_T_M = 0.06, SILL_OUT_M = 0.07; // facade_parts.cpp SILL_PAST (each side), SILL_T, SILL_OUT
const float SILL_LEAST = 0.05; // of the storey: an opening down to the floor has no sill
const float REVEAL_TONE = 0.82, SILL_TONE = 1.18;

vec3 packed_rgb(float packed) {
	int c = int(packed + 0.5);
	return vec3(float((c >> (2 * BYTE_BITS)) & BYTE_MASK), float((c >> BYTE_BITS) & BYTE_MASK), float(c & BYTE_MASK)) / BYTE_MAX;
}

// A line of the bay by its name in the mesh: 0, the sill's end, the jamb, the jamb, the sill's end, 1.
float bay_line(float i, float a0, float a1, float past) {
	return i < 0.25 ? 0.0 : i < 0.75 ? a0 - past : i < 1.5 ? a0 : i < 2.25 ? a1 : i < 2.75 ? a1 + past : 1.0;
}

void bay_place(inout vec3 p, inout vec2 uv, inout vec2 uv2, inout vec4 col, out vec4 room, out vec4 trim, vec4 custom, float bay_w, float bay_h) {
	int shares = int(custom.x + 0.5), code = int(custom.w + 0.5);
	float width = float((shares >> (2 * BYTE_BITS)) & BYTE_MASK) / BYTE_MAX;
	float sill = float((shares >> BYTE_BITS) & BYTE_MASK) / BYTE_MAX, head = float(shares & BYTE_MASK) / BYTE_MAX;
	float a0 = 0.5 - 0.5 * width, a1 = 0.5 + 0.5 * width;
	float depth = float(code >> DEPTH_SHIFT) * DEPTH_STEP_M;
	int kind = int(uv.x + 0.5);
	bool sill_shown = sill > SILL_LEAST;
	float x = bay_line(p.x, a0, a1, sill_shown ? SILL_PAST_M / bay_w : 0.0);
	float y = p.y < 0.25 ? 0.0 : p.y < 0.75 ? sill - (sill_shown ? SILL_T_M / bay_h : 0.0) : p.y < 1.5 ? sill : p.y < 2.5 ? head : 1.0;
	float z = p.z < -0.5 ? -depth : p.z > 0.5 ? (sill_shown ? SILL_OUT_M : 0.0) : 0.0;
	p = vec3(x, y, z);
	uv = vec2(x, y);
	vec3 wall = packed_rgb(custom.y);
	float tone = kind == PIECE_REVEAL ? REVEAL_TONE : kind == PIECE_SILL ? SILL_TONE : 1.0;
	col = vec4(wall * tone, kind == PIECE_PANE ? KIND_FAR_WALL / BYTE_MAX : 0.0);
	float seed = float(code & BYTE_MASK) / 256.0;
	uv2 = vec2(float((code >> FIN_SHIFT) & FIN_MASK) + seed, 0.0);
	room = vec4(width, sill, head, float((code >> FLAGS_SHIFT) & BYTE_MASK) / BYTE_MAX);
	trim = vec4(packed_rgb(custom.z), 0.0);
}
