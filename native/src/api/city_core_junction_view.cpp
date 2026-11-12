// Junction-mark read model: prepare once per network publication, prune spatial subtrees, then project visible candidates.
#include "api/city_core.h"

#include <godot_cpp/variant/plane.hpp>
#include <godot_cpp/variant/typed_array.hpp>

#include <algorithm>
#include <cmath>
#include <limits>

using namespace godot;

namespace city {
namespace {
constexpr size_t JUNCTION_MARK_ARMS = 3;
constexpr int FIRST_SIDE_PLANE = 2; // Camera3D.get_frustum: near, far, left, top, right, bottom
constexpr double PLANE_ROUNDING_ULPS = 16; // conservative roundoff allowance for float camera-plane construction/projection

Dictionary mark_dict(const JunctionMarkCache::Mark &mark) {
	Dictionary out;
	out["node"] = mark.node; out["at"] = mark.at; out["ctl"] = mark.control;
	return out;
}
} // namespace

void CityCore::prepare_junction_marks() {
	const CityState &s = *state_;
	JunctionMarkCache &view = junction_marks_;
	if (view.generation == s.generation && view.revision == s.edit_revision) return;
	view = JunctionMarkCache();
	for (size_t id = 0; id < s.roads.nodes.size(); id++) {
		const RoadNode &node = s.roads.nodes[id];
		if (!node.alive || node.segs.size() < JUNCTION_MARK_ARMS) continue;
		const Vector3 at{ float(node.x), float(node.y), float(node.z) };
		if (view.marks.empty()) view.low = view.high = at.y;
		view.low = std::min(view.low, float(at.y)); view.high = std::max(view.high, float(at.y));
		view.bounds.insert(int(view.marks.size()), { at.x, at.z, at.x, at.z });
		view.marks.push_back({ int(id), int(node.ctl), at });
	}
	view.generation = s.generation; view.revision = s.edit_revision;
}

Array CityCore::junctions() {
	std::shared_lock<std::shared_mutex> map(state_->map_mutex);
	prepare_junction_marks();
	Array out;
	for (const auto &mark : junction_marks_.marks) out.push_back(mark_dict(mark));
	return out;
}

Array CityCore::junction_marks(Camera3D *camera, const Rect2 &screen) {
	Array out;
	if (!camera) return out;
	std::shared_lock<std::shared_mutex> map(state_->map_mutex);
	prepare_junction_marks();
	const JunctionMarkCache &view = junction_marks_;
	const TypedArray<Plane> planes = camera->get_frustum();
	const auto may_overlap = [&](const Bounds2 &box) {
		// The old UI tested screen and behind, without camera-far clipping. Only side planes prune here; the exact
		// original Camera3D/Rect2 predicates remain below. Extruding by all mark heights cannot exclude a candidate.
		for (int p = FIRST_SIDE_PLANE; p < planes.size(); p++) {
			const Plane plane = planes[p];
			const Vector3 &n = plane.normal;
			const double x = n.x >= 0 ? box.minx : box.maxx, y = n.y >= 0 ? view.low : view.high, z = n.z >= 0 ? box.minz : box.maxz;
			const double distance = double(n.x) * x + double(n.y) * y + double(n.z) * z - plane.d;
			const double scale = 1 + std::abs(double(n.x) * x) + std::abs(double(n.y) * y) + std::abs(double(n.z) * z) + std::abs(double(plane.d));
			if (distance > scale * std::numeric_limits<float>::epsilon() * PLANE_ROUNDING_ULPS) return false;
		}
		return true;
	};
	std::vector<int> candidates;
	view.bounds.query_if(may_overlap, [&](int id) { candidates.push_back(id); });
	std::sort(candidates.begin(), candidates.end()); // original node order and overlapping-mark draw precedence
	for (int id : candidates) {
		const auto &mark = view.marks[size_t(id)];
		if (camera->is_position_behind(mark.at)) continue;
		const Vector2 at = camera->unproject_position(mark.at);
		if (!screen.has_point(at)) continue;
		Dictionary one = mark_dict(mark);
		one["screen"] = at;
		out.push_back(one);
	}
	return out;
}

} // namespace city
