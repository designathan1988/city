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

void LotStrips::keep(const Frame &f, int zone, int profile) {
	mark_lot(f);
	if (std::floor((f.cx - cx0_) / LOT_CELL) != 0 || std::floor((f.cz - cz0_) / LOT_CELL) != 0) return;
	Lot l;
	l.x = f.cx; l.z = f.cz; l.w = f.w; l.d = f.d; // the whole piece of the run: two lots side by side share their side, no strip between them
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
