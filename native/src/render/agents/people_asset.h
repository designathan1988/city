// The cooked pedestrians (project/assets/people/people.bin, made by tools/assets/cook_people.py from Roadcraft's
// MakeHuman CC0 people with the Rocketbox MIT walk): per variation and level an ArrayMesh in the bind pose (T pose,
// metres, +Z forward, feet at y = 0) whose bone attributes carry four bone indices and their weights (summing to 1),
// UV2 = (region, tint detail), COLOR = linear texture colour; and the walk as a float palette texture (3 RGBA texels per
// bone and frame = the rows of the 3x4 bind -> pose matrix) that render/shaders/people.gdshader skins with.
#pragma once

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <string>
#include <vector>

namespace city {

// Regions painted per instance (Roadcraft people.json "regions").
enum PersonRegion : int { REGION_SKIN = 0, REGION_HAIR = 1, REGION_TOP = 2, REGION_BOTTOM = 3 };

// A clip of a variation's block of palette rows; a walk's own frames are the variation's.
struct PersonClip {
	std::string name;
	int row = 0, frames = 0;
};

// A clip on one body, at its first frame: where the pelvis is in root space, and how far the body runs from it (metres).
struct PersonFit {
	godot::Vector3 pelvis;
	float above = 0, below = 0, ahead = 0, side = 0;
};

struct PersonVariation {
	int walk_frames = 1;
	float idle_s = 1, walk_advance = 1, height = 1;
	bool female = false, senior = false, child = false;
	std::vector<godot::Ref<godot::ArrayMesh>> lods; // 0 = near
	std::vector<PersonFit> fits; // one a clip, in PeopleAsset::clips' order
};

struct PeopleAsset {
	std::vector<PersonVariation> variations;
	godot::Ref<godot::ImageTexture> palette;
	int lods = 0;
	std::vector<PersonClip> clips;
	int block_rows = 0, idle_row = 0, idle_frames = 0; // each variation's rows of the palette; the idle, as the shader asks it
	// The clip of that name, as its index in `clips`; -1 when the file has none.
	int clip(const char *name) const;
	// Reads and checks the file; false (with a message) when it is missing or malformed.
	bool load(const godot::String &path, godot::PackedStringArray &errors);
};

} // namespace city
