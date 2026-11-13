// Where a drawn vehicle's body stands (publish_poses): its middle on its path half its length behind its front, measured back
// along the pieces it drove (its lane, the connector it came off, the lane before that one), its heading the chord of the path
// heading_chord_m either side of that middle, and the sideways offset of a lane change (a bicycle's band too) across that
// heading on lanes and connectors alike; and the indicator it shows (signal_side). The body hangs from its path as a vehicle from its wheels: it keeps its place coming
// out of a junction, it does not cut the inside of a curve, and a lane change ends where it began, never in one tick.
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr float LEAST_CHORD_M = 1e-3f; // the chord's ends closer than this: the path's own direction
constexpr float SLIDING_M = 0.15f; // a lane change's offset under this is over: its indicator goes out

// Whether the connector a vehicle came off still leads into the lane it is on.
bool came_in(const TrafficState &t, const Vehicle &v) {
	if (v.came < 0 || size_t(v.came) >= t.g.conns.size()) return false;
	const Conn &c = t.g.conns[size_t(v.came)];
	return c.alive && c.to_ds == v.ds;
}

// The point of its path `back` metres behind its front, where the body stands across it: the body's offset `off` from the line
// of the lane or connector it is on now (left of its travel), carried onto the pieces behind as those lines lie from it.
void path_point(const CityState &s, const Vehicle &v, float back, float off, Pose &o) {
	const TrafficState &t = s.traffic;
	float across = off;
	if (v.conn >= 0) {
		const Conn &c = t.g.conns[size_t(v.conn)];
		const float at = v.cs - back;
		if (at >= 0) conn_pose(c, at, o);
		else lane_pose(s, c.from_ds, c.from_lane, t.g.ds[size_t(c.from_ds)].len + at, o);
	} else {
		float at = v.s - back;
		if (at >= 0 || !came_in(t, v)) {
			lane_pose(s, v.ds, v.lane, at, o); // a vehicle put on a lane's mouth (out of the window's sight) has nothing behind it
		} else {
			// behind its lane, the connector it came off ends on the line of the lane it came onto, which a lane change since
			// may have left: the body stands that much across from that line too (u is left of a -> b, `off` left of its travel)
			const Conn &c = t.g.conns[size_t(v.came)];
			const DirSeg &d = t.g.ds[size_t(v.ds)];
			across += (d.u[v.lane] - d.u[c.to_lane]) * d.sign;
			at += c.len;
			if (at >= 0) conn_pose(c, at, o);
			else lane_pose(s, c.from_ds, c.from_lane, t.g.ds[size_t(c.from_ds)].len + at, o);
		}
	}
	o.x += o.fz * across; // left of its travel: (fz, -fx)
	o.z -= o.fx * across;
}
} // namespace

int signal_side(const CityState &s, const Vehicle &v) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const auto of_turn = [](Turn turn) { return turn == Turn::LEFT || turn == Turn::UTURN ? SIGNAL_LEFT : turn == Turn::RIGHT ? SIGNAL_RIGHT : SIGNAL_NONE; };
	if (v.off) return SIGNAL_NONE;
	// sliding to its new lane: the body stands left of that lane's line while it moves right
	if (std::fabs(v.lc_off) > SLIDING_M) return v.lc_off > 0 ? SIGNAL_RIGHT : SIGNAL_LEFT;
	if (v.conn >= 0) return of_turn(t.g.conns[size_t(v.conn)].turn);
	if (v.ds < 0) return SIGNAL_NONE;
	if (v.lc_want >= 0 && v.lc_want != v.lane) return v.lc_want > v.lane ? SIGNAL_LEFT : SIGNAL_RIGHT; // lane 0 is the rightmost
	if (v.ri + 1 >= v.route.len || t.g.ds[size_t(v.ds)].len - v.s > std::max(p.signal_ahead_m, v.v * p.signal_ahead_s)) return SIGNAL_NONE;
	const int ci = pick_connector(s, v);
	return ci < 0 ? SIGNAL_NONE : of_turn(t.g.conns[size_t(ci)].turn);
}

void vehicle_pose(const CityState &s, const Vehicle &v, Pose &out) {
	const TrafficParams &p = s.catalog.traffic;
	const float half = p.classes[int(v.type)].length * 0.5f, chord = std::min(p.heading_chord_m, half);
	// the lane change's offset is left of its travel; a bicycle's band is across the segment (a -> b), as the lane's u
	float off = v.lc_off;
	if (v.conn < 0 && v.ds >= 0) off += s.traffic.g.ds[size_t(v.ds)].sign * cycle_shift(s, v, v.ds, v.s);
	Pose mid, ahead, behind;
	path_point(s, v, half, off, mid);
	path_point(s, v, half - chord, off, ahead);
	path_point(s, v, half + chord, off, behind);
	const V2 along(ahead.x - behind.x, ahead.z - behind.z);
	const float span = along.len();
	const V2 h = span > LEAST_CHORD_M ? along * (1.f / span) : V2(mid.fx, mid.fz);
	out.x = mid.x;
	out.z = mid.z;
	out.y = mid.y + p.body_lift_m;
	out.fx = h.x;
	out.fz = h.y;
	out.rise = span > LEAST_CHORD_M ? (ahead.y - behind.y) / span : 0.f;
}

} // namespace traffic
} // namespace city
