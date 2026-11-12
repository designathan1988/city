#include "api/prepared_reference.h"
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <cmath>
#include <limits>

namespace city {
using namespace godot;
namespace {
constexpr size_t CORNERS = 3;
struct Part {
	PackedVector3Array points;
	PackedInt32Array indices;
	const Vector3 *vertices = nullptr;
	const int32_t *references = nullptr;
	size_t point_count = 0, index_count = 0;
	size_t first = 0, end = 0;
	bool plan_axes = false;
};
struct Input {
	std::vector<Part> parts;
	size_t corners = 0;
	bool append(Part part) {
		part.point_count = size_t(part.points.size()); part.index_count = size_t(part.indices.size());
		part.vertices = part.points.ptr(); part.references = part.index_count ? part.indices.ptr() : nullptr;
		const size_t count = part.index_count ? part.index_count : part.point_count;
		if (count > std::numeric_limits<size_t>::max() - corners) return false;
		// Validate referenced points only, just as the original expanded input did; unused vertices may be irrelevant.
		for (size_t i = 0; i < count; ++i) {
			const int64_t at = part.references ? int64_t(part.references[i]) : int64_t(i);
			if (at < 0 || size_t(at) >= part.point_count) return false;
			const Vector3 &p = part.vertices[at];
			if (!std::isfinite(p.x) || !std::isfinite(p.y) || !std::isfinite(p.z)) return false;
		}
		if (count) {
			part.first = corners; corners += count; part.end = corners;
			parts.push_back(std::move(part));
		}
		return true;
	}
	static SurveyTriangle read(const void *context, size_t triangle, size_t &hint) {
		const Input &input = *static_cast<const Input *>(context);
		SurveyTriangle out;
		for (size_t corner = 0; corner < CORNERS; ++corner) {
			const size_t at = triangle * CORNERS + corner;
			while (at >= input.parts[hint].end) ++hint;
			const Part &part = input.parts[hint];
			const size_t local = at - part.first;
			const size_t vertex = part.references ? size_t(part.references[local]) : local;
			const Vector3 &p = part.vertices[vertex];
			out[corner] = part.plan_axes ? SurveyPoint{p.x, p.y, p.z} : SurveyPoint{p.x, -p.z, p.y};
		}
		return out;
	}
};
SurveyTriangles range(const std::shared_ptr<Input> &input) {
	return SurveyTriangles(input, input.get(), input->corners / CORNERS, Input::read);
}
} // namespace

Ref<PreparedReference> PreparedReference::make(const Array &parts) {
	if (parts.is_empty()) return {};
	auto input = std::make_shared<Input>();
	for (int64_t i = 0; i < parts.size(); ++i) {
		if (parts[i].get_type() != Variant::DICTIONARY) return {};
		const Dictionary fields = parts[i];
		const Variant points = fields.get("points", Variant()), index = fields.get("index", Variant());
		if (points.get_type() != Variant::PACKED_VECTOR3_ARRAY || index.get_type() != Variant::PACKED_INT32_ARRAY) return {};
		Part part; part.points = points; part.indices = index;
		if (!input->append(std::move(part))) return {};
	}
	Ref<PreparedReference> prepared; prepared.instantiate();
	prepared->triangles_ = range(input);
	return prepared;
}

Ref<PreparedReference> PreparedReference::legacy(const PackedVector3Array &triangles) {
	auto input = std::make_shared<Input>();
	Part part; part.points = triangles; part.plan_axes = true;
	if (!input->append(std::move(part))) return {};
	Ref<PreparedReference> prepared; prepared.instantiate();
	prepared->triangles_ = range(input);
	return prepared;
}
} // namespace city
