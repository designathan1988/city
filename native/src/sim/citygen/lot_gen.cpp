// Lots along zoned streets (Roadcraft src/jobs/lots.ts buildLots): the cell's occupancy grid, the streets' setbacks
// and the tests every lot must pass. Placement is in lot_place.cpp.
#include "sim/citygen/lot_gen.h"
#include "sim/catalog/catalog.h"

#include "sim/citygen/lot_strips.h"
#include "sim/terrain/terrain_gen.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double EPS = 1e-6; // rounding at a lattice's last step
constexpr uint8_t ROADWAY = 1, TAKEN = 2; // what the grid says of a cell
constexpr double LOT_END_IN_CELLS = 1.5; // a cell and a half: past any rounding of a point to its cell
constexpr int MIN_THROUGH_ARMS = 3; // a junction: three arms or more
} // namespace

lot_gen::LotSize lot_gen::lot_size(const LotsShape &P, const GenZones &zones, const GrownPlans &grown, int zone) {
	if (zone == zones.industrial) return { P.works_frontage, P.works_depth, true };
	if (zone >= 0 && size_t(zone) < grown.size() && grown[size_t(zone)].w > 0) return { grown[size_t(zone)].w + P.width_trim, grown[size_t(zone)].d, true };
	return { P.frontage_m, P.depth_m, false };
}

double lot_gen::curve_distance(const Quad &q, double x, double z) {
	double t = 0, dist = 0;
	closest_t(q, x, z, t, dist);
	return dist;
}

std::vector<Lot> build_lots(const GenShapes &S, const LotGraph &g, const std::vector<Profile> &profiles, const GenZones &zones, const GrownPlans &grown,
	const LotGround &ground, double cell_x0, double cell_z0) {
	return LotStrips(S, g, profiles, zones, grown, ground, cell_x0, cell_z0).run();
}

LotStrips::LotStrips(const GenShapes &s, const LotGraph &g, const std::vector<Profile> &profiles, const GenZones &zones, const GrownPlans &grown,
	const LotGround &ground, double x0, double z0) :
		S(s), K(s.strips), P(s.lots), g_(g), prof_(profiles), zones_(zones), grown_(grown), ground_(ground), cx0_(x0), cz0_(z0), ax_(x0 - CELL_MARGIN),
		az_(z0 - CELL_MARGIN),
		W_(int(std::ceil((LOT_CELL + CELL_MARGIN * 2) / K.res))) {
	grid_.assign(size_t(W_) * W_, 0);
	for (int s = 0; s < g.segs(); s++) {
		tables_.push_back(arc_table(g.quad[s], K.arc_steps));
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
	const int box[4] = { std::max(0, int(std::floor((x - r - ax_) / K.res))), std::min(W_ - 1, int(std::ceil((x + r - ax_) / K.res))),
		std::max(0, int(std::floor((z - r - az_) / K.res))), std::min(W_ - 1, int(std::ceil((z + r - az_) / K.res))) };
	fill_disc(grid_.data(), W_, ax_, az_, K.res, x, z, r, box, v);
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
		if (other != s && way.dot(way_out(other, n)) < K.straight_on) return 0;
	for (int u : arms)
		for (int v : arms)
			if (u != s && v != s && u < v && way_out(u, n).dot(way_out(v, n)) < K.straight_on) return std::max(half_[u], half_[v]);
	return 0;
}

void LotStrips::mark_roads() {
	std::vector<std::vector<int>> arms(size_t(g_.nodes()));
	for (int s = 0; s < g_.segs(); s++) {
		arms[size_t(g_.sa[s])].push_back(s);
		arms[size_t(g_.sb[s])].push_back(s);
	}
	for (int n = 0; n < g_.nodes(); n++)
		if (arms[size_t(n)].size() == 1 && g_.bulb[n] > 0) mark(g_.nx[n], g_.nz[n], g_.bulb[n], ROADWAY);
	for (int s = 0; s < g_.segs(); s++) {
		const double from = stops_short(s, g_.sa[s], arms[size_t(g_.sa[s])]), to = len(s) - stops_short(s, g_.sb[s], arms[size_t(g_.sb[s])]);
		int n = std::max(2, int(std::ceil(len(s) / K.res)) + 1);
		for (int i = 0; i < n; i++) {
			const double at = len(s) * i / (n - 1);
			if (at < from || at > to) continue;
			double x, z;
			g_.quad[s].at(t_at_length(tables_[s], at), x, z);
			mark(x, z, half_[s] + K.road_pad, ROADWAY);
		}
	}
}

void LotStrips::facing(double fx, double fz, double ox, double oz, int &zone, int &profile) const {
	double qx = fx + ox * K.facing_probe, qz = fz + oz * K.facing_probe, bd = NO_ROOM;
	int best = -1;
	for (int s = 0; s < g_.segs(); s++) {
		const Quad &q = g_.quad[s];
		double h = half_[s] + K.facing_pad;
		if (qx < std::min({ q.x0, q.cx, q.x1 }) - h || qx > std::max({ q.x0, q.cx, q.x1 }) + h ||
				qz < std::min({ q.z0, q.cz, q.z1 }) - h || qz > std::max({ q.z0, q.cz, q.z1 }) + h) continue;
		double d = lot_gen::curve_distance(q, qx, qz);
		if (d < bd) { bd = d; best = s; }
	}
	zone = profile = 0;
	if (best < 0 || bd > half_[best] + K.facing_probe) return;
	const Quad &q = g_.quad[best];
	double t = 0, td = NO_ROOM;
	for (int k = 0; k <= K.facing_samples; k++) {
		double x, z;
		q.at(double(k) / K.facing_samples, x, z);
		double d = (x - qx) * (x - qx) + (z - qz) * (z - qz);
		if (d < td) { td = d; t = double(k) / K.facing_samples; }
	}
	double px, pz, dx, dz;
	q.at(t, px, pz);
	q.deriv(t, dx, dz);
	zone = (qx - px) * dz - (qz - pz) * dx > 0 ? g_.zl[best] : g_.zr[best]; // + is left of the road
	profile = g_.profile[best];
}

// Dry, not on water or closed land (LotGround::closed) anywhere on a 4 m lattice, relief under MAX_RELIEF.
bool LotStrips::unfit(const Frame &f) const {
	double hc = ground_.height(f.cx, f.cz);
	if (hc < TerrainGen::OCEAN_Y + K.dry_min) return true;
	double lo = hc, hi = hc;
	for (double a = -f.w / 2; a <= f.w / 2 + EPS; a += K.wet_step)
		for (double b = -f.d / 2; b <= f.d / 2 + EPS; b += K.wet_step) {
			double px = f.cx + f.tx * a + f.lx * b, pz = f.cz + f.tz * a + f.lz * b, y = ground_.height(px, pz);
			lo = std::min(lo, y);
			hi = std::max(hi, y);
			if (ground_.water(px, pz) > y - K.wet_clear || (ground_.closed && ground_.closed(px, pz))) return true;
		}
	return hi - lo > K.max_relief;
}

void LotStrips::mark_lot(const Frame &f) {
	// a cell short of its ends: a cell of the grid is wider than the gap to the next lot, and the next street's strip starts right there
	const double end_in = LOT_END_IN_CELLS * K.res;
	for (double a = -f.w / 2 + end_in; a <= f.w / 2 - end_in; a += K.res / 2) {
		const double x = f.cx + f.tx * a, z = f.cz + f.tz * a;
		for (double b = -f.d / 2; b <= f.d / 2; b += K.res / 2) {
			int i = ifloor((x + f.lx * b - ax_) / K.res), j = ifloor((z + f.lz * b - az_) / K.res);
			if (i >= 0 && j >= 0 && i < W_ && j < W_) grid_[size_t(j) * W_ + i] = TAKEN;
		}
	}
}

bool LotStrips::lot_free(double x, double z) const {
	const int i = ifloor((x - ax_) / K.res), j = ifloor((z - az_) / K.res);
	if (i < 0 || j < 0 || i >= W_ || j >= W_) return true;
	return grid_[size_t(j) * W_ + i] == 0;
}

} // namespace city
