// The formulas are the paper's, numbered as there (8.1 to 8.11, the typos of 8.3-8.4 and 8.11 corrected). Each family holds for
// the pose as given, driven backwards (timeflip) and on the other hand (reflect): one call tries the four.
#include "sim/traffic/reeds_shepp.h"

#include <cmath>
#include <limits>

namespace city {
namespace traffic {

namespace {
constexpr double PI = 3.14159265358979323846, HALF_PI = PI / 2, TWO_PI = 2 * PI;
constexpr double ZERO = 10 * std::numeric_limits<double>::epsilon();
constexpr double TINY_PIECE = 1e-12;
constexpr double NONE_M = 1e18;
constexpr double QUARTER = 0.25, HALF = 0.5;
constexpr double RHO_TOP = 20, RHO_BELOW = 16; // formula 8.8

// What a formula finds: the lengths of its free pieces.
struct Tuv {
	double t = 0, u = 0, v = 0;
};

using Formula = bool (*)(double x, double y, double phi, Tuv &o);

double wrap(double a) {
	double v = std::fmod(a, TWO_PI);
	if (v < -PI) v += TWO_PI;
	else if (v > PI) v -= TWO_PI;
	return v;
}

struct Polar {
	double r, theta;
};

Polar polar(double x, double y) {
	return { std::sqrt(x * x + y * y), std::atan2(y, x) };
}

void tau_omega(double u, double v, double xi, double eta, double phi, double &tau, double &omega) {
	const double delta = wrap(u - v), a = std::sin(u) - std::sin(delta), b = std::cos(u) - std::cos(delta) - 1;
	const double t1 = std::atan2(eta * a - xi * b, xi * a + eta * b), t2 = 2 * (std::cos(delta) - std::cos(v) - std::cos(u)) + 3;
	tau = t2 < 0 ? wrap(t1 + PI) : wrap(t1);
	omega = wrap(tau - u + v - phi);
}

bool lp_sp_lp(double x, double y, double phi, Tuv &o) { // 8.1
	const Polar p = polar(x - std::sin(phi), y - 1 + std::cos(phi));
	o.u = p.r;
	o.t = p.theta;
	if (o.t < -ZERO) return false;
	o.v = wrap(phi - o.t);
	return o.v >= -ZERO;
}

bool lp_sp_rp(double x, double y, double phi, Tuv &o) { // 8.2
	const Polar p = polar(x + std::sin(phi), y - 1 - std::cos(phi));
	const double u1 = p.r * p.r;
	if (u1 < 4) return false;
	o.u = std::sqrt(u1 - 4);
	o.t = wrap(p.theta + std::atan2(2, o.u));
	o.v = wrap(o.t - phi);
	return o.t >= -ZERO && o.v >= -ZERO;
}

bool lp_rm_l(double x, double y, double phi, Tuv &o) { // 8.3, 8.4
	const Polar p = polar(x - std::sin(phi), y - 1 + std::cos(phi));
	if (p.r > 4) return false;
	o.u = -2 * std::asin(QUARTER * p.r);
	o.t = wrap(p.theta + HALF * o.u + PI);
	o.v = wrap(phi - o.t + o.u);
	return o.t >= -ZERO && o.u <= ZERO;
}

bool lp_rup_lum_rm(double x, double y, double phi, Tuv &o) { // 8.7
	const double xi = x + std::sin(phi), eta = y - 1 - std::cos(phi), rho = QUARTER * (2 + std::sqrt(xi * xi + eta * eta));
	if (rho > 1) return false;
	o.u = std::acos(rho);
	tau_omega(o.u, -o.u, xi, eta, phi, o.t, o.v);
	return o.t >= -ZERO && o.v <= ZERO;
}

bool lp_rum_lum_rp(double x, double y, double phi, Tuv &o) { // 8.8
	const double xi = x + std::sin(phi), eta = y - 1 - std::cos(phi), rho = (RHO_TOP - xi * xi - eta * eta) / RHO_BELOW;
	if (rho < 0 || rho > 1) return false;
	o.u = -std::acos(rho);
	if (o.u < -HALF_PI) return false;
	tau_omega(o.u, o.u, xi, eta, phi, o.t, o.v);
	return o.t >= -ZERO && o.v >= -ZERO;
}

bool lp_rm_sm_lm(double x, double y, double phi, Tuv &o) { // 8.9
	const Polar p = polar(x - std::sin(phi), y - 1 + std::cos(phi));
	if (p.r < 2) return false;
	const double r = std::sqrt(p.r * p.r - 4);
	o.u = 2 - r;
	o.t = wrap(p.theta + std::atan2(r, -2));
	o.v = wrap(phi - HALF_PI - o.t);
	return o.t >= -ZERO && o.u <= ZERO && o.v <= ZERO;
}

bool lp_rm_sm_rm(double x, double y, double phi, Tuv &o) { // 8.10
	const double xi = x + std::sin(phi), eta = y - 1 - std::cos(phi);
	const Polar p = polar(-eta, xi);
	if (p.r < 2) return false;
	o.t = p.theta;
	o.u = 2 - p.r;
	o.v = wrap(o.t + HALF_PI - phi);
	return o.t >= -ZERO && o.u <= ZERO && o.v <= ZERO;
}

bool lp_rm_s_lm_rp(double x, double y, double phi, Tuv &o) { // 8.11
	const double xi = x + std::sin(phi), eta = y - 1 - std::cos(phi);
	const Polar p = polar(xi, eta);
	if (p.r < 2) return false;
	o.u = 4 - std::sqrt(p.r * p.r - 4);
	if (o.u > ZERO) return false;
	o.t = wrap(std::atan2((4 - o.u) * xi - 2 * eta, -2 * xi + (o.u - 4) * eta));
	o.v = wrap(o.t - phi);
	return o.t >= -ZERO && o.v >= -ZERO;
}

// How a family lays its free lengths out along its pieces.
enum class Shape { TUV, VUT, T_U_NEG_U_V, T_U_U_V, T_H_U_V, V_U_H_T, T_H_U_H_V };

int spread(Shape s, const Tuv &o, double *l) {
	const double h = -HALF_PI;
	switch (s) {
		case Shape::TUV: l[0] = o.t; l[1] = o.u; l[2] = o.v; return 3;
		case Shape::VUT: l[0] = o.v; l[1] = o.u; l[2] = o.t; return 3;
		case Shape::T_U_NEG_U_V: l[0] = o.t; l[1] = o.u; l[2] = -o.u; l[3] = o.v; return 4;
		case Shape::T_U_U_V: l[0] = o.t; l[1] = o.u; l[2] = o.u; l[3] = o.v; return 4;
		case Shape::T_H_U_V: l[0] = o.t; l[1] = h; l[2] = o.u; l[3] = o.v; return 4;
		case Shape::V_U_H_T: l[0] = o.v; l[1] = o.u; l[2] = h; l[3] = o.t; return 4;
		default: l[0] = o.t; l[1] = h; l[2] = o.u; l[3] = h; l[4] = o.v; return RS_MOST_PIECES;
	}
}

// One family at the pose as given, timeflipped, reflected and both; `left` and `right` the pieces of the hand given and the other.
void family(RsPath &best, Formula f, Shape shape, const char *left, const char *right, double x, double y, double phi) {
	for (int k = 0; k < 4; k++) {
		const bool flip = (k & 1) != 0, mirror = (k & 2) != 0;
		Tuv o;
		if (!f(flip ? -x : x, mirror ? -y : y, flip != mirror ? -phi : phi, o)) continue;
		double l[RS_MOST_PIECES] = {};
		const int n = spread(shape, o, l);
		double total = 0;
		for (int i = 0; i < n; i++) {
			l[i] = flip ? -l[i] : l[i];
			total += std::fabs(l[i]);
		}
		if (total >= best.total) continue;
		best.pieces = mirror ? right : left;
		best.count = n;
		best.total = total;
		for (int i = 0; i < RS_MOST_PIECES; i++) best.lengths[i] = i < n ? l[i] : 0;
	}
}

bool search(double x, double y, double phi, RsPath &best) {
	best = RsPath();
	best.total = NONE_M;
	const double xb = x * std::cos(phi) + y * std::sin(phi), yb = x * std::sin(phi) - y * std::cos(phi); // the path backwards
	family(best, lp_sp_lp, Shape::TUV, "LSL", "RSR", x, y, phi);
	family(best, lp_sp_rp, Shape::TUV, "LSR", "RSL", x, y, phi);
	family(best, lp_rm_l, Shape::TUV, "LRL", "RLR", x, y, phi);
	family(best, lp_rm_l, Shape::VUT, "LRL", "RLR", xb, yb, phi);
	family(best, lp_rup_lum_rm, Shape::T_U_NEG_U_V, "LRLR", "RLRL", x, y, phi);
	family(best, lp_rum_lum_rp, Shape::T_U_U_V, "LRLR", "RLRL", x, y, phi);
	family(best, lp_rm_sm_lm, Shape::T_H_U_V, "LRSL", "RLSR", x, y, phi);
	family(best, lp_rm_sm_rm, Shape::T_H_U_V, "LRSR", "RLSL", x, y, phi);
	family(best, lp_rm_sm_lm, Shape::V_U_H_T, "LSRL", "RSLR", xb, yb, phi);
	family(best, lp_rm_sm_rm, Shape::V_U_H_T, "RSRL", "LSLR", xb, yb, phi);
	family(best, lp_rm_s_lm_rp, Shape::T_H_U_H_V, "LRSLR", "RLSRL", x, y, phi);
	return best.count > 0;
}

// The goal in the frame of the start, on a unit radius.
bool from_start(const AxlePose &a, const AxlePose &b, double radius, RsPath &out) {
	const double dx = b.x - a.x, dz = b.z - a.z, c = std::cos(a.angle), s = std::sin(a.angle);
	return search((c * dx + s * dz) / radius, (-s * dx + c * dz) / radius, b.angle - a.angle, out);
}
} // namespace

bool reeds_shepp(const AxlePose &a, const AxlePose &b, double radius, RsPath &out) {
	return from_start(a, b, radius, out);
}

double reeds_shepp_length(const AxlePose &a, const AxlePose &b, double radius) {
	RsPath p;
	return from_start(a, b, radius, p) ? p.total * radius : NONE_M;
}

void sample_reeds_shepp(const RsPath &path, const AxlePose &a, double radius, double step, std::vector<AxlePose> &out) {
	double x = 0, z = 0, phi = a.angle;
	const double unit = step / radius;
	for (int i = 0; i < path.count; i++) {
		const double len = path.lengths[i];
		if (std::fabs(len) < TINY_PIECE) continue;
		const char piece = path.pieces[i];
		const int n = std::max(1, int(std::ceil(std::fabs(len) / unit)));
		const double x0 = x, z0 = z, p0 = phi;
		for (int k = 1; k <= n; k++) {
			const double v = len * k / n;
			if (piece == 'L') {
				x = x0 + std::sin(p0 + v) - std::sin(p0);
				z = z0 - std::cos(p0 + v) + std::cos(p0);
				phi = p0 + v;
			} else if (piece == 'R') {
				x = x0 - std::sin(p0 - v) + std::sin(p0);
				z = z0 + std::cos(p0 - v) - std::cos(p0);
				phi = p0 - v;
			} else {
				x = x0 + v * std::cos(p0);
				z = z0 + v * std::sin(p0);
			}
			out.push_back({ a.x + x * radius, a.z + z * radius, phi, len < 0 });
		}
	}
}

} // namespace traffic
} // namespace city
