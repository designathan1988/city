// A mesh surface as the RenderingServer keeps it (surface_data.h): its streams written by surface_streams.h, put together as Godot 4.7's
// RenderingServer::mesh_create_surface_data_from_arrays puts them (and as mesh_get_surface gives them back: an empty stream is no key).
#include "render/surface_data.h"
#include "render/surface_streams.h"

#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>

using namespace godot;

namespace city {

Dictionary surface_data(const Array &arrays, const Dictionary &lods, uint64_t compress_format, const RID &material) {
	using RS = RenderingServer;
	if (arrays.size() != RS::ARRAY_MAX || arrays[RS::ARRAY_VERTEX].get_type() != Variant::PACKED_VECTOR3_ARRAY) return Dictionary();
	const int n = int(PackedVector3Array(arrays[RS::ARRAY_VERTEX]).size());
	if (n == 0) return Dictionary();
	int index_count = 0;
	const uint64_t format = surface::format_of(arrays, compress_format, n, index_count);
	if ((format & RS::ARRAY_FLAG_COMPRESS_ATTRIBUTES) && !(format & RS::ARRAY_FORMAT_NORMAL) && (format & RS::ARRAY_FORMAT_TANGENT)) return Dictionary(); // refused
	const surface::Layout l = surface::layout_of(format, n);
	PackedByteArray vertex, attrib, skin, index;
	vertex.resize(int64_t(l.vertex + l.normal) * n);
	attrib.resize(int64_t(l.attrib) * n);
	skin.resize(int64_t(l.skin) * n);
	index.resize(int64_t(l.offset[RS::ARRAY_INDEX]) * index_count);
	const AABB box = surface::box_of(arrays);
	const Vector4 uv_scale = surface::uv_scale_of(arrays, format, n);
	Array bone_boxes;
	if (!surface::write_vertices(arrays, format, l, box, n, vertex.ptrw()) || !surface::write_attributes(arrays, format, l, uv_scale, n, attrib.ptrw()) ||
			!surface::write_skin(arrays, format, l, n, skin.ptrw(), bone_boxes))
		return Dictionary();
	surface::write_indices(arrays, l, index_count, index.ptrw());
	Dictionary out;
	out["primitive"] = int(RS::PRIMITIVE_TRIANGLES);
	out["format"] = int64_t(format);
	out["vertex_data"] = vertex;
	if (!attrib.is_empty()) out["attribute_data"] = attrib;
	if (!skin.is_empty()) out["skin_data"] = skin;
	out["vertex_count"] = n;
	if (index_count > 0) {
		out["index_data"] = index;
		out["index_count"] = index_count;
	}
	out["aabb"] = box;
	out["uv_scale"] = uv_scale;
	out["lods"] = surface::levels_of(lods, index_count, n);
	if (!bone_boxes.is_empty()) out["bone_aabbs"] = bone_boxes;
	if (material.is_valid()) out["material"] = material;
	return out;
}

RID create_mesh(const TypedArray<Dictionary> &surfaces) {
	return RenderingServer::get_singleton()->mesh_create_from_surfaces(surfaces);
}

AABB arrays_box(const Array &arrays) {
	if (arrays.size() <= RenderingServer::ARRAY_VERTEX || arrays[RenderingServer::ARRAY_VERTEX].get_type() != Variant::PACKED_VECTOR3_ARRAY) return AABB();
	const PackedVector3Array v = arrays[RenderingServer::ARRAY_VERTEX];
	if (v.is_empty()) return AABB();
	Vector3 lo = v[0], hi = v[0];
	for (int64_t i = 1; i < v.size(); i++) {
		lo = lo.min(v[i]);
		hi = hi.max(v[i]);
	}
	return AABB(lo, hi - lo);
}

RID mesh_of_arrays(const Array &arrays, const RID &material, uint64_t compress_format) {
	const Dictionary s = surface_data(arrays, Dictionary(), compress_format, material);
	if (s.is_empty()) return RID();
	TypedArray<Dictionary> surfaces;
	surfaces.push_back(s);
	return create_mesh(surfaces);
}

} // namespace city
