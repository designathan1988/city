// Clips a pitched roof against the taller volumes of a body cut open (roof_clip.h): convex pieces by Sutherland-Hodgman.
#include "render/buildings/roof_clip.h"

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city::b40 {

namespace {
constexpr float MIN_AREA2 = 1e-10f; // squared cross product of a piece worth drawing
constexpr float AXIS_PARALLEL = 0.5f;
constexpr size_t MIN_POLY_VERTS = 3; // fewer points enclose no area
using Poly = std::vector<Vector3>;

// The part of `in` on one side of the vertical plane (p - o).dot(n) = c: `low` keeps <= c, otherwise >= c (Sutherland-Hodgman).
Poly half(const Poly &in, const Vector3 &o, const Vector3 &n, float c, bool low) {
	Poly out;
	auto inside = [&](const Vector3 &p) { return low ? c - (p - o).dot(n) : (p - o).dot(n) - c; };
	for (size_t i = 0; i < in.size(); i++) {
		const Vector3 &cur = in[i], &nxt = in[(i + 1) % in.size()];
		float gc = inside(cur), gn = inside(nxt);
		if (gc >= 0) out.push_back(cur);
		if ((gc >= 0) != (gn >= 0)) out.push_back(cur + (nxt - cur) * (gc / (gc - gn)));
	}
	return out;
}

bool drawable(const Poly &p) {
	if (p.size() < MIN_POLY_VERTS) return false;
	Vector3 area;
	for (size_t i = 1; i + 1 < p.size(); i++) area += (p[i] - p[0]).cross(p[i + 1] - p[0]);
	return area.length_squared() > MIN_AREA2;
}
} // namespace

RoofHide::RoofHide(const Placed &pl, const Volume &v) : o_(pl.lot.o), ex_(pl.lot.e0), ey_(pl.lot.e1) {
	const Body &b = *pl.b;
	if (b.cut_level < 0) return;
	for (const Volume &o : b.volumes)
		if (&o != &v && o.outline.empty() && o.base < v.top() && o.top() > v.top()) rects_.push_back({ o.x, o.x + o.w, o.y, o.y + o.d });
}

std::vector<RoofHide::Poly> RoofHide::outside(const Poly &poly) const {
	std::vector<Poly> pieces{ poly };
	for (const Rect &r : rects_) {
		std::vector<Poly> next;
		for (const Poly &q : pieces) {
			Poly rest = q;
			Poly left = half(rest, o_, ex_, r.x0, true);
			rest = half(rest, o_, ex_, r.x0, false);
			Poly right = half(rest, o_, ex_, r.x1, false);
			rest = half(rest, o_, ex_, r.x1, true);
			Poly front = half(rest, o_, ey_, r.y0, true);
			rest = half(rest, o_, ey_, r.y0, false);
			Poly back = half(rest, o_, ey_, r.y1, false);
			for (Poly *p : { &left, &right, &front, &back })
				if (drawable(*p)) next.push_back(std::move(*p));
		}
		pieces = std::move(next);
	}
	return pieces;
}

void RoofHide::emit(Shell &s, const Poly &poly, const Vector3 &out, const Lin &lin, int8_t fin) const {
	for (const Poly &piece : outside(poly))
		for (size_t i = 1; i + 1 < piece.size(); i++) s.tri(piece[0], piece[i], piece[i + 1], out, lin, fin);
}

void RoofHide::tri(Shell &s, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &out, const Lin &lin, int8_t fin) const {
	emit(s, Poly{ a, b, c }, out, lin, fin);
}

void RoofHide::quad(Shell &s, const Vector3 &a, const Vector3 &b, const Vector3 &c, const Vector3 &d, const Vector3 &out, const Lin &lin,
		int8_t fin) const {
	emit(s, Poly{ a, b, c, d }, out, lin, fin);
}

std::vector<std::pair<float, float>> RoofHide::spans(const Vector3 &from, const Vector3 &axis, float u0, float u1) const {
	std::vector<std::pair<float, float>> keep{ { u0, u1 } };
	float fx = plan_x(from), fy = plan_y(from), dx = axis.dot(ex_), dy = axis.dot(ey_);
	for (const Rect &r : rects_) {
		float h0 = -INFINITY, h1 = INFINITY; // where the line is inside the rectangle
		bool misses = false;
		auto clip = [&](float f, float d, float lo, float hi) {
			if (std::abs(d) < AXIS_PARALLEL) {
				misses = misses || f <= lo || f >= hi;
				return;
			}
			float a = (lo - f) / d, b = (hi - f) / d;
			h0 = std::max(h0, std::min(a, b));
			h1 = std::min(h1, std::max(a, b));
		};
		clip(fx, dx, r.x0, r.x1);
		clip(fy, dy, r.y0, r.y1);
		if (misses || h0 >= h1) continue;
		std::vector<std::pair<float, float>> next;
		for (const auto &k : keep) {
			if (k.first < h0) next.push_back({ k.first, std::min(k.second, h0) });
			if (k.second > h1) next.push_back({ std::max(k.first, h1), k.second });
		}
		keep = std::move(next);
	}
	return keep;
}

} // namespace city::b40
