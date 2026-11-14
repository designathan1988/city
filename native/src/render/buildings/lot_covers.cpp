// A lot's covers clipped out of one triangulated ground, in one order (lot_covers.h).
#include "render/buildings/lot_covers.h"

#include "core/math.h"
#include "sim/buildings/b40/element_shape.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float GROUND_CELL_M = 3.f; // the side of the lot ground's cells, their nodes on the lot frame's multiples of it
constexpr float FLAT_M = 1e-4f; // every node this close to one height: the ground is level, each cover its outline at that height
constexpr float HALF = 0.5f;
constexpr int CLIP_MOST = OUTLINE_MOST + 3; // a triangle clipped by an outline gains a corner for each of its sides at the most
constexpr float HAIR_M = 1e-4f; // two corners nearer than this, or a corner this near the line between its neighbours, are one place

// A point of the lot's frame and its height over the floor.
struct Q {
	float x, y, h;
};

// What of the polygon `in` lies on the left of a -> b (Sutherland-Hodgman), heights carried along its edges: on a plane they stay on it.
int clip(const Q *in, int n, const Point2 &a, const Point2 &b, Q *out) {
	auto side = [&](const Q &p) { return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x); };
	int m = 0;
	for (int i = 0; i < n; i++) {
		const Q &p = in[i], &q = in[(i + 1) % n];
		const float sp = side(p), sq = side(q);
		if (sp >= 0) out[m++] = p;
		if ((sp > 0 && sq < 0) || (sp < 0 && sq > 0)) { // a corner standing on the line is itself where the edge meets it: nothing between
			const float t = sp / (sp - sq);
			out[m++] = { p.x + (q.x - p.x) * t, p.y + (q.y - p.y) * t, p.h + (q.h - p.h) * t };
		}
	}
	return m;
}

// The lot's ground under its covers: node heights over the floor on the grid, sampled once, each at the nearest point of the covers' box.
struct CoverGround {
	int i0 = 0, j0 = 0, ni = 0, nj = 0;
	std::vector<float> h;
	bool flat = true;
	float at(int i, int j) const { return h[size_t((j - j0) * (ni + 1) + (i - i0))]; }
};

CoverGround ground_under(const Placed &pl, float x0, float y0, float x1, float y1) {
	CoverGround g;
	g.i0 = int(std::floor(x0 / GROUND_CELL_M));
	g.j0 = int(std::floor(y0 / GROUND_CELL_M));
	g.ni = std::max(1, int(std::ceil(x1 / GROUND_CELL_M)) - g.i0);
	g.nj = std::max(1, int(std::ceil(y1 / GROUND_CELL_M)) - g.j0);
	g.h.reserve(size_t((g.ni + 1) * (g.nj + 1)));
	for (int j = 0; j <= g.nj; j++)
		for (int i = 0; i <= g.ni; i++) {
			const float x = std::clamp(float(g.i0 + i) * GROUND_CELL_M, x0, x1), y = std::clamp(float(g.j0 + j) * GROUND_CELL_M, y0, y1);
			g.h.push_back(pl.ground(pl.lot.at(x, y, 0)) - pl.lot.o.y);
		}
	for (float v : g.h) g.flat = g.flat && std::fabs(v - g.h.front()) < FLAT_M;
	return g;
}

// A clipped polygon as a piece is handed: without the corners that carry no shape (one at the place of its neighbour, one on the line
// between its neighbours: what the clip's rounding leaves where a line of the outline passes a hair from a corner of the ground), and
// starting at the corner whose triangle with the next two is the widest, since whoever draws or reads a piece takes its plane from
// its first three corners (Shell::face, the probe lot_covers). Returns its corners; under three, nothing is left of it.
int tidy(Q *q, int n) {
	const auto plan = [&](int i) { return V2(q[i].x, q[i].y); };
	for (int i = 0; n >= 3 && i < n;) {
		if (dist_point_segment(plan(i), plan((i + n - 1) % n), plan((i + 1) % n)) >= HAIR_M) {
			i++;
			continue;
		}
		std::copy(q + i + 1, q + n, q + i);
		n--;
		i = 0; // its neighbours are judged again
	}
	if (n < 3) return 0;
	int widest = 0;
	float most = -1;
	for (int i = 0; i < n; i++) {
		const Q &a = q[i], &b = q[(i + 1) % n], &c = q[(i + 2) % n];
		const float spread = std::fabs((b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x));
		if (spread <= most) continue;
		most = spread;
		widest = i;
	}
	std::rotate(q, q + widest, q + n);
	return n;
}

// A convex polygon of the lot's frame into the world, `lift` over the ground it was cut from: pieces of COVER_POLY_MOST corners at the
// most, fanned from its first corner.
void send(const Placed &pl, Q *q, int n, float lift, CoverPiece &piece, const std::function<void(const CoverPiece &)> &out) {
	n = tidy(q, n);
	const auto at = [&](const Q &c) { return pl.lot.at(c.x, c.y, c.h + lift); };
	for (int k = 1; k + 1 < n; k += COVER_POLY_MOST - 2) {
		const int last = std::min(k + COVER_POLY_MOST - 2, n - 1);
		piece.count = last - k + 2;
		piece.p[0] = at(q[0]);
		for (int i = k; i <= last; i++) piece.p[i - k + 1] = at(q[i]);
		out(piece);
	}
}

// The cover's outline clipped out of the ground's triangles, cell by cell under its box.
void lay(const Placed &pl, const CoverGround &g, const Outline &o, float lift, CoverPiece &piece, const std::function<void(const CoverPiece &)> &out) {
	Q a[CLIP_MOST], b[CLIP_MOST];
	if (pl.lean) { // the far level: the cover's own outline, each corner on the ground under it (its distance shows no relief inside a cover)
		for (int k = 0; k < o.n; k++) a[k] = { o.p[k].x, o.p[k].y, pl.ground(pl.lot.at(o.p[k].x, o.p[k].y, 0)) - pl.lot.o.y };
		send(pl, a, o.n, lift, piece, out);
		return;
	}
	if (g.flat) {
		for (int k = 0; k < o.n; k++) a[k] = { o.p[k].x, o.p[k].y, g.h.front() };
		send(pl, a, o.n, lift, piece, out);
		return;
	}
	const Footprint box = o.bounds();
	const int ia = std::max(g.i0, int(std::floor(box.x0 / GROUND_CELL_M))), ib = std::min(g.i0 + g.ni, int(std::ceil(box.x1 / GROUND_CELL_M)));
	const int ja = std::max(g.j0, int(std::floor(box.y0 / GROUND_CELL_M))), jb = std::min(g.j0 + g.nj, int(std::ceil(box.y1 / GROUND_CELL_M)));
	for (int j = ja; j < jb; j++)
		for (int i = ia; i < ib; i++) {
			const Q c00{ float(i) * GROUND_CELL_M, float(j) * GROUND_CELL_M, g.at(i, j) }, c10{ float(i + 1) * GROUND_CELL_M, float(j) * GROUND_CELL_M, g.at(i + 1, j) };
			const Q c11{ float(i + 1) * GROUND_CELL_M, float(j + 1) * GROUND_CELL_M, g.at(i + 1, j + 1) }, c01{ float(i) * GROUND_CELL_M, float(j + 1) * GROUND_CELL_M, g.at(i, j + 1) };
			const Q tris[2][3] = { { c00, c10, c11 }, { c00, c11, c01 } }; // every cell split along its 00-11 diagonal
			for (const auto &t : tris) {
				int n = 3;
				std::copy(t, t + 3, a);
				for (int k = 0; k < o.n && n > 0; k++) {
					n = clip(a, n, o.p[k], o.p[(k + 1) % o.n], b);
					std::copy(b, b + n, a);
				}
				send(pl, a, n, lift, piece, out);
			}
		}
}
} // namespace

void lot_covers(const Placed &pl, const std::function<void(const CoverPiece &)> &out) {
	const Body &b = *pl.b;
	std::vector<int> covers;
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	for (size_t i = 0; i < b.elements.size(); i++) {
		const LotElement &e = b.elements[i];
		if (e.kind != EL_SURFACE || cut_away(b, e)) continue;
		const Footprint box = outline_of(e).bounds();
		x0 = covers.empty() ? box.x0 : std::min(x0, box.x0);
		x1 = covers.empty() ? box.x1 : std::max(x1, box.x1);
		y0 = covers.empty() ? box.y0 : std::min(y0, box.y0);
		y1 = covers.empty() ? box.y1 : std::max(y1, box.y1);
		covers.push_back(int(i));
	}
	if (covers.empty()) return;
	// the order: by thickness, then as the body lays them; each a step over all before it
	std::stable_sort(covers.begin(), covers.end(), [&b](int p, int q) { return b.elements[size_t(p)].h < b.elements[size_t(q)].h; });
	const CoverGround g = pl.lean ? CoverGround() : ground_under(pl, x0, y0, x1, y1); // the far level reads the ground at each outline's corners alone (lay)
	for (size_t r = 0; r < covers.size(); r++) {
		const LotElement &e = b.elements[size_t(covers[r])];
		CoverPiece piece;
		piece.element = covers[r];
		piece.rank = int(r);
		lay(pl, g, outline_of(e), pl.look->lot_plate + e.h * HALF + float(r) * pl.look->cover_step, piece, out);
	}
}

} // namespace city::b40
