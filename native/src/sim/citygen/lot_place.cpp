// Placing lots (Roadcraft src/jobs/lots.ts): closed blocks first, then each zoned street side cut into equal slots
// (straight streets) or drawn frontages (curves), the deepest depth that fits first.
#include "sim/citygen/lot_strips.h"

#include "core/seeded.h"
#include "sim/citygen/road_gen.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double FRONT_PAD = 0.3, CORNER_GAP = 2, STRAIGHT_TOL = 0.5, WIDTH_TRIM = 0.6, LOT_INSET = 0.15, CLEAR_INSET = 0.3;
constexpr double MAX_SHIFT = 3, SHIFT_PAD = 0.05, DRY_MIN = 0.5, WET_CLEAR = 0.5, MAX_RELIEF = 10, WET_STEP = 4;
constexpr double DEPTH_STEP = 3, MIN_DEPTH = 12, NARROW_STEP = 2, NARROW_SHARE = 4, KEY_SCALE = 4, SEED_SCALE = 2;
constexpr double NO_ROOM = 1e300, EPS = 1e-6;
constexpr uint32_t SIDE_KEY = 77, SIDE_SALT = 31;
constexpr int CORNER_FLOATS = 8, BOX_FLOATS = 4;
constexpr double BOX_GAP = 1e-6; // boxes this far apart are apart whatever the rounding of the separating-axis test
// Frontage/depth bands by base zone (Roadcraft lots.ts LOT_SIZE and DEEP).
constexpr double RES_W0 = 12, RES_W1 = 19, RES_D0 = 26, RES_D1 = 34, RES_DEEP0 = 38, RES_DEEP1 = 48;
constexpr double COM_W0 = 16, COM_W1 = 28, COM_D0 = 30, COM_D1 = 40, COM_DEEP0 = 55, COM_DEEP1 = 70;
constexpr double IND_W0 = 30, IND_W1 = 55, IND_D0 = 40, IND_D1 = 60;
constexpr double MIX_W0 = 14, MIX_W1 = 22, MIX_D0 = 28, MIX_D1 = 36, MIX_DEEP0 = 45, MIX_DEEP1 = 58;
constexpr double RES_HIGH_DEEP0 = 45, RES_HIGH_DEEP1 = 58, COM_HIGH_DEEP0 = 58, COM_HIGH_DEEP1 = 72;

void box_of(const double *c, double *box) {
	box[0] = box[2] = NO_ROOM;
	box[1] = box[3] = -NO_ROOM;
	for (int k = 0; k < CORNER_FLOATS; k += 2) {
		box[0] = std::min(box[0], c[k]); box[1] = std::max(box[1], c[k]);
		box[2] = std::min(box[2], c[k + 1]); box[3] = std::max(box[3], c[k + 1]);
	}
}

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
		case ZONE_RES: s = { RES_W0, RES_W1, RES_D0, RES_D1, RES_DEEP0, RES_DEEP1, true }; break;
		case ZONE_COM: s = { COM_W0, COM_W1, COM_D0, COM_D1, COM_DEEP0, COM_DEEP1, true }; break;
		case ZONE_IND: s = { IND_W0, IND_W1, IND_D0, IND_D1, 0, 0, false }; break;
		case ZONE_MIXED: s = { MIX_W0, MIX_W1, MIX_D0, MIX_D1, MIX_DEEP0, MIX_DEEP1, true }; break;
		default: return false;
	}
	if (zone == ZONE_RES_HIGH) { s.deep0 = RES_HIGH_DEEP0; s.deep1 = RES_HIGH_DEEP1; }
	if (zone == ZONE_COM_HIGH) { s.deep0 = COM_HIGH_DEEP0; s.deep1 = COM_HIGH_DEEP1; }
	return true;
}
} // namespace

double LotStrips::exact_shift(const Frame &f) const {
	double c[CORNER_FLOATS];
	corners(f, CLEAR_INSET, c);
	std::vector<double> pts(c, c + CORNER_FLOATS);
	for (int k = 0; k < 4; k++) {
		pts.push_back((c[k * 2] + c[((k + 1) % 4) * 2]) / 2);
		pts.push_back((c[k * 2 + 1] + c[((k + 1) % 4) * 2 + 1]) / 2);
	}
	double x0 = NO_ROOM, x1 = -NO_ROOM, z0 = NO_ROOM, z1 = -NO_ROOM;
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
	double box[BOX_FLOATS];
	box_of(c, box);
	for (size_t k = 0, b = 0; k < taken_.size(); k += CORNER_FLOATS, b += BOX_FLOATS) {
		const double *t = &taken_box_[b];
		if (t[0] > box[1] + BOX_GAP || t[1] < box[0] - BOX_GAP || t[2] > box[3] + BOX_GAP || t[3] < box[2] - BOX_GAP) continue;
		if (rects_overlap(c, &taken_[k])) return NO_ROOM;
	}
	return shift;
}

void LotStrips::keep(const Frame &f, int zone, int profile) {
	mark_lot(f);
	double c[CORNER_FLOATS];
	Frame inner = f;
	inner.w = f.w - WIDTH_TRIM;
	corners(inner, LOT_INSET, c);
	taken_.insert(taken_.end(), c, c + CORNER_FLOATS);
	double box[BOX_FLOATS];
	box_of(c, box);
	taken_box_.insert(taken_box_.end(), box, box + BOX_FLOATS);
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
	while (end - at >= (straight ? std::min(size.w0, equal) - EPS : size.w0)) {
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
			if (hc < TerrainGen::OCEAN_Y + DRY_MIN) continue;
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
