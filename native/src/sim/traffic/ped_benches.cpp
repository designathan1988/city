// Who sits on a bench along the way: a walker alone passing a bench of its own sidewalk with a seat free sits a while, now and then
// (traffic.json ped_sit_share of those passing, for ped_sit_min_s plus up to ped_sit_span_s), on one of its two seats
// ped_seat_apart_m apart, its feet ped_seat_forward_m in front of the bench and facing the way the bench faces; a fright gets it up,
// and it goes on along its way from the bench. The crowd sees it on the bench, where no walker goes.
#include "sim/traffic/traffic_system.h"

#include "core/random.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
constexpr uint32_t SIT_SALT = 0x51754eU, STAY_SALT = 0x5374u; // streams' salts: "QuN" and "St" in ASCII, apart from the walker's others
constexpr int SEATS = 2;
constexpr float ARC_KEY_PER_M = 10.f; // a bench's draws are keyed by its arc to a tenth of a metre

bool seat_taken(const TrafficState &t, int seg, float at, int seat) {
	return std::any_of(t.peds.begin(), t.peds.end(), [&](const Pedestrian &o) {
		return o.alive && t.time < o.sit_until && o.sat_seg == seg && o.sat_s == at && o.seat == seat;
	});
}
} // namespace

bool sit_down(CityState &s, int id, float from, float to) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	const TrafficParams &p = s.catalog.traffic;
	if (w.mode != PedMode::SIDEWALK || w.with >= 0 || w.mate >= 0 || w.age == PedAge::CHILD || w.hits > 0 || t.time < w.panic_until) return false;
	const float lo = std::min(from, to), hi = std::max(from, to);
	for (const StreetObstacle &o : sidewalk_furniture(s, w.seg).side[size_t(w.side)]) {
		if (!o.bench) continue;
		const float mid = (o.s0 + o.s1) * 0.5f;
		if (mid <= lo || mid > hi || (w.sat_seg == w.seg && w.sat_s == mid)) continue;
		const uint32_t key = hash_combine(w.look, uint32_t(int(mid * ARC_KEY_PER_M)));
		if (hash_float(key ^ SIT_SALT) >= p.ped_sit_share) continue;
		for (int seat = 0; seat < SEATS; seat++) {
			if (seat_taken(t, w.seg, mid, seat)) continue;
			const float along = (float(seat) - 0.5f) * p.ped_seat_apart_m; // the two seats either side of its middle
			w.seat_x = o.x + o.fx * along + o.face_x * p.ped_seat_forward_m;
			w.seat_z = o.z + o.fz * along + o.face_z * p.ped_seat_forward_m;
			w.seat_y = o.y;
			w.seat_fx = o.face_x;
			w.seat_fz = o.face_z;
			w.sat_seg = w.seg;
			w.sat_s = mid;
			w.seat = uint8_t(seat);
			w.s = mid;
			w.u = (o.u0 + o.u1) * 0.5f; // where the crowd sees it: on the bench
			w.sit_until = float(t.time) + p.ped_sit_min_s + p.ped_sit_span_s * hash_float(key ^ STAY_SALT);
			return true;
		}
	}
	return false;
}

void seated_pose(Pedestrian &w) {
	w.cur.x = w.seat_x;
	w.cur.y = w.seat_y;
	w.cur.z = w.seat_z;
	w.cur.fx = w.seat_fx;
	w.cur.fz = w.seat_fz;
	w.cur.rise = 0;
}

} // namespace traffic
} // namespace city
