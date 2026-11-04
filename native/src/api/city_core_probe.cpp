// Probes for the tests: where the last _process went (render_cost), how the agents' poses arrive (pose_stats:
// publications, their gaps, frames shown past the newest pose) and what the traffic does (traffic_probe).
#include "api/city_core.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/roads/geometry.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

Dictionary CityCore::agent_cost() const { return render_.agent_cost(); }

Dictionary CityCore::render_cost() const {
	const RenderWorld::Cost &c = render_.cost();
	Dictionary d;
	d["process"] = process_ms_;
	d["worn_cells"] = render_.worn_cells();
	d["wear_publications"] = render_.wear_publications();
	d["agents"] = agents_ms_;
	d["lock"] = c.lock;
	d["stream"] = c.stream;
	d["levels"] = c.levels;
	d["flush"] = c.flush;
	d["chunk_max"] = c.chunk_max;
	d["super_max"] = c.super_max;
	d["super_mb_max"] = double(render_.super_bytes_max()) / BYTES_PER_MIB; // largest far mesh built
	d["b_scan"] = c.building.scan;
	d["b_freed"] = c.building.freed;
	d["b_sort"] = c.building.sort;
	d["b_admit"] = c.building.admit;
	d["b_hand"] = c.building.hand;
	d["b_tail"] = c.building.tail;
	d["roads"] = c.roads;
	d["props"] = c.props;
	d["buildings"] = c.buildings;
	d["trees"] = c.trees;
	d["overlays"] = overlays_ms_;
	d["uploads"] = c.uploads; // MiB
	const BuildingRenderer::ViewNeed need = render_.building_need();
	d["backlog"] = c.backlog;
	d["budget_left"] = c.budget_left;
	d["b_want"] = need.want;
	d["b_unmet"] = need.unmet;
	d["b_requested"] = need.requested;
	d["b_done"] = need.done;
	d["jobs_replaced"] = int64_t(state_->jobs ? state_->jobs->replaced() : 0);
	return d;
}

namespace {
Dictionary count_of(const RoadChunks::Count &n) {
	Dictionary d;
	d["chunks"] = n.chunks; // one instance and one draw call each
	d["triangles"] = n.all.triangles;
	d["vertices"] = n.all.vertices;
	d["bytes"] = n.all.bytes;
	d["largest_bytes"] = n.largest;
	return d;
}
} // namespace

Dictionary CityCore::road_mesh_counts() const {
	Dictionary d;
	d["roads"] = count_of(render_.road_count());
	d["verges"] = count_of(render_.verge_count());
	d["wear_bytes"] = render_.wear_bytes();
	return d;
}

namespace {
constexpr float EARLY_GAP = 0.5f, LATE_GAP = 1.5f; // pose_stats: a publication this share of a tick after the last
} // namespace

void CityCore::note_pose(const PoseFrame &f, bool fresh, const PoseSample &at) {
	PoseStats &st = pose_stats_;
	st.frames++;
	if (at.beyond) st.held++; // the moment drawn is past the newest pose: the agents stand still this frame
	if (at.before) st.behind++; // older than the trail: they jump to its oldest pose
	if (!fresh || f.period_ns <= 0) return;
	const int64_t newest = f.stamp_ns[POSE_TRAIL - 1];
	if (st.last_tick_ns > 0) {
		const int64_t gap = newest - st.last_tick_ns;
		st.gap_min_ns = st.publications > 0 ? std::min(st.gap_min_ns, gap) : gap;
		st.gap_max_ns = std::max(st.gap_max_ns, gap);
		if (float(gap) < EARLY_GAP * float(f.period_ns)) st.early++;
		if (float(gap) > LATE_GAP * float(f.period_ns)) st.late++;
		st.publications++;
	}
	st.last_tick_ns = newest;
}

Dictionary CityCore::pose_stats() {
	PoseStats &st = pose_stats_;
	Dictionary d;
	d["frames"] = st.frames;
	d["held"] = st.held;
	d["behind"] = st.behind;
	d["publications"] = st.publications;
	d["early"] = st.early;
	d["late"] = st.late;
	d["gap_min_ms"] = double(st.gap_min_ns) / NS_PER_MS;
	d["gap_max_ms"] = double(st.gap_max_ns) / NS_PER_MS;
	const int64_t last = st.last_tick_ns;
	st = PoseStats();
	st.last_tick_ns = last;
	return d;
}

namespace {

constexpr float WAITING_S = 20.f; // traffic_probe: vehicles held this long count as waiting
constexpr float BOX_STOP_MS = 0.5f; // traffic_probe: a vehicle on a connector slower than this stands in the junction

// The signalled node with the most pedestrians at its kerbs or on its crosswalks (captures), -1 if none.
int busy_node(const CityState &s) {
	const TrafficState &t = s.traffic;
	std::vector<int> count(t.g.junctions.size(), 0);
	for (const Pedestrian &w : t.peds)
		if (w.alive && w.mode != PedMode::SIDEWALK && w.way_node >= 0) count[w.way_node]++;
	int best = -1;
	for (int n = 0; n < int(count.size()); n++)
		if (t.g.junctions[n].ctl == Control::SIGNAL && (best < 0 || count[n] > count[best])) best = n;
	return best;
}

Vector3 node_ground(const CityState &s, int n) {
	if (n < 0) return Vector3();
	const RoadNode &node = s.roads.nodes[n];
	return Vector3(float(node.x), float(node.y), float(node.z));
}

// The mouth of node n's first arm (captures): the kerb line on the arm's left (u = asph_left), level with the middle of
// its crosswalk (catalog street_furniture.json crossing), the unit vector along the arm away from the node and the one
// across it toward that kerb. Empty when there is no node.
Array crossing_kerb(const CityState &s, int n) {
	Array out;
	if (n < 0 || s.roads.nodes[n].segs.empty()) return out;
	const int sid = s.roads.nodes[n].segs[0];
	const RoadSegment &g = s.roads.segs[sid];
	const CrossingDef &x = s.catalog.furniture.crossing;
	const bool at_a = g.a == n;
	const double mid = (x.zebra_from_m + x.zebra_to_m) / 2, at = at_a ? g.retreat_a + mid : g.len - g.retreat_b - mid;
	double t = t_at_length(g.table, at), px, pz, dx, dz;
	g.q.at(t, px, pz);
	g.q.deriv(t, dx, dz);
	double l = std::hypot(dx, dz);
	if (l == 0) l = 1;
	const double away = at_a ? 1.0 : -1.0;
	const Vector3 left(float(dz / l), 0, float(-dx / l));
	const float u = s.catalog.profile(g.profile).asph_left;
	out.push_back(Vector3(float(px), float(s.roads.seg_y(sid, at)), float(pz)) + left * u);
	out.push_back(Vector3(float(away * dx / l), 0, float(away * dz / l)));
	out.push_back(left);
	return out;
}

// Walkers whose body overlaps another's (centres closer than `touch`): [walking, waiting at a kerb].
// Two bodies side by side (their centres two radii apart, to the rounding of the sum) touch; they do not overlap.
constexpr float TOUCH_EPS_M = 1e-3f;
Array people_overlaps(const TrafficState &t, float touch) {
	std::vector<int> alive;
	for (int i = 0; i < int(t.peds.size()); i++)
		if (t.peds[size_t(i)].alive) alive.push_back(i);
	int walking = 0, waiting = 0;
	for (int i : alive) {
		const Pedestrian &a = t.peds[size_t(i)];
		for (int j : alive) {
			const Pedestrian &b = t.peds[size_t(j)];
			const float dx = a.cur.x - b.cur.x, dz = a.cur.z - b.cur.z;
			if (i == j || dx * dx + dz * dz >= touch * touch) continue;
			(a.mode == PedMode::WAIT ? waiting : walking)++;
			break;
		}
	}
	Array out;
	out.push_back(walking);
	out.push_back(waiting);
	return out;
}

// The street furniture of the whole map by kind (signal posts, lamps, hydrants, bins, benches, trees).
Array furniture_counts(const CityState &s) {
	int count[FURNITURE_KINDS] = {};
	std::vector<FurnitureSpot> spots;
	for (int sid = 0; sid < int(s.roads.segs.size()); sid++) {
		if (!s.roads.segs[size_t(sid)].alive) continue;
		spots.clear();
		traffic::street_furniture_of(s, sid, FURNITURE_ALL, spots);
		for (const FurnitureSpot &f : spots) count[int(f.kind)]++;
	}
	Array out;
	for (int k = 0; k < FURNITURE_KINDS; k++) out.push_back(count[k]);
	return out;
}

// Everything the HUD/captures show about one held vehicle (why it waits and what blocks it).
String jam_detail(const CityState &s, int id, const Vehicle &v, int r) {
	const TrafficState &t = s.traffic;
	String info = String("hold={0} type={1} conn={2} v={3} ds={4} s={5} L={6} id={7}").format(
			Array::make(r, int(v.type), v.conn, v.v, v.ds, v.s, v.ds >= 0 ? t.g.ds[v.ds].len : 0.f, id));
	int ci = v.ds >= 0 ? traffic::pick_connector(s, v) : v.conn;
	if (v.conn >= 0) {
		const Conn &c = t.g.conns[v.conn];
		const auto &on = t.conn_vehicles[v.conn];
		int at = int(std::find(on.begin(), on.end(), id) - on.begin());
		info += String(" node={0} deg={8} ctl={1} cs={2}/{3} order={4}/{5} turn={6} peds={7}").format(Array::make(c.node,
				int(t.g.junctions[c.node].ctl), v.cs, c.len, at, int(on.size()), int(c.turn), traffic::peds_in_way(s, v.conn), t.g.junctions[c.node].degree));
	}
	if (ci >= 0 && r == int(Hold::CONFLICT_BUSY)) {
		for (int oc : t.g.conns[ci].conflicts) {
			if (t.conn_vehicles[oc].empty()) continue;
			const Vehicle &o = t.vehicles[t.conn_vehicles[oc].front()];
			info += String(" busy conn={0} n={1} first v={2} cs={3}/{4} wait={5} peds={6}").format(Array::make(oc, int(t.conn_vehicles[oc].size()),
					o.v, o.cs, t.g.conns[oc].len, o.wait, traffic::peds_in_way(s, oc)));
			break;
		}
	}
	if (ci >= 0) {
		const Conn &c = t.g.conns[ci];
		const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
		info += String(" -> ds={0} lane={1} L={2} n={3}").format(Array::make(c.to_ds, c.to_lane, t.g.ds[c.to_ds].len, int(lane.size())));
		if (!lane.empty()) {
			const Vehicle &o = t.vehicles[lane.front()];
			info += String(" tail id={0} s={1} v={2} wait={3} hold={4} ri={5}/{6}").format(Array::make(lane.front(), o.s, o.v, o.wait,
					int(traffic::hold_reason(s, lane.front())), o.ri, o.route.len));
		}
	}
	return info;
}

} // namespace

bool CityCore::amber_stops(float v, float stop_m, float top, float left, int type) const {
	const TrafficParams &p = state_->catalog.traffic;
	const VehicleClass &vc = p.classes[size_t(type)];
	traffic::AmberApproach a;
	a.v = v;
	a.stop_m = stop_m;
	a.top = top;
	a.brake = vc.b;
	a.brake_hard = vc.b_emergency;
	a.left = left;
	return traffic::amber_stops(p, a);
}

float CityCore::amber_wanted(float kmh) const {
	return traffic::amber_wanted(state_->catalog.traffic, kmh / KMH_PER_MS);
}

Dictionary CityCore::traffic_probe(int sample) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const TrafficState &t = state_->traffic;
	Dictionary d;
	d["vehicles"] = t.vehicle_count;
	d["people"] = t.ped_count;
	d["trips_arrived"] = t.arrived;
	d["trips_lost"] = t.lost;
	d["lost_stuck"] = t.stuck_lost; // of which: removed after standing stuck_despawn_s
	d["people_arrived"] = t.ped_arrived;
	d["people_waiting"] = t.ped_waiting; // at a kerb for the walk phase or a gap
	d["people_overlaps"] = people_overlaps(t, 2 * state_->catalog.traffic.ped_body_m - TOUCH_EPS_M); // walkers inside another's body: walking, waiting
	d["people_in_furniture"] = traffic::walkers_in_furniture(*state_); // walkers whose body stands in a street piece
	// pieces of the map by kind (signal, lamp, hydrant, bin, bench, tree): every segment placed again, so only with a sample
	if (sample > 0) d["street_furniture"] = furniture_counts(*state_);
	d["reroutes"] = t.reroutes;
	d["released"] = t.released; // jammed-junction releases
	d["governor"] = t.gov;
	d["drawn_vehicles"] = render_.drawn_vehicles();
	d["drawn_people"] = render_.drawn_people();
	d["drawn_signal_lenses"] = render_.lit_lenses(); // lit on the signal posts, from the published lights
	d["lane_km"] = t.g.lane_metres / METRES_PER_KM;
	// the pool keeps freed entries: the probe reports the connections that exist
	d["connectors"] = int(std::count_if(t.g.conns.begin(), t.g.conns.end(), [](const Conn &c) { return c.alive; }));
	int signals = 0, moving = 0, waiting = 0, on_connectors = 0;
	for (const Junction &j : t.g.junctions) signals += j.ctl == Control::SIGNAL ? 1 : 0;
	Array poses, dirs, types, held, jams, jam_info, behind, box;
	box.resize(int(traffic::BoxHold::COUNT));
	box.fill(0);
	held.resize(int(Hold::COUNT));
	held.fill(0);
	behind.resize(int(Hold::COUNT));
	behind.fill(0);
	for (int id = 0; id < int(t.vehicles.size()); id++) {
		const Vehicle &v = t.vehicles[id];
		if (!v.alive || !v.micro) continue; // the probe reads the near model; far vehicles are queues and events
		if (v.wait > WAITING_S) {
			int r = int(traffic::hold_reason(*state_, id));
			held[r] = int(held[r]) + 1;
			if (r == int(Hold::QUEUED)) { // what the head of its lane waits for
				int h = traffic::lane_head(*state_, id), hr = h >= 0 ? int(traffic::hold_reason(*state_, h)) : int(Hold::FREE);
				behind[hr] = int(behind[hr]) + 1;
			}
			if (r != int(Hold::QUEUED) && r != int(Hold::FREE) && jams.size() < sample) {
				jams.push_back(Vector3(v.cur.x, v.cur.y, v.cur.z));
				jam_info.push_back(jam_detail(*state_, id, v, r));
			}
		}
		moving += v.v > 1.f ? 1 : 0;
		waiting += v.wait > WAITING_S ? 1 : 0;
		on_connectors += v.conn >= 0 ? 1 : 0;
		if (v.conn >= 0 && v.v < BOX_STOP_MS) {
			const int k = int(traffic::box_hold(*state_, id));
			box[k] = int(box[k]) + 1;
		}
		if (poses.size() < sample) {
			poses.push_back(Vector3(v.cur.x, v.cur.y, v.cur.z));
			dirs.push_back(Vector3(v.cur.fx, 0, v.cur.fz));
			types.push_back(int(v.type));
		}
	}
	d["signals"] = signals;
	const int busy = busy_node(*state_);
	d["busy_crossing"] = node_ground(*state_, busy); // the lit junction with the most people waiting or crossing
	d["crossing_kerb"] = crossing_kerb(*state_, busy); // a dropped kerb of it: [kerb point, along the arm, across]
	d["moving"] = moving;
	d["waiting"] = waiting; // held more than WAITING_S
	d["on_connectors"] = on_connectors;
	d["queued_behind"] = behind; // queued vehicles by what their lane head waits for
	d["box_stops"] = box; // standing on connectors now: behind the one ahead, for walkers, moving off (traffic::BoxHold)
	d["red_entries"] = t.red_entries; // vehicles that went onto a connector with their light red
	d["red_from_stop"] = t.red_from_stop; // ... of which from a standstill (not an amber they could not stop for)
	d["held_by"] = held; // waiting vehicles by Hold: queued, wrong lane, red, exit room, conflict busy, yield, connector, free, crosswalk
	d["sample"] = poses;
	d["sample_dir"] = dirs;
	d["sample_type"] = types; // VehicleType, for the close looks at a bus, a truck, a motorbike
	Array walkers, walker_dirs; // pedestrians, for close looks in the visual captures
	for (const Pedestrian &w : t.peds)
		if (w.alive && walkers.size() < sample) {
			walkers.push_back(Vector3(w.cur.x, w.cur.y, w.cur.z));
			walker_dirs.push_back(Vector3(w.cur.fx, 0, w.cur.fz));
		}
	d["walkers"] = walkers;
	d["walker_dirs"] = walker_dirs;
	d["jam_info"] = jam_info;
	d["jams"] = jams; // held heads (not queued behind someone)
	return d;
}

} // namespace city
