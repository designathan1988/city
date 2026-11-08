// The models of the element brush (data/render/cover.json "elements"): each prop kind's meshes, from the imported glTF
// scenes of project/assets/elements, one unit across and standing on y = 0 once scaled by `fit`. Fallen leaves are a
// flat quad of their texture. An item picks its variant by its seed.
#pragma once

#include "render/data_reader.h"
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

struct ElementModel {
	godot::Ref<godot::Mesh> mesh;
	float fit = 1; // scale that makes the model one unit across
	float foot = 0; // the height of its lowest point, in its own units
};

class ElementModels {
public:
	bool load(const DataValue &elements, godot::PackedStringArray &errors); // cover.json elements, through the strict reader's values
	int kinds() const { return int(first_.size()); } // the prop kinds, effects not among them
	int first(int kind) const { return first_[size_t(kind)]; }
	int variants(int kind) const { return count_[size_t(kind)]; }
	const std::vector<ElementModel> &all() const { return models_; }

private:
	std::vector<ElementModel> models_;
	std::vector<int> first_, count_;
};

} // namespace city
