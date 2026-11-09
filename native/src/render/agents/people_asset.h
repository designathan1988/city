// The cast (project/assets/people/people.bin, its clips cooked from the lab's own motion by tools/assets/cook_people_lab.py, and atlas.bin by
// compress_atlas.gd, from Roadcraft's MakeHuman CC0 people on the Rocketbox MIT skeleton): per body and level an ArrayMesh in the bind pose
// (metres, +Z forward, feet at y = 0) whose bone attributes carry four bone indices and their weights (summing to 1), UV = where in the
// body's atlas, UV2 = (region, roughness), COLOR = the vertex's own colour over the atlas; the atlases as one array, a layer a body; the
// clips as a palette texture (3 RGBA texels per bone and frame = the rows of the 3x4 bind -> pose matrix, the bodies' blocks side by side in
// `columns` columns) that render/shaders/people.gdshader skins with, each clip of a kind (standing, forward, turn, cycle) with, per body,
// its stride and a curve over its frames (the ground a start or a stop has covered, the angle a turn has turned); and a card for the far
// off, showing the strip of yaw views each atlas holds (render/shaders/people_far.gdshader).
#pragma once

#include "core/bytes.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/texture2d_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/vector4.hpp>

#include <string>
#include <vector>

namespace city {

// What a clip is to the gait (the lab's citizenGait.ts): played by the clock or a phase, a start or stop by the ground it covers, a turn by
// the angle it turns, a walk or run cycle by the stride.
enum class ClipKind : uint8_t { STAND = 0, FORWARD, TURN, CYCLE, COUNT };

// A clip of a variation's block of palette rows.
struct PersonClip {
	std::string name;
	int row = 0, frames = 0;
	float seconds[2] = { 1, 1 }; // a man's and a woman's: the library's takes differ
	ClipKind kind = ClipKind::STAND;
	bool loop = true; // a loop's last row runs into its first; a one-shot's last row is its end
};

// A clip on one body, at its first frame: where the pelvis is in root space, and how far the body runs from it (metres); and the ground
// one cycle covers on this body (a cycle's stride, metres; 0 for what does not walk).
struct PersonFit {
	godot::Vector3 pelvis;
	float above = 0, below = 0, ahead = 0, side = 0, stride = 0;
};

constexpr int PALM_FLOATS = 12; // three rows of four: a transform without its last row
constexpr int JOINT_COUNT = 18, JOINT_FLOATS = JOINT_COUNT * 3; // ragdoll.h RAGDOLL_JOINTS

struct PersonVariation {
	float height = 1, alpha_test = 0, mood = 0; // mood: -1 low .. 1 bright
	bool female = false, senior = false, child = false;
	float helmet[PALM_FLOATS] = {}; // three rows, a shell of width 1 fitted to this head, at rest
	float palm[PALM_FLOATS] = {}; // three rows, the palm at rest: X along the fingers, Y to the thumb, Z out of the back of the hand
	float joints[JOINT_FLOATS] = {}; // where the ragdoll's joints stand at rest
	std::vector<godot::Array> lods; // each level's surface arrays, 0 = near (made a mesh with its material by the renderer: surface_data.h)
	float reach = 0; // the near level's farthest vertex from the bind origin, metres: how far a bone carries skin (body_renderer.cpp)
	std::vector<PersonFit> fits; // one a clip, in PeopleAsset::clips' order
	std::vector<float> curves; // each clip's frames + 1 values, in clips' order (PeopleAsset::curve)
	std::vector<int> curve_at; // where each clip's values start in `curves`
};

struct PeopleAsset {
	std::vector<PersonVariation> variations;
	godot::Ref<godot::ImageTexture> palette;
	godot::Ref<godot::ImageTexture> faces; // where each face vertex moves, by channel (people_face.gdshaderinc)
	godot::Ref<godot::Texture2DArray> atlas; // a layer a body
	godot::Array card; // the quad of the far off, as surface arrays
	int card_frames = 1;
	// The strip in the atlas: x, y, a frame's width and height, in uv. A Vector4, not a Color: a Color set on a vec4 uniform is taken as sRGB and
	// turned linear by the engine (renderer_rd material_storage.cpp convert_to_vector), which shrank the strip to a black texel.
	godot::Vector4 card_strip;
	int lods = 0;
	std::vector<PersonClip> clips;
	int block_rows = 0, hand_bone = 0, head_bone = 0; // each variation's rows of the palette; the bone of the hand that holds things; the head's
	int columns = 1; // the bodies' blocks side by side in the palette: body v in column v % columns, row block v / columns
	std::vector<uint8_t> bone_part, joint_bone; // the ragdoll: which part each bone follows, which bone each joint sits on
	godot::PackedByteArray palette_bytes; // the palette as the file has it
	// A body's bones in a frame of a clip, bind -> pose rows, twelve floats a bone.
	void clip_rows(int variation, int clip_index, int frame, float *rows) const;
	bool read_clips(ByteReader &r, uint32_t count); // the clip table: false when a clip runs past the block or names no kind
	void read_curves(ByteReader &r); // each body's clips' curves
	void read_card(ByteReader &r);
	void read_ragdoll(ByteReader &r, uint32_t bones);
	// The clip of that name, as its index in `clips`; -1 when the file has none.
	int clip(const char *name) const;
	// A clip's curve on a body at frame `frame` (0 .. frames): the ground a forward clip has covered, the angle a turn has turned.
	float curve(int variation, int clip_index, int frame) const;
	// Reads and checks the file; false (with a message) when it is missing or malformed.
	bool load(const godot::String &path, const godot::String &atlas_path, godot::PackedStringArray &errors);
};

} // namespace city
