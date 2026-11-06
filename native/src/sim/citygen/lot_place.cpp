// Placing lots (Roadcraft world/lots.ts): closed blocks first, then the open land along each zoned street side (streetPlan): a run of street with free
// land behind its footway is one strip, of a lot's depth, cut in equal lots.
#include "sim/citygen/lot_strips.h"

#include "core/seeded.h"
#include "sim/citygen/road_gen.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double FRONT_PAD = 0.3, WIDTH_TRIM = 0.6;
constexpr double STRIP_DEPTH = 32, STRIP_FRONTAGE = 22, STRIP_LEAST = 10; // lots.ts LOT_DEPTH, LOT_FRONTAGE, MIN_LOT
// Works take more than the 4.0 gives them: a lorry has to turn in the yard. On its lots a tenth of the docks had no way in (yard_test).
constexpr double WORKS_DEPTH = 50, WORKS_FRONTAGE = 42;
constexpr double RUN_STEP = 1, BEHIND_FOOTWAY = 1.5, BACK_IN = 1; // where a run reads the land: lots.ts streetPlan
constexpr double KEY_SCALE = 4, SEED_SCALE = 2;
constexpr uint32_t SIDE_KEY = 77;

// JavaScript Math.round (halves go up).
int32_t js_round(double v) { return int32_t(std::floor(v + 0.5)); }
} // namespace

void LotStrips::keep(const Frame &f, int zone, int profile) {
	mark_lot(f);
	if (std::floor((f.cx - cx0_) / LOT_CELL) != 0 || std::floor((f.cz - cz0_) / LOT_CELL) != 0) return;
	Lot l;
	l.x = f.cx; l.z = f.cz; l.w = f.w - WIDTH_TRIM; l.d = f.d;
	l.rot = std::atan2(-f.lx, -f.lz);
	l.zone = zone;
	l.profile = profile;
	l.seed = hash2(js_round(f.cx * SEED_SCALE), js_round(f.cz * SEED_SCALE), uint32_t(zone));
	out_.push_back(l);
}

void LotStrips::place_blocks() {
	auto facing_fn = [this](double fx, double fz, double ox, double oz, int &zone, int &profile) { facing(fx, fz, ox, oz, zone, profile); };
	for (const lot_gen::BlockLot &b : lot_gen::block_lots(g_, prof_, tables_, cx0_, cz0_, facing_fn)) {
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
	const double sign = side == 0 ? 1 : -1, face = (side == 0 ? info.left : info.right) + FRONT_PAD, length = len(s);
	const bool works = zone == ZONE_IND;
	const double depth = works ? WORKS_DEPTH : STRIP_DEPTH, frontage = works ? WORKS_FRONTAGE : STRIP_FRONTAGE;
	const auto clear = [&](double at) {
		const Along a = along(s, at, sign);
		const double near = face + BEHIND_FOOTWAY, far = face + depth - BACK_IN;
		return land_at(a.px + a.lx * near, a.pz + a.lz * near) && land_at(a.px + a.lx * far, a.pz + a.lz * far);
	};
	double from = -1;
	for (double at = 0;; at += RUN_STEP) {
		const bool last = at + RUN_STEP > length;
		const double here = last ? length : at; // the run reaches the street's end: the next street's strip starts there
		const bool ok = clear(here);
		if (ok && from < 0) from = here;
		if ((!ok || last) && from >= 0) {
			cut_run({ s, sign, face, depth, frontage, zone }, from, ok ? here : here - RUN_STEP);
			from = -1;
		}
		if (last) break;
	}
}

// A run cut in equal lots, each kept where its ground will take it.
void LotStrips::cut_run(const Strip &strip, double from, double to) {
	if (to - from < STRIP_LEAST) return;
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
