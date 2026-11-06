// Fewer triangles for the distance: what a mesh made in code does not get by itself (the engine makes levels only for what it imports). The levels go to the `lods` of a surface,
// by the error each allows in metres, the key the engine compares with the distance; the same level for a whole MultiMesh, by the point of its box nearest the camera.
#pragma once

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

#include <initializer_list>

namespace city {

// What a level may move the surface by, metres: a building's own design, the street's small things.
constexpr std::initializer_list<float> DESIGN_LEVELS_M = { 0.05f, 0.25f, 1.f, 4.f };
constexpr std::initializer_list<float> STREET_LEVELS_M = { 0.03f, 0.12f, 0.5f, 2.f };

// `errors` from the smallest. Loose boxes go whole past their error (meshopt_SimplifyPrune): the plain simplifier has no edge of theirs to collapse. On a job's thread too.
godot::Dictionary mesh_levels(const godot::PackedVector3Array &pos, const godot::PackedInt32Array &idx, std::initializer_list<float> errors);

} // namespace city
