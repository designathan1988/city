// Conservative spatial rejection only: action/spawn callers retain every original final float predicate and tie.
#include "sim/traffic/actor_index.h"
#include "sim/state/city_state.h"

#include <algorithm>

namespace city {
namespace {
constexpr double ROUNDING_ULPS = 32;
Bounds2 body_bounds(float x, float z, double halfx, double halfz) {
	const double error = ROUNDING_ULPS * std::numeric_limits<float>::epsilon() *
		(1 + std::abs(double(x)) + std::abs(double(z)) + halfx + halfz);
	return {double(x) - halfx - error, double(z) - halfz - error, double(x) + halfx + error, double(z) + halfz + error};
}
}
void ActorIndex::put(int id, const Bounds2 &box) {
	if (size_t(id) >= entries_.size()) entries_.resize(size_t(id) + 1);
	Entry &entry = entries_[size_t(id)];
	if (entry.live && entry.box.minx == box.minx && entry.box.minz == box.minz && entry.box.maxx == box.maxx && entry.box.maxz == box.maxz) return;
	if (entry.live) index_.erase(id, entry.box);
	entry = Entry{box, true};
	index_.insert(id, box);
}
void ActorIndex::erase(int id) {
	if (id < 0 || size_t(id) >= entries_.size() || !entries_[size_t(id)].live) return;
	Entry &entry = entries_[size_t(id)];
	index_.erase(id, entry.box); entry.live = false;
}
Bounds2 ActorIndex::neighborhood(float x, float z, float radius) {
	const double r = std::abs(double(radius));
	const double error = ROUNDING_ULPS * std::numeric_limits<float>::epsilon() *
		(1 + std::abs(double(x)) + std::abs(double(z)) + r);
	return {double(x) - r - error, double(z) - r - error, double(x) + r + error, double(z) + r + error};
}
std::vector<int> ActorIndex::near(float x, float z, float radius) const {
	std::vector<int> result;
	index_.query(neighborhood(x, z, radius), [&](int id) { result.push_back(id); });
	std::sort(result.begin(), result.end());
	return result;
}
std::vector<int> ActorIndex::ray(float ox, float oz, float dx, float dz, float limit) const {
	std::vector<int> result;
	index_.query_if([&](const Bounds2 &b) {
		const double error = ROUNDING_ULPS * std::numeric_limits<float>::epsilon() *
			(1 + std::abs(double(ox)) + std::abs(double(oz)) + std::abs(double(limit)) +
			 std::abs(b.minx) + std::abs(b.maxx) + std::abs(b.minz) + std::abs(b.maxz)) +
			2 * double(SHOT_PARALLEL_EPS) * std::abs(double(limit));
		double first = 0, last = limit;
		const auto axis = [&](double origin, double direction, double lo, double hi) {
			lo -= error; hi += error;
			if (direction == 0) return origin >= lo && origin <= hi;
			double a = (lo - origin) / direction, c = (hi - origin) / direction;
			if (a > c) std::swap(a, c);
			first = std::max(first, a); last = std::min(last, c);
			return first <= last;
		};
		return axis(ox, dx, b.minx, b.maxx) && axis(oz, dz, b.minz, b.maxz);
	}, [&](int id) { result.push_back(id); });
	std::sort(result.begin(), result.end());
	return result;
}
namespace traffic {
void index_walker(CityState &s, int id) {
	const Pedestrian &w = s.traffic.peds[size_t(id)];
	if (!w.alive) { s.traffic.walker_index.erase(id); return; }
	const BlastParams &p = s.catalog.blast;
	const float height = p.body_height_m * w.scale, standing = p.body_reach_m * w.scale;
	const double halfx = std::max(double(standing), std::abs(double(w.cur.fx)) * (height * 0.5f) + LYING_BODY_RADIUS_M);
	const double halfz = std::max(double(standing), std::abs(double(w.cur.fz)) * (height * 0.5f) + LYING_BODY_RADIUS_M);
	s.traffic.walker_index.put(id, body_bounds(w.cur.x, w.cur.z, halfx, halfz));
}
void index_vehicle(CityState &s, int id) {
	const Vehicle &v = s.traffic.vehicles[size_t(id)];
	if (!v.alive) { s.traffic.vehicle_index.erase(id); return; }
	const VehicleClass &cl = s.catalog.traffic.classes[int(v.type)];
	const double fx = v.cur.fx, fz = v.cur.fz, norm2 = fx * fx + fz * fz;
	if (norm2 == 0) {
		const double inf = std::numeric_limits<double>::infinity();
		s.traffic.vehicle_index.put(id, {-inf, -inf, inf, inf}); return;
	}
	const float half_l = cl.length * 0.5f, half_w = std::max(cl.half_width, two_wheeler(v.type) ? RIDER_HALF_WIDTH_M : 0.f);
	// Invert the actual heading frame used by shooting, retaining correctness through float normalization error.
	const double halfx = (std::abs(fx) * half_l + std::abs(fz) * half_w) / norm2;
	const double halfz = (std::abs(fz) * half_l + std::abs(fx) * half_w) / norm2;
	s.traffic.vehicle_index.put(id, body_bounds(v.cur.x, v.cur.z, halfx, halfz));
}
}
} // namespace city
