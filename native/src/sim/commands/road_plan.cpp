// Snapping and road validation (Roadcraft src/roads/construction/snap.ts and plan.ts). The preview and the build run
// the same plan, so what the player sees refused is exactly what the build refuses.
#include "sim/commands/commands.h"
#include "sim/roads/junction.h"
#include "sim/commands/road_edit.h"
#include "sim/economy/road_economy.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int CROSS_STEPS = 32; // polyline samples per curve when looking for crossings
constexpr int ARC_STEPS = 64; // arc-length table of the planned curve (plan.ts arcTable(q, 64))
constexpr double SEG_SNAP_SHARE = 0.8; // segments snap within this share of the node radius (snap.ts)
constexpr double SIDE_BY_SIDE_STRIDE_M = 4; // overlap test stride along the new road (plan.ts overlapsRoad)
constexpr int JUNCTION_DEGREE_GAIN = 2; // a crossing on an existing node adds two arms to it

double dir_cos(const Quad &a, double ta, const Quad &b, double tb) {
	double dx1, dz1, dx2, dz2;
	a.deriv(ta, dx1, dz1);
	b.deriv(tb, dx2, dz2);
	double l = std::hypot(dx1, dz1) * std::hypot(dx2, dz2);
	return l > 0 ? std::fabs(dx1 * dx2 + dz1 * dz2) / l : 1;
}

// Crossings of two quadratic curves through their polylines, refined to the curves.
void intersect_quads(const Quad &a, const Quad &b, std::vector<std::pair<double, double>> &out) {
	double ax[CROSS_STEPS + 1], az[CROSS_STEPS + 1], bx[CROSS_STEPS + 1], bz[CROSS_STEPS + 1];
	for (int i = 0; i <= CROSS_STEPS; i++) {
		a.at(double(i) / CROSS_STEPS, ax[i], az[i]);
		b.at(double(i) / CROSS_STEPS, bx[i], bz[i]);
	}
	for (int i = 0; i < CROSS_STEPS; i++)
		for (int k = 0; k < CROSS_STEPS; k++) {
			double s, t;
			if (!seg_seg(ax[i], az[i], ax[i + 1], az[i + 1], bx[k], bz[k], bx[k + 1], bz[k + 1], s, t)) continue;
			double px = ax[i] + (ax[i + 1] - ax[i]) * s, pz = az[i] + (az[i + 1] - az[i]) * s, ta, tb, d;
			closest_t(a, px, pz, ta, d);
			closest_t(b, px, pz, tb, d);
			out.push_back({ ta, tb });
		}
}

bool overlaps_road(const CityState &s, const RoadPlan &plan, const std::vector<double> &table, int profile, const Anchor &a, const Anchor &b) {
	const Quad &q = plan.q;
	const auto lift_at = [&](double t) { return plan.lift_at(t); };
	const RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	const EndMouth from = end_mouth(s, a, q, 0, profile), to_end = end_mouth(s, b, q, 1, profile);
	double len = table.back(), half = s.catalog.profile(profile).right, widest = 0, run = 0;
	for (const Profile &c : s.catalog.profiles) widest = std::max(widest, double(c.right));
	const double pad = (half + widest) * tn.road_overlap_share;
	std::vector<int> cands;
	net.for_segments_near(std::min({ q.x0, q.cx, q.x1 }) - pad, std::min({ q.z0, q.cz, q.z1 }) - pad,
			std::max({ q.x0, q.cx, q.x1 }) + pad, std::max({ q.z0, q.cz, q.z1 }) + pad, [&](int o) { cands.push_back(o); });
	for (double at = from.own; at <= len - to_end.own; at += SIDE_BY_SIDE_STRIDE_M) {
		double t = t_at_length(table, at), px, pz;
		q.at(t, px, pz);
		bool over = false;
		for (int o : cands) {
			double to, dist;
			closest_t(net.segs[o].q, px, pz, to, dist);
			if (dist >= (half + net.profile_of(o).right) * tn.road_overlap_share) continue;
			if (dir_cos(q, t, net.segs[o].q, to) < tn.road_parallel_cos) continue;
			if (from.covers(net, o, to, SIDE_BY_SIDE_STRIDE_M) || to_end.covers(net, o, to, SIDE_BY_SIDE_STRIDE_M)) continue;
			if (std::fabs(s.terrain.height_at(float(px), float(pz)) + lift_at(t) - net.seg_y(o, to * net.segs[o].len)) >= tn.road_level_dy_m) continue;
			over = true;
			break;
		}
		run = over ? run + SIDE_BY_SIDE_STRIDE_M : 0;
		if (run >= tn.road_overlap_run_m) return true;
	}
	return false;
}

// The crossings at grade with the roads the new curve passes over (plan.ts: junctions), sorted along the new road.
bool find_crossings(const CityState &s, const Anchor &a, const Anchor &b, RoadPlan &p) {
	const RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	std::vector<int> skip;
	for (const Anchor *e : { &a, &b }) {
		if (e->node >= 0) skip.insert(skip.end(), net.nodes[e->node].segs.begin(), net.nodes[e->node].segs.end());
		if (e->seg >= 0) skip.push_back(e->seg);
	}
	const Quad &q = p.q;
	net.for_segments_near(std::min({ q.x0, q.cx, q.x1 }), std::min({ q.z0, q.cz, q.z1 }), std::max({ q.x0, q.cx, q.x1 }),
			std::max({ q.z0, q.cz, q.z1 }), [&](int o) {
				if (!p.reason.empty() || std::find(skip.begin(), skip.end(), o) != skip.end()) return;
				std::vector<std::pair<double, double>> hits;
				intersect_quads(q, net.segs[o].q, hits);
				for (auto &h : hits) {
					double px, pz;
					q.at(h.first, px, pz);
					if (std::fabs(s.terrain.height_at(float(px), float(pz)) + p.lift_at(h.first) - net.seg_y(o, h.second * net.segs[o].len)) >= tn.road_level_dy_m) continue;
					// Heights authored apart: past the clearance one passes over the other, under the join's they meet, and between them neither can (commit.ts).
					const RoadSegment &og = net.segs[size_t(o)];
					const float theirs = net.nodes[size_t(og.a)].lift + (net.nodes[size_t(og.b)].lift - net.nodes[size_t(og.a)].lift) * float(h.second);
					const float gap = std::fabs(p.lift_at(h.first) - theirs);
					if (gap > tn.grade_clear_elevated_m) continue;
					if (gap > tn.grade_join_m) { p.reason = "hint.road.clearance"; return; }
					if (dir_cos(q, h.first, net.segs[o].q, h.second) > tn.road_sharp_cos) { p.reason = "refuse.road.sharpAngle"; return; }
					p.crossings.push_back({ h.first, o, h.second });
				}
			});
	if (!p.reason.empty()) return false;
	auto &c = p.crossings;
	std::sort(c.begin(), c.end(), [](const Crossing &x, const Crossing &y) { return x.t_new < y.t_new; });
	for (int i = int(c.size()) - 1; i > 0; i--)
		if ((c[i].t_new - c[i - 1].t_new) * p.len < tn.road_merge_m) c.erase(c.begin() + i);
	while (!c.empty() && c.front().t_new * p.len < tn.road_merge_m) c.erase(c.begin());
	while (!c.empty() && (1 - c.back().t_new) * p.len < tn.road_merge_m) c.pop_back();
	return true;
}

// No node may end with more arms than road_max_degree (plan.ts fullNode).
bool degrees_fit(const CityState &s, const Anchor &a, const Anchor &b, const RoadPlan &p) {
	const RoadNetwork &net = s.roads;
	std::vector<std::pair<int, int>> extra;
	if (a.node >= 0) extra.push_back({ a.node, 1 });
	if (b.node >= 0) extra.push_back({ b.node, 1 });
	for (const Crossing &c : p.crossings) {
		double sl = net.segs[c.seg].len, end = s.catalog.tune.road_snap_end_m;
		if (c.t_old * sl < end) extra.push_back({ net.segs[c.seg].a, JUNCTION_DEGREE_GAIN });
		else if ((1 - c.t_old) * sl < end) extra.push_back({ net.segs[c.seg].b, JUNCTION_DEGREE_GAIN });
	}
	for (auto &e : extra) {
		int k = int(net.nodes[e.first].segs.size());
		for (auto &f : extra) k += f.first == e.first ? f.second : 0;
		if (k > s.catalog.tune.road_max_degree) return false;
	}
	return true;
}
// Whether a road leaving end `e` along (dx, dz) would meet a road already there at less than the smallest angle
// (Roadcraft world/legAngles.ts MIN_LEG_ANGLE, road_sharp_cos): a node's arms, or the two ways of the road it splits.
bool sharp_at(const CityState &s, const Anchor &e, double dx, double dz) {
	const RoadNetwork &net = s.roads;
	const double l = std::hypot(dx, dz), limit = s.catalog.tune.road_sharp_cos;
	if (l <= 0) return false;
	dx /= l;
	dz /= l;
	if (e.seg >= 0) {
		const Quad &q = net.segs[size_t(e.seg)].q;
		const double tx = (1 - e.t) * (q.cx - q.x0) + e.t * (q.x1 - q.cx), tz = (1 - e.t) * (q.cz - q.z0) + e.t * (q.z1 - q.cz);
		const double tl = std::hypot(tx, tz);
		return tl > 0 && std::fabs(tx * dx + tz * dz) / tl > limit;
	}
	if (e.node < 0) return false;
	for (int sid : net.nodes[size_t(e.node)].segs) {
		const Arm arm = arm_of(net.segs[size_t(sid)].q, net.profile_of(sid), net.segs[size_t(sid)].a == e.node ? 0 : 1);
		if (arm.dx * dx + arm.dz * dz > limit) return true;
	}
	return false;
}
} // namespace

Anchor snap_point(const CityState &s, double x, double z, double radius) {
	const RoadNetwork &net = s.roads;
	Anchor a;
	a.x = x;
	a.z = z;
	int n = net.find_node_near(x, z, radius);
	if (n >= 0) {
		a.node = n;
		a.x = net.nodes[n].x;
		a.z = net.nodes[n].z;
		return a;
	}
	double t, d;
	int sg = net.find_segment_near(x, z, radius * SEG_SNAP_SHARE, t, d);
	if (sg < 0) return a;
	const RoadSegment &g = net.segs[sg];
	double end = s.catalog.tune.road_snap_end_m;
	if (t * g.len < end) return snap_point(s, g.q.x0, g.q.z0, 1);
	if ((1 - t) * g.len < end) return snap_point(s, g.q.x1, g.q.z1, 1);
	a.seg = sg;
	a.t = t;
	g.q.at(t, a.x, a.z);
	return a;
}

RoadPlan plan_road(const CityState &s, const Anchor &a, bool has_control, double cx, double cz, const Anchor &b, int profile) {
	const Tuning &tn = s.catalog.tune;
	RoadPlan p;
	p.q = Quad{ a.x, a.z, has_control ? cx : (a.x + b.x) / 2, has_control ? cz : (a.z + b.z) / 2, b.x, b.z };
	std::vector<double> table = arc_table(p.q, ARC_STEPS);
	p.len = table.back();
	if (profile < 0 || profile >= int(s.catalog.profiles.size())) p.reason = "tool.failed";
	else if (p.len < tn.road_min_len_m) p.reason = "refuse.road.tooShort";
	else if (a.node >= 0 && a.node == b.node) p.reason = "refuse.road.selfLoop";
	// The end is held to the steepest grade a road is drawn at: a free end stops short of the height asked, a connection cannot move.
	p.lift_a = anchor_lift(s, a);
	p.lift_b = anchor_lift(s, b);
	const float rise = float(p.len) * tn.grade_authored, want = p.lift_b;
	p.lift_b = std::max(p.lift_a - rise, std::min(p.lift_a + rise, want));
	if (p.reason.empty() && p.lift_b != want && (b.node >= 0 || b.seg >= 0)) p.reason = "refuse.road.tooShort";
	if (!p.reason.empty() || !find_crossings(s, a, b, p)) return p;
	if (sharp_at(s, a, p.q.cx - a.x, p.q.cz - a.z) || sharp_at(s, b, p.q.cx - b.x, p.q.cz - b.z)) p.reason = "refuse.road.sharpAngle";
	else if (overlaps_road(s, p, table, profile, a, b)) p.reason = "road.reason.overlap";
	else if (!degrees_fit(s, a, b, p)) p.reason = "road.reason.fullNode";
	if (!p.reason.empty()) return p;
	double prev = 0;
	for (const Crossing &c : p.crossings) {
		double at = arc_at(table, c.t_new);
		if (at - prev < tn.road_fixed_gap_m) p.reason = "refuse.road.junctionsClose";
		prev = at;
	}
	if (p.len - prev < tn.road_fixed_gap_m) p.reason = "refuse.road.junctionsClose";
	if (!p.reason.empty()) return p;
	p.price = std::round(build_value(s, profile, p.len, RoadBuild{ Structure::GROUND, p.lift_a, p.lift_b }));
	p.ok = true;
	return p;
}

} // namespace city
