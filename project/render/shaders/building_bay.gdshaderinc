// A bay of the mid level placed from its instance (native/src/render/buildings/bay_kit.cpp, shell.cpp Shell::bay). The mesh's x and y
// name the lines of the bay and z its plane; here they become the opening's own shares, its recess and its sill. From then on the
// bay is a face of building.gdshader like any other: the wall and its reveals by their finish, the pane by far_window.
#include "res://render/shaders/contracts/buildings.gdshaderinc"
#include "res://render/shaders/bytes.gdshaderinc"
const float BAY_MESH = BAY_MESH_MARK - 0.5; // UV2.y over this: the bay mesh
const int FIN_MASK = (1 << (BAY_FLAGS_SHIFT - BAY_FIN_SHIFT)) - 1; // a finish's bits in the bay's flags
const float RECESS_M = 0.2; // how deep the pane of a far wall stands in it
const float SILL_LEAST = 0.05; // of the storey: an opening down to the floor has no sill
const float REVEAL_TONE = 0.82, SILL_TONE = 1.18; // the mid level's reveal and sill tones (the near level's differ: log/review.md, B3)

vec3 packed_rgb(float packed) {
	int c = int(packed + 0.5);
	return vec3(float((c >> (2 * BYTE_BITS)) & BYTE_MASK), float((c >> BYTE_BITS) & BYTE_MASK), float(c & BYTE_MASK)) / BYTE_TOP;
}

// Halfway from one line of the mesh to the next: a line's name holds up to there.
float halfway(float a, float b) { return 0.5 * (a + b); }

// A line of the bay by its name in the mesh (BayLines): 0, the sill's end, the jamb, the jamb, the sill's end, 1.
float bay_line(float i, float a0, float a1, float past) {
	return i < halfway(BAY_X_LEFT, BAY_X_SILL_LEFT) ? 0.0 : i < halfway(BAY_X_SILL_LEFT, BAY_X_JAMB_LEFT) ? a0 - past
		: i < halfway(BAY_X_JAMB_LEFT, BAY_X_JAMB_RIGHT) ? a0 : i < halfway(BAY_X_JAMB_RIGHT, BAY_X_SILL_RIGHT) ? a1
		: i < halfway(BAY_X_SILL_RIGHT, BAY_X_RIGHT) ? a1 + past : 1.0;
}

void bay_place(inout vec3 p, inout vec2 uv, inout vec2 uv2, inout vec4 col, out vec4 room, out vec4 trim, vec4 custom, float bay_w, float bay_h) {
	int shares = int(custom.x + 0.5), code = int(custom.w + 0.5);
	float width = float((shares >> (2 * BYTE_BITS)) & BYTE_MASK) / BYTE_TOP;
	float sill = float((shares >> BYTE_BITS) & BYTE_MASK) / BYTE_TOP, head = float(shares & BYTE_MASK) / BYTE_TOP;
	float a0 = 0.5 - 0.5 * width, a1 = 0.5 + 0.5 * width;
	float depth = float(code >> BAY_DEPTH_SHIFT) * BAY_DEPTH_STEP_M;
	int kind = int(uv.x + 0.5);
	bool sill_shown = sill > SILL_LEAST;
	float x = bay_line(p.x, a0, a1, sill_shown ? SILL_PAST_M * 0.5 / bay_w : 0.0);
	float y = p.y < halfway(BAY_Y_FOOT, BAY_Y_UNDER_SILL) ? 0.0 : p.y < halfway(BAY_Y_UNDER_SILL, BAY_Y_SILL) ? sill - (sill_shown ? SILL_T_M / bay_h : 0.0)
		: p.y < halfway(BAY_Y_SILL, BAY_Y_HEAD) ? sill : p.y < halfway(BAY_Y_HEAD, BAY_Y_TOP) ? head : 1.0;
	float z = p.z < halfway(BAY_Z_PANE, BAY_Z_WALL) ? -depth : p.z > halfway(BAY_Z_WALL, BAY_Z_SILL) ? (sill_shown ? SILL_OUT_M : 0.0) : 0.0;
	p = vec3(x, y, z);
	uv = vec2(x, y);
	vec3 wall = packed_rgb(custom.y);
	float tone = kind == BAY_PIECE_REVEAL ? REVEAL_TONE : kind == BAY_PIECE_SILL ? SILL_TONE : 1.0;
	col = vec4(wall * tone, kind == BAY_PIECE_PANE ? float(SK_FAR_WALL) / BYTE_TOP : 0.0);
	float seed = float(code & BYTE_MASK) / BYTE_STEPS;
	uv2 = vec2(float((code >> BAY_FIN_SHIFT) & FIN_MASK) + seed, 0.0);
	room = vec4(width, sill, head, float((code >> BAY_FLAGS_SHIFT) & BYTE_MASK) / BYTE_TOP);
	trim = vec4(packed_rgb(custom.z), 0.0);
}
