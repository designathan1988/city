// Lots along zoned streets (Roadcraft src/jobs/lots.ts buildLots): the cell's occupancy grid, the streets' setbacks
// and the tests every lot must pass. Placement is in lot_place.cpp.
#include "sim/citygen/lot_gen.h"

#include "sim/citygen/lot_strips.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double RES = 2, ROAD_PAD = 0.4, DRY_MIN = 0.5, WET_CLEAR = 0.5, MAX_RELIEF = 10, WET_STEP = 4;
constexpr double FACING_PROBE = 4, FACING_PAD = 8;
constexpr int ARC_STEPS = 24, FACING_SAMPLES = 32;
constexpr double EPS = 1e-6;
constexpr uint8_t ROADWAY = 1, TAKEN = 2; // what the grid says of a cell
constexpr double LOT_END_IN = 3; // a cell and a half: past any rounding of a point to its cell
constexpr int MIN_THROUGH_ARMS = 3;
constexpr float STRAIGHT_ON = -0.95f; // two ways out of a node this opposed are one street
} // namespace

double lot_gen::curve_distance(const Quad &q, double x, double z) {
	double t = 0, dist = 0;
	closest_t(q, x, z, t, dist);
	return dist;
}

std::vector<Lot> build_lots(const LotGraph &g, const std::vector<Profile> &profiles, const LotGround &ground, double cell_x0, double cell_z0) {
	return LotStrips(g, profiles, ground, cell_x0, cell_z0).run();
}

LotStrips::LotStrips(const LotGraph &g, const std::vector<Profile> &profiles, const LotGround &ground, double x0, double z0) :
		g_(g), prof_(profiles), ground_(ground), cx0_(x0), cz0_(z0), ax_(x0 - CELL_MARGIN), az_(z0 - CELL_MARGIN),
		W_(int(std::ceil((LOT_CELL + CELL_MARGIN * 2) / RES))) {
	grid_.assign(size_t(W_) * W_, 0);
	for (int s = 0; s < g.segs(); s++) {
		tables_.push_back(arc_table(g.quad[s], ARC_STEPS));
		const Profile &p = prof_[g.profile[s]];
		half_.push_back(std::max(p.left, p.right));
	}
}

std::vector<Lot> LotStrips::run() {
	mark_roads();
	place_blocks();
	place_strips();
	return std::move(out_);
}

void LotStrips::mark(double x, double z, double r, uint8_t v) {
	const int box[4] = { std::max(0, int(std::floor((x - r - ax_) / RES))), std::min(W_ - 1, int(std::ceil((x + r - ax_) / RES))),
		std::max(0, int(std::floor((z - r - az_) / RES))), std::min(W_ - 1, int(std::ceil((z + r - az_) / RES))) };
	fill_disc(grid_.data(), W_, ax_, az_, RES, x, z, r, box, v);
}

// The way out of node `n` along street `s`.
V2 LotStrips::way_out(int s, int n) const {
	const Quad &q = g_.quad[s];
	return (g_.sa[s] == n ? V2(float(q.cx - q.x0), float(q.cz - q.z0)) : V2(float(q.cx - q.x1), float(q.cz - q.z1))).norm();
}

// How far short of node `n` the paving of street `s` stops: a street that ends against one running straight on stops at that one's edge, and past
// it, on the side with no arm, the footway goes on. A street that runs on itself, or a corner, reaches the node.
double LotStrips::stops_short(int s, int n, const std::vector<int> &arms) const {
	if (int(arms.size()) < MIN_THROUGH_ARMS) return 0;
	const V2 way = way_out(s, n);
	for (int other : arms)
		if (other != s && way.dot(way_out(other, n)) < STRAIGHT_ON) return 0;
	for (int u : arms)
		for (int v : arms)
			if (u != s && v != s && u < v && way_out(u, n).dot(way_out(v, n)) < STRAIGHT_ON) return std::max(half_[u], half_[v]);
	return 0;
}

void LotStrips::mark_roads() {
	std::vector<std::vector<int>> arms(size_t(g_.nodes()));
	for (int s = 0; s < g_.segs(); s++) {
		arms[size_t(g_.sa[s])].push_back(s);
		arms[size_t(g_.sb[s])].push_back(s);
	}
	for (int n = 0; n < g_.nodes(); n++)
		if (arms[size_t(n)].size() == 1 && g_.bulb[n]) mark(g_.nx[n], g_.nz[n], BULB_R + BULB_WALK, ROADWAY);
	for (int s = 0; s < g_.segs(); s++) {
		const double from = stops_short(s, g_.sa[s], arms[size_t(g_.sa[s])]), to = len(s) - stops_short(s, g_.sb[s], arms[size_t(g_.sb[s])]);
		int n = std::max(2, int(std::ceil(len(s) / RES)) + 1);
		for (int i = 0; i < n; i++) {
			const double at = len(s) * i / (n - 1);
			if (at < from || at > to) continue;
			double x, z;
			g_.quad[s].at(t_at_length(tables_[s], at), x, z);
			mark(x, z, half_[s] + ROAD_PAD, ROADWAY);
		}
	}
}

void LotStrips::facing(double fx, double fz, double ox, double oz, int &zone, int &profile) const {
	double qx = fx + ox * FACING_PROBE, qz = fz + oz * FACING_PROBE, bd = NO_ROOM;
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
	double t = 0, td = NO_ROOM;
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
	if (hc < TerrainGen::OCEAN_Y + DRY_MIN) return true;
	double lo = hc, hi = hc;
	for (double a = -f.w / 2; a <= f.w / 2 + EPS; a += WET_STEP)
		for (double b = -f.d / 2; b <= f.d / 2 + EPS; b += WET_STEP) {
			double px = f.cx + f.tx * a + f.lx * b, pz = f.cz + f.tz * a + f.lz * b, y = ground_.height(px, pz);
			lo = std::min(lo, y);
			hi = std::max(hi, y);
			if (ground_.water(px, pz) > y - WET_CLEAR) return true;
		}
	return hi - lo > MAX_RELIEF;
}

void LotStrips::mark_lot(const Frame &f) {
	// a cell short of its ends: the next lot starts right at its end (its run's next one, or the next street's strip), and a cell of the
	// grid is wider than any rounding there
	for (double a = -f.w / 2 + LOT_END_IN; a <= f.w / 2 - LOT_END_IN; a += RES / 2) {
		const double x = f.cx + f.tx * a, z = f.cz + f.tz * a;
		for (double b = -f.d / 2; b <= f.d / 2; b += RES / 2) {
			int i = ifloor((x + f.lx * b - ax_) / RES), j = ifloor((z + f.lz * b - az_) / RES);
			if (i >= 0 && j >= 0 && i < W_ && j < W_) grid_[size_t(j) * W_ + i] = TAKEN;
		}
	}
}

bool LotStrips::lot_free(double x, double z) const {
	const int i = ifloor((x - ax_) / RES), j = ifloor((z - az_) / RES);
	if (i < 0 || j < 0 || i >= W_ || j >= W_) return true;
	return grid_[size_t(j) * W_ + i] == 0;
}

} // namespace city
