// Low-poly trees (Roadcraft render/treeMeshes.ts, without leaf cards): a tapered trunk and one or two crown masses,
// icospheres displaced by value noise and flat shaded per triangle between a dark and a lit tone with a darker heart,
// built once per species and level of detail from data/render/streets.json "trees". Crowns carry PropTag::TINT so
// each instance can shift their colour.
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

constexpr int TREE_LODS = 2;

struct TreeSpecies {
	godot::Ref<godot::ArrayMesh> mesh[TREE_LODS];
	float crown_r = 0; // crown reach at scale 1 (keeps wild trees off the roads)
};

class TreeKit {
public:
	bool build(const godot::Dictionary &trees, godot::PackedStringArray &errors);
	int count() const { return int(species_.size()); }
	const TreeSpecies &species(int i) const { return species_[size_t(i)]; }

private:
	std::vector<TreeSpecies> species_;
};

} // namespace city
