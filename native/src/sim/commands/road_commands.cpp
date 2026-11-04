// Building and bulldozing roads (Roadcraft construction/execute.ts buildLive, streetEdits.ts demolishSeg, the bulldoze
// tool) and the money they move (roads/economy.ts chargeFor, journal.ts settle).
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "core/math.h"
#include "sim/roads/junction.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr size_t MIN_JUNCTION_ARMS = 3, JOINT_ARMS = 2;
constexpr double STRAIGHT_JOIN_M = 0.05; // two pieces this close to their chords are straight
constexpr double STRAIGHT_JOIN_COS = -0.999; // the arms of a joint leave it this close to opposite ways

// The cosine between the way `first` arrives at its end and the opposite of the way `second` leaves: -1 in line.
double dir_turn(const Quad &first, const Quad &second) {
	const double ax = first.x1 - first.cx, az = first.z1 - first.cz, bx = second.cx - second.x0, bz = second.cz - second.z0;
	const double l = std::hypot(ax, az) * std::hypot(bx, bz);
	return l > 0 ? -(ax * bx + az * bz) / l : 0;
}

constexpr int ARC_STEPS = 64;
constexpr double MIN_CHARGE = 1; // economy.ts chargeFor: |delta| < 1 moves no money

struct Stop {
	double t = 0;
	int node = -1;
};

// The node where the new road meets a crossed segment: its end when close, else a split (the segment may already have
// been split by an earlier stop: the piece under the point is searched again).
int crossing_node(CityState &s, const Quad &q, const Crossing &c) {
	RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	double px, pz, t, d;
	q.at(c.t_new, px, pz);
	int seg = net.find_segment_near(px, pz, tn.road_merge_m, t, d);
	if (seg < 0) return -1;
	double sl = net.segs[seg].len;
	if (t * sl < tn.road_snap_end_m) return net.segs[seg].a;
	if ((1 - t) * sl < tn.road_snap_end_m) return net.segs[seg].b;
	return split_segment(s, seg, t);
}

// Lays the road from stop a to stop b in pieces no longer than road_max_seg_m (execute.ts refine, at grade).
void lay_pieces(CityState &s, const Quad &q, const std::vector<double> &table, const Stop &a, const Stop &b, int profile,
		const RoadKeeps &keeps, std::vector<int> &out) {
	RoadNetwork &net = s.roads;
	double sa = arc_at(table, a.t), sb = arc_at(table, b.t);
	int pieces = std::max(1, int(std::ceil((sb - sa) / s.catalog.tune.road_max_seg_m)));
	int prev = a.node;
	for (int k = 1; k <= pieces; k++) {
		double t0 = t_at_length(table, sa + (sb - sa) * (k - 1) / pieces), t1 = k == pieces ? b.t : t_at_length(table, sa + (sb - sa) * k / pieces);
		Quad sub = sub_quad(q, t0, t1);
		int next = k == pieces ? b.node : j_add_node(s, sub.x1, sub.z1, s.terrain.height_at(float(sub.x1), float(sub.z1)));
		const RoadNode &np = net.nodes[prev], &nn = net.nodes[next];
		sub.cx += ((np.x - sub.x0) + (nn.x - sub.x1)) / 2;
		sub.cz += ((np.z - sub.z0) + (nn.z - sub.z1)) / 2;
		sub.x0 = np.x; sub.z0 = np.z; sub.x1 = nn.x; sub.z1 = nn.z;
		out.push_back(j_add_segment(s, prev, next, sub, profile, keeps.zl, keeps.zr, keeps.flags));
		prev = next;
	}
}

bool affords(const CityState &s, double price) { return s.unlimited_money || price <= s.money; }
} // namespace

EditResult build_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile) {
	RoadBox box;
	box.add(a.x, a.z);
	box.add(b.x, b.z);
	const std::map<int, double> before = squeezes_in(s, box);
	EditResult r = lay_road(s, a, has_control, cx, cz, b, profile, RoadKeeps{});
	if (r.ok && squeezed_worse(s, before, box)) {
		r = EditResult{};
		r.error = "hint.rule.squeezed";
	}
	return r;
}

// The curve through both ends of two pieces of one road: their end tangents meet at its control point.
bool joined_quad(const Quad &first, const Quad &second, Quad &out) {
	if (first.is_straight(STRAIGHT_JOIN_M) && second.is_straight(STRAIGHT_JOIN_M)) {
		out = Quad::straight(first.x0, first.z0, second.x1, second.z1);
		return true;
	}
	double u, v;
	if (!line_line(first.x0, first.z0, first.cx - first.x0, first.cz - first.z0, second.x1, second.z1, second.cx - second.x1,
				second.cz - second.z1, u, v)) return false;
	out = { first.x0, first.z0, first.x0 + (first.cx - first.x0) * u, first.z0 + (first.cz - first.z0) * u, second.x1, second.z1 };
	return true;
}

// A junction beside a place where a road was only cut in pieces (lay_pieces) has its mouth capped by the stub between them,
// and the corner comes out square: the stub and the piece beyond it go back to being one road.
void merge_stubs(CityState &s, int node, RoadBox &box) {
	RoadNetwork &net = s.roads;
	const std::vector<int> arms = net.nodes[size_t(node)].segs;
	if (arms.size() < MIN_JUNCTION_ARMS) return;
	for (int seg : arms) {
		const RoadSegment g = net.segs[size_t(seg)];
		if (wanted_retreat(net, node, seg) <= g.len * MOUTH_SHARE) continue;
		const int far = g.a == node ? g.b : g.a;
		const RoadNode &joint = net.nodes[size_t(far)];
		if (joint.segs.size() != JOINT_ARMS) continue;
		const RoadSegment o = net.segs[size_t(joint.segs[0] == seg ? joint.segs[1] : joint.segs[0])];
		const int other = joint.segs[0] == seg ? joint.segs[1] : joint.segs[0];
		const bool onward = g.b == far && o.a == far, back = g.a == far && o.b == far;
		if ((!onward && !back) || o.profile != g.profile || o.zl != g.zl || o.zr != g.zr || o.flags != g.flags) continue;
		const RoadSegment &first = onward ? g : o, &second = onward ? o : g;
		Quad q;
		if (dir_turn(first.q, second.q) > STRAIGHT_JOIN_COS || !joined_quad(first.q, second.q, q)) continue;
		box.add(g);
		box.add(o);
		j_remove_segment(s, seg);
		j_remove_segment(s, other);
		const int merged = j_add_segment(s, first.a, second.b, q, g.profile, g.zl, g.zr, g.flags);
		j_remove_node_if_orphan(s, far);
		conform_under(s, merged);
		net.mark_segment(merged);
	}
}

EditResult lay_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile, const RoadKeeps &keeps) {
	EditResult r;
	RoadPlan plan = plan_road(s, a, has_control, cx, cz, b, profile);
	if (!plan.ok) {
		r.error = plan.reason;
		return r;
	}
	if (!affords(s, plan.price)) {
		r.error = "ui.insp.noMoney";
		return r;
	}
	std::vector<Stop> stops;
	stops.push_back({ 0, anchor_node(s, a) });
	for (const Crossing &c : plan.crossings) {
		int n = crossing_node(s, plan.q, c);
		if (n >= 0 && n != stops.back().node) stops.push_back({ c.t_new, n });
	}
	stops.push_back({ 1, anchor_node(s, b) });
	std::vector<double> table = arc_table(plan.q, ARC_STEPS);
	std::vector<int> made;
	for (size_t i = 0; i + 1 < stops.size(); i++)
		if (stops[i].node != stops[i + 1].node) lay_pieces(s, plan.q, table, stops[i], stops[i + 1], profile, keeps, made);
	double minx = BIG, minz = BIG, maxx = -BIG, maxz = -BIG;
	for (int sg : made) {
		const RoadSegment &g = s.roads.segs[sg];
		r.buildings_removed += clear_under(s, sg);
		conform_under(s, sg);
		s.roads.mark_segment(sg);
		minx = std::min(minx, double(g.minx)); minz = std::min(minz, double(g.minz));
		maxx = std::max(maxx, double(g.maxx)); maxz = std::max(maxz, double(g.maxz));
	}
	RoadBox box;
	box.add(minx, minz);
	box.add(maxx, maxz);
	for (const Stop &stop : stops) merge_stubs(s, stop.node, box);
	j_money(s, -plan.price);
	finish_edit(s, box.minx, box.minz, box.maxx, box.maxz);
	r.ok = true;
	r.money = s.unlimited_money ? 0 : plan.price;
	r.segments = int(made.size());
	return r;
}

EditResult demolish_at(CityState &s, double x, double z) {
	EditResult r;
	int hit = -1;
	float reach = s.catalog.tune.road_pick_m;
	s.buildings.query(float(x) - reach, float(z) - reach, float(x) + reach, float(z) + reach, [&](int id) {
		const Building &b = s.buildings.items[id];
		double c = std::cos(b.rot), sn = std::sin(b.rot), dx = x - b.x, dz = z - b.z;
		if (std::fabs(dx * c - dz * sn) < b.w / 2 && std::fabs(dx * sn + dz * c) < b.d / 2) hit = id;
	});
	if (hit >= 0) {
		const Building &b = s.buildings.items[hit];
		double bx = b.x, bz = b.z;
		j_remove_building(s, hit);
		finish_edit(s, bx, bz, bx, bz);
		r.ok = true;
		r.buildings_removed = 1;
		return r;
	}
	const int ruin = s.damage.ruin_at(float(x), float(z), 0.f); // the rubble a blow left: clearing it frees the lot
	if (ruin >= 0) {
		const Ruin gone = s.damage.ruins[size_t(ruin)];
		j_remove_ruin(s, ruin);
		finish_edit(s, gone.x, gone.z, gone.x, gone.z);
		r.ok = true;
		return r;
	}
	double t, d;
	RoadNetwork &net = s.roads;
	int seg = net.find_segment_near(x, z, reach, t, d);
	if (seg < 0) {
		r.error = "help.tool.bulldoze";
		return r;
	}
	const RoadSegment g = net.segs[seg];
	r.money = -demolish_segment(s, seg);
	finish_edit(s, g.minx, g.minz, g.maxx, g.maxz);
	r.ok = true;
	r.segments = 1;
	return r;
}

double demolish_segment(CityState &s, int seg) {
	const RoadSegment g = s.roads.segs[seg];
	const double refund = (g.flags & SEG_FREE) ? 0 : segment_value(s, seg) * s.catalog.tune.economy_refund_share;
	j_remove_segment(s, seg);
	j_remove_node_if_orphan(s, g.a);
	j_remove_node_if_orphan(s, g.b);
	if (refund < MIN_CHARGE || s.unlimited_money) return 0;
	j_money(s, std::round(refund));
	return std::round(refund);
}

} // namespace city
