// Building and bulldozing roads (Roadcraft construction/execute.ts buildLive, streetEdits.ts demolishSeg, the bulldoze
// tool) and the money they move (roads/economy.ts chargeFor, journal.ts settle).
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/street_journal.h"
#include "sim/street/street_names.h"

#include "core/math.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
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
		std::vector<int> &out) {
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
		out.push_back(j_add_segment(s, prev, next, sub, profile, 0, 0, s.new_road_flags));
		prev = next;
	}
}

bool affords(const CityState &s, double price) { return s.unlimited_money || price <= s.money; }
} // namespace

EditResult build_road(CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile) {
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
		if (stops[i].node != stops[i + 1].node) lay_pieces(s, plan.q, table, stops[i], stops[i + 1], profile, made);
	double minx = BIG, minz = BIG, maxx = -BIG, maxz = -BIG;
	for (int sg : made) {
		const RoadSegment &g = s.roads.segs[sg];
		r.buildings_removed += clear_under(s, sg);
		conform_under(s, sg);
		s.roads.mark_segment(sg);
		minx = std::min(minx, double(g.minx)); minz = std::min(minz, double(g.minz));
		maxx = std::max(maxx, double(g.maxx)); maxz = std::max(maxz, double(g.maxz));
	}
	for (const StreetName &n : names_for(s.roads, s.street, made)) {
		StreetItem it;
		it.kind = ItemKind::STREETNAME;
		it.x = n.x;
		it.z = n.z;
		it.text = n.text;
		j_add_item(s, it);
	}
	j_money(s, -plan.price);
	finish_edit(s, minx, minz, maxx, maxz);
	r.ok = true;
	r.money = s.unlimited_money ? 0 : plan.price;
	r.segments = int(made.size());
	return r;
}

EditResult demolish_at(CityState &s, double x, double z) {
	EditResult r;
	int hit = -1;
	float reach = s.catalog.tune.road_pick_m;
	if (remove_street_at(s, x, z, s.catalog.furniture.items.demolish_reach_m)) {
		r.ok = true;
		return r;
	}
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
	double t, d;
	RoadNetwork &net = s.roads;
	int seg = net.find_segment_near(x, z, reach, t, d);
	if (seg < 0) {
		r.error = "help.tool.bulldoze";
		return r;
	}
	remove_street_kit(s, seg);
	RoadSegment g = net.segs[seg];
	double refund = (g.flags & SEG_FREE) ? 0 : segment_value(s, seg) * s.catalog.tune.economy_refund_share;
	j_remove_segment(s, seg);
	j_remove_node_if_orphan(s, g.a);
	j_remove_node_if_orphan(s, g.b);
	if (refund >= MIN_CHARGE && !s.unlimited_money) {
		r.money = -std::round(refund);
		j_money(s, -r.money);
	}
	finish_edit(s, g.minx, g.minz, g.maxx, g.maxz);
	r.ok = true;
	r.segments = 1;
	return r;
}

} // namespace city
