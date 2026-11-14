// A lot's covers clipped out of one triangulated ground, in one order (lot_covers.h).
#include "render/buildings/lot_covers.h"

#include "core/math.h"
#include "core/polygon_ops.h"
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
constexpr int TRIANGLE = 3;
// A triangle clipped by an outline gains a corner for each of its sides at the most, and as many again by a triangle of the lot's own
// outline (ring_pieces); an outline clipped by such a triangle gains no more than that.
constexpr int CLIP_MOST = OUTLINE_MOST + 2 * TRIANGLE;
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

// What of the polygon `a` lies on an outline (each of its sides in turn): its corners left in `a`, their number returned.
int clip_to(Q *a, int n, const Outline &o) {
	Q b[CLIP_MOST];
	for (int k = 0; k < o.n && n > 0; k++) {
		n = clip(a, n, o.p[k], o.p[(k + 1) % o.n], b);
		std::copy(b, b + n, a);
	}
	return n;
}

// The outline of a lot that is no rectangle (Body::lot_ring) as the triangles it is cut into, each anticlockwise (clip keeps the left
// of a side): what lies on such a lot is clipped by them, since nothing of a lot is drawn past its own line. None for a lot that is
// its plan's rectangle.
std::vector<Outline> ring_pieces(const Body &b) {
	std::vector<Outline> out;
	std::vector<double> x, y;
	for (const Point2 &p : b.lot_ring) {
		x.push_back(p.x);
		y.push_back(p.y);
	}
	const std::vector<int> tris = ring_triangles(x, y);
	for (size_t t = 0; t + TRIANGLE <= tris.size(); t += TRIANGLE) {
		Outline o;
		o.n = TRIANGLE;
		for (int k = 0; k < TRIANGLE; k++) o.p[k] = b.lot_ring[size_t(tris[t + size_t(k)])];
		if ((o.p[1].x - o.p[0].x) * (o.p[2].y - o.p[0].y) - (o.p[1].y - o.p[0].y) * (o.p[2].x - o.p[0].x) < 0) std::swap(o.p[1], o.p[2]);
		out.push_back(o);
	}
	return out;
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

// The cover's outline clipped out of the ground's triangles, cell by cell under its box; `within`, when given, a piece of the lot's
// own outline the cover is held to as well.
void lay(const Placed &pl, const CoverGround &g, const Outline &o, float lift, CoverPiece &piece, const std::function<void(const CoverPiece &)> &out,
		const Outline *within = nullptr) {
	Q a[CLIP_MOST];
	if (pl.lean || g.flat) {
		// the far level: the cover's own outline, each corner on the ground under it (its distance shows no relief inside a cover);
		// level ground: its outline at that height
		for (int k = 0; k < o.n; k++) a[k] = { o.p[k].x, o.p[k].y, pl.lean ? pl.ground(pl.lot.at(o.p[k].x, o.p[k].y, 0)) - pl.lot.o.y : g.h.front() };
		send(pl, a, within ? clip_to(a, o.n, *within) : o.n, lift, piece, out);
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
				std::copy(t, t + TRIANGLE, a);
				int n = clip_to(a, TRIANGLE, o);
				if (within) n = clip_to(a, n, *within);
				send(pl, a, n, lift, piece, out);
			}
		}
}

// The box that holds the boxes given so far and one more.
struct Reach {
	bool any = false;
	float x0 = 0, y0 = 0, x1 = 0, y1 = 0;
	void take(const Footprint &f) {
		x0 = any ? std::min(x0, f.x0) : f.x0;
		x1 = any ? std::max(x1, f.x1) : f.x1;
		y0 = any ? std::min(y0, f.y0) : f.y0;
		y1 = any ? std::max(y1, f.y1) : f.y1;
		any = true;
	}
};
} // namespace

void lot_covers(const Placed &pl, const std::function<void(const CoverPiece &)> &out) {
	const Body &b = *pl.b;
	std::vector<int> covers;
	Reach reach;
	for (size_t i = 0; i < b.elements.size(); i++) {
		const LotElement &e = b.elements[i];
		if (e.kind != EL_SURFACE || cut_away(b, e)) continue;
		reach.take(outline_of(e).bounds());
		covers.push_back(int(i));
	}
	const std::vector<Outline> lot = ring_pieces(b); // a lot that is no rectangle: its own outline, which holds everything laid on it
	for (const Outline &t : lot) reach.take(t.bounds());
	if (!reach.any) return;
	// the order: by thickness, then as the body lays them; each a step over all before it
	std::stable_sort(covers.begin(), covers.end(), [&b](int p, int q) { return b.elements[size_t(p)].h < b.elements[size_t(q)].h; });
	// the far level reads the ground at each outline's corners alone (lay)
	const CoverGround g = pl.lean ? CoverGround() : ground_under(pl, reach.x0, reach.y0, reach.x1, reach.y1);
	// The lot's own lawn, to its outline and a step under its first cover: the land between the plan's rectangle and the lot's lines
	// is the lot's garden. Cut out of the one ground the covers are cut out of, so no cover ever lies under it.
	CoverPiece lawn;
	lawn.element = LOT_LAWN;
	lawn.rank = LOT_LAWN;
	for (const Outline &t : lot) lay(pl, g, t, pl.look->lot_plate - pl.look->cover_step, lawn, out);
	for (size_t r = 0; r < covers.size(); r++) {
		const LotElement &e = b.elements[size_t(covers[r])];
		CoverPiece piece;
		piece.element = covers[r];
		piece.rank = int(r);
		const float lift = pl.look->lot_plate + e.h * HALF + float(r) * pl.look->cover_step;
		if (lot.empty()) lay(pl, g, outline_of(e), lift, piece, out);
		for (const Outline &t : lot) lay(pl, g, outline_of(e), lift, piece, out, &t);
	}
}

} // namespace city::b40
