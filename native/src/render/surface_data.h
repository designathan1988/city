// A mesh surface as the RenderingServer keeps it, made here so every mesh is created whole, its materials in it, by
// RenderingServer::mesh_create_from_surfaces: the one way of making a mesh the engine precompiles the pipelines of (Godot 4.7
// rendering_server_default.h mesh_create_from_surfaces calls mesh_generate_pipelines with the surfaces' own materials, in the
// background when the caller is not the server's thread). A mesh made by mesh_create and mesh_add_surface_from_arrays, or given its
// material by an instance override, has its pipelines made on the frame it is first drawn (the "surface" compilations of the Godot
// docs, Reducing stutter from shader (pipeline) compilations): the stall of doc/owner-review.md §9. The bytes follow Godot's own
// RenderingServer::mesh_create_surface_data_from_arrays and _surface_set_data (servers/rendering/rendering_server.cpp, 4.7-stable),
// which the extension API does not expose.
#pragma once

#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/rid.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <cstdint>

namespace city {

// The surface of `arrays` (Mesh::ARRAY_MAX entries, as mesh_add_surface_from_arrays takes them: triangles, 3D vertices, an index
// array; no blend shapes), its levels `lods` (edge length -> indices), `compress_format` (the flags and custom formats the RenderingServer
// takes) and `material`, as mesh_create_from_surfaces reads it. An empty dictionary when the arrays are not such a surface.
godot::Dictionary surface_data(const godot::Array &arrays, const godot::Dictionary &lods, uint64_t compress_format, const godot::RID &material);

// A mesh of `surfaces` (surface_data's), created whole: the engine makes their pipelines as it makes the mesh.
godot::RID create_mesh(const godot::TypedArray<godot::Dictionary> &surfaces);

// A mesh of one surface of `arrays` with `material` (a primitive's get_mesh_arrays, a cooked body's): none when they are not a surface.
godot::RID mesh_of_arrays(const godot::Array &arrays, const godot::RID &material, uint64_t compress_format = 0);

// The box of the vertices of `arrays` (an empty box when there are none).
godot::AABB arrays_box(const godot::Array &arrays);

} // namespace city
