// Port of Roadcraft src/roads/junction.ts (arms, setbacks, corners, bulbs).
#include "core/math.h"
#include "core/units.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace city {

namespace {
constexpr double KERB_RADIUS_MIN = 2.8, TAPER_ANGLE = 12 * PI_D / DEG_PER_HALF_TURN, ACUTE = 12 * PI_D / DEG_PER_HALF_TURN, CORNER_MITER = 2.5;
constexpr double BULB_HALF_MAX = 6.5, BULB_LEN_MIN = 40, MAX_RETREAT = 60, BENT_RADIUS = 0.3;
constexpr double BEVEL_PAD = 1.02, STRAIGHT_EPS = 1e-3, TAPER_MIN_STEP = 0.05, FILLET_SHARE = 0.5;
constexpr double DEGENERATE_ARM_EPS = 1e-6;
} // namespace

Arm arm_of(const Quad &q, const Profile &p, int end) {
	double dx = end == 0 ? q.cx - q.x0 : q.cx - q.x1, dz = end == 0 ? q.cz - q.z0 : q.cz - q.z1;
	if (std::hypot(dx, dz) < DEGENERATE_ARM_EPS) {
		dx = end == 0 ? q.x1 - q.x0 : q.x0 - q.x1;
		dz = end == 0 ? q.z1 - q.z0 : q.z0 - q.z1;
	}
	double l = std::hypot(dx, dz);
	if (l == 0) l = 1;
	Arm a;
	a.dx = dx / l;
	a.dz = dz / l;
	if (end == 0) { a.left = p.left; a.right = p.right; a.asph_left = p.asph_left; a.asph_right = p.asph_right; }
	else { a.left = p.right; a.right = p.left; a.asph_left = p.asph_right; a.asph_right = p.asph_left; }
	return a;
}

double bulb_retreat(const Arm &arm, double len) {
	double half = std::max(arm.asph_left, arm.asph_right);
	if (half > BULB_HALF_MAX || half < 1 || len < BULB_LEN_MIN) return 0;
	return std::min(std::sqrt(BULB_R * BULB_R - half * half), len * MOUTH_SHARE);
}

void junction_retreats(const std::vector<Arm> &arms, const std::vector<double> &lens, std::vector<double> &out,
		std::vector<int> &order, bool bulb) {
	size_t n = arms.size();
	out.assign(n, 0.0);
	order.resize(n);
	std::iota(order.begin(), order.end(), 0);
	if (n < 2) {
		if (n == 1) out[0] = bulb ? bulb_retreat(arms[0], lens[0]) : 0;
		return;
	}
	std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
		return arm_angle(arms[a].dx, arms[a].dz) < arm_angle(arms[b].dx, arms[b].dz);
	});
	std::vector<double> tmp;
	if (n == 2) {
		arm_retreats({ arms[order[0]], arms[order[1]] }, BENT_RADIUS, tmp);
		for (size_t k = 0; k < n; k++) out[order[k]] = std::min(tmp[k], lens[order[k]] * MOUTH_SHARE);
		return;
	}
	tmp.assign(n, 0.0);
	for (size_t k = 0; k < n; k++) {
		Corner f = corner_fillet(arms[order[k]], arms[order[(k + 1) % n]]);
		tmp[k] = std::max(tmp[k], f.trim_a);
		tmp[(k + 1) % n] = std::max(tmp[(k + 1) % n], f.trim_b);
	}
	for (size_t k = 0; k < n; k++) out[order[k]] = std::max(0.0, std::min({ tmp[k], MAX_RETREAT, lens[order[k]] * MOUTH_SHARE }));
}

Corner corner_fillet(const Arm &a, const Arm &b) {
	Corner none;
	double aw = a.asph_left, bw = b.asph_right;
	double dot = std::max(-1.0, std::min(1.0, a.dx * b.dx + a.dz * b.dz));
	double psi = std::acos(dot);
	if (psi > PI_D - STRAIGHT_EPS) return none;
	double step = std::fabs(aw - bw);
	if (PI_D - psi < TAPER_ANGLE) {
		if (step < TAPER_MIN_STEP) return none;
		double run = step / std::tan(TAPER_ANGLE) / 2;
		return { CornerMode::TAPER, run, run, 0, 0, 0 };
	}
	double s = 0, t = 0;
	bool hit = line_line(a.dz * aw, -a.dx * aw, a.dx, a.dz, -b.dz * bw, b.dx * bw, b.dx, b.dz, s, t);
	double max_hw = std::max({ a.left, a.right, b.left, b.right });
	if (!hit || psi < ACUTE || std::hypot(a.dz * aw + a.dx * s, -a.dx * aw + a.dz * s) > CORNER_MITER * max_hw || s < -max_hw || t < -max_hw) {
		double tr = BEVEL_PAD * (aw + bw) / (2 * std::sin(std::max(psi, STRAIGHT_EPS) / 2));
		return { CornerMode::BEVEL, tr, tr, 0, 0, 0 };
	}
	double r = std::max(KERB_RADIUS_MIN, std::min(KERB_RADIUS_MAX, FILLET_SHARE * std::min(a.left, b.right)));
	double run = r / std::tan(psi / 2);
	double xx = a.dz * aw + a.dx * s, xz = -a.dx * aw + a.dz * s;
	double bx = a.dx + b.dx, bz = a.dz + b.dz, bl = std::hypot(bx, bz);
	if (bl == 0) bl = 1;
	double k = r / std::sin(psi / 2);
	return { CornerMode::FILLET, s + run, t + run, r, xx + bx / bl * k, xz + bz / bl * k };
}

} // namespace city
