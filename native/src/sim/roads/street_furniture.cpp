// Street furniture placement, see street_furniture.h; every distance comes from data/catalog/street_furniture.json and
// its sources. Each sidewalk is a free width for walking and a service strip at the kerb; the pieces stand in the strip,
// the signal posts and lamps first (they must be there), then each other piece at the first place along the arc from
// where its rule wants it that no placed footprint covers, and only where it still leaves a walker a way past. No piece
// but a signal post stands in front of a lot's gate: the strip at the kerb holds the vehicles' ramps and the persons' way
// in as well as the furniture (NBR 9050; Decreto SP 59.671/2020), so a piece is moved along past the gates of its side.
#include "sim/roads/street_furniture_internal.h"

#include "sim/roads/standing_body.h"
#include "sim/roads/street_lines.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace furniture;

namespace {
constexpr int BASE_RESIDENTIAL = 1, BASE_INDUSTRIAL = 3; // ZoneDef::base
// What is placed after the lamps, keeping off them and off each other; the lamps read none of it.
constexpr uint32_t AFTER_LAMPS = furniture_bit(FurnitureKind::HYDRANT) | furniture_bit(FurnitureKind::BIN) | furniture_bit(FurnitureKind::BENCH) |
		furniture_bit(FurnitureKind::TREE);
constexpr int MAX_BESIDE = 16; // pieces a passage check looks at across one cross-section
constexpr int SLOT_TRIES = 64; // a piece is moved along past at most this many others before it gives up
constexpr double TOUCH_EPS_M = BODY_ROUNDING_M; // the bodies' one rounding (standing_body.h)

// The sidewalk of `sid` at the right of its way out of node `n`: the corner it faces (one per corner of the node).
int corner_side(const RoadSegment &g, int n) { return g.a == n ? 0 : 1; }

// A kerbside piece of kind `k` on `side`, its face kerb_clear_m from the kerb's face, facing the road.
FurnitureSpot kerbside(const RoadNetwork &net, FurnitureKind k, int sid, const Profile &p, int side, double v) {
	return spot(net, k, sid, p, v, kerb_in(p, side, kerbside_reach(net, k)), toward_road(side), 0.f);
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
	const std::vector<ArcSpan> *gates; // the lots' gates on each side (lot_access.h street_gates), or null

	bool fits(int side, float depth) const { return fits_walk(net, p, side, depth); }
	// Moves `s` along by `dir` past every gate of its side its footprint stands in; true when it moved.
	bool off_gates(FurnitureSpot &s, int dir) const {
		if (!gates || s.side < 0) return false;
		bool moved = false;
		for (const ArcSpan &gate : gates[size_t(s.side)]) {
			if (s.v + s.half_v <= gate.first + TOUCH_EPS_M || s.v - s.half_v >= gate.second - TOUCH_EPS_M) continue;
			s.v = dir > 0 ? gate.second + s.half_v : gate.first - s.half_v;
			moved = true;
		}
		return moved;
	}
	// Moves `s` along by `dir` from where its rule wants it until it stands off every gate of its side and the hand's gap two pieces
	// of the street keep (items.gap_m) clear of every placed piece of its side: never against one, edge to edge.
	bool slot(FurnitureSpot &s, int dir) const {
		const float gap = defs(net).items.gap_m;
		for (int tries = 0; tries < SLOT_TRIES; tries++) {
			bool moved = off_gates(s, dir);
			for (const FurnitureSpot &q : all) {
				const float across = q.half_u + s.half_u + gap, along = q.half_v + s.half_v + gap;
				if (q.side != s.side || std::fabs(q.u - s.u) >= across || std::fabs(q.v - s.v) > along - TOUCH_EPS_M) continue;
				s.v = dir > 0 ? q.v + along : q.v - along;
				moved = true;
			}
			if (!moved) return s.v - s.half_v >= v0 && s.v + s.half_v <= v1;
		}
		return false;
	}
	// A pole the rule stations (a lamp, a column): moved past the gates of its side, ahead or else back, its middle kept within
	// [lo, hi] (a row's: past both mouths' crosswalks and signal posts).
	void station(const FurnitureSpot &s, double lo, double hi) {
		for (const int dir : { 1, -1 }) {
			FurnitureSpot q = s;
			for (int tries = 0; tries < SLOT_TRIES && off_gates(q, dir); tries++) {}
			if (q.v < lo || q.v > hi) continue;
			all.push_back(q);
			return;
		}
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

// The main row's spans, its one owner: each pole it stood strung to the pole it stood before it (a station that could not stand is
// bridged, as a line strings the poles it has), the first and the last marked as the row's ends.
void string_row(Placer &c, size_t first) {
	int before = -1;
	FurnitureSpot *last = nullptr;
	for (size_t i = first; i < c.all.size(); i++) {
		FurnitureSpot &q = c.all[i];
		if (q.kind != FurnitureKind::LAMP || q.row != 0) continue;
		q.strung = before;
		if (before < 0) q.row_ends |= ROW_HEAD;
		before = q.station;
		last = &q;
	}
	if (last) last->row_ends |= ROW_TAIL;
}

// Lamps: spread evenly at most span_m apart between a pole just past each crosswalk (and the signal post's place). The
// lighting columns of the other face keep to the stations, staggered, or to a beat of their own (column_every_m).
void lamps(Placer &c) {
	const LampRow row = lamp_row(c.net, c.sid);
	const LampDef &d = defs(c.net).lamps;
	if (row.lone) { // no line: the block's light on a column at its middle, and the other face's beside it
		FurnitureSpot light = spot(c.net, FurnitureKind::LAMP, c.sid, c.p, row.origin, row.pole_u, toward_road(row.side), 0.f);
		light.row = 1;
		light.station = 0;
		const double lo = c.v0 + light.half_v, hi = c.v1 - light.half_v; // a block too short for a line: within its mouths
		c.station(light, lo, hi);
		if (row.twin) c.station(column(c, row, row.origin, 0), lo, hi);
		return;
	}
	const size_t first = c.all.size();
	for (int k = 0; k < row.stations; k++) {
		if ((k == 0 && !row.own_first) || (k + 1 == row.stations && !row.own_last)) continue; // a corner's or a joint's pole
		const double v = std::min(std::max(row.origin + row.spacing * k, row.first_v), row.last_v); // past both mouths' crosswalks
		FurnitureSpot main_pole = spot(c.net, FurnitureKind::LAMP, c.sid, c.p, v, row.pole_u, toward_road(row.side), 0.f);
		main_pole.station = k;
		main_pole.lit = !row.twin || k % int(d.pole_lit_every) == 0;
		c.station(main_pole, row.first_v, row.last_v);
		const bool one = row.stations == 1; // a short block's only station has the other face's lamp beside it
		if (!row.twin || d.column_every_m > 0.f || (!one && k + 1 == row.stations)) continue;
		const double between = one ? v : v + row.spacing * d.twin_stagger;
		c.station(column(c, row, between, k), row.first_v, row.last_v);
	}
	string_row(c, first);
	if (!row.twin || row.stations == 0 || d.column_every_m <= 0.f) return;
	int k = 0;
	for (double v = c.v0 + d.column_first_m; v <= c.v1 - defs(c.net).furniture.row_end_m; v += d.column_every_m)
		c.station(column(c, row, v, k++), row.first_v, row.last_v);
}

// The plates the rules stand on the segment: each one's post at its rule's arc, or at the first place on from it, away from its
// node, that keeps the hand's gap to the signal posts and the poles standing there. The pieces placed after keep off it.
void stand_plates(Placer &c, const std::vector<PlateWanted> &wanted) {
	const float from_kerb = plate_kerb(c.net, c.sid);
	for (size_t i = 0; i < wanted.size(); i++) {
		const PlateWanted &w = wanted[i];
		if (!walk_on(c.p, w.side)) continue;
		FurnitureSpot s = spot(c.net, FurnitureKind::SIGN, c.sid, c.p, w.v, kerb_in(c.p, w.side, from_kerb), toward_road(w.side), 0.f);
		s.node = w.node;
		s.station = int(i);
		if (c.slot(s, w.node == c.g.a ? 1 : -1)) c.all.push_back(s);
	}
}

// The arm of node `n` a hydrant may stand on: the lowest-numbered one with room at the corner it faces, or -1.
int hydrant_arm(const RoadNetwork &net, int n) {
	const float depth = defs(net).sidewalk.kerb_clear_m + defs(net).footprint[int(FurnitureKind::HYDRANT)].z_m;
	int best = -1;
	for (int sid : net.nodes[size_t(n)].segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (!g.alive || g.a == g.b || p.edges.empty() || furniture_level(g) == FURNITURE_LEVEL_NONE) continue;
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
// The bins come with the basic kit, the benches (`benches`) with the full one alone.
void bins_and_benches(Placer &c, bool benches) {
	const StreetFurnitureDef &d = defs(c.net);
	const Catalog &cat = *c.net.cat;
	const float clear = std::max(d.furniture.corner_clear_m, d.crossing.zebra_to_m);
	const float bin_depth = d.sidewalk.kerb_clear_m + d.footprint[int(FurnitureKind::BIN)].z_m;
	const float bench_depth = d.sidewalk.kerb_clear_m + d.footprint[int(FurnitureKind::BENCH)].z_m;
	if (d.furniture.bin_every_m > 0.f) { // a style that lays them in rows
		row_of(c, FurnitureKind::BIN, d.furniture.bin_every_m, d.furniture.bin_first_m, bin_depth);
		if (benches) row_of(c, FurnitureKind::BENCH, d.furniture.bench_every_m, d.furniture.bench_first_m, bench_depth);
		return;
	}
	for (int end = 0; end < 2 && c.v1 - c.v0 >= d.furniture.bin_spacing_m; end++) {
		const int side = corner_side(c.g, end == 0 ? c.g.a : c.g.b);
		if (cat.zone(side == 0 ? c.g.zr : c.g.zl).base != BASE_INDUSTRIAL && c.fits(side, bin_depth))
			c.place(kerbside(c.net, FurnitureKind::BIN, c.sid, c.p, side, end == 0 ? c.v0 + clear : c.v1 - clear), end == 0 ? 1 : -1);
	}
	for (int side = 0; benches && side < 2; side++)
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
	if ((c.g.flags & SEG_GENERATED) && t.in_generated_city == 0.f) return; // a style whose generated streets have none
	if (is_raised(structure_of(c.g.flags))) return; // a tree wants soil: a deck has none, though the 4.0 plants them there
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
			if (c.off_gates(s, 1)) { // in front of a gate: past it, and every piece looked at again from there
				v = s.v;
				continue;
			}
			if (leaves_way(c.net, c.p, c.all, s)) c.all.push_back(s);
			v += t.spacing_m;
		}
	}
}

} // namespace

void segment_furniture(const RoadNetwork &net, int sid, const bool signalled[2], const std::vector<ArcSpan> *gates,
		const std::vector<PlateWanted> *plates, uint32_t kinds, std::vector<FurnitureSpot> &out) {
	thread_local std::vector<FurnitureSpot> all;
	all.clear();
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	if (!g.alive || p.edges.empty()) return;
	Placer c{ net, sid, g, p, g.retreat_a, g.len - g.retreat_b, all, gates };
	FurnitureSpot post;
	if (g.a != g.b)
		for (int end = 0; end < 2; end++) {
			const int n = end == 0 ? g.a : g.b;
			if (!signalled[end]) continue;
			for (int approach : net.nodes[size_t(n)].segs)
				if (signal_post(net, n, approach, post) && post.seg == sid) all.push_back(post);
		}
	const int level = furniture_level(g);
	const bool later = (kinds & AFTER_LAMPS) != 0; // each keeps off the ones before it: asked one, all of them are placed
	if (level != FURNITURE_LEVEL_NONE) {
		list_corner_poles(net, sid, all); // on the corner, before the mouth: never stationed between the mouths
		lamps(c);
	}
	// the rules' plates stand whatever the street's kit, after the posts and poles that must be where they are
	if (plates && (later || (kinds & furniture_bit(FurnitureKind::SIGN)))) stand_plates(c, *plates);
	if (level != FURNITURE_LEVEL_NONE && later) { // basic: the lamps and the bins; full: the hydrants, the benches and the trees too
		const bool full = level == FURNITURE_LEVEL_FULL;
		if (full) hydrants(c);
		bins_and_benches(c, full);
		if (full) trees(c);
	}
	float open0 = 0, open1 = 1;
	const bool open = open_stretch(net, g, open0, open1);
	for (const FurnitureSpot &s : all) {
		// Nothing stands on a road under the ground but its junction's signals.
		const double share = g.len > 0 ? s.v / g.len : 0;
		if (s.kind != FurnitureKind::SIGNAL && (!open || share < open0 || share > open1)) continue;
		if (kinds & furniture_bit(s.kind)) out.push_back(s);
	}
}

} // namespace city
