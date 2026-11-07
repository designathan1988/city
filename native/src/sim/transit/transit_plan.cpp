// What the running transit reads from its plan and from the edits of the roads (transit_plan.h): the plan's stops and lines by id, the
// lane beside each bus stop, each line's buses placed on it and the loops; after an edit of the roads alone, only what it reached.
#include "sim/transit/transit_plan.h"

#include "sim/state/city_state.h"
#include "sim/transit/rail_run.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace transit {

namespace {
constexpr int STOP_SIDE_RIGHT = 0;

// The kerb lane beside a stop: the direction of its road with the stop's footway on its right.
bool lane_beside(const CityState &s, const TransitStop &q, StopLane &out) {
	if (q.seg < 0 || size_t(q.seg) >= s.roads.segs.size() || !s.roads.segs[size_t(q.seg)].alive) return false;
	const RoadSegment &g = s.roads.segs[size_t(q.seg)];
	double t, dist, cx, cz, dx, dz;
	closest_t(g.q, q.at.x, q.at.y, t, dist);
	g.q.at(t, cx, cz);
	g.q.deriv(t, dx, dz);
	const int side = dz * (q.at.x - cx) - dx * (q.at.y - cz) > 0 ? 1 : STOP_SIDE_RIGHT;
	const int ds = q.seg * 2 + side;
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	if (!d.ok) return false;
	out = { ds, 0, std::clamp((float(t) * float(g.len) - d.at0) * d.sign, 0.f, d.len) };
	return true;
}

// A line's buses: those it had stay where they are, more wait at its terminal, up to its number.
void place_buses(CityState &s, const TransitLine &l, int index, std::vector<Bus> &was, std::vector<Bus> &out) {
	const TrafficParams &p = s.catalog.traffic;
	int served = 0, start = 0;
	for (size_t i = 0; i < l.stops.size(); i++) {
		served += s.transit_run.stop_lanes.count(l.stops[i]) ? 1 : 0;
		const TransitStop *q = stop_of(s, l.stops[i]);
		if (q && q->terminal && start == 0) start = int(i);
	}
	if (served < 2) return;
	int k = 0;
	for (Bus &b : was) {
		if (b.line != l.id || k >= l.vehicles) continue;
		if (b.next >= int(l.stops.size())) {
			b.next = start;
			b.dir = 1;
		}
		b.line_index = index;
		out.push_back(b);
		b.line = -1; // kept
		k++;
	}
	for (; k < l.vehicles; k++) {
		Bus b;
		b.line = l.id;
		b.line_index = index;
		b.next = start;
		b.wait = float(k) * p.bus_spacing_s;
		out.push_back(b);
	}
}

// The buses placed again on their lines, those of lines with `send` set (by place in TransitData::lines; all without it) sent on
// to their next stops; buses whose line went, or has fewer, leave the road.
void place_all_buses(CityState &s, const std::vector<uint8_t> *send) {
	TransitRun &run = s.transit_run;
	std::vector<Bus> was = std::move(run.buses);
	run.buses.clear();
	for (size_t i = 0; i < s.transit.lines.size(); i++)
		if (s.transit.lines[i].mode == TransitMode::BUS) place_buses(s, s.transit.lines[i], int(i), was, run.buses);
	for (const Bus &b : was)
		if (b.line >= 0 && b.vehicle >= 0) { // its line went, or has fewer
			s.traffic.vehicles[size_t(b.vehicle)].bus = -1;
			traffic::despawn_vehicle(s, b.vehicle, true);
		}
	for (size_t i = 0; i < run.buses.size(); i++) {
		Bus &b = run.buses[i];
		if (b.vehicle < 0) continue;
		Vehicle &v = s.traffic.vehicles[size_t(b.vehicle)];
		v.bus = int(i);
		if (send && !(*send)[size_t(b.line_index)]) continue;
		if (const TransitLine *l = line_of(s, b)) send_bus(s, *l, b, v);
	}
}

// An edit of the plan: every stop's lane found again, the lines' buses going on from where they are, the loops made again.
void rebuild(CityState &s) {
	TransitRun &run = s.transit_run;
	s.traffic.roads_touched.clear(); // every stop and crossing is read below
	run.stop_index.clear();
	run.stops_on_seg.clear();
	run.lines_at_stop.clear();
	for (size_t i = 0; i < s.transit.stops.size(); i++) {
		const TransitStop &q = s.transit.stops[i];
		run.stop_index.emplace(q.id, int(i));
		if (q.mode == TransitMode::BUS && q.seg >= 0) run.stops_on_seg[q.seg].push_back(q.id);
	}
	for (size_t i = 0; i < s.transit.lines.size(); i++) {
		const TransitLine &l = s.transit.lines[i];
		if (l.mode != TransitMode::BUS) continue;
		for (int id : l.stops) {
			std::vector<int> &at = run.lines_at_stop[id];
			if (at.empty() || at.back() != int(i)) at.push_back(int(i));
		}
	}
	run.stop_lanes.clear();
	for (const TransitStop &q : s.transit.stops) {
		StopLane lane;
		if (q.mode == TransitMode::BUS && lane_beside(s, q, lane)) run.stop_lanes[q.id] = lane;
	}
	place_all_buses(s, nullptr);
	place_rails(s);
	run.plan_seen = s.transit_revision;
	run.roads_seen = uint64_t(s.roads.version);
	run.built = true;
}

bool same_lane(const StopLane &a, const StopLane &b) { return a.ds == b.ds && a.lane == b.lane && a.at == b.at; }

// An edit of the roads alone (Roadcraft 4.0 transit.ts followLanes): the lane of each bus stop on a road it changed found again, and
// only the lines calling at a stop whose lane moved placed and sent on again; the level crossings of the train lines it may reach made
// again. The loops are the plan's: they stand.
void follow_roads(CityState &s) {
	TransitRun &run = s.transit_run;
	std::vector<int> segs;
	segs.swap(s.traffic.roads_touched);
	ChangeLog::dedup(segs);
	std::vector<uint8_t> send(s.transit.lines.size(), 0);
	bool moved = false;
	for (int seg : segs) {
		const auto on = run.stops_on_seg.find(seg);
		if (on == run.stops_on_seg.end()) continue;
		for (int id : on->second) {
			const TransitStop *q = stop_of(s, id);
			if (!q) continue;
			StopLane lane;
			const bool has = lane_beside(s, *q, lane);
			const auto it = run.stop_lanes.find(id);
			const bool had = it != run.stop_lanes.end();
			if (has == had && (!has || same_lane(lane, it->second))) continue;
			if (has) run.stop_lanes[id] = lane;
			else run.stop_lanes.erase(it);
			const auto lines = run.lines_at_stop.find(id);
			if (lines != run.lines_at_stop.end())
				for (int i : lines->second) send[size_t(i)] = 1;
			moved = true;
		}
	}
	if (moved) place_all_buses(s, &send);
	refresh_crossings(s, segs);
	run.roads_seen = uint64_t(s.roads.version);
}
} // namespace

// A stop of the plan by id, from the places read when the plan last moved (the plan does not move without TransitRun::plan_seen).
const TransitStop *stop_of(const CityState &s, int id) {
	const auto it = s.transit_run.stop_index.find(id);
	return it == s.transit_run.stop_index.end() ? nullptr : &s.transit.stops[size_t(it->second)];
}

// The bus's line, by the place read when the plan last moved; none when its line went.
const TransitLine *line_of(const CityState &s, const Bus &b) {
	return b.line_index >= 0 && size_t(b.line_index) < s.transit.lines.size() ? &s.transit.lines[size_t(b.line_index)] : nullptr;
}

void read_plan(CityState &s) {
	TransitRun &run = s.transit_run;
	if (!run.built || run.plan_seen != s.transit_revision) rebuild(s);
	else if (run.roads_seen != uint64_t(s.roads.version)) follow_roads(s);
}

} // namespace transit
} // namespace city
