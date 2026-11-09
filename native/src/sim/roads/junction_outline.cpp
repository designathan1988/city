// The outlines of a node's shape (junction.h): a transition's cubic axis and its eased width (the lab's world/junction/transition.ts),
// a turning bulb's line round its circle and kerb returns (bulb.ts), the point a squared footway corner runs to (derive.ts), and the
// ring a node's paving covers, the one the ground, the lots and the drawing all take.
#include "core/math.h"
#include "core/units.h"
#include "sim/roads/junction.h"

#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace city {

namespace {
constexpr double FLAT_TANGENT_EPS = 1e-6; // a curve's tangent this short is none: the arm's own way is taken
constexpr double THIRD = 1.0 / 3.0; // transition.ts TransitionAxis: each end's handle a third of the chord
constexpr double CUBIC_SLOPE = 3, CUBIC_BEND = 6; // the Bernstein coefficients of a cubic Bezier and of its derivative

// The short way from angle a0 to a1.
double short_sweep(double a0, double a1) {
	double s = a1 - a0;
	while (s > PI_D) s -= TAU_D;
	while (s <= -PI_D) s += TAU_D;
	return s;
}

// The arc round (cx, cz) of radius r from angle a0 (atan2 of z over x) sweeping `sweep`, in `steps` pieces, its first point left out.
void arc_into(std::vector<XZ> &out, double cx, double cz, double r, double a0, double sweep, int steps) {
	for (int i = 1; i <= steps; i++) {
		const double a = a0 + sweep * double(i) / double(steps);
		out.push_back({ cx + std::cos(a) * r, cz + std::sin(a) * r });
	}
}
} // namespace

bool bulb_line(const Tuning &tn, double half_left, double half_right, double grow, int round_steps, int return_steps, BulbLine &out) {
	out.pts.clear();
	const double r = tn.junction_bulb_radius_m + grow, rf = std::max(tn.junction_bulb_return_min_m, tn.junction_bulb_return_m - grow);
	if (r <= std::max(half_left, half_right) + tn.junction_bulb_clear_m) return false;
	// Each return's centre a return's radius off the road's edge and off the circle: where it leaves the edge, along the arm.
	const auto along = [&](double hw) { return std::sqrt(std::max(0.0, (r + rf) * (r + rf) - (hw + rf) * (hw + rf))); };
	const double xl = along(half_left), xr = along(half_right), d = std::max(xl, xr);
	out.mouth = d;
	const XZ fl{ xl, half_left + rf }, fr{ xr, -(half_right + rf) };
	const double k = r / (r + rf);
	const XZ tl{ fl.x * k, fl.z * k }, tr{ fr.x * k, fr.z * k };
	out.pts.push_back({ d, half_left });
	out.pts.push_back({ xl, half_left });
	const double el = std::atan2(half_left - fl.z, xl - fl.x), etl = std::atan2(tl.z - fl.z, tl.x - fl.x);
	arc_into(out.pts, fl.x, fl.z, rf, el, short_sweep(el, etl), return_steps);
	// the long way round the circle, through the side away from the road (behind the node): from the left (+z) round to the right
	const double a0 = std::atan2(tl.z, tl.x), a1 = std::atan2(tr.z, tr.x);
	double sweep = a1 - a0;
	while (sweep <= 0) sweep += TAU_D;
	arc_into(out.pts, 0, 0, r, a0, sweep, round_steps);
	const double etr = std::atan2(tr.z - fr.z, tr.x - fr.x), er = std::atan2(-half_right - fr.z, xr - fr.x);
	arc_into(out.pts, fr.x, fr.z, rf, etr, short_sweep(etr, er), return_steps);
	out.pts.push_back({ d, -half_right });
	return true;
}

double bulb_reach(const Tuning &tn) { return tn.junction_bulb_radius_m + 2 * tn.junction_bulb_return_m; }

double bulb_round(const Tuning &tn, const Arm &arm) {
	return tn.junction_bulb_radius_m + std::max(arm.left - arm.asph_left, arm.right - arm.asph_right);
}

XZ TransitionAxis::point(double u) const {
	const double v = 1 - u, a = v * v * v, b = CUBIC_SLOPE * v * v * u, c = CUBIC_SLOPE * v * u * u, d = u * u * u;
	return { a * p0.x + b * p1.x + c * p2.x + d * p3.x, a * p0.z + b * p1.z + c * p2.z + d * p3.z };
}

XZ TransitionAxis::normal(double u) const {
	const double v = 1 - u;
	const double tx = CUBIC_SLOPE * v * v * (p1.x - p0.x) + CUBIC_BEND * v * u * (p2.x - p1.x) + CUBIC_SLOPE * u * u * (p3.x - p2.x);
	const double tz = CUBIC_SLOPE * v * v * (p1.z - p0.z) + CUBIC_BEND * v * u * (p2.z - p1.z) + CUBIC_SLOPE * u * u * (p3.z - p2.z);
	double l = std::hypot(tx, tz);
	if (l == 0) l = 1;
	return { tz / l, -tx / l }; // left of the way (geometry.h: left of d is (d.z, -d.x))
}

int TransitionAxis::samples(const Tuning &tn) const {
	return std::max(tn.junction_transition_samples_min, int(std::ceil(length / tn.junction_transition_step_m)));
}

TransitionAxis transition_axis(const XZ &ma, const XZ &da, const XZ &mb, const XZ &db) {
	TransitionAxis t;
	t.p0 = ma;
	t.p3 = mb;
	t.length = std::hypot(mb.x - ma.x, mb.z - ma.z);
	t.p1 = ma - da * (t.length * THIRD); // the handles point back into the node: each arm's direction is outward
	t.p2 = mb - db * (t.length * THIRD);
	return t;
}

double taper_ease(double u) {
	const double t = std::max(0.0, std::min(1.0, u));
	return t * t * (3 - 2 * t);
}

bool corner_tip(const Tuning &tn, const XZ &pa, const XZ &da, const XZ &pb, const XZ &db, XZ &tip) {
	const double psi = std::acos(std::max(-1.0, std::min(1.0, da.dot(db)))), least = DEG_TO_RAD_D * tn.junction_square_min_deg;
	if (psi < least || psi > PI_D - least) return false;
	double s = 0, t = 0;
	if (!line_line(pa.x, pa.z, da.x, da.z, pb.x, pb.z, db.x, db.z, s, t)) return false;
	if (std::fabs(s) > tn.junction_square_reach_m || std::fabs(t) > tn.junction_square_reach_m) return false;
	tip = pa + da * s;
	return true;
}

std::vector<double> node_outline(const RoadNetwork &net, int n) {
	const RoadNode &nd = net.nodes[size_t(n)];
	const Tuning &tn = net.cat->tune;
	std::vector<double> ring;
	const size_t k = nd.segs.size();
	const XZ node{ nd.x, nd.z };
	const auto put = [&](const XZ &p) { ring.insert(ring.end(), { p.x, p.z }); };
	std::vector<Arm> arms;
	std::vector<XZ> mouth, way; // each arm's mouth on its own curve and the way out of the node there (the strip ends square to it)
	for (int s : nd.segs) {
		const RoadSegment &g = net.segs[size_t(s)];
		const bool at_a = g.a == n;
		arms.push_back(arm_of(g.q, net.profile_of(s), at_a ? 0 : 1));
		const double t = t_at_length(g.table, at_a ? g.retreat_a : g.len - g.retreat_b);
		double x, z, dx, dz;
		g.q.at(t, x, z);
		g.q.deriv(t, dx, dz);
		double l = std::hypot(dx, dz);
		if (l < FLAT_TANGENT_EPS) {
			dx = at_a ? arms.back().dx : -arms.back().dx;
			dz = at_a ? arms.back().dz : -arms.back().dz;
			l = 1;
		}
		mouth.push_back({ x, z });
		way.push_back(at_a ? XZ{ dx / l, dz / l } : XZ{ -dx / l, -dz / l });
	}
	if (k == 1) {
		if (!nd.bulb) return ring;
		const Arm &a = arms[0];
		BulbLine line;
		const double grow = std::max(a.left - a.asph_left, a.right - a.asph_right);
		if (!bulb_line(tn, a.left, a.right, grow, tn.junction_cap_steps, tn.junction_cap_steps / 2, line)) return ring;
		const XZ d{ a.dx, a.dz }, left{ a.dz, -a.dx };
		for (const XZ &p : line.pts) put(node + d * p.x + left * p.z);
		return ring;
	}
	if (k == 2 && nd.joint) return ring;
	std::vector<int> order(k);
	std::iota(order.begin(), order.end(), 0);
	std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
		return arm_angle(arms[size_t(a)].dx, arms[size_t(a)].dz) < arm_angle(arms[size_t(b)].dx, arms[size_t(b)].dz);
	});
	double widest = 0;
	for (const Arm &a : arms) widest = std::max({ widest, a.left, a.right });
	const bool transition = k == 2 && transition_pair(tn, arms[size_t(order[0])], arms[size_t(order[1])]);
	for (size_t i = 0; i < k; i++) {
		const size_t ia = size_t(order[i]), ib = size_t(order[(i + 1) % k]);
		const Arm &a = arms[ia], &b = arms[ib];
		const XZ da = way[ia], la{ da.z, -da.x }, db = way[ib], lb{ db.z, -db.x }, ma = mouth[ia], mb = mouth[ib];
		put(ma - la * a.right); // the mouth across a, right to left
		put(ma + la * a.left);
		if (transition) { // the outer kerb line from a's left to b's right: the axis's right at the eased width (transition.ts)
			const TransitionAxis axis = transition_axis(ma, da, mb, db);
			const int count = axis.samples(tn);
			for (int j = 1; j < count; j++) {
				const double u = double(j) / count, half = a.left + (b.right - a.left) * taper_ease(u);
				put(axis.point(u) - axis.normal(u) * half);
			}
		} else if (XZ tip; corner_tip(tn, ma + la * a.left, da, mb - lb * b.right, db, tip)) {
			// a squared back: the footway fills the corner to the point where its two edges meet (render/roads/road_shapes.cpp
			// sharpens the drawn footway to the same point), round a kerb return or a node's cap alike (derive.ts)
			put(tip);
		} else if (capped_corner(a, b)) { // the cap: back along a's edge to the circle, round behind the node, out along b's
			const double r = widest, ua = std::min(a.left, r), ub = std::min(b.right, r);
			const XZ na{ a.dx, a.dz }, nla{ a.dz, -a.dx }, nb{ b.dx, b.dz }, nlb{ b.dz, -b.dx }; // the arms' own frames at the node
			const XZ ea = node + na * std::sqrt(r * r - ua * ua) + nla * ua, eb = node + nb * std::sqrt(r * r - ub * ub) - nlb * ub;
			put(ea);
			const double a0 = std::atan2(ea.z - node.z, ea.x - node.x), a1 = std::atan2(eb.z - node.z, eb.x - node.x);
			double sweep = a1 - a0; // arm_angle runs against atan2's sense of z over x: the gap from a to b is a turn the other way
			while (sweep >= 0) sweep -= TAU_D;
			std::vector<XZ> arc;
			arc_into(arc, node.x, node.z, r, a0, sweep, tn.junction_cap_steps);
			for (const XZ &p : arc) put(p);
		} // else a rounded back, drawn round the kerb: its outline here is the chord between the two mouths
	}
	return ring;
}

} // namespace city
