// `probes` area: a scene by construction at a crossing between junctions, for the tests that ask what somebody on foot and a car
// do there (street_crossing_test, people_test): a walker stood on the footway before the crossing whose trip ends on the other
// sidewalk past it (scene_walker), a car stood on the lane into the crossing that runs beside the kerb that walker is crossing to
// (scene_car), and what the two are doing (scene_watch). Both are made by the makers of sim/traffic with a trip given, where the
// town's are drawn: nothing here waits for a draw.
#include "api/city_probes.h"

#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <cmath>
#include <mutex>
#include <shared_mutex>
#include <vector>

using namespace godot;

namespace city {

namespace {

constexpr float WALK_UP_M = 6.f; // the walker starts this far along its sidewalk before the crossing's end of it
constexpr float FAR_M = 1e9f;
constexpr int NOBODY = -1;

// The crossing between junctions nearest `at`, or -1.
int nearest_mid_block(const CityState &s, const Vector3 &at) {
	int best = -1;
	float nearest = FAR_M;
	for (int n = 0; n < int(s.roads.nodes.size()); n++) {
		if (!s.roads.mid_block(n)) continue;
		const RoadNode &node = s.roads.nodes[size_t(n)];
		const float d = std::hypot(float(node.x) - at.x, float(node.z) - at.z);
		if (d < nearest) {
			nearest = d;
			best = n;
		}
	}
	return best;
}

} // namespace

Dictionary CityProbes::scene_walker(const Vector3 &at) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	CityState &s = *state_;
	Dictionary d;
	d["id"] = NOBODY;
	const int n = nearest_mid_block(s, at);
	if (n < 0) return d;
	const std::vector<int> &arms = s.roads.nodes[size_t(n)].segs;
	Waypoint ways[MAX_WAYS];
	for (int in : arms)
		for (int out : arms) {
			if (in == out || !traffic::walkable(s, in) || !traffic::walkable(s, out)) continue;
			const RoadSegment &g = s.roads.segs[size_t(in)], &o = s.roads.segs[size_t(out)];
			const Profile &past = s.roads.profile_of(out);
			for (int side = 0; side < 2; side++) {
				// the sidewalk past the node it would keep to with no wish (the fewest crossings): its trip ends on the other one
				int stays = 0;
				if (traffic::plan_corner(s, n, in, side, out, -1, ways, stays) < 0) continue;
				const int across = 1 - stays;
				if (!(across == 0 ? past.has_walk_right : past.has_walk_left)) continue;
				const float arc = g.b == n ? traffic::arc_hi(g) - WALK_UP_M : traffic::arc_lo(g) + WALK_UP_M;
				const int id = traffic::walker_with_trip(s, in, side, arc, { n, o.a == n ? o.b : o.a }, across);
				if (id < 0) continue;
				d["id"] = id;
				d["look"] = int64_t(s.traffic.peds[size_t(id)].look); // which walker this id is: a slot is used again once its walker is gone
				d["goal_seg"] = out;
				d["goal_side"] = across;
				return d;
			}
		}
	return d;
}

int CityProbes::scene_car(int walker, float metres, float speed) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	CityState &s = *state_;
	const TrafficState &t = s.traffic;
	if (walker < 0 || size_t(walker) >= t.peds.size()) return NOBODY;
	const Pedestrian &w = t.peds[size_t(walker)];
	if (!w.alive || w.cross_seg < 0 || w.way_node < 0) return NOBODY; // on a crosswalk: the kerb it is going to is known
	const int n = w.way_node;
	const Waypoint &kerb = w.ways[w.wi];
	const std::vector<int> &arms = s.roads.nodes[size_t(n)].segs;
	int from = -1, lane_from = 0, onto = -1;
	float beside = FAR_M;
	for (int in : arms)
		for (int out : arms) {
			if (in == out) continue;
			const int ds = traffic::in_ds(s.roads, in, n), next = traffic::out_ds(s.roads, out, n);
			if (size_t(ds) >= t.g.ds.size() || size_t(next) >= t.g.ds.size() || !t.g.ds[size_t(ds)].ok || !t.g.ds[size_t(next)].ok) continue;
			const DirSeg &d = t.g.ds[size_t(ds)];
			for (int lane = 0; lane < d.lanes; lane++) {
				if (!traffic::lane_reaches(s, ds, lane, next)) continue;
				Pose p;
				traffic::lane_pose(s, ds, lane, d.len - d.stop_back, p);
				const float across = std::fabs((kerb.x - p.x) * -p.fz + (kerb.z - p.z) * p.fx); // of that kerb from the lane's line
				if (across < beside) {
					beside = across;
					from = ds;
					lane_from = lane;
					onto = next;
				}
			}
		}
	if (from < 0) return NOBODY;
	const DirSeg &d = t.g.ds[size_t(from)];
	return traffic::vehicle_on_lane(s, VehicleType::SEDAN, from, lane_from, d.len - d.stop_back - metres, speed, { from, onto });
}

Dictionary CityProbes::scene_watch(int walker, int64_t look, int car) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	const Pedestrian *w = walker >= 0 && size_t(walker) < t.peds.size() ? &t.peds[size_t(walker)] : nullptr;
	if (w && (!w->alive || int64_t(w->look) != look)) w = nullptr;
	const Vehicle *v = car >= 0 && size_t(car) < t.vehicles.size() && t.vehicles[size_t(car)].alive ? &t.vehicles[size_t(car)] : nullptr;
	Dictionary d;
	d["walker"] = w != nullptr;
	if (w) {
		d["mode"] = int(w->mode);
		d["crossing"] = w->cross_seg >= 0;
		d["seg"] = w->seg;
		d["side"] = w->side;
	}
	d["car"] = v != nullptr;
	if (v) {
		const bool on_lane = v->conn < 0 && v->ds >= 0;
		d["speed"] = v->v;
		d["ds"] = on_lane ? v->ds : -1; // -1: over its line, on its way through the crossing
		d["line_m"] = on_lane ? t.g.ds[size_t(v->ds)].len - v->s - t.g.ds[size_t(v->ds)].stop_back : 0.f;
		d["hold"] = int(on_lane ? traffic::hold_reason(s, car) : Hold::ON_CONNECTOR);
	}
	if (w && v) {
		Pose p;
		traffic::vehicle_pose(s, *v, p);
		d["apart_m"] = std::hypot(p.x - w->cur.x, p.z - w->cur.z); // the car's middle from the walker's
	}
	return d;
}

} // namespace city
