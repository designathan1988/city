// The streams of a mesh surface as the RenderingServer keeps them (surface_data.h's own parts): the layout a format gives, and each
// stream written byte for byte as Godot 4.7's RenderingServer::_surface_set_data writes it (servers/rendering/rendering_server.cpp,
// 4.7-stable). Each writer returns false when its arrays do not fit the surface.
#pragma once

#include <godot_cpp/classes/rendering_server.hpp>
#include <godot_cpp/variant/aabb.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/vector4.hpp>

#include <cstdint>

namespace city::surface {

// RenderingServer::mesh_surface_make_offsets_from_format: where each array starts in its stream, and each stream's stride.
struct Layout {
	uint32_t offset[godot::RenderingServer::ARRAY_MAX] = {};
	uint32_t vertex = 0, normal = 0, attrib = 0, skin = 0;
};

// The full format of `arrays` (mesh_create_surface_data_from_arrays): each array given, the tangent with a normal, eight bone weights when
// the bones say so, the custom formats and flags of `compress_format`, the current version; `index_count` the indices given.
uint64_t format_of(const godot::Array &arrays, uint64_t compress_format, int vertex_count, int &index_count);
Layout layout_of(uint64_t format, int vertex_count);
// The box of the positions, each side at least the engine's least.
godot::AABB box_of(const godot::Array &arrays);
// The packed UVs' scale: zero when every UV is inside 0..1, else twice their largest reach.
godot::Vector4 uv_scale_of(const godot::Array &arrays, uint64_t format, int vertex_count);

// The positions, and the normals and tangents (packed: an axis and an angle) into the vertex stream.
bool write_vertices(const godot::Array &arrays, uint64_t format, const Layout &l, const godot::AABB &box, int n, uint8_t *vw);
// The colours, UVs and custom arrays into the attribute stream.
bool write_attributes(const godot::Array &arrays, uint64_t format, const Layout &l, const godot::Vector4 &uv_scale, int n, uint8_t *aw);
// The bones and weights into the skin stream, and each bone's box round what it weighs on (none when no bone weighs on anything).
bool write_skin(const godot::Array &arrays, uint64_t format, const Layout &l, int n, uint8_t *sw, godot::Array &bone_boxes);
// The indices, sixteen bits up to the engine's count of vertices, else thirty-two.
void write_indices(const godot::Array &arrays, const Layout &l, int index_count, uint8_t *iw);
// The levels as the engine keeps them, nearest first, each with fewer indices than the whole.
godot::Array levels_of(const godot::Dictionary &lods, int index_count, int vertex_count);

} // namespace city::surface
