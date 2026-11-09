// A grown building made to its lot's own shape (Roadcraft world/growth/zoning.ts fitToLot, rectInside, meets): the plan was laid on
// the lot's front rectangle; on a lot of another shape (an L, a curved side, a lot narrowing or widening at the back) the ground of
// the whole lot is laid as lawn, the plan's covers are cut to the lot, the boundary is taken off the rectangle and run along the
// lot's own sides but the front, and whatever of the plan falls outside the lot is left out. A rectangular lot is left as planned.
// The lot's covers are the body's flat elements (EL_SURFACE, rectangles): a cut is laid as the bands of the lot between its corners'
// depths, each band the run of the lot across it, narrowed to stay inside where a side runs at a slant.
#include "sim/buildings/b40/lot_site.h"

#include "core/plane_geometry.h"
#include "sim/buildings/b40/gen.h"
#include "sim/buildings/b40/lot_layout.h"

#include <algorithm>
#include <cmath>
#include <utility>
#include <vector>

namespace city::b40 {

namespace {
constexpr float MIDDLE = 0.5f; // a piece stands in the middle of its share of a side

bool inside(const std::vector<V2> &ring, float x, float y) {
	return inside_ring(ring, x, y, [](const V2 &p) { return std::make_pair(p.x, p.y); });
}

// Whether segment a-b meets segment c-d, touching counted (zoning.ts meets).
bool meets(V2 a, V2 b, V2 c, V2 d) {
	const float o1 = (b - a).cross(c - a), o2 = (b - a).cross(d - a), o3 = (d - c).cross(a - c), o4 = (d - c).cross(b - c);
	if (o1 * o2 > 0 || o3 * o4 > 0) return false;
	if (o1 == 0 && o2 == 0) // collinear: their spans along the line overlap
		return std::max(a.x, b.x) >= std::min(c.x, d.x) && std::min(a.x, b.x) <= std::max(c.x, d.x) && std::max(a.y, b.y) >= std::min(c.y, d.y) &&
				std::min(a.y, b.y) <= std::max(c.y, d.y);
	return true;
}

// Whether a rectangle lies strictly inside the ring: a corner inside and no side of it meeting a side of the ring (zoning.ts rectInside).
bool rect_inside(const Rect &r, const std::vector<V2> &ring) {
	if (!inside(ring, r.x0, r.y0)) return false;
	const V2 c[4] = { { r.x0, r.y0 }, { r.x1, r.y0 }, { r.x1, r.y1 }, { r.x0, r.y1 } };
	for (size_t i = 0; i < ring.size(); i++) {
		const V2 a = ring[i], b = ring[(i + 1) % ring.size()];
		if (std::max(a.x, b.x) < r.x0 || std::min(a.x, b.x) > r.x1 || std::max(a.y, b.y) < r.y0 || std::min(a.y, b.y) > r.y1) continue;
		for (int k = 0; k < 4; k++)
			if (meets(a, b, c[k], c[(k + 1) % 4])) return false;
	}
	return true;
}

// The ring within `clip` as rectangles: bands between its corners' depths (and `step` apart where a side runs at a slant), each the
// lot's runs across the band, each run narrowed to the side's inner end over the band.
void bands(const std::vector<V2> &ring, const Rect &clip, float step, float least, std::vector<Rect> &out) {
	std::vector<float> ys{ clip.y0, clip.y1 };
	for (const V2 &p : ring)
		if (p.y > clip.y0 && p.y < clip.y1) ys.push_back(p.y);
	std::sort(ys.begin(), ys.end());
	ys.erase(std::unique(ys.begin(), ys.end(), [](float a, float b) { return std::fabs(a - b) < EPS; }), ys.end());
	struct Cross {
		float x0, x1; // the side's x at the band's two ends
	};
	for (size_t k = 0; k + 1 < ys.size(); k++) {
		const float ya = ys[k], yb = ys[k + 1];
		bool slant = false;
		for (size_t i = 0; i < ring.size(); i++) {
			const V2 a = ring[i], b = ring[(i + 1) % ring.size()];
			if (std::min(a.y, b.y) < yb && std::max(a.y, b.y) > ya && std::fabs(a.x - b.x) > EPS) slant = true;
		}
		const int parts = slant ? std::max(1, int(std::ceil((yb - ya) / step))) : 1;
		for (int q = 0; q < parts; q++) {
			const float y0 = ya + (yb - ya) * float(q) / float(parts), y1 = ya + (yb - ya) * float(q + 1) / float(parts), ym = (y0 + y1) / 2;
			std::vector<Cross> xs;
			for (size_t i = 0; i < ring.size(); i++) {
				const V2 a = ring[i], b = ring[(i + 1) % ring.size()];
				if ((a.y > ym) == (b.y > ym)) continue;
				auto at = [&](float y) { return a.x + (b.x - a.x) * std::clamp((y - a.y) / (b.y - a.y), 0.f, 1.f); };
				xs.push_back({ at(y0), at(y1) });
			}
			std::sort(xs.begin(), xs.end(), [](const Cross &p, const Cross &r) { return p.x0 + p.x1 < r.x0 + r.x1; });
			for (size_t i = 0; i + 1 < xs.size(); i += 2) {
				const Rect r{ std::max({ xs[i].x0, xs[i].x1, clip.x0 }), y0, std::min({ xs[i + 1].x0, xs[i + 1].x1, clip.x1 }), y1 };
				if (r.w() >= least && r.d() >= least) out.push_back(r);
			}
		}
	}
}

bool is_boundary(Element k) { return k == EL_WALL || k == EL_FENCE || k == EL_HEDGE || k == EL_RAILING; }

LotElement cover_at(const LotElement &like, const Rect &r) {
	LotElement e = like;
	e.x = r.cx(); e.y = r.cy(); e.w = r.w(); e.d = r.d();
	return e;
}

// The boundary piece the lot's own sides are run with: the plan's own kind, height and paint (its sides' first, else any), else a fence.
LotElement edge_like(const LotFitShape &K, const Body &b) {
	const LotElement *like = nullptr;
	for (const LotElement &e : b.elements)
		if (is_boundary(e.kind) && (!like || (std::fabs(like->y) < K.front_band && std::fabs(e.y) >= K.front_band))) like = &e;
	LotElement edge;
	edge.kind = like ? like->kind : EL_FENCE;
	edge.h = like ? like->h : K.edge_h;
	edge.base = like ? like->base : 0;
	edge.paint = like ? like->paint : -1;
	return edge;
}

// The boundary along every side of the ring but the front, in pieces of `piece` at most, short of each corner by half its thickness
// and set in by half of it, so it stands on the lot's side of the line (a square end met a corner not quite square over the line).
void run_boundary(const Data &data, const Body &b, const std::vector<V2> &ring, float turn, std::vector<LotElement> &kept) {
	const LotFitShape &K = data.shapes.fit;
	const LotBoundaryShape &BK = data.shapes.boundary;
	const LotElement edge = edge_like(K, b);
	const float thick = edge.kind == EL_FENCE || edge.kind == EL_RAILING ? BK.fence_thick : edge.kind == EL_HEDGE ? BK.hedge_thick : BK.boundary_wall_t;
	for (size_t i = 1; i < ring.size(); i++) {
		const V2 a = ring[i], c = ring[(i + 1) % ring.size()];
		const float full = std::hypot(c.x - a.x, c.y - a.y);
		if (full < K.shortest + thick) continue;
		const V2 u = (c - a) / full, in = V2(-u.y, u.x) * (turn * thick / 2), s = a + u * (thick / 2);
		const float len = full - thick;
		const int n = int(std::ceil(len / K.piece));
		for (int k = 0; k < n && int(kept.size()) < data.lots.max_elements; k++) {
			const V2 m = s + u * (len * (float(k) + MIDDLE) / float(n)) + in;
			LotElement e = edge;
			e.x = m.x; e.y = m.y; e.w = len / float(n); e.d = thick; e.z = 0;
			e.angle = std::atan2(u.y, u.x);
			e.facing = FACE_STREET;
			kept.push_back(e);
		}
	}
}

// The building's own blocks kept where they stand on the lot (a corner or the middle on it).
void keep_blocks(Body &b, const std::vector<V2> &ring) {
	std::vector<Volume> blocks;
	int entrance = 0;
	for (size_t i = 0; i < b.volumes.size(); i++) {
		const Volume &v = b.volumes[i];
		const bool on = inside(ring, v.x, v.y) || inside(ring, v.x + v.w, v.y) || inside(ring, v.x + v.w, v.y + v.d) || inside(ring, v.x, v.y + v.d) ||
				inside(ring, v.x + v.w / 2, v.y + v.d / 2);
		if (!on) continue;
		if (int(i) == b.entrance_volume) entrance = int(blocks.size());
		blocks.push_back(v);
	}
	if (blocks.empty()) return;
	b.volumes = std::move(blocks);
	b.entrance_volume = entrance;
}
} // namespace

void fit_to_lot(const Data &data, Body &b, const LotSite &site) {
	const std::vector<V2> &ring = site.ring;
	if (ring.size() < 3) return;
	const LotFitShape &K = data.shapes.fit;
	Rect box{ FAR_AWAY, FAR_AWAY, -FAR_AWAY, -FAR_AWAY };
	float area = 0;
	for (size_t i = 0; i < ring.size(); i++) {
		const V2 p = ring[i], q = ring[(i + 1) % ring.size()];
		box = { std::min(box.x0, p.x), std::min(box.y0, p.y), std::max(box.x1, p.x), std::max(box.y1, p.y) };
		area += p.cross(q);
	}
	if (ring.size() == 4 && std::fabs(area) / 2 > box.w() * box.d() * K.rect_share) return; // most lots are their rectangle
	// What stays: the lawn under the whole lot, laid first; the front's boundary and gates; the parts on the lot; the covers cut to it.
	std::vector<LotElement> kept;
	const LotElement *lawn_like = nullptr;
	for (const LotElement &e : b.elements)
		if (e.kind == EL_SURFACE && e.angle == 0 && (!lawn_like || e.h < lawn_like->h)) lawn_like = &e;
	std::vector<Rect> pieces;
	if (lawn_like) {
		LotElement lawn = *lawn_like;
		lawn.paint = b.add_paint(data.color("lawn"));
		lawn.h = data.shapes.parts.cover_h * K.lawn_under;
		bands(ring, box, K.slice, K.least, pieces);
		for (const Rect &r : pieces) kept.push_back(cover_at(lawn, r));
	}
	for (const LotElement &e : b.elements) {
		if (is_boundary(e.kind) || e.kind == EL_GATE || e.kind == EL_CAR_GATE) {
			if (std::fabs(e.y) < K.front_band) kept.push_back(e);
		} else if (e.kind == EL_SURFACE && e.angle == 0 && !rect_inside(Rect::around(e.x, e.y, e.w, e.d), ring)) {
			pieces.clear();
			bands(ring, Rect::around(e.x, e.y, e.w, e.d), K.slice, K.least, pieces);
			for (const Rect &p : pieces) kept.push_back(cover_at(e, p));
		} else if (e.kind == EL_SURFACE || inside(ring, e.x, e.y)) {
			kept.push_back(e);
		}
	}
	run_boundary(data, b, ring, area > 0 ? 1.f : -1.f, kept);
	if (int(kept.size()) > data.lots.max_elements) kept.resize(size_t(data.lots.max_elements));
	b.elements = std::move(kept);
	keep_blocks(b, ring);
}

} // namespace city::b40
