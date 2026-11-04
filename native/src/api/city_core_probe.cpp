// Probes for the tests: where the last _process went (render_cost), how the agents' poses arrive (pose_stats:
// publications, their gaps, frames shown past the newest pose) and what the traffic does (traffic_probe).
#include "api/city_core.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>

using namespace godot;

namespace city {

Dictionary CityCore::agent_cost() const { return render_.agent_cost(); }

Dictionary CityCore::render_cost() const {
	const RenderWorld::Cost &c = render_.cost();
	Dictionary d;
	d["process"] = process_ms_;
	d["agents"] = agents_ms_;
	d["lock"] = c.lock;
	d["stream"] = c.stream;
	d["levels"] = c.levels;
	d["flush"] = c.flush;
	d["chunk_max"] = c.chunk_max;
	d["super_max"] = c.super_max;
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
constexpr float BOX_LEADER_M = 6.f; // ... behind the one ahead when it is this close
enum BoxStop { BOX_LEADER = 0, BOX_CROSSING, BOX_OTHER, BOX_KINDS };
constexpr float BODIES_TOUCH_M = 0.45f; // traffic_probe: two walkers' centres closer than this overlap (two body radii)

// Ground position of the signalled node with the most pedestrians at its kerbs or on its crosswalks (captures).
Vector3 busy_crossing(const CityState &s) {
	const TrafficState &t = s.traffic;
	std::vector<int> count(t.g.junctions.size(), 0);
	for (const Pedestrian &w : t.peds)
		if (w.alive && w.mode != PedMode::SIDEWALK && w.way_node >= 0) count[w.way_node]++;
	int best = -1;
	for (int n = 0; n < int(count.size()); n++)
		if (t.g.junctions[n].ctl == Control::SIGNAL && (best < 0 || count[n] > count[best])) best = n;
	if (best < 0) return Vector3();
	const RoadNode &node = s.roads.nodes[best];
	return Vector3(float(node.x), float(node.y), float(node.z));
}

// Why vehicle `id`, on a connector, stands in the junction box: the one ahead close (on its connector or the exit
// lane's tail), walkers on the exit crosswalk, or nothing in its way.
int box_stop(const CityState &s, int id) {
	const TrafficState &t = s.traffic;
	const Vehicle &v = t.vehicles[id];
	const Conn &c = t.g.conns[v.conn];
	const auto &on = t.conn_vehicles[v.conn];
	const auto me = std::find(on.begin(), on.end(), id);
	float gap = BIG;
	if (me != on.begin() && me != on.end()) {
		const Vehicle &o = t.vehicles[*(me - 1)];
		gap = o.cs - s.catalog.traffic.classes[int(o.type)].length - v.cs;
	} else {
		const auto &lane = t.lane_vehicles[size_t(c.to_ds) * MAX_LANES + c.to_lane];
		if (!lane.empty()) {
			const Vehicle &o = t.vehicles[lane.front()];
			gap = c.len - v.cs + o.s - s.catalog.traffic.classes[int(o.type)].length;
		}
	}
	if (gap < BOX_LEADER_M) return BOX_LEADER;
	return traffic::peds_in_way(s, v.conn, traffic::FRONT_PAST, 0.f) ? BOX_CROSSING : BOX_OTHER;
}

// Walkers whose body overlaps another's (centres closer than BODIES_TOUCH_M): [walking, waiting at a kerb].
Array people_overlaps(const TrafficState &t) {
	std::vector<int> alive;
	for (int i = 0; i < int(t.peds.size()); i++)
		if (t.peds[size_t(i)].alive) alive.push_back(i);
	int walking = 0, waiting = 0;
	for (int i : alive) {
		const Pedestrian &a = t.peds[size_t(i)];
		for (int j : alive) {
			const Pedestrian &b = t.peds[size_t(j)];
			const float dx = a.cur.x - b.cur.x, dz = a.cur.z - b.cur.z;
			if (i == j || dx * dx + dz * dz >= BODIES_TOUCH_M * BODIES_TOUCH_M) continue;
			(a.mode == PedMode::WAIT ? waiting : walking)++;
			break;
		}
	}
	Array out;
	out.push_back(walking);
	out.push_back(waiting);
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
	d["people_overlaps"] = people_overlaps(t); // walkers inside another's body: walking, waiting
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
	box.resize(BOX_KINDS);
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
			const int k = box_stop(*state_, id);
			box[k] = int(box[k]) + 1;
		}
		if (poses.size() < sample) {
			poses.push_back(Vector3(v.cur.x, v.cur.y, v.cur.z));
			dirs.push_back(Vector3(v.cur.fx, 0, v.cur.fz));
			types.push_back(int(v.type));
		}
	}
	d["signals"] = signals;
	d["busy_crossing"] = busy_crossing(*state_); // the lit junction with the most people waiting or crossing
	d["moving"] = moving;
	d["waiting"] = waiting; // held more than WAITING_S
	d["on_connectors"] = on_connectors;
	d["queued_behind"] = behind; // queued vehicles by what their lane head waits for
	d["box_stops"] = box; // standing on connectors now: behind the one ahead, for walkers, for nothing
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
