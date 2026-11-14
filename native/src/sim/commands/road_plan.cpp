// Snapping and road validation (Roadcraft src/roads/construction/snap.ts and plan.ts). The preview and the build run
// the same plan, so what the player sees refused is exactly what the build refuses.
#include "sim/commands/commands.h"
#include "sim/roads/junction.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/road_grade_sync.h"
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
			if (std::fabs(s.terrain.height_at(float(px), float(pz)) + plan.lift_at(at / len) - net.seg_y(o, arc_at(net.segs[o].table, to))) >= tn.road_level_dy_m) continue;
			over = true;
			break;
		}
		run = over ? run + SIDE_BY_SIDE_STRIDE_M : 0;
		if (run >= tn.road_overlap_run_m) return true;
	}
	return false;
}

// The crossings at grade with the roads the new curve passes over (plan.ts: junctions), sorted along the new road.
bool find_crossings(const CityState &s, const std::vector<double> &table, const Anchor &a, const Anchor &b, RoadPlan &p) {
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
					const float ours = p.lift_at(arc_at(table, h.first) / p.len); // its height there, by the length run
					if (std::fabs(s.terrain.height_at(float(px), float(pz)) + ours - net.seg_y(o, arc_at(net.segs[o].table, h.second))) >= tn.road_level_dy_m) continue;
					// Heights authored apart: past the clearance one passes over the other, under the join's they meet, and between them neither can (commit.ts).
					const float gap = std::fabs(ours - net.lift_on(o, h.second));
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
		if (k > int(s.catalog.tune.road_max_degree)) return false;
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

PieceEnd at_node(const RoadNetwork &net, int node) {
	const RoadNode &n = net.nodes[size_t(node)];
	return { n.x, n.z, float(n.y), n.lift, node };
}

// Where an end of the road in hand will stand, before anything is laid: what anchor_node (road_edit.cpp) will make of it
// (its node; the point of the road it lands on, at that road's height; on open ground what stands within road_merge_m at
// its height, else the natural ground and its lift). A path's joint stands where the piece before it was priced to end.
PieceEnd end_of(const CityState &s, const Anchor &e, float lift) {
	const RoadNetwork &net = s.roads;
	PieceEnd at{ e.x, e.z, road_ground(s)(float(e.x), float(e.z)) + lift, lift, -1 };
	if (e.node >= 0) {
		at = at_node(net, e.node);
	} else if (e.seg >= 0) {
		at.stand = float(net.seg_y(e.seg, arc_at(net.segs[size_t(e.seg)].table, e.t)));
	} else {
		const Anchor met = snap_point(s, e.x, e.z, s.catalog.tune.road_merge_m);
		const float join = s.catalog.tune.grade_join_m;
		if (met.node >= 0 && std::fabs(net.nodes[size_t(met.node)].lift - lift) <= join) {
			at = at_node(net, met.node);
		} else if (met.seg >= 0 && std::fabs(net.lift_on(met.seg, met.t) - lift) <= join) {
			at.x = met.x;
			at.z = met.z;
			at.stand = float(net.seg_y(met.seg, arc_at(net.segs[size_t(met.seg)].table, met.t)));
		}
	}
	at.stand = e.stand.value_or(at.stand);
	return at;
}

// Where the road in hand meets a road it crosses: that road's end node when close to it, else the point it will be cut
// at, standing at that road's height there (road_commands.cpp crossing_node, road_edit.cpp split_segment).
PieceEnd crossing_end(const CityState &s, const Crossing &c) {
	const RoadNetwork &net = s.roads;
	const RoadSegment &g = net.segs[size_t(c.seg)];
	const double close = s.catalog.tune.road_snap_end_m;
	if (c.t_old * g.len < close) return at_node(net, g.a);
	if ((1 - c.t_old) * g.len < close) return at_node(net, g.b);
	PieceEnd at{ 0, 0, float(net.seg_y(c.seg, arc_at(g.table, c.t_old))), net.lift_on(c.seg, c.t_old), -1 };
	g.q.at(c.t_old, at.x, at.z);
	return at;
}

// What the plan costs, how its metres are built and its stations, piece by piece as the build will lay and charge them
// (road_commands.cpp lay_road: the same stops, the same pieces between them, each built alone between the heights its
// ends stand at): what the draft shows is what the click costs. A recorded worth is a float: the price counts it so.
void price_plan(const CityState &s, const std::vector<double> &table, const Anchor &a, const Anchor &b, int profile, RoadPlan &p) {
	struct Stop {
		double t = 0;
		PieceEnd at;
	};
	std::vector<Stop> stops{ { 0.0, end_of(s, a, p.lift_a) } };
	for (const Crossing &c : p.crossings) {
		const PieceEnd at = crossing_end(s, c);
		if (at.node < 0 || at.node != stops.back().at.node) stops.push_back({ c.t_new, at });
	}
	stops.push_back({ 1.0, end_of(s, b, p.lift_b) });
	p.stand_a = stops.front().at.stand;
	p.stand_b = stops.back().at.stand;
	const double a_metre = s.catalog.profile(profile).cost_per_m;
	double value = 0;
	for (size_t i = 0; i + 1 < stops.size(); i++) {
		const Stop &from = stops[i], &to = stops[i + 1];
		if (from.at.node >= 0 && from.at.node == to.at.node) continue;
		for (const LaidPiece &piece : pieces_between(s, p, table, from.t, from.at, to.t, to.at, profile, s.new_road_flags)) {
			value += a_metre * double(float(piece.built.worth));
			p.built.add(piece.built.lengths);
			place_stations(s.catalog.tune, piece.q, piece.table, structure_of(piece.flags), piece.built.stations, p.stations);
		}
	}
	p.price = std::round(value);
}
} // namespace

std::vector<LaidPiece> pieces_between(const CityState &s, const RoadPlan &plan, const std::vector<double> &table, double t_a, const PieceEnd &a,
		double t_b, const PieceEnd &b, int profile, uint16_t kept) {
	const double from_m = arc_at(table, t_a), run = arc_at(table, t_b) - from_m;
	const int pieces = std::max(1, int(std::ceil(run / s.catalog.tune.road_max_seg_m)));
	const GroundAt ground = road_ground(s);
	std::vector<LaidPiece> out(static_cast<size_t>(pieces));
	PieceEnd from = a;
	for (int k = 1; k <= pieces; k++) {
		const double t0 = k == 1 ? t_a : t_at_length(table, from_m + run * (k - 1) / pieces);
		const double t1 = k == pieces ? t_b : t_at_length(table, from_m + run * k / pieces);
		LaidPiece &piece = out[size_t(k - 1)];
		Quad sub = sub_quad(plan.q, t0, t1);
		const float lift = plan.lift_at((from_m + run * k / pieces) / table.back()); // by the length run, as every height along a road
		piece.to = k == pieces ? b : PieceEnd{ sub.x1, sub.z1, ground(float(sub.x1), float(sub.z1)) + lift, lift, -1 };
		// the curve between the points its two ends stand on
		sub.cx += ((from.x - sub.x0) + (piece.to.x - sub.x1)) / 2;
		sub.cz += ((from.z - sub.z0) + (piece.to.z - sub.z1)) / 2;
		sub.x0 = from.x;
		sub.z0 = from.z;
		sub.x1 = piece.to.x;
		sub.z1 = piece.to.z;
		// A road laid at grade over water is a bridge, decided as it is laid so the journal keeps it (road_grade_sync.h).
		const uint16_t runs = uint16_t((kept & ~(SEG_RUNS_ON_A | SEG_RUNS_ON_B)) | (k > 1 ? SEG_RUNS_ON_A : 0) | (k < pieces ? SEG_RUNS_ON_B : 0));
		piece.flags = structure_of(runs) == Structure::GROUND && crosses_water(s, sub) ? with_structure(runs, Structure::BRIDGE) : runs;
		piece.q = sub;
		piece.table = arc_table(sub, ROAD_ARC_STEPS);
		piece.built = built_alone(s, sub, piece.table, profile, piece.flags, from.stand, piece.to.stand, from.lift != 0 || piece.to.lift != 0);
		from = piece.to;
	}
	return out;
}

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
	p.limited = p.lift_b != want; // a free end is told so by the draft; a connection cannot move: refused by its own name
	if (p.reason.empty() && p.limited && (b.node >= 0 || b.seg >= 0)) p.reason = "refuse.road.tooSteep";
	if (!p.reason.empty() || !find_crossings(s, table, a, b, p)) return p;
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
	price_plan(s, table, a, b, profile, p);
	p.ok = true;
	return p;
}

} // namespace city
