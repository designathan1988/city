// Incremental lane graph (Roadcraft lanes.ts dirtyNodes/rebuildNode/clearNode): a road edit marks the segments and
// nodes it touched in the ChangeLog; traffic::step drains them here, rebuilding only those, and keeps the fleet and the
// walkers on the streets that are still there. The walkers' street furniture of those segments, of every segment of
// those nodes (their mouths, lamp stations and signal posts move) and of the segments the log names for it (a hydrant
// or a bin moved) is built again when next met. What the traffic keys by a record id goes here with the record, since an id
// freed is the next record's: a lot's bays and the kerbs its gates kept, by the box it stood in when it changed, and the
// flows and control choices of a removed road or node (flow_control.h forget_flows).
#include "sim/traffic/lane_graph_internal.h"

#include "sim/traffic/flow_control.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {

// Takes `c` out of the list of connections leaving its lane of origin.
void unlink_from_lane(LaneGraph &g, int ci) {
	Conn &c = g.conns[size_t(ci)];
	const int key = c.from_ds * MAX_LANES + c.from_lane;
	if (key < 0 || key >= int(g.lane_conn.size())) return;
	int prev = -1;
	for (int x = g.lane_conn[size_t(key)]; x >= 0; x = g.conns[size_t(x)].next_from) {
		if (x != ci) { prev = x; continue; }
		if (prev < 0) g.lane_conn[size_t(key)] = g.conns[size_t(x)].next_from;
		else g.conns[size_t(prev)].next_from = g.conns[size_t(x)].next_from;
		return;
	}
}

// A connection is going: its vehicles move to the start of the arrival lane, or leave the scene when it is gone.
void on_conn_removed(CityState &s, int ci) {
	TrafficState &t = s.traffic;
	LaneGraph &g = t.g;
	Conn &c = g.conns[size_t(ci)];
	auto &on = t.conn_vehicles[size_t(ci)];
	while (!on.empty()) {
		const int id = on.back();
		on.pop_back();
		Vehicle &v = t.vehicles[size_t(id)];
		const bool lane_ok = c.to_ds >= 0 && c.to_ds < int(g.ds.size()) && g.ds[c.to_ds].ok &&
				c.to_lane >= 0 && c.to_lane < g.ds[c.to_ds].lanes;
		if (!v.alive) continue;
		if (!lane_ok) { despawn_vehicle(s, id, false); continue; }
		v.conn = -1;
		v.ds = c.to_ds;
		v.lane = c.to_lane;
		v.s = 0;
		v.cs = 0;
		v.pass = -1;
		lane_add(s, c.to_ds * MAX_LANES + c.to_lane, id);
	}
	conn_clear(s, ci);
	for (int id : t.micro_list)
		if (t.vehicles[size_t(id)].alive && t.vehicles[size_t(id)].pass == ci) t.vehicles[size_t(id)].pass = -1;
	unlink_from_lane(g, ci);
	c.conflicts.clear();
	c.conflict_clear.clear();
	c.forks.clear();
	c.fork_clear.clear();
	c.alive = false;
	if (size_t(ci) < t.conn_pass.size()) t.conn_pass[size_t(ci)] = -1;
}

// A segment is gone: its lanes' vehicles and walkers leave; the connections are dropped by its (dirty) nodes.
void drop_segment(CityState &s, int seg) {
	LaneGraph &g = s.traffic.g;
	if (seg < 0 || seg * 2 + 1 >= int(g.ds.size())) return;
	for (int dir = 0; dir < 2; dir++) {
		DirSeg &d = g.ds[size_t(seg) * 2 + dir];
		if (d.ok) g.lane_metres -= double(d.len) * d.lanes;
		drop_lane_state(s, seg * 2 + dir);
		d = DirSeg();
	}
	drop_pedestrians_on_seg(s, seg);
}

} // namespace

int alloc_conn(CityState &s) {
	LaneGraph &g = s.traffic.g;
	int ci;
	if (!g.free_conns.empty()) {
		ci = g.free_conns.back();
		g.free_conns.pop_back();
	} else {
		ci = int(g.conns.size());
		g.conns.emplace_back();
	}
	ensure_conn_state(s.traffic);
	Conn &c = g.conns[size_t(ci)];
	c = Conn();
	c.alive = true;
	s.traffic.conn_pass[size_t(ci)] = -1;
	s.traffic.conn_pass_t[size_t(ci)] = 0;
	s.traffic.conn_claim_t[size_t(ci)] = -double(BIG);
	s.traffic.conn_vehicles[size_t(ci)].clear();
	return ci;
}

void ensure_conn_state(TrafficState &t) {
	const size_t n = t.g.conns.size();
	if (t.conn_vehicles.size() < n) t.conn_vehicles.resize(n);
	if (t.conn_pass.size() < n) t.conn_pass.resize(n, -1);
	if (t.conn_pass_t.size() < n) t.conn_pass_t.resize(n, 0.0);
	if (t.conn_claim_t.size() < n) t.conn_claim_t.resize(n, -double(BIG));
	if (t.conn_active.size() < n) t.conn_active.resize(n, 0);
}

void graph_grow(CityState &s, int segs, int nodes) {
	TrafficState &t = s.traffic;
	LaneGraph &g = t.g;
	const size_t ds = size_t(std::max(segs, 0)) * 2, lanes = ds * MAX_LANES, nn = size_t(std::max(nodes, 0));
	if (g.ds.size() < ds) g.ds.resize(ds);
	if (g.lane_conn.size() < lanes) g.lane_conn.resize(lanes, -1);
	if (g.node_conn.size() < nn) g.node_conn.resize(nn, -1);
	if (g.junctions.size() < nn) g.junctions.resize(nn);
	if (g.signal_at.size() < nn) g.signal_at.resize(nn, 0);
	if (t.lane_vehicles.size() < lanes) t.lane_vehicles.resize(lanes);
	if (t.lane_sorted.size() < lanes) t.lane_sorted.resize(lanes, 1);
	if (t.lane_active.size() < lanes) t.lane_active.resize(lanes, 0);
	if (t.meso_head.size() < lanes) t.meso_head.resize(lanes, -1);
	if (t.meso_tail.size() < lanes) t.meso_tail.resize(lanes, -1);
	if (t.meso_count.size() < lanes) t.meso_count.resize(lanes, 0);
	if (t.lane_last_exit.size() < lanes) t.lane_last_exit.resize(lanes, 0.0);
	if (t.exit_claim.size() < lanes) t.exit_claim.resize(lanes, -1);
	if (t.exit_claim_t.size() < lanes) t.exit_claim_t.resize(lanes, 0.0);
	if (t.light_now.size() < ds) t.light_now.resize(ds, uint8_t(Light::GREEN));
	if (t.node_crossers.size() < nn) t.node_crossers.resize(nn);
	if (t.last_admit.size() < nn) t.last_admit.resize(nn, t.time);
	if (t.release_at.size() < nn) t.release_at.resize(nn, 0.0);
	if (t.pass_count.size() < nn) t.pass_count.resize(nn, 0);
	const size_t arms = nn * NODE_ARMS_MAX * CROSS_CACHE_KINDS;
	if (t.cross_stamp.size() < arms) {
		t.cross_stamp.resize(arms, STAMP_NONE);
		t.cross_walk.resize(arms, 0);
		t.cross_cars.resize(arms, 0);
	}
}

void clear_node(CityState &s, int n) {
	LaneGraph &g = s.traffic.g;
	if (n < 0 || n >= int(g.node_conn.size())) return;
	int ci = g.node_conn[size_t(n)];
	while (ci >= 0) {
		const int next = g.conns[size_t(ci)].next_at_node;
		on_conn_removed(s, ci);
		g.free_conns.push_back(ci);
		g.conns[size_t(ci)] = Conn();
		ci = next;
	}
	g.node_conn[size_t(n)] = -1;
	if (n < int(g.junctions.size())) g.junctions[size_t(n)] = Junction();
	file_signal(g, n);
}

void apply_network_changes(CityState &s) {
	TrafficState &t = s.traffic;
	// the step holds the map shared: the net lists are taken out under the log's own mutex (change_log.h)
	std::vector<int> nodes, removed, changed, furniture, lot_ids, signals, gone_nodes;
	std::vector<LotBox> lots;
	{
		std::lock_guard<std::mutex> lk(s.log.mutex);
		nodes.swap(s.log.net_nodes);
		removed.swap(s.log.net_segs_removed);
		changed.swap(s.log.net_segs_changed);
		gone_nodes.swap(s.log.net_nodes_removed);
		furniture.swap(s.log.furniture_segs);
		lot_ids.swap(s.log.lots_changed); // the same buildings as `lots` (which carry their boxes): the checks' scope until drained
		lots.swap(s.log.lot_fronts);
		signals.swap(s.log.signal_nodes);
	}
	ChangeLog::dedup(signals);
	for (int n : signals) replan_signal(s, n);
	std::sort(lots.begin(), lots.end());
	lots.erase(std::unique(lots.begin(), lots.end()), lots.end());
	// the streets a changed lot may face: their furniture is laid off its gates (street_furniture.h), so it is met again
	const float reach = s.catalog.traffic.kerb_gate_reach_m;
	for (const LotBox &l : lots)
		s.roads.for_segments_near(l.box.x0 - reach, l.box.z0 - reach, l.box.x1 + reach, l.box.z1 + reach, [&](int seg) { furniture.push_back(seg); });
	ChangeLog::dedup(furniture);
	for (int seg : furniture) {
		forget_furniture(s, seg);
		forget_doors(s, seg);
	}
	for (const LotBox &l : lots) forget_parking_lot(s, l.id, l.box);
	ChangeLog::dedup(removed);
	ChangeLog::dedup(changed);
	ChangeLog::dedup(nodes);
	ChangeLog::dedup(gone_nodes);
	forget_flows(s, removed, nodes, gone_nodes);
	if (nodes.empty() && removed.empty() && changed.empty()) {
		t.g.version = s.roads.version;
		return;
	}
	graph_grow(s, int(s.roads.segs.size()), int(s.roads.nodes.size()));
	forget_yards_near(s, removed, changed); // their ways cross the streets: those of the streets that changed are planned again
	for (int seg : removed) drop_segment(s, seg);
	for (int seg : removed)
		if (size_t(seg) < t.seg_volume.size()) t.seg_volume[size_t(seg)] = 0;
	for (int seg : removed) {
		forget_furniture(s, seg);
		forget_parking_seg(s, seg);
		forget_doors(s, seg);
	}
	for (int seg : changed) {
		forget_furniture(s, seg);
		forget_parking_seg(s, seg);
		forget_doors(s, seg);
	}
	for (int n : nodes) {
		if (n < 0 || n >= int(s.roads.nodes.size())) continue;
		for (int seg : s.roads.nodes[size_t(n)].segs) {
			forget_furniture(s, seg);
			forget_doors(s, seg); // a door's foot is placed clear of that furniture (ped_doors.cpp)
			forget_parking_seg(s, seg);
		}
	}
	for (int n : nodes) clear_node(s, n);
	for (int seg : changed) build_dir(s, seg);
	for (int n : nodes) {
		if (n < 0 || n >= int(s.roads.nodes.size())) continue;
		if (s.roads.nodes[n].alive) {
			build_node(s, n);
			build_junction(s, n);
		} else if (n < int(t.g.junctions.size())) {
			t.g.junctions[size_t(n)] = Junction();
			file_signal(t.g, n);
		}
	}
	t.g.version = s.roads.version;
	s.lane_graph_serial++;
	if constexpr (CHECKS) { // the tests' DLL (core/checks.h); the scoped traffic.lane_graph check follows at the batch's end
		if (t.graph_fault.empty()) t.graph_fault = lane_graph_fault(s);
	}
}

} // namespace traffic
} // namespace city
