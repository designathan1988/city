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
void populate_trains(CityState &s, RailLine &r, int vehicles) {
	r.trains.clear();
	for (int k = 0; k < vehicles; k++) {
		Train t;
		t.s = r.length * float(k) / float(vehicles);
		const auto next = std::find_if(r.stations.begin(), r.stations.end(), [&](const LoopStation &st) { return st.s >= t.s; });
		t.next = next == r.stations.end() ? 0 : int(next - r.stations.begin());
		t.cars.resize(size_t(cars_of(s, r)));
		r.trains.push_back(std::move(t));
	}
}
} // namespace

int cars_of(const CityState &s, const RailLine &r) {
	const TrafficParams &p = s.catalog.traffic;
	return int(r.metro ? p.metro_cars : p.train_cars);
}

void place_rails(CityState &s, bool rebuild_loops) {
	TransitRun &run = s.transit_run;
	bool order_changed = !run.built;
	if (rebuild_loops) {
		const float join = s.catalog.traffic.transit_join_m;
		const bool join_changed = run.graph_join != join;
		std::array<bool, 2> needed{}, topology_changed{};
		for (const TransitLine &l : s.transit.lines()) if (l.mode != TransitMode::BUS) needed[l.mode == TransitMode::METRO ? 1 : 0] = true;
		for (size_t mode = 0; mode < needed.size(); ++mode) {
			const TransitMode kind = mode ? TransitMode::METRO : TransitMode::TRAIN;
			const uint64_t version = s.transit.track_graph_version(kind);
			topology_changed[mode] = join_changed || run.tracks_seen[mode] != version;
			if (!needed[mode]) {
				if (run.tracks_seen[mode]) { run.rail_graphs[mode] = RailGraph(); run.tracks_seen[mode] = 0; }
			} else if (topology_changed[mode]) {
				constexpr size_t MIB = 1024 * 1024;
				const size_t retained = size_t(std::max(0.f, s.catalog.tune.sim_rail_path_cache_mb) * MIB);
				run.rail_graphs[mode] = rail_graph(s.transit, kind, join, retained);
				run.tracks_seen[mode] = version;
			}
		}
		run.graph_join = join;
		std::vector<RailLine> before = std::move(run.rails);
		run.rails.clear();
		std::unordered_map<int, size_t> before_by_line;
		for (size_t i = 0; i < before.size(); ++i) before_by_line.emplace(before[i].line, i);
		for (const TransitLine &l : s.transit.lines()) {
			if (l.mode == TransitMode::BUS) continue;
			const size_t mode = l.mode == TransitMode::METRO ? 1 : 0;
			const uint64_t version = s.transit.rail_line_version(l.id);
			const auto was = before_by_line.find(l.id);
			const bool reuse = was != before_by_line.end() && !topology_changed[mode] && before[was->second].plan_version == version;
			if (reuse) {
				order_changed = order_changed || was->second != run.rails.size();
				RailLine r = std::move(before[was->second]);
				r.colour = l.colour;
				// Preserve the original policy: changing fleet size lays the full fleet out on the same loop again.
				if (int(r.trains.size()) != l.vehicles) populate_trains(s, r, l.vehicles);
				run.rails.push_back(std::move(r));
				continue;
			}
			RailLine r;
			r.line = l.id;
			r.plan_version = version;
			r.metro = l.mode == TransitMode::METRO;
			r.colour = l.colour;
			if (!make_loop(s.transit, l, run.rail_graphs[mode], r)) continue;
			order_changed = order_changed || was == before_by_line.end() || was->second != run.rails.size();
			const bool keep = was != before_by_line.end() && before[was->second].points.size() == r.points.size()
				&& before[was->second].length == r.length && int(before[was->second].trains.size()) == l.vehicles;
			if (keep) r.trains = std::move(before[was->second].trains);
			else populate_trains(s, r, l.vehicles);
			run.rails.push_back(std::move(r));
		}
		order_changed = order_changed || before.size() != run.rails.size();
	}
	// Only new loop geometry or a new road graph changes these intersections. Paint/fleet changes keep them.
	bool crossings_changed = false;
	const uint64_t roads = uint64_t(s.roads.version);
	for (RailLine &r : run.rails) {
		if (r.crossings_built && r.crossing_roads == roads) continue;
		r.crossings.clear();
		if (!r.metro) crossings_of(s, r);
		r.crossing_roads = roads; r.crossings_built = true; crossings_changed = true;
	}
	if (crossings_changed || order_changed) {
		run.crossings_by_lane.clear();
		for (size_t index = 0; index < run.rails.size(); ++index)
			for (size_t c = 0; c < run.rails[index].crossings.size(); c++) run.crossings_by_lane[run.rails[index].crossings[c].key].push_back({int(index), int(c)});
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
		if (c.at <= at) continue; // over the line already: it goes on across
		const float stop = std::max(0.f, c.at - p.crossing_stop_m - at);
		if (best >= 0 && stop >= best) continue;
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

float crossing_near_end(const CityState &s, int key, float length) {
	const TransitRun &run = s.transit_run;
	const auto it = run.crossings_by_lane.find(key);
	if (it == run.crossings_by_lane.end()) return -1;
	float best = -1;
	for (const auto &e : it->second) {
		const LevelCrossing &c = run.rails[size_t(e.first)].crossings[size_t(e.second)];
		if (length - c.at < s.catalog.traffic.crossing_keep_clear_m && (best < 0 || c.at < best)) best = c.at;
	}
	return best;
}

void train_zones(CityState &s) {
	TransitRun &run = s.transit_run;
	run.zones.clear();
	const TrafficParams &p = s.catalog.traffic;
	const V2 cam(s.camera_x.load(), s.camera_z.load());
	for (const RailLine &r : run.rails) {
		if (r.metro) continue;
		const int cars = cars_of(s, r);
		for (const Train &t : r.trains) {
			const float ahead = std::max(p.train_clear_ahead_m, t.v * p.train_clear_time_s), reach = float(cars) * p.train_car_m + ahead;
			if ((point_at(r, t.s).at - cam).len() > p.ped_window_m + reach) continue;
			TrainZone z;
			z.x0 = z.z0 = FREE_LINE_M;
			z.x1 = z.z1 = -FREE_LINE_M;
			auto disc = [&](const V2 &at, float radius) {
				z.discs.push_back({ at, radius });
				z.x0 = std::min(z.x0, at.x - radius);
				z.z0 = std::min(z.z0, at.y - radius);
				z.x1 = std::max(z.x1, at.x + radius);
				z.z1 = std::max(z.z1, at.y + radius);
			};
			// standing at a station it is ground to keep off only once it is about to leave
			if (t.v >= p.train_idle_ms || t.dwell <= p.train_clear_time_s)
				for (float d = p.train_clear_from_m; d < ahead; d += p.train_clear_step_m) disc(point_at(r, t.s + d).at, p.train_clear_disc_m);
			const float half = p.train_car_m * 0.5f - p.train_car_margin_m;
			for (int k = 0; k < cars; k++) {
				const LoopPoint car = point_at(r, t.s - p.train_car_m * (float(k) + 0.5f));
				for (float d = -half; d <= half; d += p.train_car_step_m) disc(car.at + car.dir * d, p.train_car_disc_m);
			}
			run.zones.push_back(std::move(z));
		}
	}
}

namespace {
bool in_zone(const TransitRun &run, const V2 &q) {
	for (const TrainZone &z : run.zones) {
		if (q.x < z.x0 || q.x > z.x1 || q.y < z.z0 || q.y > z.z1) continue;
		for (const WalkerDisc &d : z.discs)
			if ((d.at - q).len() < d.r) return true;
	}
	return false;
}
} // namespace

bool walker_held(const CityState &s, const V2 &at, const V2 &ahead) {
	const TransitRun &run = s.transit_run;
	return !run.zones.empty() && in_zone(run, at + ahead * s.catalog.traffic.train_walker_look_m) && !in_zone(run, at);
}

void publish(CityState &s, PoseFrame &f, int64_t stamp, int64_t before, float cx, float cz, float radius) {
	f.trains.clear();
	const TrafficParams &p = s.catalog.traffic;
	for (RailLine &r : s.transit_run.rails)
		for (Train &t : r.trains)
			for (size_t k = 0; k < t.cars.size(); k++) {
				const LoopPoint q = point_at(r, t.s - p.train_car_m * (float(k) + 0.5f));
				Pose pose;
				pose.x = q.at.x;
				pose.z = q.at.y;
				pose.y = s.terrain.height_at(q.at.x, q.at.y);
				pose.fx = q.dir.x;
				pose.fz = q.dir.y;
				t.cars[k].push(pose, stamp, before);
				if ((q.at - V2(cx, cz)).len() > radius) continue;
				TrainRecord rec;
				rec.row_id = t.cars[k].row_id;
				for (int i = 0; i < POSE_TRAIL; i++) rec.at[i] = t.cars[k].at[i];
				rec.id = r.metro ? 1u : 0u;
				rec.look = r.colour;
				f.trains.push_back(rec);
			}
}

} // namespace transit
} // namespace city
