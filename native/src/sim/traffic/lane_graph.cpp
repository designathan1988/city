// The lane graph (Roadcraft src/sim/lanes.ts): directed segments with their lanes between the two mouths, lane
// connectors through every node (turn classes, lanes allowed by position, pairing) and their conflict sets (junction_sweeps.cpp). A road
// edit rebuilds only the segments and nodes it touched (lane_graph_edit.cpp); build_lane_graph is the full build of a
// new map or a load.
#include "sim/traffic/lane_graph_internal.h"

#include "core/job_pool.h"

#include "sim/traffic/traffic_system.h"

#include "core/units.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float SHORTEST_PIECE_M = 0.5f;
constexpr float UTURN_NODE_PAD_M = 3.f; // dead-end U-turn: the control point goes this far past the node, doubled

// Vehicles and meso entries of one lane leave the simulation.
void drop_one_lane_state(CityState &s, int key) {
	TrafficState &t = s.traffic;
	if (key < 0 || key >= int(t.lane_vehicles.size())) return;
	while (!t.lane_vehicles[size_t(key)].empty()) despawn_vehicle(s, t.lane_vehicles[size_t(key)].back(), false);
	while (size_t(key) < t.meso_head.size() && t.meso_head[size_t(key)] >= 0) despawn_vehicle(s, t.meso_head[size_t(key)], false);
	if (size_t(key) < t.g.lane_conn.size()) t.g.lane_conn[size_t(key)] = -1;
}

constexpr float FLAT_CURVE = 1e-6f; // a tangent shorter than this has no curvature to read

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
	c.len = c.table[CONN_STEPS]; // its curve's own: a vehicle never drives length the curve does not have (a road joint's is short)
	const V2 second = (p0 - ctrl * 2.f + p1) * 2.f; // the curve's second derivative, the same all along it
	c.bend = 0;
	for (int i = 0; i <= CONN_STEPS; i++) {
		const float t = float(i) / CONN_STEPS;
		const V2 first = (ctrl - p0) * (2 * (1 - t)) + (p1 - ctrl) * (2 * t);
		const float l = first.len();
		if (l > FLAT_CURVE) c.bend = std::max(c.bend, std::fabs(first.x * second.y - first.y * second.x) / (l * l * l));
	}
}

// Where the segment's curve bends, for directed segment `d` of it (DirSeg::bend): the centre line's greatest curvature, signed
// left of `d`'s travel, and the stretch of `d` at or over the kept share of it; none under the least a driver slows for.
void set_bend(const RoadSegment &sg, const TrafficParams &tp, DirSeg &d) {
	const int n = int(sg.table.size()) - 1;
	if (n < 1) return;
	const double ax = 2 * (sg.q.x1 - 2 * sg.q.cx + sg.q.x0), az = 2 * (sg.q.z1 - 2 * sg.q.cz + sg.q.z0);
	static thread_local std::vector<float> k;
	k.assign(size_t(n) + 1, 0.f);
	float peak = 0;
	for (int i = 0; i <= n; i++) {
		double dx, dz;
		sg.q.deriv(double(i) / n, dx, dz);
		const double l = std::hypot(dx, dz);
		if (l < FLAT_CURVE) continue;
		k[size_t(i)] = float((dz * ax - dx * az) / (l * l * l)); // left of a -> b is (dz, -dx): positive turning left
		peak = std::max(peak, std::fabs(k[size_t(i)]));
	}
	if (peak < tp.bend_least) return;
	int first = -1, last = -1, at = 0;
	for (int i = 0; i <= n; i++) {
		if (std::fabs(k[size_t(i)]) < tp.bend_keep_share * peak) continue;
		if (first < 0) first = i;
		last = i;
		if (std::fabs(k[size_t(i)]) >= std::fabs(k[size_t(at)])) at = i;
	}
	// this direction's s runs from at0 along `sign`
	const float one = (float(sg.table[size_t(first)]) - d.at0) * d.sign, two = (float(sg.table[size_t(last)]) - d.at0) * d.sign;
	d.bend = k[size_t(at)] * d.sign;
	d.bend_from = clampf(std::min(one, two), 0.f, d.len);
	d.bend_to = clampf(std::max(one, two), 0.f, d.len);
}

// One connection, allocated from the pool and threaded onto the node and lane lists.
void add_conn(CityState &s, int node, int din, int from_lane, int dout, int to_lane, Turn turn, std::vector<int> &made) {
	LaneGraph &g = s.traffic.g;
	const int ci = alloc_conn(s);
	Conn &c = g.conns[size_t(ci)];
	c = Conn();
	c.alive = true;
	c.node = node;
	c.from_ds = din;
	c.from_lane = from_lane;
	c.to_ds = dout;
	c.to_lane = to_lane;
	c.turn = turn;
	make_curve(s, c, s.catalog.traffic);
	const int key = din * MAX_LANES + from_lane;
	c.next_at_node = g.node_conn[size_t(node)];
	c.next_from = g.lane_conn[size_t(key)];
	g.node_conn[size_t(node)] = ci;
	g.lane_conn[size_t(key)] = ci;
	made.push_back(ci);
}

// Whether a hand-made link still stands: both roads at the node, both lanes on them.
bool link_stands(const CityState &s, int node, const LaneLink &l) {
	const LaneGraph &g = s.traffic.g;
	const RoadNetwork &net = s.roads;
	const std::vector<int> &segs = net.nodes[size_t(node)].segs;
	if (std::find(segs.begin(), segs.end(), l.from) == segs.end() || std::find(segs.begin(), segs.end(), l.to) == segs.end()) return false;
	const DirSeg &in = g.ds[size_t(in_ds(net, l.from, node))], &out = g.ds[size_t(out_ds(net, l.to, node))];
	return in.ok && out.ok && l.from_lane >= 0 && l.to_lane >= 0 && l.from_lane < in.lanes && l.to_lane < out.lanes;
}

// Whether the player said where this arriving lane goes.
bool lane_by_hand(const CityState &s, int node, int seg, int lane) {
	for (const LaneLink &l : s.roads.nodes[size_t(node)].links)
		if (l.from == seg && l.from_lane == lane && link_stands(s, node, l)) return true;
	return false;
}

// The turns painted on the lanes of an arriving direction that hold at this node: a lane's paint binds where the node
// offers one of its turns (`offered`: the turns to the other roads there, as PAINTED_ bits); a lane painted for turns
// the node does not have, and every lane at a node that only carries the road on, goes by its place.
struct LanePaint {
	uint8_t turns[MAX_LANES] = {};
	LanePaint(const DirSeg &d, uint8_t offered, int degree) {
		for (int l = 0; l < d.lanes && degree > 2; l++)
			if (d.turns[l] & offered) turns[l] = d.turns[l];
	}
	// Whether lane `l` of `n` makes turn `t`: by its paint where it binds (a U-turn is never painted: by place always).
	bool takes(int l, int n, Turn t) const {
		if (turns[l] == 0 || t == Turn::UTURN) return lane_allows(l, n, t);
		return (turns[l] & (t == Turn::LEFT ? PAINTED_LEFT : t == Turn::RIGHT ? PAINTED_RIGHT : PAINTED_STRAIGHT)) != 0;
	}
};

uint8_t painted_bit(Turn t) { return t == Turn::LEFT ? PAINTED_LEFT : t == Turn::RIGHT ? PAINTED_RIGHT : t == Turn::STRAIGHT ? PAINTED_STRAIGHT : 0; }

// The lane of `d` nearest `lane` that is not the buses' alone; `lane` itself when every lane is.
int general_lane(const DirSeg &d, int lane) {
	for (int off = 0; off < d.lanes; off++)
		for (int at : { lane - off, lane + off })
			if (at >= 0 && at < d.lanes && !d.for_buses(at)) return at;
	return lane;
}

// Connections of one turn class from `din` to `dout`. A lane that is not the buses' is never led into one that is
// while the road it turns into has a general lane: a driver enters a bus lane to turn or to reach a lot, never to
// travel (the Brazilian traffic code, art. 184, I: an exclusive lane on the right is entered only to reach a property
// beside it or to turn right; its words are in traffic.json lane_entry_m).
void link_pair(CityState &s, int node, int din, int dout, Turn turn, const LanePaint &paint, std::vector<int> &made) {
	LaneGraph &g = s.traffic.g;
	const DirSeg &di = g.ds[din], &do_ = g.ds[dout];
	std::vector<int> ok;
	for (int l = 0; l < di.lanes; l++)
		if (paint.takes(l, di.lanes, turn) && !lane_by_hand(s, node, di.seg, l)) ok.push_back(l);
	const int k = do_.lanes, m = int(ok.size());
	for (int r = 0; r < m; r++) {
		bool from_left = turn == Turn::LEFT || turn == Turn::UTURN;
		const int by_place = from_left ? std::max(0, k - 1 - std::min(m - 1 - r, k - 1)) : std::min(r, k - 1);
		add_conn(s, node, din, ok[r], dout, di.for_buses(ok[r]) ? by_place : general_lane(do_, by_place), turn, made);
	}
}

} // namespace

void links_built(const CityState &s, int node, std::vector<LaneLink> &out) {
	out.clear();
	const LaneGraph &g = s.traffic.g;
	if (node < 0 || node >= int(g.node_conn.size())) return;
	for (int c = g.node_conn[size_t(node)]; c >= 0; c = g.conns[size_t(c)].next_at_node) {
		const Conn &k = g.conns[size_t(c)];
		out.push_back({ g.ds[size_t(k.from_ds)].seg, k.from_lane, g.ds[size_t(k.to_ds)].seg, k.to_lane });
	}
}

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
		const float len = std::max(SHORTEST_PIECE_M, float(sg.len - sg.retreat_a - sg.retreat_b));
		d.ok = true;
		d.seg = seg;
		d.from = dir == 0 ? sg.a : sg.b;
		d.to = dir == 0 ? sg.b : sg.a;
		d.lanes = std::min(int(lanes.size()), MAX_LANES);
		for (int k = 0; k < d.lanes; k++) {
			d.u[k] = lanes[k].u;
			d.turns[k] = lanes[k].turns;
			if (lanes[k].bus) d.bus_only |= uint8_t(1 << k);
			// the line between lane k and the one inside it: continuous where the inner lane says so, or one is the buses'
			if (k + 1 < d.lanes && (lanes[k + 1].solid || lanes[k].bus != lanes[k + 1].bus)) d.barred |= uint8_t(1 << k);
		}
		d.bikes = p.walk_left > 0 || p.walk_right > 0;
		d.cycle_shift = bike_strip_shift(p, d.u[0], dir);
		if (d.cycle_shift == 0 && d.bikes) d.edge_shift = (dir == 0 ? -1.f : 1.f) * std::max(0.f, lanes[0].w * 0.5f - s.catalog.traffic.bike_edge_m);
		d.len = len;
		d.speed = p.speed / KMH_PER_MS;
		d.at0 = float(dir == 0 ? sg.retreat_a : sg.len - sg.retreat_b);
		d.sign = dir == 0 ? 1.f : -1.f;
		set_bend(sg, s.catalog.traffic, d);
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

// The node's connections, without their conflicts: `made` gets them, and whether a junction's conflicts are due.
static bool link_node(CityState &s, int n, std::vector<int> &made) {
	LaneGraph &g = s.traffic.g;
	const RoadNetwork &net = s.roads;
	if (n < 0 || n >= int(net.nodes.size()) || n >= int(g.node_conn.size()) || !net.nodes[n].alive) return false;
	std::vector<int> arms;
	for (int si : net.nodes[n].segs)
		if (si * 2 + 1 < int(g.ds.size()) && (g.ds[si * 2].ok || g.ds[si * 2 + 1].ok)) arms.push_back(si);
	const int degree = int(arms.size());
	const std::vector<int> main = main_legs(s, n); // the road the junction is on: it goes straight on through it
	const auto carried = [&](int from, int to) {
		return from != to && std::find(main.begin(), main.end(), from) != main.end() && std::find(main.begin(), main.end(), to) != main.end();
	};
	for (int si : arms) {
		int din = in_ds(net, si, n);
		if (!g.ds[din].ok) continue;
		Pose ph;
		lane_pose(s, din, 0, g.ds[din].len, ph);
		// the movements this arrival has, then the lanes of each: a lane's painted turns bind against what the node offers
		struct Movement {
			int dout;
			Turn turn;
		};
		std::vector<Movement> movements;
		uint8_t offered = 0;
		for (int so : arms) {
			if ((so == si && degree > 1 && net.uturn_arm(n) != si) || net.movement_banned(n, si, so)) continue;
			int dout = out_ds(net, so, n);
			if (!g.ds[dout].ok) continue;
			Pose po;
			lane_pose(s, dout, 0, 0.f, po);
			Turn t = turn_class(V2(ph.fx, ph.fz), V2(po.fx, po.fz), so == si, carried(si, so), degree, s.catalog.traffic);
			movements.push_back({ dout, t });
			offered |= painted_bit(t);
		}
		const LanePaint paint(g.ds[din], offered, degree);
		for (const Movement &m : movements) link_pair(s, n, din, m.dout, m.turn, paint, made);
	}
	for (const LaneLink &l : net.nodes[n].links) {
		if (!link_stands(s, n, l)) continue;
		const int din = in_ds(net, l.from, n), dout = out_ds(net, l.to, n);
		Pose ph, po;
		lane_pose(s, din, 0, g.ds[din].len, ph);
		lane_pose(s, dout, 0, 0.f, po);
		add_conn(s, n, din, l.from_lane, dout, l.to_lane, turn_class(V2(ph.fx, ph.fz), V2(po.fx, po.fz), l.from == l.to, carried(l.from, l.to), degree, s.catalog.traffic), made);
	}
	return degree >= 2;
}

void build_node(CityState &s, int n) {
	std::vector<int> made;
	if (link_node(s, n, made)) find_conflicts(s, made); // an edit's node: the full build's counts stay what that build did
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
	// linked in order (the connections are a pool); each node's conflicts touch only its own: over the pool, the same lists
	std::vector<std::vector<int>> junctions;
	for (int n = 0; n < int(s.roads.nodes.size()); n++) {
		std::vector<int> made;
		if (s.roads.nodes[n].alive && link_node(s, n, made)) junctions.push_back(std::move(made));
	}
	std::vector<SweepWork> work(junctions.size());
	auto conflicts = [&](size_t k) { work[k] = find_conflicts(s, junctions[k]); };
	if (s.jobs) s.jobs->parallel_for(junctions.size(), conflicts);
	else for (size_t k = 0; k < junctions.size(); k++) conflicts(k);
	for (const SweepWork &w : work) {
		g.sweep_pairs += w.pairs;
		g.sweep_tests += w.tests;
	}
	ensure_conn_state(s.traffic);
	build_junctions(s);
}

} // namespace traffic
} // namespace city
