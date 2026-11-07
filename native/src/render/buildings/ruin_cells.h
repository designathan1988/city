// The Voronoi cells of a building's fracture, and the cut of a face along them (ruin_fracture.cpp alone reads this).
#pragma once

#include "core/random.h"

#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/vector3.hpp>

#include <algorithm>
#include <cmath>
#include <vector>

namespace city::ruin {

constexpr float VORONOI_EDGE_M = 2.8f; // the edge of a Voronoi cell: the size of a piece
constexpr float SEED_INSET = 0.15f, SEED_SPAN = 0.7f; // where a seed may lie in its grid cell
constexpr float INSIDE_CORNER = 0.02f; // of the way to a triangle's middle, where a corner's cell is read
constexpr int GRID_MARGIN = 3;
constexpr float FAR_SQUARED = 1e30f;
constexpr uint32_t PRIME_X = 73856093u, PRIME_Y = 19349663u, PRIME_Z = 83492791u;

using godot::Color;
using godot::Vector2;
using godot::Vector3;

struct Corner {
	Vector3 p, n;
	Color c;
	Vector2 t, t2;
};
struct Tri {
	Corner v[3];
	bool sheet = false;
};

// The Voronoi cells over the triangles: a seed jittered in each cell of a grid, worked out from the hash when it is looked at (no
// table of them: a large building's grid is never laid whole); the nearest of the 27 round a point.
struct Cells {
	Vector3 lo;
	int nx = 0, ny = 0, nz = 0; // with a cell of margin on every side
	uint32_t seed = 0;

	Cells(const Vector3 &low, const Vector3 &high, uint32_t s) : lo(low), seed(s) {
		const Vector3 span = (high - low) / VORONOI_EDGE_M;
		nx = int(span.x) + GRID_MARGIN;
		ny = int(span.y) + GRID_MARGIN;
		nz = int(span.z) + GRID_MARGIN;
	}
	Vector3 seed_at(int at) const {
		const int k = at % nz, j = at / nz % ny, i = at / (nz * ny);
		const uint32_t h = hash_combine(seed, uint32_t(i) * PRIME_X ^ uint32_t(j) * PRIME_Y ^ uint32_t(k) * PRIME_Z);
		const Vector3 in(SEED_INSET + hash_float(h) * SEED_SPAN, SEED_INSET + hash_float(h + 1) * SEED_SPAN, SEED_INSET + hash_float(h + 2) * SEED_SPAN);
		return lo + (Vector3(float(i - 1), float(j - 1), float(k - 1)) + in) * VORONOI_EDGE_M;
	}
	int index(int i, int j, int k) const { return (i * ny + j) * nz + k; }
	int of(const Vector3 &p) const {
		const Vector3 g = (p - lo) / VORONOI_EDGE_M;
		const int i0 = std::clamp(int(std::floor(g.x)) + 1, 1, nx - 2), j0 = std::clamp(int(std::floor(g.y)) + 1, 1, ny - 2), k0 = std::clamp(int(std::floor(g.z)) + 1, 1, nz - 2);
		int best = index(i0, j0, k0);
		float nearest = FAR_SQUARED;
		for (int i = i0 - 1; i <= i0 + 1; i++)
			for (int j = j0 - 1; j <= j0 + 1; j++)
				for (int k = k0 - 1; k <= k0 + 1; k++) {
					const int at = index(i, j, k);
					const float d = seed_at(at).distance_squared_to(p);
					if (d < nearest) {
						nearest = d;
						best = at;
					}
				}
		return best;
	}
};

inline Corner along(const Corner &a, const Corner &b, float t) {
	return Corner{ a.p.lerp(b.p, t), a.n, a.c.lerp(b.c, t), a.t.lerp(b.t, t), a.t2 };
}

inline void fan(const Corner *ring, int count, bool sheet, const Cells &cells, std::vector<Tri> &out, int depth);

// Cut along the plane between the two cells it lies in, so a piece ends on a straight break and not in the teeth of whole triangles, until
// every part lies in one cell.
inline void cut(const Tri &t, const Cells &cells, std::vector<Tri> &out, int depth) {
	const Vector3 middle = (t.v[0].p + t.v[1].p + t.v[2].p) / 3.f;
	int in[3];
	for (int q = 0; q < 3; q++) in[q] = cells.of(t.v[q].p.lerp(middle, INSIDE_CORNER)); // a corner on a cut belongs to the side it was cut to
	if ((in[0] == in[1] && in[1] == in[2]) || depth == 0) {
		out.push_back(t);
		return;
	}
	const Vector3 a = cells.seed_at(in[0]), b = cells.seed_at(in[0] != in[1] ? in[1] : in[2]);
	const Vector3 across = b - a, on = (a + b) * 0.5f;
	Corner near[4], far[4];
	int nears = 0, fars = 0;
	for (int q = 0; q < 3; q++) {
		const Corner &p = t.v[q], &next = t.v[(q + 1) % 3];
		const float dp = across.dot(p.p - on), dn = across.dot(next.p - on);
		if (dp <= 0) near[nears++] = p;
		if (dp >= 0) far[fars++] = p;
		if ((dp < 0 && dn > 0) || (dp > 0 && dn < 0)) {
			const Corner m = along(p, next, dp / (dp - dn));
			near[nears++] = m;
			far[fars++] = m;
		}
	}
	if (nears < 3 || fars < 3) { // the plane misses it: only a corner's cell said otherwise
		out.push_back(t);
		return;
	}
	fan(near, nears, t.sheet, cells, out, depth - 1);
	fan(far, fars, t.sheet, cells, out, depth - 1);
}

inline void fan(const Corner *ring, int count, bool sheet, const Cells &cells, std::vector<Tri> &out, int depth) {
	for (int i = 1; i + 1 < count; i++) cut(Tri{ { ring[0], ring[i], ring[i + 1] }, sheet }, cells, out, depth);
}

} // namespace city::ruin
