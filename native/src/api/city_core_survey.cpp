// A 3D reference read into a building, as a script command: the model's triangles and its pictures come from project/render/reference_model.gd.
#include "api/body_api.h"
#include "api/city_builder.h"

#include "sim/commands/body_survey.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/model_ops.h"

#include <godot_cpp/variant/packed_byte_array.hpp>

#include <cstring>

using namespace godot;

namespace city {

namespace {
constexpr const char *SURVEY_LABEL = "cmd.build.model";
constexpr int TRIANGLE_CORNERS = 3, REFERENCE_FUNCTION = 0, REFERENCE_ZONE = 2;

SurveyPoint point_of(const Vector3 &v) { return { v.x, v.y, v.z }; }

SurveyView view_of(const Dictionary &d) {
	SurveyView v;
	const PackedByteArray bytes = d["rgba"];
	v.rgba.resize(size_t(bytes.size()));
	std::memcpy(v.rgba.data(), bytes.ptr(), v.rgba.size());
	v.w = int(d["w"]);
	v.h = int(d["h"]);
	v.centre = point_of(d["centre"]);
	v.right = point_of(d["right"]);
	v.up = point_of(d["up"]);
	v.out = point_of(d["out"]);
	v.half_w = float(d["half_w"]);
	v.half_h = float(d["half_h"]);
	return v;
}
} // namespace

// The building the model shows, at (x, z), turned by `rot`, in place of the building `replaced` (-1 for none). Both are one step of undo.
Dictionary CityBuilder::build_reference(const PackedVector3Array &triangles, const Array &views, double x, double z, double rot, int replaced) {
	std::vector<SurveyTriangle> tris;
	tris.reserve(size_t(triangles.size()) / TRIANGLE_CORNERS);
	for (int64_t i = 0; i + 2 < triangles.size(); i += TRIANGLE_CORNERS) tris.push_back({ point_of(triangles[i]), point_of(triangles[i + 1]), point_of(triangles[i + 2]) });
	std::vector<SurveyView> pictures;
	for (int64_t i = 0; i < views.size(); i++) pictures.push_back(view_of(views[i]));
	// The model is read before the map is locked: the survey reads only the model and the generator's tables (set at boot,
	// read-only after); under the lock the command only places what it read.
	b40::Body body;
	const bool found = survey_body(state_->buildings.designs, tris, pictures, body);
	return run_edit(*state_, SURVEY_LABEL, [&](CityState &s) {
		EditResult r;
		if (!found) {
			r.error = "hint.reference.empty";
			return r;
		}
		if (replaced >= 0 && replaced < int(s.buildings.items.size()) && s.buildings.items[size_t(replaced)].alive) j_remove_building(s, replaced);
		return place_body(s, body, x, z, rot, REFERENCE_FUNCTION, REFERENCE_ZONE);
	});
}

} // namespace city
