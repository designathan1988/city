// A 3D reference read into a building, as a script command: the model's triangles and its pictures come from project/render/reference_model.gd.
#include "api/body_api.h"
#include "api/city_builder.h"

#include "sim/commands/body_survey.h"
#include "sim/commands/model_ops.h"

#include <godot_cpp/variant/packed_byte_array.hpp>

#include <cmath>
#include <limits>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr const char *SURVEY_LABEL = "cmd.build.model";
constexpr int REFERENCE_FUNCTION = 0, REFERENCE_ZONE = 2;

SurveyPoint point_of(const Vector3 &v) { return { v.x, v.y, v.z }; }

bool finite(const SurveyPoint &p) { return std::isfinite(p.x) && std::isfinite(p.y) && std::isfinite(p.z); }

bool current_reference_target(const CityState &s, int replaced, const Dictionary &expected) {
	if (expected.is_empty()) return true;
	if (uint64_t(int64_t(expected.get("generation", 0))) != s.generation || int(expected.get("id", -1)) != replaced) return false;
	const uint64_t version = s.buildings.item_version(replaced);
	return replaced < 0 || (version != 0 && version == uint64_t(int64_t(expected.get("version", 0))));
}

bool view_of(const Dictionary &d, SurveyView &v) {
	constexpr int RGBA_CHANNELS = 4;
	const int64_t width = d.get("w", 0), height = d.get("h", 0);
	if (width <= 0 || height <= 0 || width > std::numeric_limits<int>::max() || height > std::numeric_limits<int>::max()) return false;
	if (width > std::numeric_limits<int64_t>::max() / RGBA_CHANNELS / height) return false;
	const PackedByteArray bytes = d["rgba"];
	if (bytes.size() != width * height * RGBA_CHANNELS) return false;
	v.w = int(width); v.h = int(height);
	v.centre = point_of(d["centre"]);
	v.right = point_of(d["right"]);
	v.up = point_of(d["up"]);
	v.out = point_of(d["out"]);
	v.half_w = float(d["half_w"]);
	v.half_h = float(d["half_h"]);
	if (!finite(v.centre) || !finite(v.right) || !finite(v.up) || !finite(v.out)
		|| !std::isfinite(v.half_w) || !std::isfinite(v.half_h) || v.half_w <= 0 || v.half_h <= 0) return false;
	auto retained = std::make_shared<const PackedByteArray>(bytes);
	v.rgba = SurveyPixels{retained, retained->ptr(), size_t(retained->size())};
	return true;
}
} // namespace

// The building the model shows, at (x, z), turned by `rot`, in place of the building `replaced` (-1 for none). Both are one step of undo.
Dictionary CityBuilder::reference_target(int replaced) const {
	std::shared_lock<std::shared_mutex> map(state_->map_mutex);
	Dictionary target; target["generation"] = int64_t(state_->generation); target["id"] = replaced;
	target["version"] = int64_t(state_->buildings.item_version(replaced)); return target;
}

Ref<PreparedReference> CityBuilder::prepare_reference(const Array &parts) const { return PreparedReference::make(parts); }

Dictionary CityBuilder::build_reference(const Variant &geometry, const Array &views, double x, double z, double rot, int replaced, const Dictionary &expected) {
	const auto invalid = [] { Dictionary r; r["ok"] = false; r["error"] = "hint.reference.failed"; return r; };
	if (!std::isfinite(x) || !std::isfinite(z) || !std::isfinite(rot)) return invalid();
	b40::ModelInputs inputs;
	uint64_t generation, table_generation, target_version;
	double built_at;
	{
		std::shared_lock<std::shared_mutex> map(state_->map_mutex);
		if (!state_->ready || !current_reference_target(*state_, replaced, expected)) return invalid();
		inputs = model_inputs(state_->buildings.designs);
		generation = state_->generation;
		table_generation = state_->buildings.designs.edited->generation();
		target_version = state_->buildings.item_version(replaced);
		built_at = state_->clock.minutes.load();
	}
	Ref<PreparedReference> source;
	if (geometry.get_type() == Variant::OBJECT) {
		Object *object = geometry;
		if (auto *prepared = Object::cast_to<PreparedReference>(object)) source = Ref<PreparedReference>(prepared);
	} else if (geometry.get_type() == Variant::PACKED_VECTOR3_ARRAY) {
		source = PreparedReference::legacy(PackedVector3Array(geometry));
	}
	if (source.is_null()) return invalid();
	const SurveyTriangles &tris = source->triangles();
	std::vector<SurveyView> pictures;
	for (int64_t i = 0; i < views.size(); i++) {
		SurveyView view; if (!view_of(views[i], view)) return invalid(); pictures.push_back(std::move(view));
	}
	b40::Body body;
	if (!survey_body(inputs, tris, pictures, body)) {
		Dictionary failed; failed["ok"] = false; failed["error"] = "hint.reference.empty"; return failed;
	}
	return run_edit(*state_, SURVEY_LABEL, [&](CityState &s) {
		EditResult r;
		if (s.generation != generation || s.buildings.designs.edited->generation() != table_generation ||
				(replaced >= 0 && s.buildings.item_version(replaced) != target_version) || !current_reference_target(s, replaced, expected)) {
			r.error = "hint.reference.failed"; return r;
		}
		if (replaced >= 0 && replaced < int(s.buildings.items.size()) && s.buildings.items[size_t(replaced)].alive) j_remove_building(s, replaced);
		return place_body(s, body, x, z, rot, REFERENCE_FUNCTION, REFERENCE_ZONE, nullptr, built_at);
	});
}

} // namespace city
