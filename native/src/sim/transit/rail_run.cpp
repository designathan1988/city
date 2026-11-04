// Trains on their loops: out along the stations and back, stopping at each, keeping a headway to the one ahead, the last car leading
// after a turn at the end of the line. Level crossings: where the loop crosses a lane; closed while a train comes up or is on them.
#include "sim/transit/rail_run.h"

#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {
namespace transit {

namespace {
constexpr float AT_STATION_M = 0.6f, STOPPING_MS = 0.5f, CREEP_MS = 0.4f; // pulled up at it; the creep that closes the last metres
constexpr float BRAKE_MARGIN = 1.5f; // braking harder than it plans to, when it must
constexpr float SHORT_LOOP = 0.6f, TRAIN_ROOM_M = 10.f; // the headway on a short line: a share of the loop each train has, at least a train and this
constexpr float FREE_LINE_M = std::numeric_limits<float>::max();
constexpr float ROAD_STEP_M = 4.f; // a road's middle line sampled this often for the crossings
constexpr float CROSS_EPS = 1e-9f;

float mod_loop(float s, float length) {
	const float m = std::fmod(s, length);
	return m < 0 ? m + length : m;
}

struct LoopPoint {
	V2 at;
	V2 dir;
};

LoopPoint point_at(const RailLine &r, float s) {
	const float d = mod_loop(s, r.length);
	const size_t n = r.points.size();
	size_t hi = size_t(std::upper_bound(r.at.begin(), r.at.end(), d) - r.at.begin());
	const size_t lo = hi - 1;
	hi = hi % n;
	const V2 &a = r.points[lo], &b = r.points[hi];
	const float span = (hi == 0 ? r.length : r.at[hi]) - r.at[lo];
	const float u = span > 0 ? (d - r.at[lo]) / span : 0;
	return { a + (b - a) * u, (b - a).norm() };
}

// Where segment a-b crosses p-q: the shares along each; false when they do not.
bool cross_at(const V2 &a, const V2 &b, const V2 &p, const V2 &q, float &u, float &v) {
	const V2 r = b - a, s = q - p;
	const float den = r.x * s.y - r.y * s.x;
	if (std::fabs(den) < CROSS_EPS) return false;
	u = ((p.x - a.x) * s.y - (p.y - a.y) * s.x) / den;
	v = ((p.x - a.x) * r.y - (p.y - a.y) * r.x) / den;
	return u >= 0 && u <= 1 && v >= 0 && v <= 1;
}

void crossings_of(const CityState &s, RailLine &r) {
	const LaneGraph &g = s.traffic.g;
	for (size_t i = 0; i < r.points.size(); i++) {
		const V2 &a = r.points[i], &b = r.points[(i + 1) % r.points.size()];
		s.roads.for_segments_near(std::min(a.x, b.x), std::min(a.y, b.y), std::max(a.x, b.x), std::max(a.y, b.y), [&](int seg) {
			const RoadSegment &road = s.roads.segs[size_t(seg)];
			const int steps = std::max(1, int(std::ceil(road.len / ROAD_STEP_M)));
			double x0, z0;
			road.q.at(0, x0, z0);
			V2 p0 = V2(float(x0), float(z0));
			for (int k = 1; k <= steps; k++) {
				double x1, z1;
				road.q.at(double(k) / steps, x1, z1);
				const V2 p1 = V2(float(x1), float(z1));
				float u, v;
				if (cross_at(a, b, p0, p1, u, v)) {
					const float at_arc = float(road.len) * (float(k - 1) + v) / float(steps);
					for (int dir = 0; dir < 2; dir++) {
						const DirSeg &d = g.ds[size_t(seg) * 2 + size_t(dir)];
						if (!d.ok) continue;
						for (int lane = 0; lane < d.lanes; lane++)
							r.crossings.push_back({ (seg * 2 + dir) * MAX_LANES + lane, (at_arc - d.at0) * d.sign, r.at[i] + (b - a).len() * u });
					}
				}
				p0 = p1;
			}
		});
	}
}

// One line's loop: the stations out and the ones back, a leg of track between each two; false when a leg has no way.
bool make_loop(const TransitData &t, const TransitLine &l, const RailGraph &g, RailLine &r) {
	std::vector<const TransitStop *> order;
	for (int id : l.stops)
		if (const TransitStop *q = t.stop(id)) order.push_back(q);
	if (order.size() < 2) return false;
	const size_t out = order.size();
	for (size_t i = out - 1; i-- > 1;) order.push_back(order[i]);
	std::vector<size_t> marks;
	auto repeats = [&](const V2 &p) { return !r.points.empty() && r.points.back().x == p.x && r.points.back().y == p.y; };
	for (size_t i = 0; i < order.size(); i++) {
		const std::vector<V2> leg = rail_path(g, order[i]->at, order[(i + 1) % order.size()]->at);
		if (leg.empty()) return false;
		marks.push_back(repeats(leg[0]) ? r.points.size() - 1 : r.points.size());
		for (const V2 &p : leg)
			if (!repeats(p)) r.points.push_back(p);
	}
	if (r.points.size() > 1 && r.points.front().x == r.points.back().x && r.points.front().y == r.points.back().y) r.points.pop_back();
	if (r.points.size() < 2) return false;
	r.at.assign(1, 0.f);
	for (size_t i = 1; i < r.points.size(); i++) r.at.push_back(r.at.back() + (r.points[i] - r.points[i - 1]).len());
	r.length = r.at.back() + (r.points.front() - r.points.back()).len();
	for (size_t i = 0; i < order.size(); i++) r.stations.push_back({ order[i]->id, r.at[marks[i] % r.points.size()], i == 0 || i + 1 == out });
	return true;
}
} // namespace

int cars_of(const CityState &s, const RailLine &r) {
	const TrafficParams &p = s.catalog.traffic;
	return int(r.metro ? p.metro_cars : p.train_cars);
}

void place_rails(CityState &s) {
	TransitRun &run = s.transit_run;
	const float join = s.catalog.traffic.transit_join_m;
	std::vector<RailLine> before = std::move(run.rails);
	run.rails.clear();
	run.crossings_by_lane.clear();
	const RailGraph graphs[2] = { rail_graph(s.transit, TransitMode::TRAIN, join), rail_graph(s.transit, TransitMode::METRO, join) };
	for (const TransitLine &l : s.transit.lines) {
		if (l.mode == TransitMode::BUS) continue;
		RailLine r;
		r.line = l.id;
		r.metro = l.mode == TransitMode::METRO;
		r.colour = l.colour;
		if (!make_loop(s.transit, l, graphs[r.metro ? 1 : 0], r)) continue;
		const auto was = std::find_if(before.begin(), before.end(), [&](const RailLine &o) { return o.line == l.id && o.points.size() == r.points.size() && o.length == r.length && int(o.trains.size()) == l.vehicles; });
		if (was != before.end()) r.trains = std::move(was->trains); // an edit elsewhere leaves them where they are
		for (int k = 0; was == before.end() && k < l.vehicles; k++) {
			Train t;
			t.s = r.length * float(k) / float(l.vehicles);
			const auto next = std::find_if(r.stations.begin(), r.stations.end(), [&](const LoopStation &st) { return st.s >= t.s; });
			t.next = next == r.stations.end() ? 0 : int(next - r.stations.begin());
			t.cars.resize(size_t(cars_of(s, r)));
			r.trains.push_back(std::move(t));
		}
		if (!r.metro) crossings_of(s, r); // the metro runs under them all
		const int index = int(run.rails.size());
		for (size_t c = 0; c < r.crossings.size(); c++) run.crossings_by_lane[r.crossings[c].key].push_back({ index, int(c) });
		run.rails.push_back(std::move(r));
	}
}

void step_train(CityState &s, RailLine &r, Train &t, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const LoopStation &station = r.stations[size_t(t.next)];
	const float ahead = mod_loop(station.s - t.s, r.length), length = float(cars_of(s, r)) * p.train_car_m;
	if (t.dwell > 0) {
		t.dwell -= dt;
		if (t.dwell > 0) return;
		t.dwell = 0;
		t.next = (t.next + 1) % int(r.stations.size());
		if (station.turn) t.s = mod_loop(t.s + length, r.length); // at an end of the line its last car leads
		return;
	}
	if (ahead < AT_STATION_M && t.v < STOPPING_MS) {
		t.v = 0;
		t.dwell = p.train_dwell_s;
		s.transit_run.trains_at_stations++;
		return;
	}
	float gap = FREE_LINE_M;
	for (const Train &o : r.trains)
		if (&o != &t) {
			const float d = mod_loop(o.s - t.s, r.length);
			if (d > 0) gap = std::min(gap, d);
		}
	const float headway = std::min(p.train_gap_m, std::max(length + TRAIN_ROOM_M, SHORT_LOOP * r.length / float(r.trains.size())));
	const float room = std::min(ahead, gap - headway), top = r.metro ? p.metro_top_ms : p.train_top_ms;
	const float want = std::clamp(std::sqrt(2 * p.train_brake * std::max(0.f, room)), 0.f, top);
	t.v = want > t.v ? std::min(want, t.v + p.train_accel * dt) : std::max(want, t.v - p.train_brake * BRAKE_MARGIN * dt);
	if (room > AT_STATION_M && t.v < CREEP_MS) t.v = std::min(CREEP_MS, want);
	t.s = mod_loop(t.s + t.v * dt, r.length);
}

float crossing_gap(const CityState &s, int key, float at) {
	const TransitRun &run = s.transit_run;
	const auto it = run.crossings_by_lane.find(key);
	if (it == run.crossings_by_lane.end()) return -1;
	const TrafficParams &p = s.catalog.traffic;
	float best = -1;
	for (const auto &e : it->second) {
		const RailLine &r = run.rails[size_t(e.first)];
		const LevelCrossing &c = r.crossings[size_t(e.second)];
		const float stop = c.at - p.crossing_stop_m - at;
		if (stop < 0 || (best >= 0 && stop >= best)) continue;
		const float cars = float(cars_of(s, r)) * p.train_car_m;
		for (const Train &t : r.trains) {
			const float coming = mod_loop(c.loop_s - t.s, r.length);
			if (coming < p.crossing_warn_m || coming > r.length - cars - TRAIN_ROOM_M) {
				best = stop;
				break;
			}
		}
	}
	return best;
}

void publish(CityState &s, PoseFrame &f, int64_t stamp, int64_t before, float cx, float cz, float radius) {
	f.trains.clear();
	const TrafficParams &p = s.catalog.traffic;
	uint32_t id = 0;
	for (RailLine &r : s.transit_run.rails)
		for (Train &t : r.trains)
			for (size_t k = 0; k < t.cars.size(); k++, id++) {
				const LoopPoint q = point_at(r, t.s - p.train_car_m * (float(k) + 0.5f));
				Pose pose;
				pose.x = q.at.x;
				pose.z = q.at.y;
				pose.y = s.terrain.height_at(q.at.x, q.at.y);
				pose.fx = q.dir.x;
				pose.fz = q.dir.y;
				t.cars[k].push(pose, stamp, before);
				if ((q.at - V2(cx, cz)).len() > radius) continue;
				AgentRecord rec;
				for (int i = 0; i < POSE_TRAIL; i++) rec.at[i] = t.cars[k].at[i];
				rec.id = r.metro ? 1u : 0u;
				rec.look = r.colour;
				f.trains.push_back(rec);
			}
}

} // namespace transit
} // namespace city
