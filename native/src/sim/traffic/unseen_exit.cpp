// A car the camera's window shows never vanishes from it (unseen_exit.h): the window's reach, and the way a car with no trip
// drives on by until nobody sees it.
#include "sim/traffic/unseen_exit.h"

namespace city {
namespace traffic {

// the reach the trips' starts keep clear of (demand.cpp): what is drawn round the camera's point
bool window_shows(const CityState &s, const Vehicle &v) {
	const V2 eye(s.camera_x.load(), s.camera_z.load());
	return (V2(v.cur.x, v.cur.z) - eye).len() <= s.catalog.traffic.publish_radius_m;
}

bool drive_on(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	const int cls = body_class(s.catalog.traffic, v.type);
	int to = -1;
	for (int c = t.g.lane_conn[size_t(v.ds) * MAX_LANES + v.lane]; c >= 0 && to < 0; c = t.g.conns[size_t(c)].next_from) {
		const Conn &k = t.g.conns[size_t(c)];
		if (k.alive && k.max_class >= cls && k.turn != Turn::UTURN) to = k.to_ds;
	}
	if (to < 0) return false;
	const int way[2] = { v.ds, to };
	route_set(t.route_pool, v.route, t.route_waste, way, 2);
	v.ri = 0;
	v.goal_s = t.g.ds[size_t(to)].len;
	return true;
}

} // namespace traffic
} // namespace city
