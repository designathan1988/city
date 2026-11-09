// Public transport commands (transit_commands.h). The plan is small: each edit works on a copy and hands it to the journal.
#include "sim/commands/transit_commands.h"

#include "sim/commands/edit_journal.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"
#include "sim/transit/transit_ground.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr float SAME_POINT_M = 1.f; // two clicks closer than this are one point
constexpr float ROAD_STEP_M = 1.f;
constexpr float JOIN_TIMES = 2.f; // of the joining reach
constexpr float STATION_PICK = 1.5f; // of the pick reach
constexpr int MIN_STOPS = 2;
constexpr float BUILDING_PAD_M = 1.f, FREE_STEP_M = 1.f; // a way down keeps this off a building; the footway walked this often

EditResult done() {
	EditResult r;
	r.ok = true;
	return r;
}

EditResult refused(const char *why) {
	EditResult r;
	r.error = why;
	return r;
}

// The point of the nearest footway within `reach`, on no building when `clear`; seg -1 when none.
struct Footway {
	int seg = -1;
	V2 at;
};

Footway footway_at(const CityState &s, const V2 &p, float reach, bool clear) {
	Footway best;
	float dist = reach;
	s.roads.for_segments_near(p.x - reach, p.y - reach, p.x + reach, p.y + reach, [&](int seg) {
		const RoadSegment &g = s.roads.segs[size_t(seg)];
		const Profile &pf = s.roads.profile_of(seg);
		double t, d;
		closest_t(g.q, p.x, p.y, t, d);
		const float arc = float(t) * float(g.len);
		auto at_arc = [&](int side, float along) {
			Pose q;
			traffic::sidewalk_pose(s, seg, side, along, 0, q);
			return V2(q.x, q.z);
		};
		for (int side : { 0, 1 }) {
			if (!(side == 0 ? pf.has_walk_right : pf.has_walk_left)) continue;
			V2 at = at_arc(side, arc);
			float away = (at - p).len();
			if (away >= dist) continue;
			if (clear && transit_built(s, at, BUILDING_PAD_M)) { // the nearest of this footway that is free, a step apart along it
				bool found = false;
				for (float along = 0, end = float(g.len); along <= end; along += FREE_STEP_M) {
					const V2 q = at_arc(side, along);
					const float d2 = (q - p).len();
					if (d2 < dist && (!found || d2 < away) && !transit_built(s, q, BUILDING_PAD_M)) {
						found = true;
						at = q;
						away = d2;
					}
				}
				if (!found) continue;
			}
			dist = away;
			best = { seg, at };
		}
	});
	return best;
}
} // namespace

EditResult transit_place_stop(CityState &s, float x, float z) {
	const TrafficParams &p = s.catalog.traffic;
	const Footway at = footway_at(s, V2(x, z), p.transit_stop_reach_m, false);
	if (at.seg < 0) return refused("hint.transit.noStreet");
	if (stop_near(s.transit, at.at, p.transit_pick_m, TransitMode::BUS, false)) return refused("hint.transit.taken");
	TransitData next = s.transit;
	TransitStop stop;
	stop.at = at.at;
	stop.seg = at.seg;
	transit_add_stop(next, stop);
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_toggle_terminal(CityState &s, float x, float z) {
	const TransitStop *stop = stop_near(s.transit, V2(x, z), s.catalog.traffic.transit_pick_m, TransitMode::BUS, false);
	if (!stop) return refused("hint.transit.noStop");
	TransitData next = s.transit;
	transit_set_terminal(next, stop->id, !stop->terminal);
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_place_station(CityState &s, float x, float z, TransitMode mode) {
	const TrafficParams &p = s.catalog.traffic;
	const TrackHit on = nearest_track(s.transit, V2(x, z), p.transit_pick_m * STATION_PICK, mode);
	if (on.track < 0) return refused("hint.transit.noTrack");
	if (stop_near(s.transit, on.at, p.transit_pick_m, mode, false)) return refused("hint.transit.taken");
	TransitData next = s.transit;
	TransitStop stop;
	stop.mode = mode;
	stop.at = on.at;
	stop.track = on.track;
	if (mode == TransitMode::METRO) { // its way down opens on the footway nearest the station
		const Footway way = footway_at(s, on.at, p.transit_entrance_reach_m, true);
		stop.has_entrance = way.seg >= 0;
		stop.entrance = way.at;
	}
	transit_add_stop(next, stop);
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_lay_track(CityState &s, const std::vector<V2> &points, TransitMode mode) {
	std::vector<V2> run;
	for (const V2 &q : points)
		if (run.empty() || (q - run.back()).len() > SAME_POINT_M) run.push_back(q);
	if (run.size() < 2) return refused("hint.transit.trackShort");
	const TrafficParams &p = s.catalog.traffic;
	// a train track crosses streets, it does not run down one; the metro is under them
	if (mode == TransitMode::TRAIN && longest_on_road(run, ROAD_STEP_M, [&](const V2 &q) { return transit_on_road(s, q); }) > p.transit_along_road_m) return refused("hint.transit.trackOnRoad");
	TransitData next = s.transit;
	transit_add_track(next, mode, run);
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_make_line(CityState &s, const std::vector<int> &stops) {
	if (int(stops.size()) < MIN_STOPS) return refused("hint.transit.lineShort");
	const TransitStop *first = s.transit.stop(stops[0]);
	if (!first) return refused("hint.transit.noStop");
	for (int id : stops) {
		const TransitStop *q = s.transit.stop(id);
		if (!q || q->mode != first->mode) return refused("hint.transit.noStop");
	}
	const TrafficParams &p = s.catalog.traffic;
	TransitData next = s.transit;
	int same = 1;
	for (const TransitLine &l : next.lines) same += l.mode == first->mode ? 1 : 0;
	const char *mark = first->mode == TransitMode::METRO ? "M" : (first->mode == TransitMode::TRAIN ? "T" : "");
	transit_add_line(next, first->mode, stops, p.line_colours, int(first->mode == TransitMode::BUS ? p.transit_bus_vehicles : p.transit_rail_vehicles), mark + std::to_string(same));
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_remove_at(CityState &s, float x, float z) {
	const float pick = s.catalog.traffic.transit_pick_m;
	const V2 p(x, z);
	TransitData next = s.transit;
	if (const TransitStop *stop = stop_near(s.transit, p, pick, TransitMode::BUS, true)) transit_remove_stop(next, stop->id);
	else {
		const TrackHit track = nearest_track(s.transit, p, pick, TransitMode::BUS);
		if (track.track < 0) return refused("hint.transit.noStop");
		transit_remove_track(next, track.track);
	}
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_remove_line(CityState &s, int id) {
	if (!s.transit.line(id)) return refused("hint.transit.noStop");
	TransitData next = s.transit;
	transit_remove_line(next, id);
	j_transit(s, std::move(next));
	return done();
}

EditResult transit_set_line_vehicles(CityState &s, int id, int vehicles) {
	if (!s.transit.line(id)) return refused("hint.transit.noStop");
	TransitData next = s.transit;
	transit_set_vehicles(next, id, vehicles, int(s.catalog.traffic.transit_most_vehicles));
	j_transit(s, std::move(next));
	return done();
}

bool transit_find_footway(const CityState &s, const V2 &p, float reach, V2 &at, int &seg) {
	const Footway way = footway_at(s, p, reach, false);
	at = way.at;
	seg = way.seg;
	return way.seg >= 0;
}

V2 transit_snap_track(const CityState &s, const V2 &p, TransitMode mode) {
	const float reach = s.catalog.traffic.transit_join_m * JOIN_TIMES;
	V2 point;
	if (track_point_near(s.transit, p, reach, mode, point)) return point;
	const TrackHit on = nearest_track(s.transit, p, reach, mode);
	return on.track >= 0 ? on.at : p;
}

} // namespace city
