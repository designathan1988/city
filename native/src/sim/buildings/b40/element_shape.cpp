// A lot element's outline on the ground, and what is asked of it (element_shape.h).
#include "sim/buildings/b40/element_shape.h"

#include "core/math.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float HALF = 0.5f;

// How far (x, y) stands on the inner side of the side a -> b of an anticlockwise outline, times the side's length.
float inside(const Point2 &a, const Point2 &b, float x, float y) { return (b.x - a.x) * (y - a.y) - (b.y - a.y) * (x - a.x); }
float length(const Point2 &a, const Point2 &b) { return std::hypot(b.x - a.x, b.y - a.y); }
// The unit step out of the outline across its side a -> b.
Point2 outward(const Point2 &a, const Point2 &b) {
	const float l = length(a, b);
	return l > 0 ? Point2{ (b.y - a.y) / l, (a.x - b.x) / l } : Point2{};
}
} // namespace

Outline outline_of(const LotElement &e) {
	Outline o;
	const float c = std::cos(e.angle), s = std::sin(e.angle), a = e.w * HALF, b = e.d * HALF;
	const auto corner = [&](float u, float v) { o.p[o.n++] = { e.x + u * c - v * s, e.y + u * s + v * c }; };
	if (!e.round) {
		corner(-a, -b);
		corner(a, -b);
		corner(a, b);
		corner(-a, b);
		return o;
	}
	for (int k = 0; k < ROUND_SIDES; k++) {
		const float t = TAU_F * float(k) / float(ROUND_SIDES);
		corner(a * std::cos(t), b * std::sin(t));
	}
	return o;
}

Outline outline_of(const Footprint &f) {
	Outline o;
	o.n = 4;
	o.p[0] = { f.x0, f.y0 };
	o.p[1] = { f.x1, f.y0 };
	o.p[2] = { f.x1, f.y1 };
	o.p[3] = { f.x0, f.y1 };
	return o;
}

Footprint part_ground(const LotElement &e, float pool_deck) {
	const Footprint f = outline_of(e).bounds();
	const float deck = e.kind == EL_POOL ? pool_deck : 0;
	return { f.x0 - deck, f.y0 - deck, f.x1 + deck, f.y1 + deck };
}

Footprint Outline::bounds() const {
	Footprint f{ p[0].x, p[0].y, p[0].x, p[0].y };
	for (int k = 1; k < n; k++) {
		f.x0 = std::min(f.x0, p[k].x);
		f.x1 = std::max(f.x1, p[k].x);
		f.y0 = std::min(f.y0, p[k].y);
		f.y1 = std::max(f.y1, p[k].y);
	}
	return f;
}

float Outline::area() const {
	float twice = 0;
	for (int k = 0; k < n; k++) twice += p[k].x * p[(k + 1) % n].y - p[(k + 1) % n].x * p[k].y;
	return twice * HALF;
}

bool Outline::holds(float x, float y) const {
	for (int k = 0; k < n; k++) {
		const Point2 &a = p[k], &b = p[(k + 1) % n];
		if (inside(a, b, x, y) < -EPS * length(a, b)) return false;
	}
	return n > 0;
}

// Two convex shapes share no ground when a line stands between them: one of the box's own lines, or one of the outline's sides.
bool Outline::meets(const Footprint &f) const {
	const Footprint mine = bounds();
	if (mine.x1 <= f.x0 || f.x1 <= mine.x0 || mine.y1 <= f.y0 || f.y1 <= mine.y0) return false;
	for (int k = 0; k < n; k++) {
		const Point2 &a = p[k], &b = p[(k + 1) % n];
		const float in = std::max({ inside(a, b, f.x0, f.y0), inside(a, b, f.x1, f.y0), inside(a, b, f.x1, f.y1), inside(a, b, f.x0, f.y1) });
		if (in <= 0) return false; // the whole box on the outer side of this side
	}
	return true;
}

Point2 Outline::nearest(float x, float y) const {
	if (holds(x, y)) return { x, y };
	Point2 best = p[0];
	float least = FAR_AWAY;
	for (int k = 0; k < n; k++) {
		const Point2 &a = p[k], &b = p[(k + 1) % n];
		float along = 0; // of the side, where its nearest point lies
		const float away = dist_point_segment(V2(x, y), V2(a.x, a.y), V2(b.x, b.y), &along);
		if (away >= least) continue;
		least = away;
		best = { a.x + (b.x - a.x) * along, a.y + (b.y - a.y) * along };
	}
	return best;
}

// The line's point at `t` is (t, line) across or (line, t) along. It stands on the inner side of a side where t lies past the place
// the line meets that side's own line, which is worked out from the side's first corner: for a side along the lot's axes it is that
// corner's own number, with no rounding, so a rectangle's stretch ends exactly on its lines.
bool Outline::span(bool across, float line, float &from, float &to) const {
	from = -FAR_AWAY;
	to = FAR_AWAY;
	for (int k = 0; k < n; k++) {
		const Point2 &a = p[k], &b = p[(k + 1) % n];
		const float ex = b.x - a.x, ey = b.y - a.y;
		const float grows = across ? -ey : ex; // how the side's inner measure changes with t
		if (grows == 0) {
			if (inside(a, b, across ? a.x : line, across ? line : a.y) < 0) return false; // the line runs beside this side, outside it
			continue;
		}
		const float meets = across ? a.x + ex * (line - a.y) / ey : a.y + ey * (line - a.x) / ex;
		if (grows > 0) from = std::max(from, meets);
		else to = std::min(to, meets);
	}
	return n > 0 && from < to;
}

Outline Outline::grown(float by) const {
	Outline o;
	o.n = n;
	for (int k = 0; k < n; k++) {
		const Point2 before = outward(p[(k + n - 1) % n], p[k]), after = outward(p[k], p[(k + 1) % n]);
		// the corner of the two moved sides: along the sum of their steps out, as far as keeps each `by` from its side
		const float share = by / (1 + before.x * after.x + before.y * after.y);
		o.p[k] = { p[k].x + (before.x + after.x) * share, p[k].y + (before.y + after.y) * share };
	}
	return o;
}

} // namespace city::b40
