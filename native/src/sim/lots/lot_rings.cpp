// The geometry of a lot on the (x, z) plane (lot_rings.h), as world/lots.ts has it on its (x, y): the same turns, the same left.
#include "sim/lots/lot_rings.h"
#include "core/plane_geometry.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr double MIN_LOT_M = 10, NEAR_M = 1.2, CUT_PAST_M = 1, SIDE_LEAST_M = 1, OFF_ANGLE_M = 40, STAND_STEP_M = 0.5, FRONT_EPS = 1e-9, INNER_SHARE = 0.2;
constexpr int STAND_MOST_ROWS = 160, CURVE_STEPS = 12;
constexpr double QUARTER_LOT = 4; // of the least lot's square: what a cut may leave

LotPoint lerp(const LotPoint &a, const LotPoint &b, double t) { return { a.x + (b.x - a.x) * t, a.z + (b.z - a.z) * t }; }

// The part of a ring on the left of a -> b (Sutherland-Hodgman against one half-plane).
std::vector<LotPoint> clip_left(const std::vector<LotPoint> &q, const LotPoint &a, const LotPoint &b) {
	const auto side = [&](const LotPoint &p) { return (b.x - a.x) * (p.z - a.z) - (b.z - a.z) * (p.x - a.x); };
	std::vector<LotPoint> out;
	for (size_t i = 0; i < q.size(); i++) {
		const LotPoint &p = q[i], &r = q[(i + 1) % q.size()];
		const double sp = side(p), sr = side(r);
		if (sp >= 0) out.push_back(p);
		if ((sp >= 0) != (sr >= 0)) out.push_back(lerp(p, r, sp / (sp - sr)));
	}
	return out;
}

// A piece of a cut lot with its front: the side along the old front, or the side nearest it.
std::vector<LotPoint> with_front(const std::vector<LotPoint> &piece, const LotPoint &fa, const LotPoint &fb) {
	const double len = std::max(std::hypot(fb.x - fa.x, fb.z - fa.z), FRONT_EPS);
	const auto off = [&](const LotPoint &p) { return std::abs((fb.x - fa.x) * (p.z - fa.z) - (fb.z - fa.z) * (p.x - fa.x)) / len; };
	size_t best = 0;
	double best_score = std::numeric_limits<double>::infinity();
	for (size_t i = 0; i < piece.size(); i++) {
		const LotPoint &p = piece[i], &r = piece[(i + 1) % piece.size()];
		const double l = std::hypot(r.x - p.x, r.z - p.z);
		if (l < SIDE_LEAST_M) continue;
		const double parallel = std::abs(((r.x - p.x) * (fb.x - fa.x) + (r.z - p.z) * (fb.z - fa.z)) / (l * len));
		const double score = off(lerp(p, r, 0.5)) + (1 - parallel) * OFF_ANGLE_M;
		if (score < best_score) {
			best_score = score;
			best = i;
		}
	}
	std::vector<LotPoint> out(piece.begin() + std::ptrdiff_t(best), piece.end());
	out.insert(out.end(), piece.begin(), piece.begin() + std::ptrdiff_t(best));
	return out;
}

LotPoint centre_of(const std::vector<LotPoint> &ring) {
	LotPoint c;
	for (const LotPoint &p : ring) {
		c.x += p.x / double(ring.size());
		c.z += p.z / double(ring.size());
	}
	return c;
}

bool inner(const std::vector<LotPoint> &p, const std::vector<LotPoint> &q) {
	const LotPoint c = centre_of(p);
	if (inside_lot(c, q)) return true;
	for (const LotPoint &x : p)
		if (inside_lot(lerp(x, c, INNER_SHARE), q)) return true;
	return false;
}
} // namespace

std::vector<LotPoint> piece_with_front(const std::vector<LotPoint> &piece, const LotPoint &fa, const LotPoint &fb) { return with_front(piece, fa, fb); }

double lot_area(const std::vector<LotPoint> &q) {
	double a = 0;
	for (size_t i = 0; i < q.size(); i++) {
		const LotPoint &p = q[i], &r = q[(i + 1) % q.size()];
		a += p.x * r.z - r.x * p.z;
	}
	return a / 2;
}

std::vector<int> lot_triangles(const std::vector<LotPoint> &ring) {
	std::vector<int> left(ring.size()), out;
	for (size_t i = 0; i < ring.size(); i++) left[i] = int(i);
	if (lot_area(ring) < 0) std::reverse(left.begin(), left.end());
	const auto turn = [&](int a, int b, int c) { return (ring[size_t(b)].x - ring[size_t(a)].x) * (ring[size_t(c)].z - ring[size_t(a)].z) - (ring[size_t(b)].z - ring[size_t(a)].z) * (ring[size_t(c)].x - ring[size_t(a)].x); };
	const auto holds = [&](int a, int b, int c, int p) { return turn(a, b, p) >= 0 && turn(b, c, p) >= 0 && turn(c, a, p) >= 0; };
	while (left.size() > 2) {
		const size_t n = left.size();
		bool cut = false;
		for (size_t i = 0; i < n && !cut; i++) {
			const int a = left[(i + n - 1) % n], b = left[i], c = left[(i + 1) % n];
			if (turn(a, b, c) <= 0) continue;
			bool ear = true;
			for (int p : left)
				if (p != a && p != b && p != c && holds(a, b, c, p)) ear = false;
			if (!ear) continue;
			out.insert(out.end(), { a, b, c });
			left.erase(left.begin() + std::ptrdiff_t(i));
			cut = true;
		}
		if (!cut) break; // what is left has no area: points in a line
	}
	return out;
}

bool big_enough(const std::vector<LotPoint> &ring) { return ring.size() >= 3 && std::abs(lot_area(ring)) >= MIN_LOT_M * MIN_LOT_M / 2; }

bool inside_lot(const LotPoint &p, const std::vector<LotPoint> &q) {
	return inside_ring(q, p.x, p.z, [](const LotPoint &c) { return std::make_pair(c.x, c.z); });
}

std::vector<LotPoint> facing_corners(const std::vector<LotPoint> &c) {
	if (lot_area(c) >= 0) return c;
	std::vector<LotPoint> out{ c[1], c[0] };
	out.insert(out.end(), c.rbegin(), c.rend() - 2);
	return out;
}

LotStand lot_stand(const std::vector<LotPoint> &q) {
	const LotPoint &a = q[0], &b = q[1];
	const double len = std::max(std::hypot(b.x - a.x, b.z - a.z), FRONT_EPS);
	const double ux = (b.x - a.x) / len, uz = (b.z - a.z) / len, nx = -uz, nz = ux;
	std::vector<double> ls(q.size()), lt(q.size());
	double t_min = std::numeric_limits<double>::infinity(), t_max = -t_min;
	for (size_t i = 0; i < q.size(); i++) {
		ls[i] = (q[i].x - a.x) * ux + (q[i].z - a.z) * uz;
		lt[i] = (q[i].x - a.x) * nx + (q[i].z - a.z) * nz;
		t_min = std::min(t_min, lt[i]);
		t_max = std::max(t_max, lt[i]);
	}
	const int rows = std::clamp(int(std::ceil((t_max - t_min) / STAND_STEP_M)), 1, STAND_MOST_ROWS);
	const double h = (t_max - t_min) / rows;
	std::vector<double> left(size_t(rows), 0), right(size_t(rows), -1), xs;
	for (int k = 0; k < rows; k++) {
		const double t = t_min + (k + 0.5) * h;
		xs.clear();
		for (size_t i = 0; i < q.size(); i++) {
			const size_t j = (i + 1) % q.size();
			if ((lt[i] > t) == (lt[j] > t)) continue;
			xs.push_back(ls[i] + (t - lt[i]) / (lt[j] - lt[i]) * (ls[j] - ls[i]));
		}
		std::sort(xs.begin(), xs.end());
		for (size_t i = 0; i + 1 < xs.size(); i += 2)
			if (xs[i + 1] - xs[i] > right[size_t(k)] - left[size_t(k)]) {
				left[size_t(k)] = xs[i];
				right[size_t(k)] = xs[i + 1];
			}
	}
	double best = -std::numeric_limits<double>::infinity(), s0 = 0, s1 = len, t0 = 0, t1 = 0;
	for (int i = 0; i < rows; i++) {
		double lo = -std::numeric_limits<double>::infinity(), hi = -lo;
		const double top = t_min + i * h, front = std::max(0.0, top);
		for (int j = i; j < rows; j++) {
			lo = std::max(lo, left[size_t(j)]);
			hi = std::min(hi, right[size_t(j)]);
			if (hi - lo <= 0) break;
			const double end = t_min + (j + 1) * h, score = (hi - lo) * (end - top) - (hi - lo) * front;
			if (score > best) {
				best = score;
				s0 = lo;
				s1 = hi;
				t0 = top;
				t1 = end;
			}
		}
	}
	LotStand st;
	st.w = std::max(0.0, s1 - s0);
	st.d = std::max(0.0, t1 - t0);
	const double sm = (s0 + s1) / 2, tm = (t0 + t1) / 2;
	st.x = a.x + ux * sm + nx * tm;
	st.z = a.z + uz * sm + nz * tm;
	st.rot = std::atan2(-nx, -nz); // its local +z towards the street
	return st;
}

std::vector<LotPoint> lot_rect(const LotPoint &a, const LotPoint &b, double angle) {
	const double ux = std::cos(angle), uz = std::sin(angle), vx = -uz, vz = ux;
	const double ds = (b.x - a.x) * ux + (b.z - a.z) * uz, dt = (b.x - a.x) * vx + (b.z - a.z) * vz;
	if (std::abs(ds) < MIN_LOT_M / 2 || std::abs(dt) < MIN_LOT_M / 2) return {};
	const auto p = [&](double s, double t) { return LotPoint{ a.x + ux * s + vx * t, a.z + uz * s + vz * t }; };
	const double s0 = std::min(0.0, ds), s1 = std::max(0.0, ds), t0 = std::min(0.0, dt), t1 = std::max(0.0, dt);
	return { p(s0, t0), p(s1, t0), p(s1, t1), p(s0, t1) };
}

std::vector<LotPoint> cut_lines(const std::vector<LotPoint> &ring, const LotCut &cut) {
	if (cut.kind == LotCutKind::LINE) return { cut.a, cut.b };
	const LotPoint &a = ring[0], &b = ring[1];
	const double w = std::max(std::hypot(b.x - a.x, b.z - a.z), FRONT_EPS), ux = (b.x - a.x) / w, uz = (b.z - a.z) / w, nx = -uz, nz = ux;
	double s0 = std::numeric_limits<double>::infinity(), s1 = -s0, t0 = s0, t1 = -s0;
	for (const LotPoint &q : ring) {
		const double s = (q.x - a.x) * ux + (q.z - a.z) * uz, t = (q.x - a.x) * nx + (q.z - a.z) * nz;
		s0 = std::min(s0, s);
		s1 = std::max(s1, s);
		t0 = std::min(t0, t);
		t1 = std::max(t1, t);
	}
	const auto at = [&](double s, double t) { return LotPoint{ a.x + ux * s + nx * t, a.z + uz * s + nz * t }; };
	std::vector<LotPoint> out;
	for (int k = 1; k < cut.parts; k++) {
		if (cut.kind == LotCutKind::ACROSS) {
			const double s = double(k) / cut.parts * w; // equal widths along the front itself
			out.push_back(at(s, t1 + CUT_PAST_M));
			out.push_back(at(s, t0 - CUT_PAST_M));
		} else {
			const double t = t0 + (t1 - t0) * k / cut.parts;
			out.push_back(at(s0 - CUT_PAST_M, t));
			out.push_back(at(s1 + CUT_PAST_M, t));
		}
	}
	return out;
}

std::vector<std::vector<LotPoint>> cut_lot(const std::vector<LotPoint> &ring, const LotCut &cut) {
	const LotPoint fa = ring[0], fb = ring[1];
	std::vector<std::vector<LotPoint>> pieces{ ring };
	const std::vector<LotPoint> lines = cut_lines(ring, cut);
	for (size_t k = 0; k + 1 < lines.size(); k += 2) {
		std::vector<std::vector<LotPoint>> next;
		for (const std::vector<LotPoint> &piece : pieces)
			for (const std::vector<LotPoint> &half : { clip_left(piece, lines[k], lines[k + 1]), clip_left(piece, lines[k + 1], lines[k]) })
				if (half.size() >= 3 && std::abs(lot_area(half)) > MIN_LOT_M * MIN_LOT_M / QUARTER_LOT) next.push_back(half);
		pieces = std::move(next);
	}
	for (std::vector<LotPoint> &piece : pieces) piece = facing_corners(with_front(piece, fa, fb));
	return pieces;
}

std::vector<LotPoint> joined_lots(const std::vector<LotPoint> &A, const std::vector<LotPoint> &B) {
	const size_t n = A.size(), m = B.size();
	std::vector<int> on_b(n, -1);
	for (size_t i = 0; i < n; i++)
		for (size_t j = 0; j < m; j++)
			if (lot_point_near(A[i], B[j], NEAR_M)) on_b[i] = int(j);
	size_t start = n;
	for (size_t i = 0; i < n && start == n; i++)
		if (on_b[i] >= 0 && on_b[(i + 1) % n] >= 0) start = i;
	if (start == n) return {};
	size_t i0 = start;
	while (on_b[(i0 + n - 1) % n] >= 0 && (i0 + n - 1) % n != start) i0 = (i0 + n - 1) % n;
	size_t i1 = (start + 1) % n;
	while (on_b[(i1 + 1) % n] >= 0 && (i1 + 1) % n != i0) i1 = (i1 + 1) % n;
	std::vector<LotPoint> ring;
	for (size_t k = i1;; k = (k + 1) % n) {
		ring.push_back(A[k]);
		if (k == i0) break;
	}
	for (size_t k = (size_t(on_b[i0]) + 1) % m; k != size_t(on_b[i1]); k = (k + 1) % m) ring.push_back(B[k]);
	if (ring.size() < 3) return {};
	size_t f = 0; // the front stays the first lot's
	for (size_t k = 0; k < ring.size(); k++)
		if (lot_point_near(ring[k], A[0], NEAR_M) && lot_point_near(ring[(k + 1) % ring.size()], A[1], NEAR_M)) f = k;
	std::rotate(ring.begin(), ring.begin() + std::ptrdiff_t(f), ring.end());
	return facing_corners(ring);
}

bool lots_overlap(const std::vector<LotPoint> &a, const std::vector<LotPoint> &b) { return inner(a, b) || inner(b, a); }

std::vector<LotPoint> curved_side(const LotPoint &a, const LotPoint &b, const LotPoint &through) {
	const LotPoint control{ 2 * through.x - (a.x + b.x) / 2, 2 * through.z - (a.z + b.z) / 2 };
	std::vector<LotPoint> out;
	for (int k = 1; k < CURVE_STEPS; k++) {
		const double t = double(k) / CURVE_STEPS, s = 1 - t;
		out.push_back({ s * s * a.x + 2 * s * t * control.x + t * t * b.x, s * s * a.z + 2 * s * t * control.z + t * t * b.z });
	}
	return out;
}

bool lot_point_near(const LotPoint &p, const LotPoint &q, double reach) { return std::hypot(p.x - q.x, p.z - q.z) < reach; }

} // namespace city
