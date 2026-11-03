// The cooked pedestrians (project/assets/people/people.bin, made by tools/assets/cook_people.py from Roadcraft's
// MakeHuman CC0 people with the Rocketbox MIT walk): per variation and level an ArrayMesh in the bind pose (T pose,
// metres, +Z forward, feet at y = 0) whose CUSTOM0 carries four bone indices and CUSTOM1 their weights (RGBA8 unorm),
// UV2 = (region, tint detail), COLOR = linear texture colour; and the walk as a float palette texture (3 RGBA texels per
// bone and frame = the rows of the 3x4 bind -> pose matrix) that render/shaders/people.gdshader skins with.
#pragma once

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <vector>

namespace city {

// Regions painted per instance (Roadcraft people.json "regions").
enum PersonRegion : int { REGION_SKIN = 0, REGION_HAIR = 1, REGION_TOP = 2, REGION_BOTTOM = 3 };

struct PersonVariation {
	int palette_row = 0, frames = 1;
	float fps = 1, walk_advance = 1, height = 1;
	bool female = false, senior = false;
	std::vector<godot::Ref<godot::ArrayMesh>> lods; // 0 = near
};

struct PeopleAsset {
	std::vector<PersonVariation> variations;
	godot::Ref<godot::ImageTexture> palette;
	int lods = 0;
	// Reads and checks the file; false (with a message) when it is missing or malformed.
	bool load(const godot::String &path, godot::PackedStringArray &errors);
};

} // namespace city
