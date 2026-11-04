// The grid of a yard and its clearance (way_search.h): what is solid, and how far every cell is from it.
#include "sim/traffic/way_search.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr double HALF = 0.5;
const float NO_ROOM = -std::numeric_limits<float>::infinity();
} // namespace

void Field::resize(double x, double z, double size, int cells_x, int cells_z, uint8_t value) {
	x0 = x;
	z0 = z;
	cell = size;
	nx = cells_x;
	nz = cells_z;
	blocked.assign(size_t(nx) * nz, value);
}

float Field::clear_at(double x, double z) const {
	const int i = int(std::floor((x - x0) / cell)), j = int(std::floor((z - z0) / cell));
	return i < 0 || j < 0 || i >= nx || j >= nz ? NO_ROOM : clear[size_t(j) * nx + i];
}

bool Field::solid_at(double x, double z) const {
	const int i = int(std::floor((x - x0) / cell)), j = int(std::floor((z - z0) / cell));
	return i < 0 || j < 0 || i >= nx || j >= nz || blocked[size_t(j) * nx + i] != 0;
}

void fill_ring(Field &f, const std::vector<V2> &ring, uint8_t value) {
	float lo = std::numeric_limits<float>::infinity(), hi = -lo;
	for (const V2 &p : ring) {
		lo = std::min(lo, p.y);
		hi = std::max(hi, p.y);
	}
	const int j0 = std::max(0, int(std::ceil((lo - f.z0) / f.cell - HALF))), j1 = std::min(f.nz - 1, int(std::floor((hi - f.z0) / f.cell - HALF)));
	std::vector<double> xs;
	for (int j = j0; j <= j1; j++) {
		const double z = f.z0 + (j + HALF) * f.cell;
		xs.clear();
		for (size_t a = 0, b = ring.size() - 1; a < ring.size(); b = a++) {
			const V2 &p = ring[a], &q = ring[b];
			if ((p.y > z) != (q.y > z)) xs.push_back(p.x + (z - p.y) * (q.x - p.x) / (q.y - p.y));
		}
		std::sort(xs.begin(), xs.end());
		for (size_t k = 0; k + 1 < xs.size(); k += 2) {
			const int i0 = std::max(0, int(std::ceil((xs[k] - f.x0) / f.cell - HALF))), i1 = std::min(f.nx - 1, int(std::floor((xs[k + 1] - f.x0) / f.cell - HALF)));
			for (int i = i0; i <= i1; i++) f.blocked[size_t(j) * f.nx + i] = value;
		}
	}
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

void fill_clearance(Field &f) {
	const size_t n = size_t(std::max(f.nx, f.nz)), cells = size_t(f.nx) * f.nz;
	const double inf = std::numeric_limits<double>::infinity();
	std::vector<double> sq(cells), in(n), out(n), edge(n + 1);
	std::vector<int> v(n);
	for (size_t i = 0; i < sq.size(); i++) sq[i] = f.blocked[i] ? 0 : inf;
	for (int i = 0; i < f.nx; i++) {
		for (int j = 0; j < f.nz; j++) in[size_t(j)] = sq[size_t(j) * f.nx + i];
		transform_row(in, f.nz, out, v, edge);
		for (int j = 0; j < f.nz; j++) sq[size_t(j) * f.nx + i] = out[size_t(j)];
	}
	for (int j = 0; j < f.nz; j++) {
		for (int i = 0; i < f.nx; i++) in[size_t(i)] = sq[size_t(j) * f.nx + i];
		transform_row(in, f.nx, out, v, edge);
		for (int i = 0; i < f.nx; i++) sq[size_t(j) * f.nx + i] = out[size_t(i)];
	}
	f.clear.resize(sq.size());
	for (size_t i = 0; i < sq.size(); i++) f.clear[i] = float(std::sqrt(sq[i]) * f.cell);
}

} // namespace traffic
} // namespace city
