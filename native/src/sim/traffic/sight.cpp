// The traffic's sight rule (sight.h).
#include "sim/traffic/sight.h"

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

namespace {
constexpr float HALF = 0.5f;

bool within_drawn(const CityState &s, float x, float z) {
	const float dx = x - s.camera_x.load(), dz = z - s.camera_z.load(), r = s.catalog.traffic.publish_radius_m;
	return dx * dx + dz * dz <= r * r;
}
} // namespace

bool unseen_start(const CityState &s, int ds) {
	if (s.traffic.time < double(s.catalog.traffic.spawn_fresh_s)) return true;
	Pose mid; // the segment's middle
	lane_pose(s, ds, 0, s.traffic.g.ds[size_t(ds)].len * HALF, mid);
	return !within_drawn(s, mid.x, mid.z);
}

bool drives_on(const CityState &s, const Vehicle &v) {
	return v.micro && v.bus < 0 && v.car < 0 && v.lorry < 0 && v.errand == Errand::NONE && v.kerb < 0 && v.stop_at < 0 && within_drawn(s, v.cur.x, v.cur.z);
}

} // namespace traffic
} // namespace city
