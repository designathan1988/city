// Route slices for vehicles and pedestrians (see RouteRef in traffic_state.h): one pool, reused capacity, no
// per-entity vector allocation; compacted when the waste passes half the pool.
#include "sim/traffic/traffic_state.h"

#include <cstring>

namespace city {

void route_set(std::vector<int> &pool, RouteRef &r, size_t &waste, const int *src, int n) {
	if (n <= 0) {
		if (r.off >= 0) waste += size_t(r.cap);
		r = RouteRef();
		return;
	}
	if (r.off >= 0 && n <= r.cap) {
		std::memcpy(pool.data() + r.off, src, size_t(n) * sizeof(int));
		r.len = n;
		return;
	}
	if (r.off >= 0) waste += size_t(r.cap);
	r.off = int(pool.size());
	r.cap = r.len = n;
	pool.insert(pool.end(), src, src + n);
}

void route_compact(TrafficState &t) {
	if (t.route_waste > t.route_pool.size() / 2) {
		std::vector<int> packed;
		packed.reserve(t.route_pool.size() - t.route_waste);
		for (Vehicle &v : t.vehicles) {
			if (!v.alive || v.route.len <= 0) continue;
			const int off = int(packed.size());
			packed.insert(packed.end(), t.route_pool.begin() + v.route.off, t.route_pool.begin() + v.route.off + v.route.len);
			v.route.off = off;
			v.route.cap = v.route.len;
		}
		t.route_pool.swap(packed);
		t.route_waste = 0;
	}
	if (t.ped_waste > t.ped_pool.size() / 2) {
		std::vector<int> packed;
		packed.reserve(t.ped_pool.size() - t.ped_waste);
		for (Pedestrian &w : t.peds) {
			if (!w.alive || w.route.len <= 0) continue;
			const int off = int(packed.size());
			packed.insert(packed.end(), t.ped_pool.begin() + w.route.off, t.ped_pool.begin() + w.route.off + w.route.len);
			w.route.off = off;
			w.route.cap = w.route.len;
		}
		t.ped_pool.swap(packed);
		t.ped_waste = 0;
	}
}

} // namespace city
