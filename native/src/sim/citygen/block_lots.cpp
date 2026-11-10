// A closed block cut into lots as Roadcraft 4.0 cuts it (blockLots.ts): the land between the streets found on a 1 m
// raster, each closed piece a block; its least-area rectangle cut into one or two rows (two when >= 44 m deep, back to
// back) and each row into equal columns of about LOT_FRONTAGE. Every lot reaches the footway.
#include "sim/citygen/lot_gen.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace city {

namespace {
constexpr double LOT_FRONTAGE = 22, TWO_ROWS = 44, MIN_LOT = 10, RES = 1, MAX_BLOCK = 320, PLATE_SHARE = 1.15;
constexpr double LATTICE_START = 0.1, LATTICE_STEP = 0.2, LAND_SHARE = 0.5, STREET_PROBE = 12;
constexpr int MIN_DEGREE_PLATE = 3;
constexpr double EPS_LEN = 1e-9, EPS_AREA = 1e-6;

using P2 = std::pair<double, double>;

std::vector<P2> convex_hull(std::vector<P2> s) {
	std::sort(s.begin(), s.end());
	if (s.size() < 3) return s;
	auto cross = [](const P2 &o, const P2 &a, const P2 &b) { return (a.first - o.first) * (b.second - o.second) - (a.second - o.second) * (b.first - o.first); };
	std::vector<P2> lower, upper;
	for (const P2 &q : s) {
		while (lower.size() >= 2 && cross(lower[lower.size() - 2], lower.back(), q) <= 0) lower.pop_back();
		lower.push_back(q);
	}
	for (int k = int(s.size()) - 1; k >= 0; k--) {
		while (upper.size() >= 2 && cross(upper[upper.size() - 2], upper.back(), s[k]) <= 0) upper.pop_back();
		upper.push_back(s[k]);
	}
	lower.pop_back();
	upper.pop_back();
	lower.insert(lower.end(), upper.begin(), upper.end());
	return lower;
}

struct Box {
	double cx, cz, ux, uz, vx, vz, L, D;
};

bool min_area_box(const std::vector<P2> &h, Box &best) {
	if (h.size() < 3) return false;
	double best_a = NO_ROOM;
	bool found = false;
	for (size_t e = 0; e < h.size(); e++) {
		const P2 &a = h[e], &b = h[(e + 1) % h.size()];
		double l = std::hypot(b.first - a.first, b.second - a.second);
		if (l < EPS_LEN) continue;
		double ux = (b.first - a.first) / l, uz = (b.second - a.second) / l, vx = -uz, vz = ux;
		double s0 = NO_ROOM, s1 = -NO_ROOM, t0 = NO_ROOM, t1 = -NO_ROOM;
		for (const P2 &q : h) {
			double s = q.first * ux + q.second * uz, t = q.first * vx + q.second * vz;
			s0 = std::min(s0, s); s1 = std::max(s1, s); t0 = std::min(t0, t); t1 = std::max(t1, t);
		}
		double area = (s1 - s0) * (t1 - t0);
		if (area >= best_a - EPS_AREA) continue;
		best_a = area;
		double sm = (s0 + s1) / 2, tm = (t0 + t1) / 2, ls = s1 - s0, dt = t1 - t0;
		double cx = ux * sm + vx * tm, cz = uz * sm + vz * tm;
		best = ls >= dt ? Box{ cx, cz, ux, uz, vx, vz, ls, dt } : Box{ cx, cz, vx, vz, -ux, -uz, dt, ls };
		found = true;
	}
	return found;
}

class Blocks {
public:
	Blocks(double x0, double z0, const lot_gen::Facing &f) :
			ax_(x0 - CELL_MARGIN), az_(z0 - CELL_MARGIN), W_(int(std::ceil((LOT_CELL + CELL_MARGIN * 2) / RES))), facing_(f) {
		paved_.assign(size_t(W_) * W_, 0);
		label_.assign(size_t(W_) * W_, -1);
	}

	void disc(double x, double z, double r) {
		const int box[4] = { std::max(0, int(std::floor((x - r - ax_) / RES))), std::min(W_ - 1, int(std::floor((x + r - ax_) / RES))),
			std::max(0, int(std::floor((z - r - az_) / RES))), std::min(W_ - 1, int(std::floor((z + r - az_) / RES))) };
		fill_disc(paved_.data(), W_, ax_, az_, RES, x, z, r, box, 1);
	}

	// The land's closed pieces, by runs of a row joined to the runs they touch in the row above: the same pieces a flood of
	// the raster finds, 4-connected, in the order its row-major scan meets them, at the cost of the runs and not of the cells.
	std::vector<lot_gen::BlockLot> cut_all() {
		find_runs();
		std::vector<Piece> pieces;
		std::vector<int> piece_of(runs_.size(), -1), next(runs_.size(), -1);
		for (int r = 0; r < int(runs_.size()); r++) {
			const int root = find(r);
			if (piece_of[size_t(root)] < 0) {
				piece_of[size_t(root)] = int(pieces.size());
				pieces.push_back({ r, r, 0, W_, -1, runs_[size_t(r)].row, -1 });
			}
			Piece &p = pieces[size_t(piece_of[size_t(root)])];
			const Run &run = runs_[size_t(r)];
			if (p.last != r) next[size_t(p.last)] = r;
			p.last = r;
			p.cells += run.to - run.from + 1;
			p.i0 = std::min(p.i0, run.from);
			p.i1 = std::max(p.i1, run.to);
			p.j1 = run.row;
		}
		std::vector<std::array<int, 3>> rows;
		for (const Piece &p : pieces) {
			if (is_open(p) || p.cells * RES * RES < MIN_LOT * MIN_LOT * 2) continue;
			const int id = runs_[size_t(p.first)].row * W_ + runs_[size_t(p.first)].from;
			rows.clear();
			for (int r = p.first; r >= 0; r = next[size_t(r)]) {
				const Run &run = runs_[size_t(r)];
				std::fill(label_.begin() + run.row * W_ + run.from, label_.begin() + run.row * W_ + run.to + 1, id);
				if (!rows.empty() && rows.back()[0] == run.row) rows.back()[2] = run.to;
				else rows.push_back({ run.row, run.from, run.to });
			}
			cut_block(rows, id);
		}
		return std::move(out_);
	}

private:
	double ax_, az_;
	int W_;
	const lot_gen::Facing &facing_;
	std::vector<uint8_t> paved_;
	std::vector<int> label_;
	std::vector<lot_gen::BlockLot> out_;
	struct Run {
		int row, from, to, parent; // columns from..to of a row, and the run it was joined to (itself: a root)
	};
	struct Piece { // a closed or open piece of land: its first and last run in row order, cells and box
		int first, last, cells, i0, i1, j0, j1;
	};
	std::vector<Run> runs_;

	int find(int r) {
		while (runs_[size_t(r)].parent != r) {
			runs_[size_t(r)].parent = runs_[size_t(runs_[size_t(r)].parent)].parent;
			r = runs_[size_t(r)].parent;
		}
		return r;
	}

	// The roots keep the lowest run, so a piece is found at the run a row-major scan reaches first.
	void join(int a, int b) {
		a = find(a);
		b = find(b);
		if (a != b) runs_[size_t(std::max(a, b))].parent = std::min(a, b);
	}

	void find_runs() {
		int above = 0, above_end = 0;
		for (int j = 0; j < W_; j++) {
			const int here = int(runs_.size());
			const uint8_t *row = paved_.data() + size_t(j) * W_;
			for (int i = 0; i < W_;) {
				if (row[i]) {
					i++;
					continue;
				}
				const int from = i;
				while (i < W_ && !row[i]) i++;
				const int id = int(runs_.size());
				runs_.push_back({ j, from, i - 1, id });
				while (above < above_end && runs_[size_t(above)].to < from) above++;
				for (int k = above; k < above_end && runs_[size_t(k)].from <= i - 1; k++) join(id, k);
			}
			above = here;
			above_end = int(runs_.size());
		}
	}

	bool is_open(const Piece &p) const {
		const double cap = MAX_BLOCK / RES;
		return p.i0 == 0 || p.j0 == 0 || p.i1 == W_ - 1 || p.j1 == W_ - 1 || p.i1 - p.i0 > cap || p.j1 - p.j0 > cap;
	}

	bool land_at(double x, double z, int id) const {
		int i = int(std::floor((x - ax_) / RES)), j = int(std::floor((z - az_) / RES));
		return i >= 0 && j >= 0 && i < W_ && j < W_ && label_[size_t(j) * W_ + i] == id;
	}
	bool paved_at(double x, double z) const {
		int i = int(std::floor((x - ax_) / RES)), j = int(std::floor((z - az_) / RES));
		return i >= 0 && j >= 0 && i < W_ && j < W_ && paved_[size_t(j) * W_ + i] == 1;
	}

	// spans: each row of the block with its first and last column, in row order.
	void cut_block(const std::vector<std::array<int, 3>> &spans, int id) {
		std::vector<P2> pts;
		for (const std::array<int, 3> &r : spans) {
			pts.push_back({ ax_ + (r[1] + 0.5) * RES, az_ + (r[0] + 0.5) * RES });
			if (r[2] != r[1]) pts.push_back({ ax_ + (r[2] + 0.5) * RES, az_ + (r[0] + 0.5) * RES });
		}
		Box box;
		if (!min_area_box(convex_hull(pts), box)) return;
		double L = box.L + RES, D = box.D + RES;
		auto at = [&](double s, double t, double &x, double &z) { x = box.cx + box.ux * s + box.vx * t; z = box.cz + box.uz * s + box.vz * t; };
		int rows = D >= TWO_ROWS ? 2 : 1;
		double depth = D / rows;
		int columns = std::max(1, int(std::lround(L / std::max(MIN_LOT, std::min(depth, LOT_FRONTAGE)))));
		double width = L / columns;
		for (int r = 0; r < rows; r++)
			for (int c = 0; c < columns; c++) {
				double s0 = -L / 2 + c * width, s1 = s0 + width, t0 = -D / 2 + r * depth, t1 = t0 + depth;
				int inside = 0, total = 0;
				for (double a = LATTICE_START; a < 1; a += LATTICE_STEP)
					for (double b = LATTICE_START; b < 1; b += LATTICE_STEP) {
						double x, z;
						at(s0 + (s1 - s0) * a, t0 + (t1 - t0) * b, x, z);
						total++;
						if (land_at(x, z, id)) inside++;
					}
				if (double(inside) / total < LAND_SHARE) continue;
				bool minus = rows == 2 ? r == 0 : nearer_minus(box, D, (s0 + s1) / 2, id);
				double ox = minus ? -box.vx : box.vx, oz = minus ? -box.vz : box.vz;
				double fx, fz, cx, cz;
				at((s0 + s1) / 2, minus ? t0 : t1, fx, fz);
				int zone = 0, profile = 0;
				facing_(fx, fz, ox, oz, zone, profile);
				if (!zone) continue;
				at((s0 + s1) / 2, (t0 + t1) / 2, cx, cz);
				out_.push_back({ cx, cz, width, depth, std::atan2(ox, oz), zone, profile });
			}
	}

	bool nearer_minus(const Box &box, double D, double s, int id) const {
		for (double d = 0; d < STREET_PROBE; d += 1) {
			double mx = box.cx + box.ux * s + box.vx * (-D / 2 - d), mz = box.cz + box.uz * s + box.vz * (-D / 2 - d);
			double px = box.cx + box.ux * s + box.vx * (D / 2 + d), pz = box.cz + box.uz * s + box.vz * (D / 2 + d);
			bool minus = !land_at(mx, mz, id) && paved_at(mx, mz), plus = !land_at(px, pz, id) && paved_at(px, pz);
			if (minus != plus) return minus;
		}
		return true;
	}
};
} // namespace

std::vector<lot_gen::BlockLot> lot_gen::block_lots(const LotGraph &g, const std::vector<Profile> &profiles,
		const std::vector<std::vector<double>> &tables, double x0, double z0, const Facing &facing) {
	Blocks b(x0, z0, facing);
	std::vector<double> node_half(g.nodes(), 0.0);
	std::vector<int> deg(g.nodes(), 0);
	for (int s = 0; s < g.segs(); s++) {
		const Profile &p = profiles[g.profile[s]];
		double half = std::max(p.left, p.right);
		node_half[g.sa[s]] = std::max(node_half[g.sa[s]], half);
		node_half[g.sb[s]] = std::max(node_half[g.sb[s]], half);
		deg[g.sa[s]]++;
		deg[g.sb[s]]++;
		double len = tables[s].back();
		int n = std::max(2, int(std::ceil(len / RES)) + 1);
		for (int k = 0; k < n; k++) {
			double x, z;
			g.quad[s].at(double(k) / (n - 1), x, z);
			b.disc(x, z, half);
		}
	}
	for (int n = 0; n < g.nodes(); n++)
		if (deg[n] >= MIN_DEGREE_PLATE) b.disc(g.nx[n], g.nz[n], node_half[n] * PLATE_SHARE);
	return b.cut_all();
}

} // namespace city
