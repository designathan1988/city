// Probes for the tests of what the traffic does (traffic_probe) and of the captures' sites. The render's own probes are city_core_probe_render.cpp.
#include "api/city_core.h"

#include "core/math.h"
#include "core/units.h"
#include "sim/buildings/lot_access.h"
#include "sim/economy/road_economy.h"
#include "sim/roads/geometry.h"
#include "sim/traffic/traffic_system.h"

#include <godot_cpp/variant/array.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

// The Simulation panel's numbers (Roadcraft main.ts currentStatus): the mean speed of the vehicles driven near the camera
// and how many of them stand (the far ones live in lane queues, with no speed of their own).
// The traffic's last publication (TrafficView): the HUD asks four times a second and never waits for a traffic step.
Dictionary CityCore::traffic_metrics() {
	const TrafficView &v = state_->traffic_views.acquire();
	Dictionary d;
	d["mean_speed_ms"] = v.mean_speed_ms;
	d["queued"] = v.queued;
	d["near"] = v.near;
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

namespace {
// The parked side of the traffic: bays and sources read, scenery cars, the lots' cars.
void park_probe(const TrafficState &t, Dictionary &d) {
	const ParkingState &p = t.park;
	Array ms;
	for (double x : t.step_ms) ms.push_back(x);
	d["step_ms"] = ms;
	d["stop_entries"] = t.stop_entries;
	d["stop_skipped"] = t.stop_skipped;
	d["red_last"] = Array::make(t.red_last[0], t.red_last[1], t.red_last[2], t.red_last[3]);
	d["parked_rows"] = int(p.rows.size()); // scenery cars standing in bays near the camera
	d["lot_cars"] = int(p.cars.size());
	d["lot_coming"] = p.coming;
	d["lot_moving"] = p.moving;
	d["lot_called"] = p.called;
	d["lot_parked_in"] = p.parked_in;
	d["lot_left_by"] = p.left_by;
	d["bays"] = int(p.bays.size() - p.free_bays.size());
	d["lots_read"] = int(p.lots.size());
	d["kerbs_read"] = int(p.kerbs.size());
}

// The lane graph: its lane kilometres, the connections that exist (the pool keeps freed entries), the signalled junctions, and
// what the full build's junction sweeps did.
void graph_probe(const TrafficState &t, Dictionary &d) {
	d["lane_km"] = t.g.lane_metres / METRES_PER_KM;
	d["connectors"] = int(std::count_if(t.g.conns.begin(), t.g.conns.end(), [](const Conn &c) { return c.alive; }));
	d["signals"] = int(std::count_if(t.g.junctions.begin(), t.g.junctions.end(), [](const Junction &j) { return j.ctl == Control::SIGNAL; }));
	d["sweep_pairs"] = int64_t(t.g.sweep_pairs); // connector pairs looked at
	d["sweep_tests"] = int64_t(t.g.sweep_tests); // ... and the body rectangles tested for them
}

// The street furniture of the whole map: pieces by kind (signal, lamp, hydrant, bin, bench, tree), and those standing in front
// of a lot's gate. Every segment is placed again, so the tests ask for it only with a sample.
void furniture_probe(const CityState &s, Dictionary &d) {
	d["street_furniture"] = furniture_counts(s);
	d["furniture_in_gates"] = traffic::furniture_in_gates(s, false); // pieces standing in front of a lot's gate
	d["signals_in_gates"] = traffic::furniture_in_gates(s, true); // ... of them signal posts, which the signals' rule places
	GateOnJunction first;
	d["gates_on_junctions"] = gates_on_junctions(s.roads, s.buildings, s.buildings.designs, &first); // lots' gates opening onto a junction's kerb
	// lot, kind, x, z, street, arc; the gate's middle and width along the front; the lot's shut stretches and street sides
	Array named = Array::make(first.lot, first.kind, first.x, first.z, first.street, first.at, first.front_x, first.w);
	for (int kind = 0; kind < b40::LotEdges::KINDS; kind++)
		for (int k = 0; k < b40::LotEdges::SPANS; k++) named.append_array(Array::make(first.edges.from[kind][k], first.edges.to[kind][k]));
	named.push_back(int(first.edges.street));
	d["gates_on_junctions_first"] = named;
	d["car_gates_on_junctions"] = first.cars; // of them, cars' gates
}
} // namespace

static void push_sample(const TrafficState &t, const Vehicle &v, Array &poses, Array &dirs, Array &types, Array &mids, Array &speeds) {
	poses.push_back(Vector3(v.cur.x, v.cur.y, v.cur.z));
	dirs.push_back(Vector3(v.cur.fx, 0, v.cur.fz));
	types.push_back(int(v.type));
	mids.push_back(v.off || v.conn >= 0 ? 0.f : std::min(v.s, t.g.ds[size_t(v.ds)].len - v.s));
	speeds.push_back(v.v);
}

Dictionary CityCore::traffic_probe(int sample) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const TrafficState &t = state_->traffic;
	Dictionary d;
	d["vehicles"] = t.vehicle_count + t.park.moving; // driving: on the lanes or in and out of the bays
	park_probe(t, d);
	d["people"] = t.ped_count;
	d["trips_arrived"] = t.arrived;
	d["trips_lost"] = t.lost;
	d["lost_stuck"] = t.stuck_lost; // of which: removed after standing stuck_despawn_s
	d["kerb"] = Array::make(t.kerb_planned, t.kerb_drops, t.kerb_picks, t.kerb_deliveries, t.kerb_abandoned); // at the kerb: stops planned; set down, picked up, delivered; given up
	d["people_arrived"] = t.ped_arrived;
	d["people_waiting"] = t.ped_waiting; // at a kerb for the walk phase or a gap
	d["people_overlaps"] = people_overlaps(t, 2 * state_->catalog.traffic.ped_body_m - TOUCH_EPS_M); // walkers inside another's body: walking, waiting
	d["people_in_furniture"] = traffic::walkers_in_furniture(*state_); // walkers whose body stands in a street piece
	if (sample > 0) furniture_probe(*state_, d);
	d["reroutes"] = t.reroutes;
	d["released"] = t.released; // jammed-junction releases
	d["governor"] = t.gov;
	int on_lanes = 0; // drawn and driven on the lanes near the camera (the window's)
	for (const Vehicle &v : t.vehicles) on_lanes += v.alive && v.micro && !v.off ? 1 : 0;
	const double window_m = traffic::window_lane_metres(*state_);
	d["window"] = Array::make(window_m, traffic::demand_for(*state_, window_m), on_lanes); // its lane metres, what they carry now, how many drive there
	d["drawn_vehicles"] = render_.drawn_vehicles();
	d["drawn_people"] = render_.drawn_people();
	d["drawn_signal_lenses"] = render_.lit_lenses(); // lit on the signal posts, from the published lights
	graph_probe(t, d);
	int moving = 0, waiting = 0, on_connectors = 0;
	Array poses, dirs, types, mids, speeds, held, jams, jam_info, behind, box;
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
		if (poses.size() < sample) push_sample(t, v, poses, dirs, types, mids, speeds);
	}
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
	d["sample_mid_m"] = mids; // how far along its lane from either end; 0 on a connector or off the lanes
	d["sample_speed"] = speeds;
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
