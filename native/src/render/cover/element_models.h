// The models of the element brush (data/render/cover.json "elements"): each prop kind's meshes, from the imported glTF
// scenes of project/assets/elements, one unit across and standing on y = 0 once scaled by `fit`. Fallen leaves are a
// flat quad of their texture. An item picks its variant by its seed.
#pragma once

#include "render/data_reader.h"
#include "render/rs_util.h"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/mesh.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <vector>

namespace city {

// A model laid on the ground: an imported scene's mesh (a loaded resource, its materials on its surfaces: the engine makes its pipelines as it
// loads it), or a ground quad made here whole with its material (surface_data.h).
struct ElementModel {
	godot::RID mesh; // what is drawn
	godot::Ref<godot::Resource> keep; // what holds it: the imported mesh, or the quad's material
	OwnedMesh made; // the quad's mesh
	float fit = 1; // scale that makes the model one unit across
	float foot = 0; // the height of its lowest point, in its own units
};

class ElementModels {
public:
	// cover.json elements, through the strict reader's values: a row of models for each kind laid on the ground (`laid`, the
	// catalogue's kinds before its first effect).
	bool load(const DataValue &elements, int laid, godot::PackedStringArray &errors);
	int kinds() const { return int(first_.size()); } // the prop kinds, effects not among them
	int first(int kind) const { return first_[size_t(kind)]; }
	int variants(int kind) const { return count_[size_t(kind)]; }
	const std::vector<ElementModel> &all() const { return models_; }

private:
	std::vector<ElementModel> models_;
	std::vector<int> first_, count_;
};

} // namespace city
