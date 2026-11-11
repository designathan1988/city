// A lot's covers clipped out of one triangulated ground, in one order (lot_covers.h).
#include "render/buildings/lot_covers.h"

#include <algorithm>
#include <cmath>
#include <vector>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float GROUND_CELL_M = 3.f; // the side of the lot ground's cells, their nodes on the lot frame's multiples of it
constexpr float FLAT_M = 1e-4f; // every node this close to one height: the ground is level, each cover its outline at that height
constexpr float HALF = 0.5f;
constexpr int OUTLINE = 4;

struct P2 {
	float x, y;
};
// A point of the lot's frame and its height over the floor.
struct Q {
	float x, y, h;
};

// The element's rectangle turned about its middle, anticlockwise in the lot's frame (x along the front, y away from it).
void outline_of(const LotElement &e, P2 (&o)[OUTLINE]) {
	const float c = std::cos(e.angle), s = std::sin(e.angle);
	const float us[OUTLINE] = { -e.w * HALF, e.w * HALF, e.w * HALF, -e.w * HALF }, vs[OUTLINE] = { -e.d * HALF, -e.d * HALF, e.d * HALF, e.d * HALF };
	for (int i = 0; i < OUTLINE; i++) o[i] = { e.x + us[i] * c - vs[i] * s, e.y + us[i] * s + vs[i] * c };
}

// What of the polygon `in` lies on the left of a -> b (Sutherland-Hodgman), heights carried along its edges: on a plane they stay on it.
int clip(const Q *in, int n, const P2 &a, const P2 &b, Q *out) {
	auto side = [&](const Q &p) { return (b.x - a.x) * (p.y - a.y) - (b.y - a.y) * (p.x - a.x); };
	int m = 0;
	for (int i = 0; i < n; i++) {
		const Q &p = in[i], &q = in[(i + 1) % n];
		const float sp = side(p), sq = side(q);
		if (sp >= 0) out[m++] = p;
		if ((sp >= 0) != (sq >= 0)) {
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

// A clipped polygon into the world, `lift` over the ground it was cut from.
void send(const Placed &pl, const Q *q, int n, float lift, CoverPiece &piece, const std::function<void(const CoverPiece &)> &out) {
	if (n < 3) return;
	piece.count = n;
	for (int k = 0; k < n; k++) piece.p[k] = pl.lot.at(q[k].x, q[k].y, q[k].h + lift);
	out(piece);
}

// The cover's outline clipped out of the ground's triangles, cell by cell under its box.
void lay(const Placed &pl, const CoverGround &g, const P2 (&o)[OUTLINE], float lift, CoverPiece &piece, const std::function<void(const CoverPiece &)> &out) {
	Q a[COVER_POLY_MOST], b[COVER_POLY_MOST];
	if (g.flat) {
		for (int k = 0; k < OUTLINE; k++) a[k] = { o[k].x, o[k].y, g.h.front() };
		send(pl, a, OUTLINE, lift, piece, out);
		return;
	}
	float x0 = o[0].x, x1 = o[0].x, y0 = o[0].y, y1 = o[0].y;
	for (const P2 &p : o) {
		x0 = std::min(x0, p.x);
		x1 = std::max(x1, p.x);
		y0 = std::min(y0, p.y);
		y1 = std::max(y1, p.y);
	}
	const int ia = std::max(g.i0, int(std::floor(x0 / GROUND_CELL_M))), ib = std::min(g.i0 + g.ni, int(std::ceil(x1 / GROUND_CELL_M)));
	const int ja = std::max(g.j0, int(std::floor(y0 / GROUND_CELL_M))), jb = std::min(g.j0 + g.nj, int(std::ceil(y1 / GROUND_CELL_M)));
	for (int j = ja; j < jb; j++)
		for (int i = ia; i < ib; i++) {
			const Q c00{ float(i) * GROUND_CELL_M, float(j) * GROUND_CELL_M, g.at(i, j) }, c10{ float(i + 1) * GROUND_CELL_M, float(j) * GROUND_CELL_M, g.at(i + 1, j) };
			const Q c11{ float(i + 1) * GROUND_CELL_M, float(j + 1) * GROUND_CELL_M, g.at(i + 1, j + 1) }, c01{ float(i) * GROUND_CELL_M, float(j + 1) * GROUND_CELL_M, g.at(i, j + 1) };
			const Q tris[2][3] = { { c00, c10, c11 }, { c00, c11, c01 } }; // every cell split along its 00-11 diagonal
			for (const auto &t : tris) {
				int n = 3;
				std::copy(t, t + 3, a);
				for (int k = 0; k < OUTLINE && n > 0; k++) {
					n = clip(a, n, o[k], o[(k + 1) % OUTLINE], b);
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
		P2 o[OUTLINE];
		outline_of(e, o);
		for (const P2 &p : o) {
			x0 = covers.empty() ? p.x : std::min(x0, p.x);
			x1 = covers.empty() ? p.x : std::max(x1, p.x);
			y0 = covers.empty() ? p.y : std::min(y0, p.y);
			y1 = covers.empty() ? p.y : std::max(y1, p.y);
		}
		covers.push_back(int(i));
	}
	if (covers.empty()) return;
	// the order: by thickness, then as the body lays them; each a step over all before it
	std::stable_sort(covers.begin(), covers.end(), [&b](int p, int q) { return b.elements[size_t(p)].h < b.elements[size_t(q)].h; });
	const CoverGround g = ground_under(pl, x0, y0, x1, y1);
	for (size_t r = 0; r < covers.size(); r++) {
		const LotElement &e = b.elements[size_t(covers[r])];
		P2 o[OUTLINE];
		outline_of(e, o);
		CoverPiece piece;
		piece.element = covers[r];
		piece.rank = int(r);
		lay(pl, g, o, pl.look->lot_plate + e.h * HALF + float(r) * pl.look->cover_step, piece, out);
	}
}

} // namespace city::b40
