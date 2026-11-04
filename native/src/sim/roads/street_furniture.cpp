// Street furniture placement (Roadcraft jobs/props.ts placeSegment / placeJunction), see street_furniture.h.
#include "sim/roads/street_furniture.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr int FURN_FULL = 0, FURN_NONE = 2; // RoadSegment furniture level (SEG_FURNITURE_MASK)
constexpr int BASE_RESIDENTIAL = 1, BASE_INDUSTRIAL = 3; // ZoneDef::base
constexpr int MAX_BESIDE = 16; // pieces a passage check looks at across one cross-section

int side_of(const Profile &p, float u) {
	const StripEdge &r = p.edges.front(), &l = p.edges.back();
	if (r.strip.kind == StripKind::SIDEWALK && u >= r.u0 && u <= r.u1) return 0;
	if (l.strip.kind == StripKind::SIDEWALK && u >= l.u0 && u <= l.u1) return 1;
	return -1;
}

FurnitureSpot spot(const RoadNetwork &net, FurnitureKind k, int sid, const Profile &p, double v, float u, float face_left, float face_fwd) {
	FurnitureSpot s;
	s.kind = k;
	s.seg = sid;
	s.side = side_of(p, u);
	s.v = v;
	s.u = u;
	s.face_left = face_left;
	s.face_fwd = face_fwd;
	const Footprint &f = net.cat->furniture.footprint[int(k)];
	const bool along_traffic = face_fwd != 0; // its z runs along the segment
	s.half_v = (along_traffic ? f.z_m : f.x_m) * 0.5f;
	s.half_u = (along_traffic ? f.x_m : f.z_m) * 0.5f;
	return s;
}

// Whether a walker still finds a way past `c` on its sidewalk, among the pieces placed beside it: the centres a
// walker may take (the sidewalk less ped_margin_m each side) not all within its body and clearance of a footprint.
bool leaves_way(const RoadNetwork &net, const Profile &p, const std::vector<FurnitureSpot> &placed, size_t first, const FurnitureSpot &c) {
	if (c.side < 0) return true;
	const TrafficParams &tp = net.cat->traffic;
	const float reach = tp.ped_body_m;
	const StripEdge &e = c.side == 0 ? p.edges.front() : p.edges.back();
	const float lo = e.u0 + tp.ped_margin_m, hi = e.u1 - tp.ped_margin_m;
	if (hi <= lo) return true; // no room to walk in at all: nothing to keep free
	std::pair<float, float> shut[MAX_BESIDE];
	int n = 0;
	shut[n++] = { c.u - c.half_u - reach, c.u + c.half_u + reach };
	for (size_t i = first; i < placed.size() && n < MAX_BESIDE; i++) {
		const FurnitureSpot &q = placed[i];
		if (q.side != c.side || std::fabs(q.v - c.v) >= q.half_v + c.half_v + 2 * reach) continue;
		shut[n++] = { q.u - q.half_u - reach, q.u + q.half_u + reach };
	}
	std::sort(shut, shut + n);
	float free_from = lo;
	for (int i = 0; i < n && free_from < hi; i++) {
		if (shut[i].first > free_from) return true;
		free_from = std::max(free_from, shut[i].second);
	}
	return free_from < hi;
}

struct Placer {
	const RoadNetwork &net;
	const FurnitureRules &F;
	const LampDef &L;
	int sid;
	const RoadSegment &g;
	const Profile &p;
	double v0, v1;
	std::vector<FurnitureSpot> &all;
	size_t first;
	std::vector<double> stations; // the lamps' (empty when the profile has none)

	void fixed(const FurnitureSpot &s) { all.push_back(s); }
	void maybe(FurnitureKind k, double v, float u, float face_left) {
		FurnitureSpot s = spot(net, k, sid, p, v, u, face_left, 0.f);
		if (leaves_way(net, p, all, first, s)) all.push_back(s);
	}
	bool clear(double v) const {
		if (v < v0 + F.end_clear_m || v > v1 - F.end_clear_m) return false;
		for (double st : stations)
			if (std::fabs(st - v) < F.lamp_clear_m) return false;
		return true;
	}
};

void lamps(Placer &c) {
	const LampRow row = lamp_row(c.p, c.L);
	if (!row.any) return;
	for (double v = c.v0 + c.L.first_m; v < c.v1 - c.L.end_clear_m; v += c.L.step_m) c.stations.push_back(v);
	const float side = row.head_u > row.pole_u ? 1.f : -1.f;
	for (double v : c.stations) {
		c.fixed(spot(c.net, FurnitureKind::LAMP, c.sid, c.p, v, row.pole_u, side, 0.f));
		if (!row.twin) continue;
		FurnitureSpot s = spot(c.net, FurnitureKind::LAMP, c.sid, c.p, v, row.pole_u2, -1.f, 0.f);
		s.row = 1;
		c.fixed(s);
	}
}

void furniture(Placer &c, int level) {
	const FurnitureRules &F = c.F;
	const StripEdge &right = c.p.edges.front(), &left = c.p.edges.back();
	const double span = c.v1 - c.v0;
	const bool walk_r = right.strip.kind == StripKind::SIDEWALK && right.u1 - right.u0 >= F.min_walk_m;
	const bool walk_l = left.strip.kind == StripKind::SIDEWALK && left.u1 - left.u0 >= F.min_walk_m;
	if (walk_r && span > F.hydrant_min_span_m)
		for (double v = c.v0 + F.hydrant_first_m; v < c.v1; v += F.hydrant_step_m)
			if (c.clear(v)) c.maybe(FurnitureKind::HYDRANT, v, right.u1 - F.kerb_u_m, 1.f);
	if (level != FURN_FULL) return;
	const Catalog &cat = *c.net.cat;
	if (span > F.bin_min_span_m) {
		if (walk_r && cat.zone(c.g.zr).base != BASE_INDUSTRIAL && c.clear(c.v0 + F.bin_first_m))
			c.maybe(FurnitureKind::BIN, c.v0 + F.bin_first_m, right.u0 + F.wall_u_m, 1.f);
		if (walk_l && cat.zone(c.g.zl).base != BASE_INDUSTRIAL && c.clear(c.v1 - F.bin_first_m))
			c.maybe(FurnitureKind::BIN, c.v1 - F.bin_first_m, left.u1 - F.wall_u_m, -1.f);
	}
	const double mid = (c.v0 + c.v1) / 2;
	if (walk_r && cat.zone(c.g.zr).base == BASE_RESIDENTIAL && span > F.bench_min_span_m && right.u1 - right.u0 >= F.bench_min_walk_m &&
			c.clear(mid)) {
		c.maybe(FurnitureKind::BENCH, mid, right.u0 + F.wall_u_m, 1.f);
		c.maybe(FurnitureKind::BIN, mid + F.bin_after_bench_m, right.u0 + F.wall_u_m, 1.f);
	}
}

// Street trees on the wide footways, kept tree_clear_m off every lamp station (counted even where no lamp stands).
void trees(Placer &c) {
	const FurnitureRules &F = c.F;
	std::vector<double> stations;
	for (double v = c.v0 + c.L.first_m; v < c.v1 - c.L.end_clear_m; v += c.L.step_m) stations.push_back(v);
	for (int side = 0; side < 2; side++) {
		const StripEdge &e = side == 0 ? c.p.edges.front() : c.p.edges.back();
		if (e.strip.kind != StripKind::SIDEWALK || e.u1 - e.u0 < F.tree_min_walk_m) continue;
		const float u = side == 0 ? e.u1 - F.tree_kerb_u_m : e.u0 + F.tree_kerb_u_m;
		for (double v = c.v0 + F.tree_first_m; v < c.v1 - F.tree_first_m; v += F.tree_step_m) {
			bool near_lamp = false;
			for (double st : stations) near_lamp = near_lamp || std::fabs(st - v) < F.tree_clear_m;
			if (!near_lamp) c.maybe(FurnitureKind::TREE, v, u, 0.f);
		}
	}
}

} // namespace

LampRow lamp_row(const Profile &p, const LampDef &l) {
	LampRow r;
	if (p.edges.empty() || p.rail || p.ring) return r;
	const StripEdge &first = p.edges.front(), &last = p.edges.back();
	bool right = first.strip.kind == StripKind::SIDEWALK, left = last.strip.kind == StripKind::SIDEWALK;
	if (!right && !left) return r;
	r.any = true;
	if (right) {
		r.pole_u = first.u0 + l.inset_m;
		r.head_u = r.pole_u + l.reach_m;
	} else {
		r.pole_u = last.u1 - l.inset_m;
		r.head_u = r.pole_u - l.reach_m;
	}
	float median = 0;
	for (const StripEdge &e : p.edges)
		if (e.strip.kind == StripKind::MEDIAN || e.strip.kind == StripKind::BARRIER) median += e.u1 - e.u0;
	r.twin = right && left && median >= l.median_min_m;
	r.centre = r.head_u;
	if (r.twin) {
		r.pole_u2 = last.u1 - l.inset_m;
		r.head_u2 = r.pole_u2 - l.reach_m;
		r.centre = (r.head_u + r.head_u2) * 0.5f;
		r.half_span = (r.head_u2 - r.head_u) * 0.5f;
	}
	return r;
}

double lamp_origin(const RoadSegment &g, const LampDef &l) { return g.retreat_a + l.first_m; }

bool signal_post(const RoadNetwork &net, int n, int sid, FurnitureSpot &out) {
	const RoadSegment &g = net.segs[size_t(sid)];
	if (!g.alive || g.a == g.b) return false;
	const Profile &p = net.profile_of(sid);
	const bool at_a = g.a == n; // arriving at a: travelling b to a, the back lanes
	if ((at_a ? p.back : p.fwd).empty() || p.edges.empty()) return false;
	const FurnitureRules &F = net.cat->furniture.furniture;
	const double inner = g.len - g.retreat_a - g.retreat_b;
	const double back = std::min(double(net.cat->furniture.crossing.zebra_to_m + F.signal_back_m), inner * F.signal_max_share);
	// the kerb on the arriving traffic's right: the left edge's inner side at a, the right edge's at b
	const float u = at_a ? p.edges.back().u0 + F.signal_in_m : p.edges.front().u1 - F.signal_in_m;
	out = spot(net, FurnitureKind::SIGNAL, sid, p, at_a ? g.retreat_a + back : g.len - g.retreat_b - back, u, 0.f, at_a ? 1.f : -1.f);
	out.node = n;
	return true;
}

void segment_furniture(const RoadNetwork &net, int sid, const bool signalled[2], uint32_t kinds, std::vector<FurnitureSpot> &out) {
	thread_local std::vector<FurnitureSpot> all;
	all.clear();
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	if (!g.alive || p.edges.empty()) return;
	Placer c{ net, net.cat->furniture.furniture, net.cat->furniture.lamps, sid, g, p, g.retreat_a, g.len - g.retreat_b, all, 0, {} };
	FurnitureSpot post;
	if (g.a != g.b)
		for (int end = 0; end < 2; end++)
			if (signalled[end] && signal_post(net, end == 0 ? g.a : g.b, sid, post)) c.fixed(post);
	const int level = (g.flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT;
	if (level != FURN_NONE) {
		lamps(c);
		furniture(c, level);
		trees(c);
	}
	for (const FurnitureSpot &s : all)
		if (kinds & furniture_bit(s.kind)) out.push_back(s);
}

} // namespace city
