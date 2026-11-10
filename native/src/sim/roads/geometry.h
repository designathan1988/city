// Pure road geometry (Roadcraft src/roads/geometry.ts): quadratic Bézier segments in the XZ plane, arc-length
// tables, closest point, splitting, line crossings and the arms of a node. "Left" of a direction d is (d.z, -d.x).
#pragma once

#include <array>
#include <vector>

namespace city {

struct Quad {
	double x0 = 0, z0 = 0, cx = 0, cz = 0, x1 = 0, z1 = 0;
	static Quad straight(double ax, double az, double bx, double bz) { return { ax, az, (ax + bx) / 2, (az + bz) / 2, bx, bz }; }
	void at(double t, double &x, double &z) const {
		double s = 1 - t;
		x = s * s * x0 + 2 * s * t * cx + t * t * x1;
		z = s * s * z0 + 2 * s * t * cz + t * t * z1;
	}
	// Derivative (not normalised).
	void deriv(double t, double &dx, double &dz) const {
		dx = 2 * (1 - t) * (cx - x0) + 2 * t * (x1 - cx);
		dz = 2 * (1 - t) * (cz - z0) + 2 * t * (z1 - cz);
	}
	bool is_straight(double tol) const;
};

constexpr int ARC_STEPS_DEFAULT = 24;
double quad_length(const Quad &q, int steps = ARC_STEPS_DEFAULT);
// Cumulative chord lengths at t = i / steps (size steps + 1).
std::vector<double> arc_table(const Quad &q, int steps);
double t_at_length(const std::vector<double> &table, double s);
// Arc length at curve parameter t (the inverse of t_at_length), read off the same table.
double arc_at(const std::vector<double> &table, double t);
std::array<Quad, 2> split_quad(const Quad &q, double t);
Quad sub_quad(const Quad &q, double t0, double t1);
// The tightest radius of the curve (infinite when straight).
double quad_min_radius(const Quad &q);
// The same curve flattened just enough to be nowhere tighter than min_radius, its control sliding towards the chord.
Quad fit_quad_to_radius(const Quad &q, double min_radius);
// Closest point on the curve: t and distance (32 samples refined 12 times).
void closest_t(const Quad &q, double x, double z, double &t, double &dist);
// Intersection of two line segments ab and cd: parameters s, t in [0, 1].
bool seg_seg(double ax, double az, double bx, double bz, double cx, double cz, double dx, double dz, double &s, double &t);
// Intersection of two lines (point + direction): parameters, false when parallel.
bool line_line(double px, double pz, double dx, double dz, double qx, double qz, double ex, double ez, double &s, double &t);

// A node's arm: outward unit direction and the half widths to its left and right (total and asphalt).
struct Arm {
	double dx = 1, dz = 0, left = 0, right = 0, asph_left = 0, asph_right = 0;
};
// Sorting key: anticlockwise seen from above.
double arm_angle(double dx, double dz);
// Each arm's setback so neighbouring edges do not overlap; arms sorted by arm_angle.
void arm_retreats(const std::vector<Arm> &arms, double corner_radius, std::vector<double> &out);

} // namespace city
