// Placing lots (Roadcraft src/jobs/lots.ts): closed blocks first, then each zoned street side cut into equal slots
// (straight streets) or drawn frontages (curves), the deepest depth that fits first.
#include "sim/citygen/lot_strips.h"

#include "core/seeded.h"
#include "sim/citygen/road_gen.h"
#include "sim/citygen/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double FRONT_PAD = 0.3, CORNER_GAP = 2, STRAIGHT_TOL = 0.5, WIDTH_TRIM = 0.6, LOT_INSET = 0.15, CLEAR_INSET = 0.3;
constexpr double MAX_SHIFT = 3, SHIFT_PAD = 0.05, DRY_MIN = 0.5, WET_CLEAR = 0.5, MAX_RELIEF = 10, WET_STEP = 4;
constexpr double DEPTH_STEP = 3, MIN_DEPTH = 12, NARROW_STEP = 2, NARROW_SHARE = 4, KEY_SCALE = 4, SEED_SCALE = 2;
constexpr double NO_ROOM = 1e300;
constexpr uint32_t SIDE_KEY = 77, SIDE_SALT = 31;

// JavaScript Math.round (halves go up).
int32_t js_round(double v) { return int32_t(std::floor(v + 0.5)); }

struct Sizes {
	double w0 = 0, w1 = 0, d0 = 0, d1 = 0, deep0 = 0, deep1 = 0;
	bool deep = false;
};
// Frontage and depth by base zone; downtown depths by exact zone, else base zone (lots.ts LOT_SIZE, DEEP).
bool sizes_for(int zone, Sizes &s) {
	int base = zone == ZONE_RES_LOW || zone == ZONE_RES_HIGH ? ZONE_RES : zone == ZONE_COM_LOW || zone == ZONE_COM_HIGH ? ZONE_COM : zone;
	switch (base) {
		case ZONE_RES: s = { 12, 19, 26, 34, 38, 48, true }; break;
		case ZONE_COM: s = { 16, 28, 30, 40, 55, 70, true }; break;
		case ZONE_IND: s = { 30, 55, 40, 60, 0, 0, false }; break;
		case ZONE_MIXED: s = { 14, 22, 28, 36, 45, 58, true }; break;
		default: return false;
	}
	if (zone == ZONE_RES_HIGH) { s.deep0 = 45; s.deep1 = 58; }
	if (zone == ZONE_COM_HIGH) { s.deep0 = 58; s.deep1 = 72; }
	return true;
}
} // namespace

double LotStrips::exact_shift(const Frame &f) const {
	double c[8];
	corners(f, CLEAR_INSET, c);
	std::vector<double> pts(c, c + 8);
	for (int k = 0; k < 4; k++) {
		pts.push_back((c[k * 2] + c[((k + 1) % 4) * 2]) / 2);
		pts.push_back((c[k * 2 + 1] + c[((k + 1) % 4) * 2 + 1]) / 2);
	}
	double x0 = 1e300, x1 = -1e300, z0 = 1e300, z1 = -1e300;
	for (size_t k = 0; k < pts.size(); k += 2) {
		x0 = std::min(x0, pts[k]); x1 = std::max(x1, pts[k]); z0 = std::min(z0, pts[k + 1]); z1 = std::max(z1, pts[k + 1]);
	}
	double shift = 0;
	for (int s = 0; s < g_.segs(); s++) {
		const Quad &q = g_.quad[s];
		double h = half_[s];
		double rx0 = std::min({ q.x0, q.cx, q.x1 }) - h, rx1 = std::max({ q.x0, q.cx, q.x1 }) + h;
		double rz0 = std::min({ q.z0, q.cz, q.z1 }) - h, rz1 = std::max({ q.z0, q.cz, q.z1 }) + h;
		if (rx1 < x0 || rx0 > x1 || rz1 < z0 || rz0 > z1) continue;
		for (size_t k = 0; k < pts.size(); k += 2) {
			double x = pts[k], z = pts[k + 1];
			if (x < rx0 || x > rx1 || z < rz0 || z > rz1) continue;
			double d = lot_gen::curve_distance(q, x, z);
			if (d < h) shift = std::max(shift, h - d);
		}
	}
	for (int n = 0; n < g_.nodes(); n++) {
		if (!g_.bulb[n]) continue;
		for (size_t m = 0; m < pts.size(); m += 2)
			if (std::hypot(pts[m] - g_.nx[n], pts[m + 1] - g_.nz[n]) < BULB_R + BULB_WALK) return NO_ROOM;
	}
	for (size_t k = 0; k < taken_.size(); k += 8)
		if (rects_overlap(c, &taken_[k])) return NO_ROOM;
	return shift;
}

void LotStrips::keep(const Frame &f, int zone, int profile) {
	mark_lot(f);
	double c[8];
	Frame inner = f;
	inner.w = f.w - WIDTH_TRIM;
	corners(inner, LOT_INSET, c);
	taken_.insert(taken_.end(), c, c + 8);
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

void LotStrips::place_side(int s, int side, int zone) {
	const Profile &info = prof_[g_.profile[s]];
	Sizes size;
	if (!sizes_for(zone, size)) return;
	double sign = side == 0 ? 1 : -1, front = (side == 0 ? info.left : info.right) + FRONT_PAD;
	double start = retreat_[s * 2] + (g_.joint[g_.sa[s]] ? 0 : CORNER_GAP), end = len(s) - retreat_[s * 2 + 1] - (g_.joint[g_.sb[s]] ? 0 : CORNER_GAP);
	const Quad &q = g_.quad[s];
	Mulberry32 rng(hash2(js_round(q.x0 * SEED_SCALE), js_round(q.z0 * SEED_SCALE), uint32_t(side) * SIDE_SALT + uint32_t(zone)));
	bool straight = q.is_straight(STRAIGHT_TOL);
	double span = end - start, aim = (size.w0 + size.w1) / 2;
	if (span < size.w0) return;
	double equal = span / std::max(1, int(js_round(span / aim)));
	double at = start;
	bool narrow_tried = false;
	while (end - at >= (straight ? std::min(size.w0, equal) - 1e-6 : size.w0)) {
		double w = straight ? equal : narrow_tried ? size.w0 : rng.range(size.w0, size.w1);
		if (end - at - w < (straight ? equal : size.w0) * 0.5) w = end - at;
		double t = t_at_length(tables_[s], at + w / 2), px, pz, dx, dz;
		q.at(t, px, pz);
		q.deriv(t, dx, dz);
		double l = std::hypot(dx, dz);
		if (l == 0) l = 1;
		double tx = dx / l, tz = dz / l, lx = tz * sign, lz = -tx * sign;
		std::vector<double> depths;
		if (size.deep) depths.push_back(rng.range(size.deep0, size.deep1));
		for (double D = rng.range(size.d0, size.d1); D >= MIN_DEPTH; D -= DEPTH_STEP) depths.push_back(D);
		bool placed = false;
		for (double D : depths) {
			Frame f{ px + lx * (front + D / 2), pz + lz * (front + D / 2), tx, tz, lx, lz, w, D };
			if (!lot_free(f)) continue;
			double back = exact_shift(f);
			if (back > 0 && back < MAX_SHIFT) { f.cx += lx * (back + SHIFT_PAD); f.cz += lz * (back + SHIFT_PAD); }
			if (back >= MAX_SHIFT || (back > 0 && (exact_shift(f) > 0 || !lot_free(f)))) continue;
			double hc = ground_.height(f.cx, f.cz);
			if (hc < TerrainGen::SEA_LEVEL + DRY_MIN) continue;
			double lo = hc, hi = hc;
			bool wet = ground_.water(f.cx, f.cz) > hc - WET_CLEAR;
			static constexpr double CORNERS[4][2] = { { -0.5, -0.5 }, { 0.5, -0.5 }, { 0.5, 0.5 }, { -0.5, 0.5 } };
			for (const auto &c : CORNERS) {
				double x = f.cx + tx * w * c[0] + lx * D * c[1], z = f.cz + tz * w * c[0] + lz * D * c[1], y = ground_.height(x, z);
				lo = std::min(lo, y);
				hi = std::max(hi, y);
				wet = wet || ground_.water(x, z) > y - WET_CLEAR;
			}
			for (double a = -w / 2; a <= w / 2 && !wet; a += WET_STEP)
				for (double b = -D / 2; b <= D / 2 && !wet; b += WET_STEP) {
					double x = f.cx + tx * a + lx * b, z = f.cz + tz * a + lz * b;
					wet = ground_.water(x, z) > ground_.height(x, z) - WET_CLEAR;
				}
			if (wet || hi - lo > MAX_RELIEF) continue;
			keep(f, zone, g_.profile[s]);
			placed = true;
			break;
		}
		if (straight) { at += w; continue; }
		if (!placed && w > size.w0 + 1 && !narrow_tried) { narrow_tried = true; continue; }
		narrow_tried = false;
		at += placed ? w : std::max(NARROW_STEP, w / NARROW_SHARE);
	}
}

} // namespace city
