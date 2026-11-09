// Port of Roadcraft src/roads/geometry.ts.
#include "core/math.h"
#include "core/plane_geometry.h"
#include "core/units.h"
#include "sim/catalog/catalog.h"
#include "sim/roads/geometry.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr double STRAIGHT_CROSS = 1e-9; // a cross product this near nought is a straight line
constexpr int FIT_STEPS = 40; // bisections: the bend to a part in 2^40
} // namespace

namespace {
// the nearest point of a curve: samples along it, then halvings of the step round the best (a part in 2^12 of a sample)
constexpr int CLOSEST_SAMPLES = 32, CLOSEST_REFINE = 12;
// rounding: two lines or segments this near parallel do not cross; a curve's span this short has no length
constexpr double LINE_PARALLEL = 1e-9, SEG_PARALLEL = 1e-12;
constexpr double ARC_SPAN_EPS = 1e-9;
} // namespace

bool Quad::is_straight(double tol) const { return std::hypot(cx - (x0 + x1) / 2, cz - (z0 + z1) / 2) < tol; }

double quad_length(const Quad &q, int steps) { return arc_table(q, steps).back(); }

std::vector<double> arc_table(const Quad &q, int steps) {
	std::vector<double> t(size_t(steps) + 1, 0.0);
	double px = q.x0, pz = q.z0;
	for (int i = 1; i <= steps; i++) {
		double x, z;
		q.at(double(i) / steps, x, z);
		t[i] = t[i - 1] + std::hypot(x - px, z - pz);
		px = x; pz = z;
	}
	return t;
}

double arc_at(const std::vector<double> &table, double t) {
	int n = int(table.size()) - 1;
	double f = std::max(0.0, std::min(1.0, t)) * n;
	int i = std::min(n - 1, int(std::floor(f)));
	return table[i] + (table[i + 1] - table[i]) * (f - i);
}

double t_at_length(const std::vector<double> &table, double s) {
	int n = int(table.size()) - 1;
	if (s <= 0) return 0;
	if (s >= table[n]) return 1;
	int lo = 0, hi = n;
	while (hi - lo > 1) {
		int m = (lo + hi) >> 1;
		if (table[m] < s) lo = m; else hi = m;
	}
	double f = (s - table[lo]) / std::max(ARC_SPAN_EPS, table[hi] - table[lo]);
	return (lo + f) / n;
}

std::array<Quad, 2> split_quad(const Quad &q, double t) {
	double ax = q.x0 + (q.cx - q.x0) * t, az = q.z0 + (q.cz - q.z0) * t;
	double bx = q.cx + (q.x1 - q.cx) * t, bz = q.cz + (q.z1 - q.cz) * t;
	double mx = ax + (bx - ax) * t, mz = az + (bz - az) * t;
	return { Quad{ q.x0, q.z0, ax, az, mx, mz }, Quad{ mx, mz, bx, bz, q.x1, q.z1 } };
}

Quad sub_quad(const Quad &q, double t0, double t1) {
	if (t0 <= 0) return split_quad(q, t1)[0];
	Quad right = split_quad(q, t0)[1];
	return split_quad(right, (t1 - t0) / (1 - t0))[0];
}

void closest_t(const Quad &q, double x, double z, double &t_out, double &dist) {
	double best = 0, bd = std::numeric_limits<double>::infinity();
	auto probe = [&](double t) {
		double px, pz;
		q.at(t, px, pz);
		double d = (px - x) * (px - x) + (pz - z) * (pz - z);
		if (d < bd) { bd = d; best = t; }
	};
	for (int i = 0; i <= CLOSEST_SAMPLES; i++) probe(double(i) / CLOSEST_SAMPLES);
	double step = 1.0 / CLOSEST_SAMPLES;
	for (int k = 0; k < CLOSEST_REFINE; k++) {
		step /= 2;
		double c = best;
		for (double t : { c - step, c + step })
			if (t >= 0 && t <= 1) probe(t);
	}
	t_out = best;
	dist = std::sqrt(bd);
}

bool seg_seg(double ax, double az, double bx, double bz, double cx, double cz, double dx, double dz, double &s, double &t) {
	if (!line_shares(ax, az, bx, bz, cx, cz, dx, dz, SEG_PARALLEL, s, t)) return false;
	return !(s < 0 || s > 1 || t < 0 || t > 1);
}

bool line_line(double px, double pz, double dx, double dz, double qx, double qz, double ex, double ez, double &s, double &t) {
	double den = dx * ez - dz * ex;
	if (std::fabs(den) < LINE_PARALLEL) return false;
	double wx = qx - px, wz = qz - pz;
	s = (wx * ez - wz * ex) / den;
	t = (wx * dz - wz * dx) / den;
	return true;
}

double arm_angle(double dx, double dz) { return std::atan2(-dz, dx); }

double quad_min_radius(const Quad &q) {
	// B'(t) = 2 (A + t D) with A = c - a and D = (b - c) - A, so B' x B'' is constant and the curvature peaks where
	// |A + t D| is least: radius = 2 |A + t D|^3 / |A x D|.
	double ax = q.cx - q.x0, az = q.cz - q.z0, dx = q.x1 - q.cx - ax, dz = q.z1 - q.cz - az;
	double cross = std::fabs(ax * dz - az * dx), dd = dx * dx + dz * dz;
	if (cross < STRAIGHT_CROSS) return std::numeric_limits<double>::infinity();
	double t = dd > 0 ? std::clamp(-(ax * dx + az * dz) / dd, 0.0, 1.0) : 0;
	return 2 * std::pow(std::hypot(ax + t * dx, az + t * dz), 3) / cross;
}

Quad fit_quad_to_radius(const Quad &q, double min_radius) {
	if (quad_min_radius(q) >= min_radius) return q;
	// The control's foot on the chord keeps where along the chord the bend sits; only its height changes.
	double dx = q.x1 - q.x0, dz = q.z1 - q.z0, l2 = dx * dx + dz * dz;
	double t = l2 > 0 ? ((q.cx - q.x0) * dx + (q.cz - q.z0) * dz) / l2 : 0;
	double fx = q.x0 + dx * t, fz = q.z0 + dz * t, lo = 0, hi = 1;
	auto at = [&](double k) { return Quad{ q.x0, q.z0, fx + (q.cx - fx) * k, fz + (q.cz - fz) * k, q.x1, q.z1 }; };
	for (int i = 0; i < FIT_STEPS; i++) {
		double mid = (lo + hi) / 2;
		(quad_min_radius(at(mid)) >= min_radius ? lo : hi) = mid;
	}
	return at(lo);
}

} // namespace city
