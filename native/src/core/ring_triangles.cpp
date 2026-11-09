// A ring in triangles on its own points (ring_triangles.h): ear clipping, then Lawson's flips over a half-edge adjacency, each flip
// checking again only the four edges round it.
#include "core/ring_triangles.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace city {

namespace {
constexpr double SAME_POINT_M = 1e-6; // a micrometre: two points this near are one
constexpr double NO_AREA_M2 = 1e-12; // twice a triangle's area at or under which it has none (a millionth of a square millimetre)
// In-circle margin, relative to the fourth power of the triangle's size: points on one circle (a kerb return's stations) are left as
// they are, not flipped back and forth by the last bits of the arithmetic.
constexpr double COCIRCULAR = 1e-9;
constexpr int FLIPS_PER_PAIR = 4; // Lawson's flips end in O(n^2) (de Berg 9.3); a bound past it, for a ring the arithmetic cannot settle

struct Pt {
	double x, y;
};

double turn(const Pt &a, const Pt &b, const Pt &c) { return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x); }
bool same(const Pt &a, const Pt &b) { return std::fabs(a.x - b.x) <= SAME_POINT_M && std::fabs(a.y - b.y) <= SAME_POINT_M; }

// Whether d lies inside the circle through the counter-clockwise a, b, c, past the cocircular margin.
bool in_circle(const Pt &a, const Pt &b, const Pt &c, const Pt &d) {
	const double ax = a.x - d.x, ay = a.y - d.y, bx = b.x - d.x, by = b.y - d.y, cx = c.x - d.x, cy = c.y - d.y;
	const double la = ax * ax + ay * ay, lb = bx * bx + by * by, lc = cx * cx + cy * cy;
	const double det = la * (bx * cy - cx * by) - lb * (ax * cy - cx * ay) + lc * (ax * by - bx * ay);
	const double size = std::max({ la, lb, lc });
	return det > COCIRCULAR * size * size;
}

// The ring's points kept (consecutive repeats dropped), counter-clockwise; empty when it has no area.
std::vector<int> kept_points(const std::vector<Pt> &p) {
	std::vector<int> idx;
	for (int i = 0; i < int(p.size()); i++)
		if (idx.empty() || !same(p[size_t(i)], p[size_t(idx.back())])) idx.push_back(i);
	while (idx.size() > 1 && same(p[size_t(idx.front())], p[size_t(idx.back())])) idx.pop_back();
	if (idx.size() < 3) return {};
	double area = 0;
	for (size_t k = 0; k < idx.size(); k++) {
		const Pt &a = p[size_t(idx[k])], &b = p[size_t(idx[(k + 1) % idx.size()])];
		area += a.x * b.y - b.x * a.y;
	}
	if (std::fabs(area) <= NO_AREA_M2) return {};
	if (area < 0) std::reverse(idx.begin(), idx.end());
	return idx;
}

// Ear clipping over a linked ring of `idx`: three point indices a triangle.
std::vector<int> clip_ears(const std::vector<Pt> &p, const std::vector<int> &idx) {
	const int m = int(idx.size());
	const size_t count = idx.size();
	std::vector<int> prev(count), next(count), tris;
	tris.reserve(size_t(m - 2) * 3);
	for (int k = 0; k < m; k++) {
		prev[size_t(k)] = (k + m - 1) % m;
		next[size_t(k)] = (k + 1) % m;
	}
	const auto at = [&](int k) -> const Pt & { return p[size_t(idx[size_t(k)])]; };
	const auto ear = [&](int k) {
		const int a = prev[size_t(k)], c = next[size_t(k)];
		if (turn(at(a), at(k), at(c)) <= NO_AREA_M2) return false;
		for (int j = next[size_t(c)]; j != a; j = next[size_t(j)]) {
			const Pt &q = at(j);
			if (same(q, at(a)) || same(q, at(k)) || same(q, at(c))) continue;
			if (turn(at(a), at(k), q) >= 0 && turn(at(k), at(c), q) >= 0 && turn(at(c), at(a), q) >= 0) return false;
		}
		return true;
	};
	const auto unlink = [&](int k) {
		next[size_t(prev[size_t(k)])] = next[size_t(k)];
		prev[size_t(next[size_t(k)])] = prev[size_t(k)];
	};
	int left = m, k = 0, tried = 0;
	while (left > 3) {
		if (ear(k)) {
			tris.insert(tris.end(), { idx[size_t(prev[size_t(k)])], idx[size_t(k)], idx[size_t(next[size_t(k)])] });
			unlink(k);
			left--;
			k = prev[size_t(k)];
			tried = 0;
			continue;
		}
		k = next[size_t(k)];
		if (++tried <= left) continue;
		// No ear left: what remains has a point on the line of its neighbours, which goes with no triangle; else nothing has area.
		int flat = -1;
		for (int j = k, n = 0; n < left && flat < 0; j = next[size_t(j)], n++)
			if (std::fabs(turn(at(prev[size_t(j)]), at(j), at(next[size_t(j)]))) <= NO_AREA_M2) flat = j;
		if (flat < 0) return tris;
		unlink(flat);
		left--;
		k = next[size_t(flat)];
		tried = 0;
	}
	if (turn(at(prev[size_t(k)]), at(k), at(next[size_t(k)])) > NO_AREA_M2)
		tris.insert(tris.end(), { idx[size_t(prev[size_t(k)])], idx[size_t(k)], idx[size_t(next[size_t(k)])] });
	return tris;
}

// Each half-edge's twin (edge e of triangle t is 3t + e, from its corner e to corner e + 1), -1 on the ring.
std::vector<int> twins_of(const std::vector<int> &v) {
	struct Half {
		int lo, hi, he;
	};
	std::vector<Half> halves;
	halves.reserve(v.size());
	for (int h = 0; h < int(v.size()); h++) {
		const int a = v[size_t(h)], b = v[size_t(h - h % 3 + (h + 1) % 3)];
		halves.push_back({ std::min(a, b), std::max(a, b), h });
	}
	std::sort(halves.begin(), halves.end(), [](const Half &x, const Half &y) { return x.lo != y.lo ? x.lo < y.lo : x.hi < y.hi; });
	std::vector<int> twin(v.size(), -1);
	for (size_t i = 0; i + 1 < halves.size(); i++) {
		if (halves[i].lo != halves[i + 1].lo || halves[i].hi != halves[i + 1].hi) continue;
		twin[size_t(halves[i].he)] = halves[i + 1].he;
		twin[size_t(halves[i + 1].he)] = halves[i].he;
		i++;
	}
	return twin;
}

// Lawson's flips: every inner edge whose opposite corner lies in its triangle's circle swapped for the other diagonal.
void flip_to_delaunay(const std::vector<Pt> &p, std::vector<int> &v) {
	std::vector<int> twin = twins_of(v), stack;
	for (int h = 0; h < int(v.size()); h++)
		if (twin[size_t(h)] > h) stack.push_back(h);
	const auto corner = [&](int t, int e) -> int & { return v[size_t(3 * t + e % 3)]; };
	const auto link = [&](int h, int g) {
		twin[size_t(h)] = g;
		if (g >= 0) twin[size_t(g)] = h;
	};
	const size_t tris = v.size() / 3;
	for (size_t flips = 0; !stack.empty() && flips < FLIPS_PER_PAIR * tris * tris; ) {
		const int h = stack.back();
		stack.pop_back();
		const int g = twin[size_t(h)];
		if (g < 0) continue;
		const int k = h / 3, e = h % 3, m = g / 3, f = g % 3;
		const int a = corner(k, e), b = corner(k, e + 1), c = corner(k, e + 2), d = corner(m, f + 2);
		const Pt &pa = p[size_t(a)], &pb = p[size_t(b)], &pc = p[size_t(c)], &pd = p[size_t(d)];
		if (!in_circle(pa, pb, pc, pd) || turn(pa, pd, pc) <= NO_AREA_M2 || turn(pd, pb, pc) <= NO_AREA_M2) continue;
		const int bc = twin[size_t(3 * k + (e + 1) % 3)], ca = twin[size_t(3 * k + (e + 2) % 3)];
		const int ad = twin[size_t(3 * m + (f + 1) % 3)], db = twin[size_t(3 * m + (f + 2) % 3)];
		// (a, b, c) and (b, a, d) become (a, d, c) and (d, b, c): the quad a d b c counter-clockwise, cut along c-d.
		v[size_t(3 * k)] = a; v[size_t(3 * k + 1)] = d; v[size_t(3 * k + 2)] = c;
		v[size_t(3 * m)] = d; v[size_t(3 * m + 1)] = b; v[size_t(3 * m + 2)] = c;
		link(3 * k, ad);
		link(3 * k + 1, 3 * m + 2);
		link(3 * k + 2, ca);
		link(3 * m, db);
		link(3 * m + 1, bc);
		for (int o : { 3 * k, 3 * k + 2, 3 * m, 3 * m + 1 })
			if (twin[size_t(o)] >= 0) stack.push_back(o);
		flips++;
	}
}
} // namespace

std::vector<int> ring_triangles(const std::vector<double> &xy) {
	std::vector<Pt> p;
	p.reserve(xy.size() / 2);
	for (size_t i = 0; i + 1 < xy.size(); i += 2) p.push_back({ xy[i], xy[i + 1] });
	const std::vector<int> idx = kept_points(p);
	if (idx.empty()) return {};
	std::vector<int> tris = clip_ears(p, idx);
	flip_to_delaunay(p, tris);
	return tris;
}

} // namespace city
