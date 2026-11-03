// Lots along zoned streets (Roadcraft src/jobs/lots.ts buildLots): the cell's occupancy grid, the streets' setbacks
// and the tests every lot must pass. Placement is in lot_place.cpp.
#include "sim/citygen/lot_gen.h"

#include "sim/citygen/lot_strips.h"
#include "sim/citygen/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double RES = 2, ROAD_PAD = 0.4, PLATE_PAD = 4, DRY_MIN = 0.5, WET_CLEAR = 0.5, MAX_RELIEF = 10, WET_STEP = 4;
constexpr double BUSY_SHARE = 0.03, FACING_PROBE = 4, FACING_PAD = 8;
constexpr int ARC_STEPS = 24, FACING_SAMPLES = 32, MIN_PLATE_ARMS = 3, CURVE_SAMPLES = 12;
constexpr double CURVE_LAST_STEP = 1e-4;
} // namespace

double lot_gen::curve_distance(const Quad &q, double x, double z) {
	double best = 0, bd = 1e300;
	auto probe = [&](double t) {
		double px, pz;
		q.at(t, px, pz);
		double d = (px - x) * (px - x) + (pz - z) * (pz - z);
		if (d < bd) { bd = d; best = t; }
	};
	for (int i = 0; i <= CURVE_SAMPLES; i++) probe(double(i) / CURVE_SAMPLES);
	for (double step = 1.0 / (CURVE_SAMPLES * 2); step > CURVE_LAST_STEP; step /= 2) {
		double c = best;
		for (double t : { c - step, c + step })
			if (t >= 0 && t <= 1) probe(t);
	}
	return std::sqrt(bd);
}

std::vector<Lot> build_lots(const LotGraph &g, const std::vector<Profile> &profiles, const LotGround &ground, double cell_x0, double cell_z0) {
	return LotStrips(g, profiles, ground, cell_x0, cell_z0).run();
}

LotStrips::LotStrips(const LotGraph &g, const std::vector<Profile> &profiles, const LotGround &ground, double x0, double z0) :
		g_(g), prof_(profiles), ground_(ground), cx0_(x0), cz0_(z0), ax_(x0 - LOT_MARGIN), az_(z0 - LOT_MARGIN),
		W_(int(std::ceil((LOT_CELL + LOT_MARGIN * 2) / RES))) {
	grid_.assign(size_t(W_) * W_, 0);
	for (int s = 0; s < g.segs(); s++) {
		tables_.push_back(arc_table(g.quad[s], ARC_STEPS));
		const Profile &p = prof_[g.profile[s]];
		half_.push_back(std::max(p.left, p.right));
	}
}

std::vector<Lot> LotStrips::run() {
	mark_roads();
	mark_corners();
	place_blocks();
	place_strips();
	return std::move(out_);
}

void LotStrips::mark(double x, double z, double r, uint8_t v) {
	int i0 = std::max(0, int(std::floor((x - r - ax_) / RES))), i1 = std::min(W_ - 1, int(std::ceil((x + r - ax_) / RES)));
	int j0 = std::max(0, int(std::floor((z - r - az_) / RES))), j1 = std::min(W_ - 1, int(std::ceil((z + r - az_) / RES)));
	for (int j = j0; j <= j1; j++)
		for (int i = i0; i <= i1; i++) {
			double px = ax_ + (i + 0.5) * RES - x, pz = az_ + (j + 0.5) * RES - z;
			if (px * px + pz * pz <= r * r) grid_[size_t(j) * W_ + i] = v;
		}
}

void LotStrips::mark_roads() {
	for (int s = 0; s < g_.segs(); s++) {
		int n = std::max(2, int(std::ceil(len(s) / RES)) + 1);
		for (int i = 0; i < n; i++) {
			double x, z;
			g_.quad[s].at(t_at_length(tables_[s], len(s) * i / (n - 1)), x, z);
			mark(x, z, half_[s] + ROAD_PAD, 1);
		}
	}
}

void LotStrips::mark_corners() {
	retreat_.assign(size_t(g_.segs()) * 2, 0.0);
	std::vector<std::vector<std::pair<int, int>>> arms(g_.nodes());
	for (int s = 0; s < g_.segs(); s++) {
		arms[g_.sa[s]].push_back({ s, 0 });
		arms[g_.sb[s]].push_back({ s, 1 });
	}
	for (int n = 0; n < g_.nodes(); n++) {
		const auto &list = arms[n];
		if (list.size() == 1 && g_.bulb[n]) {
			int s = list[0].first;
			retreat_[s * 2 + list[0].second] = bulb_retreat(arm_of(g_.quad[s], prof_[g_.profile[s]], list[0].second), len(s));
		}
		if (list.size() < 2) continue;
		std::vector<Arm> a;
		std::vector<double> lens, tmp;
		std::vector<int> order;
		double max_half = 0;
		for (const auto &r : list) {
			a.push_back(arm_of(g_.quad[r.first], prof_[g_.profile[r.first]], r.second));
			lens.push_back(len(r.first));
			max_half = std::max(max_half, double(prof_[g_.profile[r.first]].right));
		}
		junction_retreats(a, lens, tmp, order);
		for (size_t i = 0; i < list.size(); i++) retreat_[list[i].first * 2 + list[i].second] = tmp[i];
		if (list.size() >= MIN_PLATE_ARMS) mark(g_.nx[n], g_.nz[n], max_half + PLATE_PAD, 1);
	}
}

void LotStrips::facing(double fx, double fz, double ox, double oz, int &zone, int &profile) const {
	double qx = fx + ox * FACING_PROBE, qz = fz + oz * FACING_PROBE, bd = 1e300;
	int best = -1;
	for (int s = 0; s < g_.segs(); s++) {
		const Quad &q = g_.quad[s];
		double h = half_[s] + FACING_PAD;
		if (qx < std::min({ q.x0, q.cx, q.x1 }) - h || qx > std::max({ q.x0, q.cx, q.x1 }) + h ||
				qz < std::min({ q.z0, q.cz, q.z1 }) - h || qz > std::max({ q.z0, q.cz, q.z1 }) + h) continue;
		double d = lot_gen::curve_distance(q, qx, qz);
		if (d < bd) { bd = d; best = s; }
	}
	zone = profile = 0;
	if (best < 0 || bd > half_[best] + FACING_PROBE) return;
	const Quad &q = g_.quad[best];
	double t = 0, td = 1e300;
	for (int k = 0; k <= FACING_SAMPLES; k++) {
		double x, z;
		q.at(double(k) / FACING_SAMPLES, x, z);
		double d = (x - qx) * (x - qx) + (z - qz) * (z - qz);
		if (d < td) { td = d; t = double(k) / FACING_SAMPLES; }
	}
	double px, pz, dx, dz;
	q.at(t, px, pz);
	q.deriv(t, dx, dz);
	zone = (qx - px) * dz - (qz - pz) * dx > 0 ? g_.zl[best] : g_.zr[best]; // + is left of the road
	profile = g_.profile[best];
}

// Dry, not on water anywhere on a 4 m lattice, relief under MAX_RELIEF.
bool LotStrips::unfit(const Frame &f) const {
	double hc = ground_.height(f.cx, f.cz);
	if (hc < TerrainGen::SEA_LEVEL + DRY_MIN) return true;
	double lo = hc, hi = hc;
	for (double a = -f.w / 2; a <= f.w / 2 + 1e-6; a += WET_STEP)
		for (double b = -f.d / 2; b <= f.d / 2 + 1e-6; b += WET_STEP) {
			double px = f.cx + f.tx * a + f.lx * b, pz = f.cz + f.tz * a + f.lz * b, y = ground_.height(px, pz);
			lo = std::min(lo, y);
			hi = std::max(hi, y);
			if (ground_.water(px, pz) > y - WET_CLEAR) return true;
		}
	return hi - lo > MAX_RELIEF;
}

void LotStrips::corners(const Frame &f, double inset, double *c) {
	double hw = f.w / 2 - inset, hd = f.d / 2 - inset;
	const double v[8] = { f.cx - f.tx * hw - f.lx * hd, f.cz - f.tz * hw - f.lz * hd, f.cx + f.tx * hw - f.lx * hd, f.cz + f.tz * hw - f.lz * hd,
		f.cx + f.tx * hw + f.lx * hd, f.cz + f.tz * hw + f.lz * hd, f.cx - f.tx * hw + f.lx * hd, f.cz - f.tz * hw + f.lz * hd };
	std::copy(v, v + 8, c);
}

void LotStrips::mark_lot(const Frame &f) {
	for (double a = -f.w / 2; a <= f.w / 2; a += RES / 2)
		for (double b = -f.d / 2; b <= f.d / 2; b += RES / 2) {
			int i = int(std::floor((f.cx + f.tx * a + f.lx * b - ax_) / RES)), j = int(std::floor((f.cz + f.tz * a + f.lz * b - az_) / RES));
			if (i >= 0 && j >= 0 && i < W_ && j < W_) grid_[size_t(j) * W_ + i] = 2;
		}
}

bool LotStrips::lot_free(const Frame &f) const {
	int total = 0, busy = 0;
	for (double a = -f.w / 2 + 1; a <= f.w / 2 - 1; a += RES)
		for (double b = -f.d / 2 + 1; b <= f.d / 2 - 1; b += RES) {
			int i = int(std::floor((f.cx + f.tx * a + f.lx * b - ax_) / RES)), j = int(std::floor((f.cz + f.tz * a + f.lz * b - az_) / RES));
			total++;
			if (i < 0 || j < 0 || i >= W_ || j >= W_) continue;
			if (grid_[size_t(j) * W_ + i]) busy++;
		}
	return busy <= total * BUSY_SHARE;
}

bool rects_overlap(const double *a, const double *b) {
	for (int pass = 0; pass < 2; pass++) {
		const double *p = pass ? b : a;
		for (int e = 0; e < 4; e++) {
			double ex = p[((e + 1) % 4) * 2] - p[e * 2], ez = p[((e + 1) % 4) * 2 + 1] - p[e * 2 + 1];
			double amin = 1e300, amax = -1e300, bmin = 1e300, bmax = -1e300;
			for (int v = 0; v < 4; v++) {
				double pa = a[v * 2] * ez - a[v * 2 + 1] * ex, pb = b[v * 2] * ez - b[v * 2 + 1] * ex;
				amin = std::min(amin, pa); amax = std::max(amax, pa); bmin = std::min(bmin, pb); bmax = std::max(bmax, pb);
			}
			if (amax <= bmin || bmax <= amin) return false;
		}
	}
	return true;
}

} // namespace city
