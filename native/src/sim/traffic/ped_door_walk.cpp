// The walkers' last and first steps: the door of a lot and the way between it and the sidewalk (lot_doors.h, ped_doors.cpp).
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

// On the last street of its route: a door ahead to go in at, drawn by who is going in at this hour.
void aim_at_door(CityState &s, Pedestrian &w) {
	const RoadSegment &g = s.roads.segs[size_t(w.seg)];
	const PeopleNow now = people_now(s.catalog.traffic, float(s.clock.hour()));
	pick_door(s, now, w.seg, w.side, false, w.dir == 0 ? w.s : arc_lo(g), w.dir == 0 ? arc_hi(g) : w.s, w.door);
}

// At its door's place on the sidewalk: across to the foot of the way in, then to the door.
void go_in(CityState &s, int id) {
	Pedestrian &w = s.traffic.peds[size_t(id)];
	Pose foot;
	sidewalk_pose(s, w.seg, w.side, w.door.s, w.door.u, foot);
	w.ways[0] = Waypoint{ foot.x, foot.y, foot.z, -1 };
	w.ways[1] = Waypoint{ w.door.x, w.door.y, w.door.z, -1 };
	w.wn = 2;
	w.wi = 0;
	w.way_node = -1;
	w.mode = PedMode::LOT;
	w.entering = true;
	aim_line(w);
}

// Its door reached. Its company, a step behind, goes in after it.
static void through_door(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	if (w.mate >= 0) {
		Pedestrian &m = t.peds[size_t(w.mate)];
		m.with = -1;
		m.on_way = false;
		m.mode = PedMode::LOT;
		m.entering = true;
		m.way_node = -1;
		m.ways[0] = w.ways[w.wn - 1];
		m.wn = 1;
		m.wi = 0;
		aim_line(m);
		w.mate = -1;
	}
	t.doors_in++;
	despawn(t, id, true);
}

// On the way between a door and the sidewalk: no crowd here, its own pace and nothing in the way (lot_doors.h).
void walk_lot(CityState &s, int id, float dt) {
	TrafficState &t = s.traffic;
	Pedestrian &w = t.peds[size_t(id)];
	const float step = w.speed * dt;
	w.walked += step;
	w.ls += step;
	if (w.company_due && w.ls >= 2 * s.catalog.traffic.ped_body_m + s.catalog.traffic.ped_gap_m) {
		w.company_due = false;
		add_company(s, id);
	}
	Pedestrian &now = t.peds[size_t(id)]; // the company may have grown the list
	if (now.ls < now.llen) return;
	if (++now.wi < now.wn) {
		aim_line(now);
		return;
	}
	if (now.entering) {
		through_door(s, id);
		return;
	}
	on_sidewalk(now);
	onto_sidewalk(s, now);
}

} // namespace traffic
} // namespace city
