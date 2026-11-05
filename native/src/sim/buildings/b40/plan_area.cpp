// Areas of building plans (plan_area.h).
#include "sim/buildings/b40/plan_area.h"

#include <algorithm>
#include <cmath>

namespace city::b40 {

namespace {
constexpr float AREA_EPS = 1e-6f;

float cross(Point2 a, Point2 b, Point2 c) { return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x); }

bool in_triangle(Point2 p, Point2 a, Point2 b, Point2 c) {
	return cross(a, b, p) >= 0 && cross(b, c, p) >= 0 && cross(c, a, p) >= 0;
}

// A simple ring as triangles: an ear is a convex corner with no other corner inside it, and cutting one leaves a simple ring.
std::vector<Ring> triangles_of(Ring ring) {
	if (ring_area(ring) < 0) std::reverse(ring.begin(), ring.end());
	std::vector<Ring> out;
	while (ring.size() > 3) {
		size_t n = ring.size(), ear = n;
		for (size_t i = 0; i < n && ear == n; i++) {
			Point2 a = ring[(i + n - 1) % n], b = ring[i], c = ring[(i + 1) % n];
			if (cross(a, b, c) <= 0) continue;
			bool clear = true;
			for (size_t k = 0; k < n && clear; k++) {
				const Point2 &p = ring[k];
				bool corner = k == i || k == (i + 1) % n || k == (i + n - 1) % n;
				clear = corner || !in_triangle(p, a, b, c);
			}
			if (clear) ear = i;
		}
		if (ear == n) break; // not simple (a damaged plan): what is left has no ear to cut
		out.push_back({ ring[(ear + n - 1) % n], ring[ear], ring[(ear + 1) % n] });
		ring.erase(ring.begin() + std::ptrdiff_t(ear));
	}
	if (ring.size() == 3 && ring_area(ring) > 0) out.push_back(ring);
	return out;
}

// What of a convex piece lies left of the edge p -> q (`keep_left`) or right of it.
Ring cut(const Ring &piece, Point2 p, Point2 q, bool keep_left) {
	Ring out;
	float sign = keep_left ? 1.f : -1.f;
	for (size_t i = 0; i < piece.size(); i++) {
		Point2 c = piece[i], d = piece[(i + 1) % piece.size()];
		float sc = sign * cross(p, q, c), sd = sign * cross(p, q, d);
		if (sc >= 0) out.push_back(c);
		if ((sc >= 0) != (sd >= 0)) {
			float t = sc / (sc - sd);
			out.push_back({ c.x + (d.x - c.x) * t, c.y + (d.y - c.y) * t });
		}
	}
	return out;
}

Ring clipped(Ring piece, const Ring &by) {
	for (size_t i = 0; i < by.size() && piece.size() >= 3; i++) piece = cut(piece, by[i], by[(i + 1) % by.size()], true);
	return piece;
}

// `piece` less the triangle `by`: past each edge of `by` a convex piece stays out, and what is inside goes on to the next edge.
void subtract(const Ring &piece, const Ring &by, std::vector<Ring> &out) {
	Ring inside = piece;
	for (size_t i = 0; i < by.size() && inside.size() >= 3; i++) {
		Point2 p = by[i], q = by[(i + 1) % by.size()];
		Ring outside = cut(inside, p, q, false);
		if (outside.size() >= 3 && std::fabs(ring_area(outside)) > AREA_EPS) out.push_back(std::move(outside));
		inside = cut(inside, p, q, true);
	}
}
} // namespace

Ring ring_of(const Volume &v) {
	if (v.outline.empty()) return { { v.x, v.y }, { v.x + v.w, v.y }, { v.x + v.w, v.y + v.d }, { v.x, v.y + v.d } };
	Ring out;
	out.reserve(v.outline.size());
	for (const Point2 &p : v.outline) out.push_back({ v.x + p.x * v.w, v.y + p.y * v.d });
	return out;
}

float ring_area(const Ring &ring) {
	float twice = 0;
	for (size_t i = 0; i < ring.size(); i++) {
		const Point2 &a = ring[i], &b = ring[(i + 1) % ring.size()];
		twice += a.x * b.y - a.y * b.x;
	}
	return twice / 2;
}

float overlap_area(const Ring &a, const Ring &b) {
	float area = 0;
	std::vector<Ring> tb = triangles_of(b);
	for (const Ring &ta : triangles_of(a))
		for (const Ring &t : tb) {
			Ring both = clipped(ta, t);
			if (both.size() >= 3) area += std::fabs(ring_area(both));
		}
	return area;
}

float support_share(const Ring &ring, const std::vector<Ring> &supports) {
	float whole = std::fabs(ring_area(ring));
	if (supports.empty() || whole < AREA_EPS) return 0;
	std::vector<Ring> left = triangles_of(ring);
	for (const Ring &support : supports)
		for (const Ring &t : triangles_of(support)) {
			std::vector<Ring> next;
			for (const Ring &piece : left) subtract(piece, t, next);
			left = std::move(next);
		}
	float uncovered = 0;
	for (const Ring &piece : left) uncovered += std::fabs(ring_area(piece));
	return std::clamp(1 - uncovered / whole, 0.f, 1.f);
}

bool contains_point(const Ring &ring, Point2 p) {
	bool inside = false;
	for (size_t i = 0, j = ring.size() - 1; i < ring.size(); j = i++) {
		const Point2 &a = ring[i], &b = ring[j];
		if ((a.y > p.y) != (b.y > p.y) && p.x < (b.x - a.x) * (p.y - a.y) / (b.y - a.y) + a.x) inside = !inside;
	}
	return inside;
}

} // namespace city::b40
