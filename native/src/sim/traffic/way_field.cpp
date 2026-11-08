// The grid of a yard and its clearance (way_search.h): what is solid, and how far every cell is from it.
#include "sim/traffic/way_search.h"


#include <algorithm>
#include <cmath>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr double HALF = 0.5, ROOT_TWO = 1.41421356237309505; // the square root of two: a diagonal step's length per cell
const float NO_ROOM = -std::numeric_limits<float>::infinity();
} // namespace

void Field::begin(double x, double z, double size, int cells_x, int cells_z) {
	x0 = x;
	z0 = z;
	cell = size;
	nx = cells_x;
	nz = cells_z;
	blocked.clear();
	blocked.reserve(size_t(nx) * nz);
	clear.clear();
}

bool Field::lay_row(uint8_t value) {
	if (!whole()) blocked.insert(blocked.end(), size_t(nx), value);
	return whole();
}

float Field::clear_at(double x, double z) const {
	const int i = int(std::floor((x - x0) / cell)), j = int(std::floor((z - z0) / cell));
	return i < 0 || j < 0 || i >= nx || j >= nz ? NO_ROOM : clear[size_t(j) * nx + i];
}

bool Field::solid_at(double x, double z) const {
	const int i = int(std::floor((x - x0) / cell)), j = int(std::floor((z - z0) / cell));
	return i < 0 || j < 0 || i >= nx || j >= nz || blocked[size_t(j) * nx + i] != 0;
}

// The rows the ring's span covers; none for a ring of fewer than three corners.
RingFill::RingFill(const Field &f, const std::vector<V2> *ring, uint8_t value) : ring_(ring), value_(value) {
	if (ring->size() < 3) return;
	float lo = std::numeric_limits<float>::infinity(), hi = -lo;
	for (const V2 &p : *ring) {
		lo = std::min(lo, p.y);
		hi = std::max(hi, p.y);
	}
	row_ = std::max(0, int(std::ceil((lo - f.z0) / f.cell - HALF)));
	last_ = std::min(f.nz - 1, int(std::floor((hi - f.z0) / f.cell - HALF)));
}

bool RingFill::advance(Field &f, int &work) {
	const std::vector<V2> &ring = *ring_;
	while (row_ <= last_ && work > 0) {
		const double z = f.z0 + (row_ + HALF) * f.cell;
		xs_.clear();
		for (size_t a = 0, b = ring.size() - 1; a < ring.size(); b = a++) {
			const V2 &p = ring[a], &q = ring[b];
			if ((p.y > z) != (q.y > z)) xs_.push_back(p.x + (z - p.y) * (q.x - p.x) / (q.y - p.y));
		}
		std::sort(xs_.begin(), xs_.end());
		int cells = 0;
		for (size_t k = 0; k + 1 < xs_.size(); k += 2) {
			const int i0 = std::max(0, int(std::ceil((xs_[k] - f.x0) / f.cell - HALF))), i1 = std::min(f.nx - 1, int(std::floor((xs_[k + 1] - f.x0) / f.cell - HALF)));
			for (int i = i0; i <= i1; i++) f.blocked[size_t(row_) * f.nx + i] = value_;
			cells += std::max(0, i1 - i0 + 1);
		}
		work -= 1 + int(ring.size()) / EDGES_PER_WORK + cells / CELLS_PER_WORK;
		row_++;
	}
	return row_ > last_;
}

namespace {
// One row of the squared distance transform: the lower envelope of the parabolas at the cells.
void transform_row(const std::vector<double> &in, int n, std::vector<double> &out, std::vector<int> &v, std::vector<double> &edge) {
	const double inf = std::numeric_limits<double>::infinity();
	int k = -1;
	for (int q = 0; q < n; q++) {
		if (in[size_t(q)] == inf) continue;
		for (;;) {
			if (k < 0) {
				k = 0;
				v[0] = q;
				edge[0] = -inf;
				edge[1] = inf;
				break;
			}
			const int p = v[size_t(k)];
			const double s = ((in[size_t(q)] + double(q) * q) - (in[size_t(p)] + double(p) * p)) / (2.0 * q - 2.0 * p);
			if (s <= edge[size_t(k)]) {
				k--;
				continue;
			}
			k++;
			v[size_t(k)] = q;
			edge[size_t(k)] = s;
			edge[size_t(k) + 1] = inf;
			break;
		}
	}
	for (int q = 0, j = 0; q < n; q++) {
		if (k < 0) {
			out[size_t(q)] = inf;
			continue;
		}
		while (edge[size_t(j) + 1] < q) j++;
		const int p = v[size_t(j)];
		out[size_t(q)] = double(q - p) * (q - p) + in[size_t(p)];
	}
}
} // namespace

void Holonomic::begin(const Field &f, const AxlePose &goal, double body_half, double size) {
	begun = true;
	cell = size;
	half = body_half;
	nx = int(std::ceil(f.nx * f.cell / cell));
	nz = int(std::ceil(f.nz * f.cell / cell));
	dist.clear();
	dist.reserve(size_t(nx) * nz);
	const int gi = int(std::floor((goal.x - f.x0) / cell)), gj = int(std::floor((goal.z - f.z0) / cell));
	goal_cell = gi < 0 || gj < 0 || gi >= nx || gj >= nz ? -1 : gj * nx + gi;
}

// Its cells laid a row at a time, the goal's set, then the cells settled one by one.
void Holonomic::advance(const Field &f, int &work) {
	const size_t cells = size_t(nx) * nz;
	while (dist.size() < cells && work > 0) {
		dist.insert(dist.end(), size_t(nx), std::numeric_limits<double>::infinity());
		work -= (1 + nx / CELLS_PER_WORK) * WRITE8_WORK_SHARE;
	}
	if (dist.size() < cells) return;
	if (!seeded) {
		seeded = true;
		if (goal_cell >= 0) {
			dist[size_t(goal_cell)] = 0;
			open.push({ 0, goal_cell });
		}
	}
	while (!open.empty() && work > 0) {
		const auto [d, c] = open.top();
		open.pop();
		work -= HOLONOMIC_CELL_WORK;
		if (d > dist[size_t(c)]) continue;
		const int i = c % nx, j = c / nx;
		for (int dj = -1; dj <= 1; dj++)
			for (int di = -1; di <= 1; di++) {
				const int a = i + di, b = j + dj;
				if ((!di && !dj) || a < 0 || b < 0 || a >= nx || b >= nz) continue;
				if (f.clear_at(f.x0 + (a + HALF) * cell, f.z0 + (b + HALF) * cell) < half) continue;
				const double nd = d + (di && dj ? ROOT_TWO : 1) * cell;
				if (nd < dist[size_t(b) * nx + a]) {
					dist[size_t(b) * nx + a] = nd;
					open.push({ nd, b * nx + a });
				}
			}
	}
}

// Only the room: the squared distances and the clearance are laid by advance, a row at a time.
ClearanceFill::ClearanceFill(Field &f) : f_(f) {
	const size_t n = size_t(std::max(f.nx, f.nz)), cells = size_t(f.nx) * f.nz;
	sq_.reserve(cells);
	in_.resize(n);
	out_.resize(n);
	edge_.resize(n + 1);
	v_.resize(n);
	f.clear.clear();
	f.clear.reserve(cells);
}

// The squared distances laid out a row at a time, then the columns, then the rows, each row's root its clearance.
bool ClearanceFill::advance(int &work) {
	const int nx = f_.nx, nz = f_.nz;
	const double inf = std::numeric_limits<double>::infinity();
	while (work > 0) {
		if (stage_ == 0) {
			if (line_ == nz) { stage_ = 1; line_ = 0; continue; }
			const uint8_t *row = f_.blocked.data() + size_t(line_) * nx;
			for (int i = 0; i < nx; i++) sq_.push_back(row[i] ? 0 : inf);
			work -= (1 + nx / CELLS_PER_WORK) * WRITE8_WORK_SHARE;
		} else if (stage_ == 1) {
			if (line_ == nx) { stage_ = 2; line_ = 0; continue; }
			for (int j = 0; j < nz; j++) in_[size_t(j)] = sq_[size_t(j) * nx + line_];
			transform_row(in_, nz, out_, v_, edge_);
			for (int j = 0; j < nz; j++) sq_[size_t(j) * nx + line_] = out_[size_t(j)];
			work -= (1 + nz / CELLS_PER_WORK) * LINE_WORK_SHARE;
		} else {
			if (line_ == nz) return true;
			const double *row = sq_.data() + size_t(line_) * nx;
			for (int i = 0; i < nx; i++) in_[size_t(i)] = row[i];
			transform_row(in_, nx, out_, v_, edge_);
			for (int i = 0; i < nx; i++) f_.clear.push_back(float(std::sqrt(out_[size_t(i)]) * f_.cell));
			work -= (1 + nx / CELLS_PER_WORK) * LINE_WORK_SHARE;
		}
		line_++;
	}
	return false;
}

} // namespace traffic
} // namespace city
