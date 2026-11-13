// `probes` area (`driving_census`): how the vehicles within a radius drive now, for the traffic's test
// (project/tests/core/driving_test.gd): each one on a curved connector against the speed its own driver takes that bend at; the
// speeds an ordinary driver takes the map's turns at, the slowest and the fastest, and the streets that curve; the drivers'
// tempers; who shows an indicator or means to change lane, and whether each one on a turn shows that turn's side (the nearest
// of those, for the photo plan that looks at a lit indicator: project/tests/visual/signal_plan.json).
#include "api/city_probes.h"

#include "sim/state/city_state.h"
#include "sim/traffic/motion_law.h"

#include <algorithm>
#include <cmath>
#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float SLIDING_M = 0.15f; // a body still sliding to its lane shows that change's side, not its turn's (vehicle_pose.cpp)
} // namespace

Dictionary CityProbes::driving_census(const Vector3 &at, float radius) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	int seen = 0, on_bend = 0, signalling = 0, wanting = 0, turning = 0, turning_shown = 0, on_wide = 0, sliding = 0;
	float over = 0, lowest = 1, highest = -1, sum = 0;
	const Vehicle *shown = nullptr; // of those showing their turn's side, the nearest to `at` (a photo plan stands on it)
	float shown_away = 0;
	int shown_side = SIGNAL_NONE;
	for (const Vehicle &v : t.vehicles) {
		const float away = std::hypot(v.cur.x - at.x, v.cur.z - at.z);
		if (!v.alive || !v.micro || v.off || away > radius) continue;
		seen++;
		lowest = std::min(lowest, v.temper);
		highest = std::max(highest, v.temper);
		sum += v.temper;
		const int side = traffic::signal_side(s, v);
		signalling += side != SIGNAL_NONE ? 1 : 0;
		wanting += v.lc_want >= 0 ? 1 : 0;
		on_wide += v.ds >= 0 && t.g.ds[size_t(v.ds)].lanes > 1 ? 1 : 0;
		sliding += std::fabs(v.lc_off) > SLIDING_M ? 1 : 0;
		if (v.conn < 0) continue;
		const Conn &c = t.g.conns[size_t(v.conn)];
		if (const float most = traffic::bend_pace(s, v, c.bend); most < NO_LEADER) {
			on_bend++;
			over = std::max(over, v.v - most);
		}
		if ((c.turn == Turn::LEFT || c.turn == Turn::RIGHT) && std::fabs(v.lc_off) <= SLIDING_M) {
			turning++;
			const bool shows = side == (c.turn == Turn::LEFT ? SIGNAL_LEFT : SIGNAL_RIGHT);
			turning_shown += shows ? 1 : 0;
			if (shows && (shown == nullptr || away < shown_away)) {
				shown = &v;
				shown_away = away;
				shown_side = side;
			}
		}
	}
	// the map's turns as an ordinary driver of a car takes them, and its streets that curve
	const Vehicle ordinary;
	float slowest = NO_LEADER, fastest = 0;
	int bends = 0, curved = 0, wide = 0;
	for (const Conn &c : t.g.conns) {
		if (!c.alive) continue;
		const float most = traffic::bend_pace(s, ordinary, c.bend);
		if (most >= NO_LEADER) continue;
		bends++;
		slowest = std::min(slowest, most);
		fastest = std::max(fastest, most);
	}
	for (const DirSeg &d : t.g.ds) {
		curved += d.ok && d.bend != 0 ? 1 : 0;
		wide += d.ok && d.lanes > 1 ? 1 : 0;
	}
	Dictionary d;
	d["vehicles"] = seen;
	d["temper_lowest"] = lowest;
	d["temper_highest"] = highest;
	d["temper_mean"] = seen > 0 ? sum / float(seen) : 0.f;
	d["on_bends"] = on_bend;
	d["over_bend_ms"] = over;
	d["turn_bends"] = bends;
	d["turn_slowest_ms"] = bends > 0 ? slowest : 0.f;
	d["turn_fastest_ms"] = fastest;
	d["curved_streets"] = curved;
	d["wide_streets"] = wide; // directed segments with more than one lane: where a lane change can be
	d["signalling"] = signalling;
	d["wanting"] = wanting;
	d["lane_signals"] = t.lane_signals; // since the city opened: changes signalled, made after their signal, forced at a line
	d["lane_changes"] = t.lane_changes;
	d["lane_forced"] = t.lane_forced;
	d["on_wide_streets"] = on_wide; // vehicles on a street of more than one lane
	d["sliding"] = sliding; // bodies sliding to the lane they changed to
	d["turning"] = turning;
	d["turning_shown"] = turning_shown;
	d["shown"] = shown != nullptr; // one on a turn shows that turn's side: where it is, which side (1 left, 2 right), its heading
	d["shown_at"] = shown != nullptr ? Vector3(shown->cur.x, shown->cur.y, shown->cur.z) : Vector3();
	d["shown_side"] = shown_side;
	d["shown_facing"] = shown != nullptr ? Vector2(shown->cur.fx, shown->cur.fz) : Vector2();
	return d;
}

} // namespace city
