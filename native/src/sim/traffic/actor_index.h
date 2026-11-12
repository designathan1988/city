// Mobile actor hit/occupancy bounds. Written under traffic_mutex, queried in original ascending actor-ID order.
#pragma once

#include "core/bounds_index.h"

#include <cmath>
#include <cstddef>
#include <limits>
#include <vector>

namespace city {
struct CityState;
constexpr float LYING_BODY_RADIUS_M = 0.3f, RIDER_HALF_WIDTH_M = 0.35f;
constexpr float SHOT_PARALLEL_EPS = 1e-6f;
class ActorIndex {
public:
	void put(int id, const Bounds2 &box);
	void erase(int id);
	void clear() { index_.clear(); std::vector<Entry>().swap(entries_); }
	std::vector<int> near(float x, float z, float radius) const;
	std::vector<int> ray(float ox, float oz, float dx, float dz, float limit) const;
	template <typename F> bool any_near(float x, float z, float radius, F &&accept) const {
		const Bounds2 box = neighborhood(x, z, radius);
		std::vector<BoundsIndex::OrderedVisit> heap;
		bool found = false;
		index_.query_ordered([&](const Bounds2 &b) { return b.overlaps(box); }, [&](int id) {
			found = accept(id); return !found;
		}, heap);
		return found;
	}
private:
	static Bounds2 neighborhood(float x, float z, float radius);
	struct Entry { Bounds2 box; bool live = false; };
	std::vector<Entry> entries_;
	BoundsIndex index_;
};
namespace traffic {
void index_walker(CityState &s, int id);
void index_vehicle(CityState &s, int id);
}
} // namespace city
