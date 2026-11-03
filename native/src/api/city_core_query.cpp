// Read-only views for the UI and the tests. Values are derived here from CityState; the UI never recomputes them.
#include "api/city_core.h"

#include "core/units.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
constexpr float RAY_MAX_M = 30000.f;
constexpr float INSPECT_RADIUS_M = 30.f;
constexpr float WAITING_S = 20.f; // traffic_probe: vehicles held this long count as waiting
constexpr float MIN_RAY_DIRECTION = 1e-6f; // below this the ray is vertical or degenerate
constexpr float NO_BOUND = 1e9f; // an empty city_bounds still has finite numbers
} // namespace

Dictionary CityCore::constants() const {
	Dictionary d;
	d["speed_paused"] = int(SimSpeed::PAUSED);
	d["speed_fastest"] = int(SimSpeed::FASTEST);
	d["minutes_per_day"] = MINUTES_PER_DAY;
	d["minutes_per_hour"] = MINUTES_PER_HOUR;
	d["zone_codes"] = ZONE_CODES;
	return d;
}

Dictionary CityCore::snapshot() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	Dictionary d;
	d["ready"] = s.ready;
	if (!s.ready) return d;
	d["money"] = s.money;
	d["unlimited"] = s.unlimited_money;
	d["hour"] = s.clock.hour();
	d["minutes"] = s.clock.minutes.load();
	d["buildings"] = s.buildings.count;
	d["segments"] = s.roads.alive_segments();
	if (s.edit_revision != snap_edit_revision_) {
		int nodes = 0;
		for (const RoadNode &n : s.roads.nodes) nodes += n.alive ? 1 : 0;
		int zoned = 0;
		for (const RoadSegment &g : s.roads.segs) zoned += g.alive ? (g.zl != 0) + (g.zr != 0) : 0;
		int residents = 0, jobs = 0;
		const OccupancyDef &o = s.catalog.occupancy;
		const float per[4] = { o.m2_per_resident, o.m2_per_job_commercial, o.m2_per_job_industrial, o.m2_per_person_mixed };
		for (const Building &b : s.buildings.items) {
			if (!b.alive) continue;
			int r, j;
			BuildingStore::occupancy(b, o.floor_share, per, r, j);
			residents += r;
			jobs += j;
		}
		snap_edit_revision_ = s.edit_revision;
		snap_nodes_ = nodes;
		snap_zoned_ = zoned;
		snap_residents_ = residents;
		snap_jobs_ = jobs;
	}
	d["nodes"] = snap_nodes_;
	d["zoned_sides"] = snap_zoned_;
	d["grown"] = s.growth.grown;
	d["growth_cells"] = int(s.growth.cells.size());
	d["residents"] = snap_residents_;
	d["jobs"] = snap_jobs_;
	d["vehicles"] = s.traffic.vehicle_count;
	d["people"] = s.traffic.ped_count;
	d["trips_arrived"] = s.traffic.arrived;
	d["trips_lost"] = s.traffic.lost;
	d["speed"] = int(sim_.speed());
	d["unsaved"] = s.has_unsaved_work();
	d["can_undo"] = !s.history.undo_stack.empty();
	d["can_redo"] = !s.history.redo_stack.empty();
	d["undo_label"] = String(s.history.undo_label().c_str());
	d["redo_label"] = String(s.history.redo_label().c_str());
	d["map"] = s.map_index;
	d["seed"] = int64_t(s.seed);
	return d;
}

float CityCore::height_at(float x, float z) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	return state_->ready ? state_->terrain.height_at(x, z) : 0.f;
}

Dictionary CityCore::raycast(const Vector3 &origin, const Vector3 &dir) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	d["hit"] = false;
	if (!state_->ready) return d;
	V2 d2(dir.x, dir.z);
	float l2 = d2.len();
	if (l2 < MIN_RAY_DIRECTION && dir.y >= 0) return d;
	float hx, hy, hz;
	if (!state_->terrain.raycast(V2(origin.x, origin.z), origin.y, d2, dir.y, RAY_MAX_M, hx, hy, hz)) return d;
	d["hit"] = true;
	d["position"] = Vector3(hx, hy, hz);
	return d;
}

Dictionary CityCore::inspect(const Vector3 &p) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const CityState &s = *state_;
	Dictionary d;
	int best = -1;
	float bd = INSPECT_RADIUS_M;
	s.buildings.query(p.x - INSPECT_RADIUS_M, p.z - INSPECT_RADIUS_M, p.x + INSPECT_RADIUS_M, p.z + INSPECT_RADIUS_M, [&](int id) {
		const Building &b = s.buildings.items[id];
		float dd = std::hypot(b.x - p.x, b.z - p.z);
		if (dd < bd) { bd = dd; best = id; }
	});
	if (best >= 0) {
		const Building &b = s.buildings.items[best];
		d["kind"] = "building";
		d["id"] = best;
		d["floors"] = int(b.floors);
		d["zone"] = int(b.zone);
		d["fn"] = int(b.fn);
		return d;
	}
	double t, dist;
	int seg = s.roads.find_segment_near(p.x, p.z, INSPECT_RADIUS_M, t, dist);
	if (seg >= 0) {
		d["kind"] = "road";
		d["id"] = seg;
		d["profile"] = String(s.catalog.profile(s.roads.segs[seg].profile).name_key.c_str());
	}
	return d;
}

Dictionary CityCore::probe_building(int id) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Dictionary d;
	const CityState &s = *state_;
	if (id < 0 || id >= int(s.buildings.items.size()) || !s.buildings.items[id].alive) return d;
	const Building &b = s.buildings.items[id];
	d["position"] = Vector3(b.x, s.terrain.height_at(b.x, b.z), b.z);
	d["w"] = b.w;
	d["d"] = b.d;
	d["rot"] = b.rot;
	d["floors"] = int(b.floors);
	d["zone"] = int(b.zone);
	d["fn"] = int(b.fn);
	return d;
}

Array CityCore::list_buildings() {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	Array a;
	const auto &items = state_->buildings.items;
	for (int i = 0; i < int(items.size()); i++)
		if (items[i].alive) a.push_back(i);
	return a;
}

namespace {

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
	d["reroutes"] = t.reroutes;
	d["released"] = t.released; // jammed-junction releases
	d["governor"] = t.gov;
	d["drawn_vehicles"] = render_.drawn_vehicles();
	d["drawn_people"] = render_.drawn_people();
	d["lane_km"] = t.g.lane_metres / METRES_PER_KM;
	d["connectors"] = int(t.g.conns.size());
	int signals = 0, moving = 0, waiting = 0, on_connectors = 0;
	for (const Junction &j : t.g.junctions) signals += j.ctl == Control::SIGNAL ? 1 : 0;
	Array poses, dirs, held, jams, jam_info, behind;
	held.resize(int(Hold::COUNT));
	held.fill(0);
	behind.resize(int(Hold::COUNT));
	behind.fill(0);
	for (int id = 0; id < int(t.vehicles.size()); id++) {
		const Vehicle &v = t.vehicles[id];
		if (!v.alive) continue;
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
		if (poses.size() < sample) {
			poses.push_back(Vector3(v.cur.x, v.cur.y, v.cur.z));
			dirs.push_back(Vector3(v.cur.fx, 0, v.cur.fz));
		}
	}
	d["signals"] = signals;
	d["busy_crossing"] = busy_crossing(*state_); // the lit junction with the most people waiting or crossing
	d["moving"] = moving;
	d["waiting"] = waiting; // held more than WAITING_S
	d["on_connectors"] = on_connectors;
	d["queued_behind"] = behind; // queued vehicles by what their lane head waits for
	d["held_by"] = held; // waiting vehicles by Hold: queued, wrong lane, red, exit room, conflict busy, yield, connector, free, crosswalk
	d["sample"] = poses;
	d["sample_dir"] = dirs;
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

Dictionary CityCore::city_bounds() const {
	Dictionary d;
	float minx = NO_BOUND, minz = NO_BOUND, maxx = -NO_BOUND, maxz = -NO_BOUND;
	for (const RoadNode &n : state_->roads.nodes) {
		if (!n.alive) continue;
		minx = std::min(minx, float(n.x)); maxx = std::max(maxx, float(n.x));
		minz = std::min(minz, float(n.z)); maxz = std::max(maxz, float(n.z));
	}
	d["min"] = Vector3(minx, 0, minz);
	d["max"] = Vector3(maxx, 0, maxz);
	d["half"] = state_->map_half;
	return d;
}

} // namespace city
