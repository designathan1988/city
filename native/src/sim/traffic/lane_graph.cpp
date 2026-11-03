// The lane graph (Roadcraft src/sim/lanes.ts:349-494): directed segments with their lanes between the two mouths,
// lane connectors through every node (turn classes, lanes allowed by position, pairing) and their conflict sets.
#include "sim/traffic/traffic_system.h"

#include <cmath>

namespace city {
namespace traffic {

namespace {

constexpr float KMH_PER_MS = 3.6f;
constexpr float MIN_LANE_M = 0.5f;
constexpr float UTURN_NODE_PAD_M = 3.f; // dead-end U-turn: the control point goes this far past the node, doubled

int in_ds(const RoadNetwork &net, int seg, int node) { return seg * 2 + (net.segs[seg].b == node ? 0 : 1); }
int out_ds(const RoadNetwork &net, int seg, int node) { return seg * 2 + (net.segs[seg].a == node ? 0 : 1); }

void build_dirs(const CityState &s, LaneGraph &g) {
	const RoadNetwork &net = s.roads;
	g.ds.assign(net.segs.size() * 2, DirSeg());
	g.lane_metres = 0;
	for (int i = 0; i < int(net.segs.size()); i++) {
		const RoadSegment &sg = net.segs[i];
		if (!sg.alive || sg.a == sg.b) continue;
		const Profile &p = net.profile_of(i);
		if (p.rail) continue;
		float len = std::max(MIN_LANE_M, float(sg.len - sg.retreat_a - sg.retreat_b));
		for (int dir = 0; dir < 2; dir++) {
			const std::vector<LaneInfo> &lanes = dir == 0 ? p.fwd : p.back;
			if (lanes.empty()) continue;
			DirSeg &d = g.ds[i * 2 + dir];
			d.ok = true;
			d.seg = i;
			d.from = dir == 0 ? sg.a : sg.b;
			d.to = dir == 0 ? sg.b : sg.a;
			d.lanes = std::min(int(lanes.size()), MAX_LANES);
			for (int k = 0; k < d.lanes; k++) d.u[k] = lanes[k].u;
			d.len = len;
			d.speed = p.speed / KMH_PER_MS;
			d.at0 = float(dir == 0 ? sg.retreat_a : sg.len - sg.retreat_b);
			d.sign = dir == 0 ? 1.f : -1.f;
			Pose end;
			lane_pose(s, i * 2 + dir, 0, len, end);
			d.ex = end.x;
			d.ez = end.z;
			g.lane_metres += double(len) * d.lanes;
		}
	}
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

void link_pair(const CityState &s, LaneGraph &g, int node, int din, int dout, Turn turn) {
	const DirSeg &di = g.ds[din], &dout_ = g.ds[dout];
	std::vector<int> ok;
	for (int l = 0; l < di.lanes; l++)
		if (lane_allows(l, di.lanes, turn)) ok.push_back(l);
	const int k = dout_.lanes, m = int(ok.size());
	for (int r = 0; r < m; r++) {
		Conn c;
		c.node = node;
		c.from_ds = din;
		c.from_lane = ok[r];
		c.to_ds = dout;
		c.turn = turn;
		bool from_left = turn == Turn::LEFT || turn == Turn::UTURN;
		c.to_lane = from_left ? std::max(0, k - 1 - std::min(m - 1 - r, k - 1)) : std::min(r, k - 1);
		make_curve(s, c, s.catalog.traffic);
		g.conns.push_back(c);
	}
}

void find_conflicts(LaneGraph &g, size_t first, int samples) {
	std::vector<std::vector<V2>> poly;
	for (size_t i = first; i < g.conns.size(); i++) {
		std::vector<V2> pts;
		for (int k = 0; k <= samples; k++) {
			Pose p;
			conn_pose(g.conns[i], g.conns[i].len * float(k) / samples, p);
			pts.push_back(V2(p.x, p.z));
		}
		poly.push_back(pts);
	}
	for (size_t i = first; i < g.conns.size(); i++)
		for (size_t j = i + 1; j < g.conns.size(); j++) {
			Conn &a = g.conns[i], &b = g.conns[j];
			if (a.from_ds == b.from_ds && a.from_lane == b.from_lane) continue;
			bool merge = a.to_ds == b.to_ds && a.to_lane == b.to_lane;
			float ta = 0, tb = 0;
			int xa = -1, yb = -1;
			const auto &pa = poly[i - first], &pb = poly[j - first];
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
			a.conflicts.push_back(int(j));
			a.conflict_at.push_back(at_b);
			b.conflicts.push_back(int(i));
			b.conflict_at.push_back(at_a);
		}
}

void build_node(const CityState &s, LaneGraph &g, int n) {
	const RoadNetwork &net = s.roads;
	std::vector<int> arms;
	for (int si : net.nodes[n].segs)
		if (g.ds[si * 2].ok || g.ds[si * 2 + 1].ok) arms.push_back(si);
	const int degree = int(arms.size());
	size_t first = g.conns.size();
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
			link_pair(s, g, n, din, dout, t);
		}
	}
	if (degree >= 2) find_conflicts(g, first, std::max(2, int(s.catalog.traffic.conflict_samples)));
}

} // namespace

void build_lane_graph(CityState &st) {
	const CityState &s = st;
	LaneGraph &g = st.traffic.g; // lane_pose() reads the graph being built through `s`
	g = LaneGraph();
	g.version = s.roads.version;
	build_dirs(s, g);
	for (int n = 0; n < int(s.roads.nodes.size()); n++)
		if (s.roads.nodes[n].alive) build_node(s, g, n);
	g.ds_out.assign(g.ds.size(), {});
	for (int c = 0; c < int(g.conns.size()); c++) g.ds_out[g.conns[c].from_ds].push_back(c);
	build_junctions(st);
}

} // namespace traffic
} // namespace city
