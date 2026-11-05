// The lane graph (Roadcraft src/sim/lanes.ts): directed segments with their lanes between the two mouths, lane
// connectors through every node (turn classes, lanes allowed by position, pairing) and their conflict sets. A road
// edit rebuilds only the segments and nodes it touched (lane_graph_edit.cpp); build_lane_graph is the full build of a
// new map or a load.
#include "sim/traffic/lane_graph_internal.h"

#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float MIN_LANE_M = 0.5f;
constexpr float UTURN_NODE_PAD_M = 3.f; // dead-end U-turn: the control point goes this far past the node, doubled

// Vehicles and meso entries of one lane leave the simulation.
void drop_one_lane_state(CityState &s, int key) {
	TrafficState &t = s.traffic;
	if (key < 0 || key >= int(t.lane_vehicles.size())) return;
	while (!t.lane_vehicles[size_t(key)].empty()) despawn_vehicle(s, t.lane_vehicles[size_t(key)].back(), false);
	while (size_t(key) < t.meso_head.size() && t.meso_head[size_t(key)] >= 0) despawn_vehicle(s, t.meso_head[size_t(key)], false);
	if (size_t(key) < t.g.lane_conn.size()) t.g.lane_conn[size_t(key)] = -1;
}

Turn classify(const V2 &h, const V2 &o, bool same, int degree, float straight_dot) {
	if (same) return Turn::UTURN;
	if (degree == 2 || h.dot(o) > straight_dot) return Turn::STRAIGHT;
	return o.dot(V2(h.y, -h.x)) > 0 ? Turn::LEFT : Turn::RIGHT;
}

bool lane_allows(int lane, int n, Turn t) {
	if (t == Turn::UTURN) return lane == n - 1;
	if (n == 1 || t == Turn::STRAIGHT) return true;
	return t == Turn::RIGHT ? lane == 0 : lane == n - 1;
}

void make_curve(const CityState &s, Conn &c, const TrafficParams &tp) {
	Pose a, b;
	lane_pose(s, c.from_ds, c.from_lane, s.traffic.g.ds[c.from_ds].len, a);
	lane_pose(s, c.to_ds, c.to_lane, 0.f, b);
	c.x0 = a.x; c.z0 = a.z; c.y0 = a.y;
	c.x1 = b.x; c.z1 = b.z; c.y1 = b.y;
	V2 p0(a.x, a.z), p1(b.x, b.z), h(a.fx, a.fz), o(b.fx, b.fz), ctrl = (p0 + p1) * 0.5f;
	float t1, t2;
	if (c.turn == Turn::UTURN) {
		const RoadNode &n = s.roads.nodes[c.node];
		float to_node = (V2(float(n.x), float(n.z)) - p0).dot(h);
		ctrl += h * std::max(tp.uturn_push_m, 2.f * (to_node + UTURN_NODE_PAD_M));
	} else if (line_line(p0, h, p1, o, t1, t2) && t1 > 0 && t1 < tp.control_max_m && t2 < 0 && -t2 < tp.control_max_m) {
		ctrl = p0 + h * t1;
	}
	c.cx = ctrl.x;
	c.cz = ctrl.y;
	c.table[0] = 0;
	V2 prev = p0;
	for (int i = 1; i <= CONN_STEPS; i++) {
		float t = float(i) / CONN_STEPS, r = 1 - t;
		V2 q = p0 * (r * r) + ctrl * (2 * r * t) + p1 * (t * t);
		c.table[i] = c.table[i - 1] + (q - prev).len();
		prev = q;
	}
	c.len = std::max(c.table[CONN_STEPS], MIN_LANE_M);
}

// Connections of one turn class from `din` to `dout`, allocated from the pool and threaded onto the node and lane lists.
void link_pair(CityState &s, int node, int din, int dout, Turn turn, std::vector<int> &made) {
	LaneGraph &g = s.traffic.g;
	const DirSeg &di = g.ds[din], &do_ = g.ds[dout];
	std::vector<int> ok;
	for (int l = 0; l < di.lanes; l++)
		if (lane_allows(l, di.lanes, turn)) ok.push_back(l);
	const int k = do_.lanes, m = int(ok.size()), key0 = din * MAX_LANES;
	for (int r = 0; r < m; r++) {
		const int ci = alloc_conn(s);
		Conn &c = g.conns[size_t(ci)];
		c = Conn();
		c.alive = true;
		c.node = node;
		c.from_ds = din;
		c.from_lane = ok[r];
		c.to_ds = dout;
		c.turn = turn;
		bool from_left = turn == Turn::LEFT || turn == Turn::UTURN;
		c.to_lane = from_left ? std::max(0, k - 1 - std::min(m - 1 - r, k - 1)) : std::min(r, k - 1);
		make_curve(s, c, s.catalog.traffic);
		const int key = key0 + c.from_lane;
		c.next_at_node = g.node_conn[size_t(node)];
		c.next_from = g.lane_conn[size_t(key)];
		g.node_conn[size_t(node)] = ci;
		g.lane_conn[size_t(key)] = ci;
		made.push_back(ci);
	}
}

// Conflicts among the connections a node just made (and only among them: other nodes keep their own).
void find_conflicts(LaneGraph &g, const std::vector<int> &ids, int samples) {
	std::vector<std::vector<V2>> poly;
	poly.reserve(ids.size());
	for (int ci : ids) {
		std::vector<V2> pts;
		for (int k = 0; k <= samples; k++) {
			Pose p;
			conn_pose(g.conns[size_t(ci)], g.conns[size_t(ci)].len * float(k) / samples, p);
			pts.push_back(V2(p.x, p.z));
		}
		poly.push_back(pts);
	}
	for (size_t i = 0; i < ids.size(); i++)
		for (size_t j = i + 1; j < ids.size(); j++) {
			Conn &a = g.conns[size_t(ids[i])], &b = g.conns[size_t(ids[j])];
			if (a.from_ds == b.from_ds && a.from_lane == b.from_lane) continue;
			bool merge = a.to_ds == b.to_ds && a.to_lane == b.to_lane;
			float ta = 0, tb = 0;
			int xa = -1, yb = -1;
			const auto &pa = poly[i], &pb = poly[j];
			for (int x = 0; x < samples && xa < 0 && a.from_ds != b.from_ds; x++)
				for (int y = 0; y < samples && xa < 0; y++)
					if (segments_cross(pa[x], pa[x + 1], pb[y], pb[y + 1], &ta)) {
						segments_cross(pb[y], pb[y + 1], pa[x], pa[x + 1], &tb);
						xa = x;
						yb = y;
					}
			if (!merge && xa < 0) continue;
			// where each path crosses the other (a merge meets at the end of both)
			float at_a = merge ? a.len : a.len * (xa + ta) / samples, at_b = merge ? b.len : b.len * (yb + tb) / samples;
			a.conflicts.push_back(ids[j]);
			a.conflict_at.push_back(at_b);
			b.conflicts.push_back(ids[i]);
			b.conflict_at.push_back(at_a);
		}
}

} // namespace

// How far across a bike strip lies from lane 0 at `lane_u`, on the kerb side of direction `dir` (0: none there).
static float bike_strip_shift(const Profile &p, float lane_u, int dir) {
	float best = 0;
	for (const StripEdge &e : p.edges) {
		if (e.strip.kind != StripKind::BIKE) continue;
		const float shift = (e.u0 + e.u1) * 0.5f - lane_u;
		const bool kerb_side = dir == 0 ? shift < 0 : shift > 0;
		if (kerb_side && (best == 0 || std::fabs(shift) < std::fabs(best))) best = shift;
	}
	return best;
}

void build_dir(CityState &s, int seg) {
	LaneGraph &g = s.traffic.g;
	const RoadNetwork &net = s.roads;
	if (seg < 0 || seg >= int(net.segs.size()) || seg * 2 + 1 >= int(g.ds.size())) return;
	for (int dir = 0; dir < 2; dir++) {
		DirSeg &d = g.ds[size_t(seg) * 2 + dir];
		if (d.ok) g.lane_metres -= double(d.len) * d.lanes;
		d = DirSeg();
		const RoadSegment &sg = net.segs[size_t(seg)];
		if (!sg.alive || sg.a == sg.b) continue;
		const Profile &p = net.profile_of(seg);
		if (p.rail) continue;
		const std::vector<LaneInfo> &lanes = dir == 0 ? p.fwd : p.back;
		if (lanes.empty()) continue;
		const float len = std::max(MIN_LANE_M, float(sg.len - sg.retreat_a - sg.retreat_b));
		d.ok = true;
		d.seg = seg;
		d.from = dir == 0 ? sg.a : sg.b;
		d.to = dir == 0 ? sg.b : sg.a;
		d.lanes = std::min(int(lanes.size()), MAX_LANES);
		for (int k = 0; k < d.lanes; k++) d.u[k] = lanes[k].u;
		d.bikes = p.walk_left > 0 || p.walk_right > 0;
		d.cycle_shift = bike_strip_shift(p, d.u[0], dir);
		if (d.cycle_shift == 0 && d.bikes) d.edge_shift = (dir == 0 ? -1.f : 1.f) * std::max(0.f, lanes[0].w * 0.5f - s.catalog.traffic.bike_edge_m);
		d.len = len;
		d.speed = p.speed / KMH_PER_MS;
		d.at0 = float(dir == 0 ? sg.retreat_a : sg.len - sg.retreat_b);
		d.sign = dir == 0 ? 1.f : -1.f;
		Pose end;
		lane_pose(s, seg * 2 + dir, 0, len, end);
		d.ex = end.x;
		d.ez = end.z;
		g.lane_metres += double(len) * d.lanes;
	}
	// lanes a narrower profile no longer has: their vehicles go and their connections are already gone
	for (int dir = 0; dir < 2; dir++)
		for (int lane = g.ds[size_t(seg) * 2 + dir].lanes; lane < MAX_LANES; lane++)
			drop_one_lane_state(s, (seg * 2 + dir) * MAX_LANES + lane);
}

void drop_lane_state(CityState &s, int ds) {
	for (int lane = 0; lane < MAX_LANES; lane++) drop_one_lane_state(s, ds * MAX_LANES + lane);
}

void build_node(CityState &s, int n) {
	LaneGraph &g = s.traffic.g;
	const RoadNetwork &net = s.roads;
	if (n < 0 || n >= int(net.nodes.size()) || n >= int(g.node_conn.size()) || !net.nodes[n].alive) return;
	std::vector<int> arms;
	for (int si : net.nodes[n].segs)
		if (si * 2 + 1 < int(g.ds.size()) && (g.ds[si * 2].ok || g.ds[si * 2 + 1].ok)) arms.push_back(si);
	const int degree = int(arms.size());
	std::vector<int> made;
	for (int si : arms) {
		int din = in_ds(net, si, n);
		if (!g.ds[din].ok) continue;
		Pose ph;
		lane_pose(s, din, 0, g.ds[din].len, ph);
		for (int so : arms) {
			if (so == si && degree > 1) continue;
			int dout = out_ds(net, so, n);
			if (!g.ds[dout].ok) continue;
			Pose po;
			lane_pose(s, dout, 0, 0.f, po);
			Turn t = classify(V2(ph.fx, ph.fz), V2(po.fx, po.fz), so == si, degree, s.catalog.traffic.straight_dot);
			link_pair(s, n, din, dout, t, made);
		}
	}
	if (degree >= 2) find_conflicts(g, made, std::max(2, int(s.catalog.traffic.conflict_samples)));
}

void build_lane_graph(CityState &s) {
	LaneGraph &g = s.traffic.g;
	g = LaneGraph();
	g.version = s.roads.version;
	g.ds.assign(s.roads.segs.size() * 2, DirSeg());
	g.node_conn.assign(s.roads.nodes.size(), -1);
	g.lane_conn.assign(g.ds.size() * MAX_LANES, -1);
	g.lane_metres = 0;
	for (int seg = 0; seg < int(s.roads.segs.size()); seg++) build_dir(s, seg);
	for (int n = 0; n < int(s.roads.nodes.size()); n++)
		if (s.roads.nodes[n].alive) build_node(s, n);
	ensure_conn_state(s.traffic);
	build_junctions(s);
}

} // namespace traffic
} // namespace city
