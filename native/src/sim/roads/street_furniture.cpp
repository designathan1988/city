// Street furniture placement, see street_furniture.h; every distance comes from data/catalog/street_furniture.json and
// its sources. Each sidewalk is a free width for walking and a service strip at the kerb; the pieces stand in the strip,
// the signal posts and lamps first (they must be there), then each other piece at the first place along the arc from
// where its rule wants it that no placed footprint covers, and only where it still leaves a walker a way past.
#include "sim/roads/street_furniture_internal.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace furniture;

namespace {
constexpr int FURN_FULL = 0, FURN_NONE = 2; // RoadSegment furniture level (SEG_FURNITURE_MASK)
constexpr int BASE_RESIDENTIAL = 1, BASE_INDUSTRIAL = 3; // ZoneDef::base
constexpr int MAX_BESIDE = 16; // pieces a passage check looks at across one cross-section
constexpr double SQRT2 = 1.4142135623730951;
constexpr int SLOT_TRIES = 64; // a piece is moved along past at most this many others before it gives up
// Two places closer than what they need by under this are apart: a piece moved to just touch another must not find it
// in the way again through the rounding of the sum it was moved by.
constexpr double TOUCH_EPS_M = 1e-3;
constexpr double STRAIGHT_SLACK_M = 0.01; // a segment this much longer than its chord bends

int side_of(const Profile &p, float u) {
	const StripEdge &r = p.edges.front(), &l = p.edges.back();
	if (r.strip.kind == StripKind::SIDEWALK && u >= r.u0 && u <= r.u1) return 0;
	if (l.strip.kind == StripKind::SIDEWALK && u >= l.u0 && u <= l.u1) return 1;
	return -1;
}

// Leaves the free width (NBR 9050) beside a piece reaching `depth` from the kerb on `side`.
bool fits_walk(const RoadNetwork &net, const Profile &p, int side, float depth) {
	return walk_on(p, side) && depth <= walk_width(p, side) - defs(net).sidewalk.free_width_m;
}
// The sidewalk of `sid` at the right of its way out of node `n`: the corner it faces (one per corner of the node).
int corner_side(const RoadSegment &g, int n) { return g.a == n ? 0 : 1; }

// A kerbside piece of kind `k` on `side`, its face kerb_clear_m from the kerb's face, facing the road.
FurnitureSpot kerbside(const RoadNetwork &net, FurnitureKind k, int sid, const Profile &p, int side, double v) {
	const float half_across = defs(net).footprint[int(k)].z_m * 0.5f;
	return spot(net, k, sid, p, v, kerb_in(p, side, defs(net).sidewalk.kerb_clear_m + half_across), toward_road(side), 0.f);
}

// Whether a walker still finds a way past `c` on its sidewalk, among the pieces placed beside it: the centres a
// walker may take (the sidewalk less ped_margin_m each side) not all within a body radius of a footprint.
bool leaves_way(const RoadNetwork &net, const Profile &p, const std::vector<FurnitureSpot> &placed, const FurnitureSpot &c) {
	if (c.side < 0) return true;
	const TrafficParams &tp = net.cat->traffic;
	const float reach = tp.ped_body_m;
	const StripEdge &e = c.side == 0 ? p.edges.front() : p.edges.back();
	const float lo = e.u0 + tp.ped_margin_m, hi = e.u1 - tp.ped_margin_m;
	if (hi <= lo) return true; // no room to walk in at all: nothing to keep free
	std::pair<float, float> shut[MAX_BESIDE];
	int n = 0;
	shut[n++] = { c.u - c.half_u - reach, c.u + c.half_u + reach };
	for (const FurnitureSpot &q : placed) {
		if (n >= MAX_BESIDE) break;
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
	int sid;
	const RoadSegment &g;
	const Profile &p;
	double v0, v1; // the mouths
	std::vector<FurnitureSpot> &all;

	bool fits(int side, float depth) const { return fits_walk(net, p, side, depth); }
	// Moves `s` along by `dir` from where its rule wants it until no placed footprint on its side covers it.
	bool slot(FurnitureSpot &s, int dir) const {
		for (int tries = 0; tries < SLOT_TRIES; tries++) {
			bool moved = false;
			for (const FurnitureSpot &q : all) {
				if (q.side != s.side || std::fabs(q.u - s.u) >= q.half_u + s.half_u || std::fabs(q.v - s.v) > q.half_v + s.half_v - TOUCH_EPS_M) continue;
				s.v = dir > 0 ? q.v + q.half_v + s.half_v : q.v - q.half_v - s.half_v;
				moved = true;
			}
			if (!moved) return s.v - s.half_v >= v0 && s.v + s.half_v <= v1;
		}
		return false;
	}
	void place(FurnitureSpot s, int dir) {
		if (slot(s, dir) && leaves_way(net, p, all, s)) all.push_back(s);
	}
};

FurnitureSpot column(const Placer &c, const LampRow &row, double v, int station) {
	FurnitureSpot s = spot(c.net, FurnitureKind::LAMP, c.sid, c.p, v, row.pole_u2, toward_road(1 - row.side), 0.f);
	s.row = 1;
	s.station = station;
	return s;
}

// Lamps: spread evenly at most span_m apart between a pole just past each crosswalk (and the signal post's place). The
// lighting columns of the other face keep to the stations, staggered, or to a beat of their own (column_every_m).
void lamps(Placer &c) {
	const LampRow row = lamp_row(c.net, c.sid);
	const LampDef &d = defs(c.net).lamps;
	for (int k = 0; k < row.stations; k++) {
		const double v = row.origin + row.spacing * k;
		FurnitureSpot main_pole = spot(c.net, FurnitureKind::LAMP, c.sid, c.p, v, row.pole_u, toward_road(row.side), 0.f);
		main_pole.station = k;
		main_pole.lit = !row.twin || k % int(d.pole_lit_every) == 0;
		c.all.push_back(main_pole);
		const bool one = row.stations == 1; // a short block's only station has the other face's lamp beside it
		if (!row.twin || d.column_every_m > 0.f || (!one && k + 1 == row.stations)) continue;
		const double between = one ? v : v + row.spacing * d.twin_stagger;
		c.all.push_back(column(c, row, between, k));
	}
	if (!row.twin || row.stations == 0 || d.column_every_m <= 0.f) return;
	int k = 0;
	for (double v = c.v0 + d.column_first_m; v <= c.v1 - defs(c.net).furniture.row_end_m; v += d.column_every_m) c.all.push_back(column(c, row, v, k++));
}

// The arm of node `n` a hydrant may stand on: the lowest-numbered one with room at the corner it faces, or -1.
int hydrant_arm(const RoadNetwork &net, int n) {
	const float depth = defs(net).sidewalk.kerb_clear_m + defs(net).footprint[int(FurnitureKind::HYDRANT)].z_m;
	int best = -1;
	for (int sid : net.nodes[size_t(n)].segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (!g.alive || g.a == g.b || p.edges.empty() || ((g.flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT) == FURN_NONE) continue;
		if (fits_walk(net, p, corner_side(g, n), depth) && (best < 0 || sid < best)) best = sid;
	}
	return best;
}

// Whether node `n` holds the hydrant of its square (hydrant_square): the node nearest the square's centre whose corner
// has room for one.
bool hydrant_node(const RoadNetwork &net, int n) {
	double x0, z0, x1, z1;
	hydrant_square(net, net.nodes[size_t(n)].x, net.nodes[size_t(n)].z, x0, z0, x1, z1);
	const double cx = (x0 + x1) / 2, cz = (z0 + z1) / 2;
	int best = -1;
	double best_d = 0;
	net.node_hash.query(float(x0), float(z0), float(x1), float(z1), [&](int k) {
		const RoadNode &o = net.nodes[size_t(k)];
		if (!o.alive || o.x < x0 || o.x >= x1 || o.z < z0 || o.z >= z1 || hydrant_arm(net, k) < 0) return;
		const double d = (o.x - cx) * (o.x - cx) + (o.z - cz) * (o.z - cz);
		if (best < 0 || d < best_d || (d == best_d && k < best)) {
			best = k;
			best_d = d;
		}
	});
	return best == n;
}

// Hydrants (NPT 034): at street corners, one in each square of hydrants.reach_m coverage.
void hydrants(Placer &c) {
	const StreetFurnitureDef &d = defs(c.net);
	for (int end = 0; end < 2; end++) {
		const int n = end == 0 ? c.g.a : c.g.b;
		if (c.g.a == c.g.b || !hydrant_node(c.net, n) || hydrant_arm(c.net, n) != c.sid) continue;
		const double v = end == 0 ? c.v0 + d.crossing.zebra_to_m : c.v1 - d.crossing.zebra_to_m;
		c.place(kerbside(c.net, FurnitureKind::HYDRANT, c.sid, c.p, corner_side(c.g, n), v), end == 0 ? 1 : -1);
	}
}

// A piece of kind `k` down both sides, from `first` past the mouth and `every` metres on, wherever one fits.
void row_of(Placer &c, FurnitureKind k, float every, float first, float depth) {
	for (int side = 0; side < 2; side++) {
		if (!c.fits(side, depth)) continue;
		for (double v = c.v0 + first; v <= c.v1 - defs(c.net).furniture.row_end_m; v += every) c.place(kerbside(c.net, k, c.sid, c.p, side, v), 1);
	}
}

// A bin at each corner (the sidewalk at the right of each arm's way out of the node) of a full-furniture street outside
// industrial zones, on blocks long enough to keep bins bin_spacing_m apart; a bench in the middle of residential blocks.
void bins_and_benches(Placer &c) {
	const StreetFurnitureDef &d = defs(c.net);
	const Catalog &cat = *c.net.cat;
	const float clear = std::max(d.furniture.corner_clear_m, d.crossing.zebra_to_m);
	const float bin_depth = d.sidewalk.kerb_clear_m + d.footprint[int(FurnitureKind::BIN)].z_m;
	const float bench_depth = d.sidewalk.kerb_clear_m + d.footprint[int(FurnitureKind::BENCH)].z_m;
	if (d.furniture.bin_every_m > 0.f) { // a style that lays them in rows
		row_of(c, FurnitureKind::BIN, d.furniture.bin_every_m, d.furniture.bin_first_m, bin_depth);
		row_of(c, FurnitureKind::BENCH, d.furniture.bench_every_m, d.furniture.bench_first_m, bench_depth);
		return;
	}
	for (int end = 0; end < 2 && c.v1 - c.v0 >= d.furniture.bin_spacing_m; end++) {
		const int side = corner_side(c.g, end == 0 ? c.g.a : c.g.b);
		if (cat.zone(side == 0 ? c.g.zr : c.g.zl).base != BASE_INDUSTRIAL && c.fits(side, bin_depth))
			c.place(kerbside(c.net, FurnitureKind::BIN, c.sid, c.p, side, end == 0 ? c.v0 + clear : c.v1 - clear), end == 0 ? 1 : -1);
	}
	for (int side = 0; side < 2; side++)
		if (cat.zone(side == 0 ? c.g.zr : c.g.zl).base == BASE_RESIDENTIAL && c.fits(side, bench_depth))
			c.place(kerbside(c.net, FurnitureKind::BENCH, c.sid, c.p, side, (c.v0 + c.v1) / 2), 1);
}

// How far a tree's centre keeps from a piece of kind `k` (its footprint's edge along the arc).
float tree_clear(const TreeDef &t, FurnitureKind k) {
	switch (k) {
		case FurnitureKind::LAMP: case FurnitureKind::SIGNAL: return t.pole_clear_m;
		case FurnitureKind::HYDRANT: return t.hydrant_clear_m;
		case FurnitureKind::TREE: return t.spacing_m;
		default: return t.furniture_clear_m;
	}
}

// Street trees spacing_m apart, clear of the corner, the crosswalk and every piece by the manual's distances; not on
// the lit face of a street lit from one side when the other face has a sidewalk (NPIP-1 5.2).
void trees(Placer &c) {
	const StreetFurnitureDef &d = defs(c.net);
	const TreeDef &t = d.trees;
	if ((c.g.flags & SEG_FREE) && t.in_generated_city == 0.f) return; // a style whose generated streets have none
	const LampRow row = lamp_row(c.net, c.sid);
	const double clear = std::max(t.corner_clear_m, d.crossing.zebra_to_m + t.crossing_clear_m);
	for (int side = 0; side < 2; side++) {
		if (!c.fits(side, t.kerb_clear_m + t.pit_m)) continue;
		if (row.any && !row.twin && row.side == side && walk_on(c.p, 1 - side)) continue;
		const float u = kerb_in(c.p, side, t.kerb_clear_m + t.pit_m * 0.5f);
		for (double v = c.v0 + clear; v <= c.v1 - clear;) {
			bool moved = false;
			for (const FurnitureSpot &q : c.all) {
				if (q.side != side || std::fabs(q.v - v) - q.half_v > tree_clear(t, q.kind) - TOUCH_EPS_M) continue;
				v = q.v + q.half_v + tree_clear(t, q.kind);
				moved = true;
			}
			if (moved) continue; // check the new place against all again
			if (v > c.v1 - clear) break;
			FurnitureSpot s = spot(c.net, FurnitureKind::TREE, c.sid, c.p, v, u, 0.f, 0.f);
			if (leaves_way(c.net, c.p, c.all, s)) c.all.push_back(s);
			v += t.spacing_m;
		}
	}
}

} // namespace

FurnitureSpot furniture::spot(const RoadNetwork &net, FurnitureKind k, int sid, const Profile &p, double v, float u, float face_left, float face_fwd) {
	FurnitureSpot s;
	s.kind = k;
	s.seg = sid;
	s.side = side_of(p, u);
	s.v = v;
	s.u = u;
	s.face_left = face_left;
	s.face_fwd = face_fwd;
	const Footprint &f = defs(net).footprint[int(k)];
	const bool along_traffic = face_fwd != 0; // its z runs along the segment
	s.half_v = (along_traffic ? f.z_m : f.x_m) * 0.5f;
	s.half_u = (along_traffic ? f.x_m : f.z_m) * 0.5f;
	return s;
}

bool sidewalk_on(const Profile &p, int side) { return !p.edges.empty() && walk_on(p, side); }

float sidewalk_u(const Profile &p, int side, float from_kerb) { return kerb_in(p, side, from_kerb); }

float plate_kerb(const RoadNetwork &net, int seg) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const RoadNode &a = net.nodes[size_t(g.a)], &b = net.nodes[size_t(g.b)];
	const SignDef &d = defs(net).signs;
	return g.len > std::hypot(b.x - a.x, b.z - a.z) + STRAIGHT_SLACK_M ? d.plate_kerb_curve_m : d.plate_kerb_m;
}

bool kerbside_place(const RoadNetwork &net, int seg, const Profile &p, int side, FurnitureKind k, float &u) {
	const StreetFurnitureDef &d = defs(net);
	if (!sidewalk_on(p, side)) return false;
	if (k == FurnitureKind::DRAIN) {
		u = kerb_in(p, side, 0);
		return true;
	}
	if (k == FurnitureKind::TREE) {
		u = kerb_in(p, side, d.trees.kerb_clear_m + d.trees.pit_m * 0.5f);
		return fits_walk(net, p, side, d.trees.kerb_clear_m + d.trees.pit_m);
	}
	if (k == FurnitureKind::SIGN) {
		u = kerb_in(p, side, plate_kerb(net, seg));
		return true;
	}
	const float across = d.footprint[int(k)].z_m;
	u = kerb_in(p, side, d.sidewalk.kerb_clear_m + across * 0.5f);
	return k == FurnitureKind::COLUMN || fits_walk(net, p, side, d.sidewalk.kerb_clear_m + across);
}

void hydrant_square(const RoadNetwork &net, double x, double z, double &x0, double &z0, double &x1, double &z1) {
	const double cell = defs(net).hydrants.reach_m / SQRT2;
	x0 = std::floor(x / cell) * cell;
	z0 = std::floor(z / cell) * cell;
	x1 = x0 + cell;
	z1 = z0 + cell;
}

void segment_furniture(const RoadNetwork &net, int sid, const bool signalled[2], uint32_t kinds, std::vector<FurnitureSpot> &out) {
	thread_local std::vector<FurnitureSpot> all;
	all.clear();
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	if (!g.alive || p.edges.empty()) return;
	Placer c{ net, sid, g, p, g.retreat_a, g.len - g.retreat_b, all };
	FurnitureSpot post;
	if (g.a != g.b)
		for (int end = 0; end < 2; end++) {
			const int n = end == 0 ? g.a : g.b;
			if (!signalled[end]) continue;
			for (int approach : net.nodes[size_t(n)].segs)
				if (signal_post(net, n, approach, post) && post.seg == sid) all.push_back(post);
		}
	const int level = (g.flags & SEG_FURNITURE_MASK) >> SEG_FURNITURE_SHIFT;
	if (level != FURN_NONE) {
		lamps(c);
		hydrants(c);
		if (level == FURN_FULL) bins_and_benches(c);
		trees(c);
	}
	for (const FurnitureSpot &s : all)
		if (kinds & furniture_bit(s.kind)) out.push_back(s);
}

} // namespace city
