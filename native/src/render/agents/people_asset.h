// The cooked pedestrians (project/assets/people/people.bin, made by tools/assets/cook_people.py from Roadcraft's
// MakeHuman CC0 people with the Rocketbox MIT walk): per variation and level an ArrayMesh in the bind pose (A pose, arms hanging at about 49 degrees from the vertical,
// metres, +Z forward, feet at y = 0) whose bone attributes carry four bone indices and their weights (summing to 1),
// UV2 = (region, tint detail), COLOR = linear texture colour; and the walk as a float palette texture (3 RGBA texels per
// bone and frame = the rows of the 3x4 bind -> pose matrix) that render/shaders/people.gdshader skins with.
#pragma once

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>
#include <godot_cpp/variant/string.hpp>

#include <cstdint>
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

// How a pedestrian looks, from the look hash of its record (Roadcraft looks.ts): which variation of the cooked file, and the codes
// people.gdshader reads (top / bottom 0 = the texture's colour, k + 1 = clothes k; hair 0..7 hairs, 8..11 greys; skin 0..7 steps).
struct PersonLook {
	int variation = 0;
	float top = 0, bottom = 0, hair = 0, skin = 0;
};

struct PeopleAsset {
	std::vector<PersonVariation> variations;
	godot::Ref<godot::ImageTexture> palette;
	int lods = 0, bones = 0; // levels of detail; bones of the cooked skeleton the weights index
	// Reads and checks the file; false (with a message) when it is missing or malformed.
	bool load(const godot::String &path, godot::PackedStringArray &errors);
	// The look a hash stands for (the same for a walker and for the body it falls as).
	PersonLook look(uint32_t hash) const;
};

} // namespace city
