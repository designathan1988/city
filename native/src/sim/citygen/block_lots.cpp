// A closed block cut into lots as Roadcraft 4.0 cuts it (blockLots.ts): the land between the streets found on a raster, each
// closed piece a block; its least-area rectangle cut into one or two rows (two when deep enough, back to back) and each row
// into equal columns of about the lots' frontage (growth.json shapes.blocks and shapes.lots). Every lot reaches the footway.
#include "sim/citygen/lot_gen.h"
#include "sim/citygen/block_box.h"
#include "core/math.h"
#include "sim/catalog/catalog.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace city {

namespace {
constexpr int MIN_DEGREE_PLATE = 3; // a junction (three arms or more) gets a plate
constexpr double RUN_EPS = 1e-6; // two places of a run under this apart are one (the rounding of its sums)

class Blocks {
public:
	Blocks(const GenShapes &s, double x0, double z0, const lot_gen::Facing &f, const GenZones &zones, const GrownPlans &grown,
			const lot_gen::DesignAt &design_at) :
			S(s), K(s.block_lots), ax_(x0 - CELL_MARGIN), az_(z0 - CELL_MARGIN), W_(int(std::ceil((LOT_CELL + CELL_MARGIN * 2) / K.res))), facing_(f),
			zones_(zones), grown_(grown), design_at_(design_at) {
		paved_.assign(size_t(W_) * W_, 0);
		label_.assign(size_t(W_) * W_, -1);
	}

	void disc(double x, double z, double r) {
		const int box[4] = { std::max(0, int(std::floor((x - r - ax_) / K.res))), std::min(W_ - 1, int(std::floor((x + r - ax_) / K.res))),
			std::max(0, int(std::floor((z - r - az_) / K.res))), std::min(W_ - 1, int(std::floor((z + r - az_) / K.res))) };
		fill_disc(paved_.data(), W_, ax_, az_, K.res, x, z, r, box, 1);
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
			if (is_open(p) || p.cells * K.res * K.res < S.lots.least_m * S.lots.least_m * 2) continue;
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
	const GenShapes &S; // growth.json shapes: the raster's and the blocks' (K), the lots' sizes
	const BlocksShape &K;
	double ax_, az_;
	int W_;
	const lot_gen::Facing &facing_;
	const GenZones &zones_; // what each zone's lots measure (lot_gen::lot_size)
	const GrownPlans &grown_; // the designs of their own the lots grow, and which one a lot grows (design_at_)
	const lot_gen::DesignAt &design_at_;
	struct Through { // the span of a block's run a design lot of its first row took through the rows behind
		double s0, s1;
	};
	std::vector<Through> through_; // of the block being cut, in the order of the run
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
		const double cap = K.max_block / K.res;
		return p.i0 == 0 || p.j0 == 0 || p.i1 == W_ - 1 || p.j1 == W_ - 1 || p.i1 - p.i0 > cap || p.j1 - p.j0 > cap;
	}

	bool land_at(double x, double z, int id) const {
		int i = int(std::floor((x - ax_) / K.res)), j = int(std::floor((z - az_) / K.res));
		return i >= 0 && j >= 0 && i < W_ && j < W_ && label_[size_t(j) * W_ + i] == id;
	}
	bool paved_at(double x, double z) const {
		int i = int(std::floor((x - ax_) / K.res)), j = int(std::floor((z - az_) / K.res));
		return i >= 0 && j >= 0 && i < W_ && j < W_ && paved_[size_t(j) * W_ + i] == 1;
	}

	// spans: each row of the block with its first and last column, in row order.
	void cut_block(const std::vector<std::array<int, 3>> &spans, int id) {
		std::vector<XZ> pts;
		for (const std::array<int, 3> &r : spans) {
			pts.push_back({ ax_ + (r[1] + 0.5) * K.res, az_ + (r[0] + 0.5) * K.res });
			if (r[2] != r[1]) pts.push_back({ ax_ + (r[2] + 0.5) * K.res, az_ + (r[0] + 0.5) * K.res });
		}
		Box box;
		if (!min_area_box(convex_hull(pts), box)) return;
		double L = box.L + K.res, D = box.D + K.res;
		// The size of the lots along each long side by the zone of the street it faces, read every least_m along the side (no lot is
		// narrower, so a change of zone between two reads falls inside one lot; each lot finds its own zone below): the two rows of a block
		// back to back face two streets, and a side's zone changes from one street to the next, so a row cut whole by one zone laid towers
		// on lots cut to the shops' frontage, too narrow for any of their designs. A block of whole lots (lot_size: works, towers and
		// offices) lays two rows only when each row holds its lots' whole depth.
		const int reads = std::max(1, int(std::ceil(L / S.lots.least_m)));
		const double step = L / reads;
		std::vector<lot_gen::LotSize> along[2]; // the minus side's reads, the plus side's
		bool deep = D >= K.two_rows;
		for (int side = 0; side < 2; side++)
			for (int k = 0; k < reads; k++) {
				const double out = side == 0 ? -1 : 1;
				double fx, fz;
				box.at(-L / 2 + (k + 0.5) * step, out * D / 2, fx, fz);
				int zone = 0, profile = 0;
				facing_(fx, fz, out * box.vx, out * box.vz, zone, profile);
				along[side].push_back(lot_gen::lot_size(S.lots, zones_, zone));
				deep = deep && (!along[side].back().whole || D >= 2 * along[side].back().depth);
			}
		const int rows = deep ? 2 : 1;
		const bool front_minus = nearer_minus(box, D, 0, id);
		through_.clear();
		const double depth = D / rows;
		for (int r = 0; r < rows; r++) {
			const std::vector<lot_gen::LotSize> &sizes = along[(rows == 2 ? r == 0 : front_minus) ? 0 : 1];
			for (int k0 = 0; k0 < reads;) { // a run of reads of one size, cut in its own columns
				int k1 = k0 + 1;
				while (k1 < reads && sizes[size_t(k1)].same(sizes[size_t(k0)])) k1++;
				cut_run(sizes[size_t(k0)], -L / 2 + k0 * step, -L / 2 + k1 * step, r, rows, depth, box, D, id);
				k0 = k1;
			}
		}
	}

	// The lot between s0 and s1 along the block and t0 and t1 across it, facing the block's minus side or its plus one; false where
	// too little of it is the block's land (land_share) or its street is not zoned.
	bool lot_at(double s0, double s1, double t0, double t1, bool minus, const Box &box, int id, lot_gen::BlockLot &out) const {
		int inside = 0, total = 0;
		for (double a = K.lattice_start; a < 1; a += K.lattice_step)
			for (double b = K.lattice_start; b < 1; b += K.lattice_step) {
				double x, z;
				box.at(s0 + (s1 - s0) * a, t0 + (t1 - t0) * b, x, z);
				total++;
				if (land_at(x, z, id)) inside++;
			}
		if (double(inside) / total < K.land_share) return false;
		const double ox = minus ? -box.vx : box.vx, oz = minus ? -box.vz : box.vz;
		double fx, fz, cx, cz;
		box.at((s0 + s1) / 2, minus ? t0 : t1, fx, fz);
		int zone = 0, profile = 0;
		facing_(fx, fz, ox, oz, zone, profile);
		if (!zone) return false;
		box.at((s0 + s1) / 2, (t0 + t1) / 2, cx, cz);
		out = { cx, cz, s1 - s0, t1 - t0, std::atan2(ox, oz), zone, profile };
		return true;
	}

	// Equal lots between s0 and s1 in the band t0..t1, of about `frontage` (never wider than deep: the lab's blockLots), or for whole
	// lots as many as fit whole, each at least its frontage wide (a run narrower than one is one lot, its building one that fits it).
	// `side`: the side they face, or by where the street is nearer (-1: a block of one row).
	void equal_lots(const lot_gen::LotSize &size, double s0, double s1, double t0, double t1, int side, const Box &box, double D, int id) {
		const double L = s1 - s0, depth = t1 - t0;
		if (L <= RUN_EPS) return;
		const int columns = std::max(1, size.whole ? int(std::floor(L / size.frontage)) : int(std::lround(L / std::max(S.lots.least_m, std::min(depth, size.frontage)))));
		const double width = L / columns;
		for (int c = 0; c < columns; c++) {
			const double a = s0 + c * width, b = a + width;
			lot_gen::BlockLot l;
			if (lot_at(a, b, t0, t1, side < 0 ? nearer_minus(box, D, (a + b) / 2, id) : side > 0, box, id, l)) out_.push_back(l);
		}
	}

	// Lots across a row's run from s_from to s_to. First, in the block's first row, the lots that grow a design of their own, as
	// along a street (lot_place.cpp cut_run): walking the run a frontage at a time, a lot laid to a design's plan is that design's
	// when its own draw picks it (design_at_). It is as deep as the plan, through the rows when the plan is deeper than one, and what
	// the block has behind it is lots of the street behind (a back lot at least least_m deep; a shallower rest stays the design's):
	// the design stands on a lot it fills, and the row behind is not cut across it (through_). Works' rows (whole) keep their own
	// size. Then the stretches between them in the run's own lots (equal_lots).
	void cut_run(const lot_gen::LotSize &size, double s_from, double s_to, int r, int rows, double depth, const Box &box, double D, int id) {
		const double t0 = -D / 2 + r * depth, t1 = t0 + depth;
		const int side = rows == 2 ? (r == 0 ? 1 : 0) : -1;
		// the row's own lots between a and b, round the spans the first row's design lots took through it
		const auto own = [&](double a, double b) {
			double from = a;
			for (const Through &t : through_)
				if (r > 0 && t.s1 > from + RUN_EPS && t.s0 < b - RUN_EPS) {
					equal_lots(size, from, std::max(from, t.s0), t0, t1, side, box, D, id);
					from = std::min(b, t.s1);
				}
			equal_lots(size, from, b, t0, t1, side, box, D, id);
		};
		double open = s_from; // where the stretch of the run's own lots not yet cut begins
		for (double at = s_from; design_at_ && r == 0 && !size.whole && at < s_to;) {
			lot_gen::BlockLot here, made;
			int grown = -1;
			double deep = 0;
			const double probe_end = std::min(at + size.frontage, s_to);
			const bool minus = rows == 2 || nearer_minus(box, D, (at + probe_end) / 2, id);
			if (lot_at(at, probe_end, t0, t1, minus, box, id, here)) {
				const std::vector<GrownPlan> &designs = grown_.of(here.zone);
				for (size_t k = 0; k < designs.size() && grown < 0; k++) {
					const double end = at + designs[k].w + S.lots.width_trim, left = s_to - end;
					if (designs[k].d > D + RUN_EPS || left < -RUN_EPS || (left > RUN_EPS && left < S.lots.least_m)) continue;
					deep = D - designs[k].d >= S.lots.least_m ? designs[k].d : D;
					const double a0 = minus ? -D / 2 : D / 2 - deep;
					if (lot_at(at, end, a0, a0 + deep, minus, box, id, made) && made.zone == here.zone && design_at_(made) == int(k)) grown = int(k);
				}
			}
			if (grown < 0) {
				at += size.frontage;
				continue;
			}
			const double end = at + made.w;
			own(open, at);
			out_.push_back(made);
			if (deep < D - RUN_EPS) { // the land behind it: lots of the street behind
				const double b0 = minus ? -D / 2 + deep : -D / 2;
				equal_lots(lot_gen::lot_size(S.lots, zones_, 0), at, end, b0, b0 + D - deep, minus ? 0 : 1, box, D, id);
			}
			if (deep > depth + RUN_EPS) through_.push_back({ at, end });
			at = open = end;
		}
		own(open, s_to);
	}

	bool nearer_minus(const Box &box, double D, double s, int id) const {
		for (double d = 0; d < K.street_probe; d += 1) {
			double mx, mz, px, pz;
			box.at(s, -D / 2 - d, mx, mz);
			box.at(s, D / 2 + d, px, pz);
			bool minus = !land_at(mx, mz, id) && paved_at(mx, mz), plus = !land_at(px, pz, id) && paved_at(px, pz);
			if (minus != plus) return minus;
		}
		return true;
	}
};
} // namespace

std::vector<lot_gen::BlockLot> lot_gen::block_lots(const GenShapes &S, const LotGraph &g, const std::vector<Profile> &profiles,
		const std::vector<std::vector<double>> &tables, double x0, double z0, const Facing &facing, const GenZones &zones, const GrownPlans &grown,
		const DesignAt &design_at) {
	const BlocksShape &K = S.block_lots;
	Blocks b(S, x0, z0, facing, zones, grown, design_at);
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
		int n = std::max(2, int(std::ceil(len / K.res)) + 1);
		for (int k = 0; k < n; k++) {
			double x, z;
			g.quad[s].at(double(k) / (n - 1), x, z);
			b.disc(x, z, half);
		}
	}
	for (int n = 0; n < g.nodes(); n++)
		if (deg[n] >= MIN_DEGREE_PLATE) b.disc(g.nx[n], g.nz[n], node_half[n] * K.plate_share);
	return b.cut_all();
}

} // namespace city
