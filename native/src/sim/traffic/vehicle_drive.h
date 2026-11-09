// How hard a driver presses the pedals this step (the lab's sim/drive/operational.ts and physicalMotion.ts): every obstacle the
// vehicle sees is gathered in a Lead, then each is driven against by the adaptive cruise control model, the pedals move at the
// driver's jerk, and the speed never passes the one it can still stop short of each obstacle from. vehicle_motion.cpp gathers.
#pragma once

#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

// What a vehicle drives against this step: the nearest (read by the junction's own rules) and every one of them, the nearest
// LEAD_MOST kept when more are seen (the lab's ConstraintSet, kept in place: nothing is made per vehicle and step).
constexpr int LEAD_MOST = 8; // a lane head sees a leader, a closure, a kerb stop, a transit hold, a light, a stop line and beyond it
struct Lead {
	struct Seen {
		float gap = NO_LEADER, v = 0, acc = 0;
	};
	float gap = NO_LEADER, v = 0;
	Seen seen[LEAD_MOST];
	int count = 0;
	void take(float g, float lv, float la = 0) {
		if (g >= NO_LEADER) return;
		if (g < gap) { gap = g; v = lv; }
		int at = count;
		if (count == LEAD_MOST) {
			at = int(std::max_element(seen, seen + count, [](const Seen &a, const Seen &b) { return a.gap < b.gap; }) - seen);
			if (seen[at].gap <= g) return;
		} else {
			count++;
		}
		seen[at] = { g, lv, la };
	}
};

// The acceleration of this step for `v` wishing for `v0` against everything in `l`.
float drive_accel(const CityState &s, const Vehicle &v, float v0, const Lead &l, float dt);

} // namespace traffic
} // namespace city
