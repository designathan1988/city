// The cast (project/assets/people/people.bin and atlas.bin, MakeHuman CC0 people on the Rocketbox MIT skeleton, provenance in
// project/assets/people/LICENSES.json; the atlases compressed by tools/assets/compress_atlas.gd): per body and level an ArrayMesh in the bind pose (metres, +Z forward, feet at
// y = 0) whose bone attributes carry four bone indices and their weights (summing to 1), UV = where in the body's atlas, UV2 = (region,
// roughness), COLOR = the vertex's own colour over the atlas; the atlases as one array, a layer a body; the clips as a palette texture
// (3 RGBA texels per bone and frame = the rows of the 3x4 bind -> pose matrix) that render/shaders/people.gdshader skins with; and a
// card for the far off, showing the strip of yaw views each atlas holds (render/shaders/people_far.gdshader).
#pragma once

#include "core/bytes.h"
#include "render/rs_util.h"

#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/texture2d_array.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <string>
#include <vector>

namespace city {

// A clip of a variation's block of palette rows; a walk's own frames are the variation's.
struct PersonClip {
	std::string name;
	int row = 0, frames = 0;
	float seconds[2] = { 1, 1 }; // a man's and a woman's: the library's takes differ
};

// A clip on one body, at its first frame: where the pelvis is in root space, and how far the body runs from it (metres).
struct PersonFit {
	godot::Vector3 pelvis;
	float above = 0, below = 0, ahead = 0, side = 0;
};

constexpr int PALM_FLOATS = 12;
constexpr int JOINT_COUNT = 18, JOINT_FLOATS = JOINT_COUNT * 3; // ragdoll.h RAGDOLL_JOINTS

struct PersonVariation {
	int walk_frames = 1;
	float walk_advance = 1, height = 1, alpha_test = 0, mood = 0; // mood: -1 low .. 1 bright
	bool female = false, senior = false, child = false;
	float helmet[PALM_FLOATS] = {}; // three rows, a shell of width 1 fitted to this head, at rest
	float palm[PALM_FLOATS] = {}; // three rows, the palm at rest: X along the fingers, Y to the thumb, Z out of the back of the hand
	float joints[JOINT_FLOATS] = {}; // where the ragdoll's joints stand at rest
	std::vector<OwnedMesh> lods; // 0 = near; each made with the people's material (surface_data.h)
	float reach = 0; // the near level's farthest vertex from the bind origin, metres: how far a bone carries skin (body_renderer.cpp)
	std::vector<PersonFit> fits; // one a clip, in PeopleAsset::clips' order
};

struct PeopleAsset {
	std::vector<PersonVariation> variations;
	godot::Ref<godot::ImageTexture> palette;
	godot::Ref<godot::ImageTexture> faces; // where each face vertex moves, by channel (people_face.gdshaderinc)
	godot::Ref<godot::Texture2DArray> atlas; // a layer a body
	OwnedMesh card; // the quad of the far off, made with the far material
	int card_frames = 1;
	godot::Color card_strip; // the strip in the atlas: x, y, a frame's width and height, in uv
	int lods = 0;
	std::vector<PersonClip> clips;
	int block_rows = 0, hand_bone = 0, head_bone = 0; // each variation's rows of the palette; the bone of the hand that holds things; the head's
	std::vector<uint8_t> bone_part, joint_bone; // the ragdoll: which part each bone follows, which bone each joint sits on
	// The palette's rows the CPU poses a body from (clip_rows: a body struck down stands in a clip's first frame and rises by
	// another's frames), of every variation, cut from the palette at load; the palette itself lives in its texture alone.
	std::vector<uint8_t> posed_bytes;
	std::vector<int> posed_at, posed_frames; // by clip: the first of its kept frames in posed_bytes, and how many are kept
	void keep_posed(const uint8_t *texels); // fills them from the palette's bytes
	// A body's bones in a frame of a clip, bind -> pose rows, twelve floats a bone: of the frames kept (keep_posed).
	void clip_rows(int variation, int clip_index, int frame, float *rows) const;
	void read_card(ByteReader &r);
	void read_ragdoll(ByteReader &r, uint32_t bones);
	// The clip of that name, as its index in `clips`; -1 when the file has none.
	int clip(const char *name) const;
	// Reads and checks the file; false (with a message) when it is missing or malformed. `material`, `far_material`: what the bodies'
	// meshes and the far card are made with.
	bool load(const godot::String &path, const godot::String &atlas_path, const godot::RID &material, const godot::RID &far_material, godot::PackedStringArray &errors);
};

} // namespace city
