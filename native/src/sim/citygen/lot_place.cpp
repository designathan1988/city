// Placing lots (Roadcraft world/lots.ts): closed blocks first, then the open land along each zoned street side (streetPlan): a run of street with free
// land behind its footway is one strip, of a lot's depth, cut in equal lots.
#include "sim/citygen/lot_strips.h"
#include "sim/catalog/catalog.h"

#include "core/seeded.h"
#include "sim/citygen/road_gen.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
// a side's key and a lot's seed from where they stand, in quarter and half metres (the same key whatever the float rounding), the
// side's number past any zone code
constexpr double KEY_SCALE = 4, SEED_SCALE = 2;
constexpr uint32_t SIDE_KEY = 77;

// JavaScript Math.round (halves go up).
int32_t js_round(double v) { return int32_t(std::floor(v + 0.5)); }
} // namespace

// From (x, z) to street s's paving edge: its distance to the street's line less the street's half on that side (+ is left of it).
double LotStrips::edge_gap(int s, double x, double z) const {
	const Quad &q = g_.quad[s];
	double t = 0, dist = 0, px, pz, dx, dz;
	closest_t(q, x, z, t, dist);
	q.at(t, px, pz);
	q.deriv(t, dx, dz);
	const Profile &p = prof_[g_.profile[s]];
	return dist - ((x - px) * dz - (z - pz) * dx > 0 ? p.left : p.right);
}

// The street other than `skip` whose paving edge lies nearest the point within reach_m, its line crossed going that way (dx, dz), or -1.
int LotStrips::street_off(double x, double z, double dx, double dz, int skip) const {
	int best = -1;
	double least = P.reach_m + P.paving_gap;
	for (int s = 0; s < g_.segs(); s++) {
		const Quad &q = g_.quad[s];
		const double h = half_[s] + least;
		if (s == skip || x < std::min({ q.x0, q.cx, q.x1 }) - h || x > std::max({ q.x0, q.cx, q.x1 }) + h || z < std::min({ q.z0, q.cz, q.z1 }) - h ||
				z > std::max({ q.z0, q.cz, q.z1 }) + h) continue;
		double t = 0, dist = 0, px, pz;
		closest_t(q, x, z, t, dist);
		q.at(t, px, pz);
		if ((px - x) * dx + (pz - z) * dz <= 0) continue; // behind the edge, not ahead of it
		const double gap = edge_gap(s, x, z);
		if (gap < least) {
			least = gap;
			best = s;
		}
	}
	return best;
}

// The least gap from street s's paving to one edge of the lot, read at its two ends and its middle: `side` the edge's way out from the
// lot's middle in the lot's own axes (its along-front and away-from-street units), `along` the edge's half length.
double LotStrips::gap_along(int s, const Frame &f, double side, double along) const {
	double least = NO_ROOM;
	for (double k : { -1.0, 0.0, 1.0 }) {
		const bool front = side == 0; // the front: its edge along t at -d/2; a side: along l at +-w/2
		const double a = front ? k * along : side * f.w / 2, b = front ? -f.d / 2 : k * along;
		least = std::min(least, edge_gap(s, f.cx + f.tx * a + f.lx * b, f.cz + f.tz * a + f.lz * b));
	}
	return least;
}

// The lot brought to the paving (lots.ts streetPlan and blockPlan: a lot reaches past its street sides and is cut back to the land, a
// hair off the paving): its front to the footway of the street it faces, and a side on a street (a corner, a block's end) to that one's,
// each moved by its least gap so no point of it stands on the paving. A raster block's box and a strip's set-back otherwise left a strip
// of bare ground between the footway and the lot's walls.
LotStrips::Frame LotStrips::to_paving(const Frame &f) const {
	Frame g = f;
	const double fx = f.cx - f.lx * f.d / 2, fz = f.cz - f.lz * f.d / 2;
	const int street = street_off(fx, fz, -f.lx, -f.lz, -1);
	if (street >= 0) {
		const double move = std::min(gap_along(street, f, 0, f.w / 2) - P.paving_gap, P.reach_m);
		g.d += move;
		g.cx -= f.lx * move / 2;
		g.cz -= f.lz * move / 2;
	}
	for (double side : { -1.0, 1.0 }) {
		const double sx = g.cx + g.tx * side * g.w / 2, sz = g.cz + g.tz * side * g.w / 2;
		const int cross = street_off(sx, sz, g.tx * side, g.tz * side, street);
		if (cross < 0) continue;
		// the lots' trim (keep) takes half its width off this side too: laid on here, the side still meets the paving
		const double move = std::min(gap_along(cross, g, side, g.d / 2) - P.paving_gap, P.reach_m) + P.width_trim / 2;
		g.w += move;
		g.cx += g.tx * side * move / 2;
		g.cz += g.tz * side * move / 2;
	}
	return g;
}

void LotStrips::keep(const Frame &laid, int zone, int profile) {
	const Frame f = to_paving(laid);
	mark_lot(f);
	if (std::floor((f.cx - cx0_) / LOT_CELL) != 0 || std::floor((f.cz - cz0_) / LOT_CELL) != 0) return;
	Lot l;
	l.x = f.cx; l.z = f.cz; l.w = f.w - P.width_trim; l.d = f.d;
	l.rot = std::atan2(-f.lx, -f.lz);
	l.zone = zone;
	l.profile = profile;
	l.seed = hash2(js_round(f.cx * SEED_SCALE), js_round(f.cz * SEED_SCALE), uint32_t(zone));
	out_.push_back(l);
}

void LotStrips::place_blocks() {
	auto facing_fn = [this](double fx, double fz, double ox, double oz, int &zone, int &profile) { facing(fx, fz, ox, oz, zone, profile); };
	for (const lot_gen::BlockLot &b : lot_gen::block_lots(S, g_, prof_, tables_, cx0_, cz0_, facing_fn)) {
		double lx = -std::sin(b.rot), lz = -std::cos(b.rot);
		Frame f{ b.x, b.z, lz, -lx, lx, lz, b.w, b.d };
		if (unfit(f)) continue;
		keep(f, b.zone, b.profile);
	}
}

void LotStrips::place_strips() {
	struct Side {
		int s, side, zone;
		uint32_t key;
	};
	std::vector<Side> sides;
	for (int s = 0; s < g_.segs(); s++) {
		double mx, mz;
		g_.quad[s].at(0.5, mx, mz);
		for (int side = 0; side < 2; side++) {
			int zone = side == 0 ? g_.zl[s] : g_.zr[s];
			if (zone) sides.push_back({ s, side, zone, hash2(js_round(mx * KEY_SCALE), js_round(mz * KEY_SCALE), uint32_t(side) + SIDE_KEY) });
		}
	}
	std::stable_sort(sides.begin(), sides.end(), [](const Side &a, const Side &b) { return a.key < b.key; });
	for (const Side &sd : sides) place_side(sd.s, sd.side, sd.zone);
}

// One place of a street's side: the point, the way along it and the way off the street.
LotStrips::Along LotStrips::along(int s, double at, double sign) const {
	double px, pz, dx, dz;
	const double t = t_at_length(tables_[s], at);
	g_.quad[s].at(t, px, pz);
	g_.quad[s].deriv(t, dx, dz);
	const double l = std::hypot(dx, dz);
	const double tx = l > 0 ? dx / l : 1, tz = l > 0 ? dz / l : 0;
	return { px, pz, tx, tz, tz * sign, -tx * sign };
}

// Runs of street where the land behind the footway is open, and its far side too.
void LotStrips::place_side(int s, int side, int zone) {
	const Profile &info = prof_[g_.profile[s]];
	const double sign = side == 0 ? 1 : -1, face = (side == 0 ? info.left : info.right) + P.front_pad, length = len(s);
	const bool works = zone == zones_.industrial;
	const double depth = works ? P.works_depth : P.depth_m, frontage = works ? P.works_frontage : P.frontage_m;
	const auto clear = [&](double at) {
		const Along a = along(s, at, sign);
		const double near = face + P.behind_footway, far = face + depth - P.back_in;
		return lot_free(a.px + a.lx * near, a.pz + a.lz * near) && lot_free(a.px + a.lx * far, a.pz + a.lz * far);
	};
	double from = -1;
	for (double at = 0;; at += P.along_step) {
		const bool last = at + P.along_step > length;
		const double here = last ? length : at; // the run reaches the street's end: the next street's strip starts there
		const bool ok = clear(here);
		if (ok && from < 0) from = here;
		if ((!ok || last) && from >= 0) {
			cut_run({ s, sign, face, depth, frontage, zone }, from, ok ? here : here - P.along_step);
			from = -1;
		}
		if (last) break;
	}
}

// A run cut in equal lots, each kept where its ground will take it.
void LotStrips::cut_run(const Strip &strip, double from, double to) {
	if (to - from < P.least_m) return;
	const int parts = std::max(1, int(js_round((to - from) / strip.frontage)));
	const double w = (to - from) / parts;
	for (int k = 0; k < parts; k++) {
		const Along a = along(strip.s, from + (k + 0.5) * w, strip.sign);
		const double middle = strip.face + strip.depth / 2;
		const Frame f{ a.px + a.lx * middle, a.pz + a.lz * middle, a.tx, a.tz, a.lx, a.lz, w, strip.depth };
		if (!unfit(f)) keep(f, strip.zone, g_.profile[strip.s]);
	}
}

} // namespace city
