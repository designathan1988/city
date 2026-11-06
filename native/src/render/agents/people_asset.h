// The cast (project/assets/people/people.bin and atlas.bin, made by tools/assets/cook_people.py and compress_atlas.gd from Roadcraft's
// MakeHuman CC0 people on the Rocketbox MIT skeleton): per body and level an ArrayMesh in the bind pose (metres, +Z forward, feet at
// y = 0) whose bone attributes carry four bone indices and their weights (summing to 1), UV = where in the body's atlas, UV2 = (region,
// roughness), COLOR = the vertex's own colour over the atlas; the atlases as one array, a layer a body; the clips as a palette texture
// (3 RGBA texels per bone and frame = the rows of the 3x4 bind -> pose matrix) that render/shaders/people.gdshader skins with; and a
// card for the far off, showing the strip of yaw views each atlas holds (render/shaders/people_far.gdshader).
#pragma once

#include <godot_cpp/classes/array_mesh.hpp>
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

struct PersonVariation {
	int walk_frames = 1;
	float walk_advance = 1, height = 1, alpha_test = 0, mood = 0; // mood: -1 low .. 1 bright
	bool female = false, senior = false, child = false;
	float helmet[PALM_FLOATS] = {}; // three rows, a shell of width 1 fitted to this head, at rest
	float palm[PALM_FLOATS] = {}; // three rows, the palm at rest: X along the fingers, Y to the thumb, Z out of the back of the hand
	std::vector<godot::Ref<godot::ArrayMesh>> lods; // 0 = near
	std::vector<PersonFit> fits; // one a clip, in PeopleAsset::clips' order
};

struct PeopleAsset {
	std::vector<PersonVariation> variations;
	godot::Ref<godot::ImageTexture> palette;
	godot::Ref<godot::ImageTexture> faces; // where each face vertex moves, by channel (people_face.gdshaderinc)
	godot::Ref<godot::Texture2DArray> atlas; // a layer a body
	godot::Ref<godot::ArrayMesh> card; // the quad of the far off
	int card_frames = 1;
	godot::Color card_strip; // the strip in the atlas: x, y, a frame's width and height, in uv
	int lods = 0;
	std::vector<PersonClip> clips;
	int block_rows = 0, hand_bone = 0, head_bone = 0; // each variation's rows of the palette; the bone of the hand that holds things; the head's
	// The clip of that name, as its index in `clips`; -1 when the file has none.
	int clip(const char *name) const;
	// Reads and checks the file; false (with a message) when it is missing or malformed.
	bool load(const godot::String &path, const godot::String &atlas_path, godot::PackedStringArray &errors);
};

} // namespace city
