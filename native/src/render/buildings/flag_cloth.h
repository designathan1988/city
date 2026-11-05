// The cloth every flag is an instance of, and the flags of a chunk packed as its instances (shell.h FlagInstance): one
// MultiMesh per chunk in the parts material, building_parts.gdshader laying each cloth downwind and waving it.
#pragma once

#include "render/buildings/shell.h"

#include <godot_cpp/classes/array_mesh.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>

#include <vector>

namespace city::b40 {

// A 1 x 1 plane, hoist at x = 0, top at y = 0, both faces (a cloth is seen from either side), tagged for the shader.
godot::Ref<godot::ArrayMesh> cloth_mesh();
// The instances in the MultiMesh layout (rs_util.h MM_PLAIN_STRIDE): the hoist top as the origin, the width and height as the
// basis lengths, the design in the custom data.
godot::PackedFloat32Array bake_flags(const std::vector<FlagInstance> &flags);

} // namespace city::b40
