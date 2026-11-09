// The traffic's and the walkers' sight rule (sight.h).
#include "sim/traffic/sight.h"

#include "sim/traffic/traffic_system.h"

namespace city {
namespace traffic {

namespace {

// Within `r` of the camera's eye on the ground: the flat distance the render's agent levels use.
bool within(const CityState &s, float x, float z, float r) {
	const float dx = x - s.camera_x.load(), dz = z - s.camera_z.load();
	return dx * dx + dz * dz <= r * r;
}

bool fresh(const CityState &s) { return s.traffic.time < double(s.catalog.traffic.spawn_fresh_s); }
} // namespace

bool vehicle_may_show(const CityState &s, float x, float z) { return within(s, x, z, s.catalog.traffic.publish_radius_m); }

bool person_may_show(const CityState &s, float x, float z) { return within(s, x, z, s.catalog.traffic.publish_people_m); }

bool unseen_start(const CityState &s, int ds, int lane, float at) {
	if (fresh(s)) return true;
	Pose here;
	lane_pose(s, ds, lane, at, here);
	return !vehicle_may_show(s, here.x, here.z);
}

bool unseen_birth(const CityState &s, float x, float z) { return fresh(s) || !person_may_show(s, x, z); }

bool drives_on(const CityState &s, const Vehicle &v) {
	return v.micro && v.bus < 0 && v.car < 0 && v.lorry < 0 && v.kerb < 0 && v.stop_at < 0 && vehicle_may_show(s, v.cur.x, v.cur.z);
}

void trip_ends_here(CityState &s, int id) {
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	if (v.bus < 0 && vehicle_may_show(s, v.cur.x, v.cur.z)) v.ended = true;
	else despawn_vehicle(s, id, true);
}

} // namespace traffic
} // namespace city
