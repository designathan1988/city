// Meshes built once at startup: unit building masses and roofs (shape rules live in code), plus every
// low-poly model defined in project/data/render/models.json (cars, people, trees, facade modules, props).
#pragma once

#include "render/mesh_builder.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_string_array.hpp>

#include <string>
#include <unordered_map>

namespace city {

// UV2.x tags read by the shaders.
enum class SurfaceTag : int { FIXED = 0, TINT = 1, GLOW = 2 };
enum class MassFace : int { SIDE_X = 0, SIDE_Z = 1, TOP = 2 };

struct ModelLibrary {
	godot::Ref<godot::ArrayMesh> mass_box; // x,z in [-0.5,0.5], y in [0,1]; UV = position on the face
	godot::Ref<godot::ArrayMesh> roof_gable; // ridge along x, height 1
	godot::Ref<godot::ArrayMesh> roof_hip;
	std::unordered_map<std::string, godot::Ref<godot::ArrayMesh>> models;

	bool build(const godot::Dictionary &models_json, godot::PackedStringArray &errors);
	godot::RID model(const char *name) const;
};

} // namespace city
