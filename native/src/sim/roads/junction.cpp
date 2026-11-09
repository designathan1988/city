// A node's shape (junction.h), from the lab's world/junction/: legs.ts (arms), corners.ts (kerb returns), trim.ts (setbacks and
// a road's two mouths against its length), polygon.ts (the slab check, the node cap), transition.ts (one road carrying on), bulb.ts
// (the turning bulb's setback); the outlines are junction_outline.cpp's.
#include "core/math.h"
#include "core/units.h"
#include "sim/roads/junction.h"

#include "sim/roads/road_network.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace city {

namespace {
constexpr double STRAIGHT_EPS = 1e-3; // an angle this near a half turn is straight on, and the least the bevel divides by
constexpr double DEGENERATE_ARM_EPS = 1e-6; // an arm's direction this short is none
constexpr double THROUGH_BEND_EPS = 1e-5; // build.ts throughBend: a bend over this, radians, is one at all
constexpr double CAP_GAP_EPS = 1e-6; // polygon.ts nodeCap: a hair over a straight back, so a T is left as it is

// The largest change of half width from one arm into the other, side by side, carriageway and casing (transition.ts legWidthStep).
double width_step(const Arm &a, const Arm &b) {
	return std::max({ std::fabs(a.left - b.right), std::fabs(a.right - b.left), std::fabs(a.asph_left - b.asph_right),
			std::fabs(a.asph_right - b.asph_left) });
}

// How far the road turns going from arm a into arm b: 0 straight on.
double turn_of(const Arm &a, const Arm &b) { return PI_D - std::acos(std::max(-1.0, std::min(1.0, a.dx * b.dx + a.dz * b.dz))); }

// How far each arm of a transition is given over to it (build.ts): the larger of the taper's run (transition.ts transitionRunFor)
// and the bend's run (a radius of twice the carriageway), or a structure seam's.
double transition_run(const Tuning &tn, const Arm &a, const Arm &b, bool seam) {
	const double carriageway = std::max({ a.asph_left, a.asph_right, b.asph_left, b.asph_right });
	const double radius = carriageway * 2, turn = turn_of(a, b);
	const double bend = std::min(radius * tn.junction_transition_radius_cap, radius * std::tan(std::min(turn, DEG_TO_RAD_D * tn.junction_transition_turn_max_deg) / 2));
	const double step = width_step(a, b);
	const double taper = step >= tn.junction_taper_min_step ? std::max(tn.junction_taper_min, step * tn.junction_taper_slope) / 2 : 0;
	return std::max({ taper, bend, seam ? tn.junction_seam_run_m : 0.0 });
}

// Whether point (px, pz), relative to the node, lies in the carriageway of an arm ahead of the node (intersect.ts inForwardSlab).
bool in_forward_slab(const Tuning &tn, double px, double pz, const Arm &arm) {
	const double along = px * arm.dx + pz * arm.dz;
	if (along <= tn.junction_slab_margin_m) return false;
	return std::fabs(arm.dx * pz - arm.dz * px) < std::max(arm.asph_left, arm.asph_right) - tn.junction_slab_margin_m;
}

// The arms (sorted) whose mouth corners land in the carriageway of an arm not next to them (polygon.ts findSlabViolations).
void slab_violations(const Tuning &tn, const std::vector<Arm> &arms, const std::vector<double> &trims, std::vector<char> &bad) {
	const size_t n = arms.size();
	bad.assign(n, 0);
	if (n < 3) return;
	for (size_t i = 0; i < n; i++) {
		const Arm &a = arms[i];
		const double t = trims[i];
		const double cx = a.dx * t, cz = a.dz * t; // the mouth's corners: left of the arm is (dz, -dx)
		const double pts[2][2] = { { cx + a.dz * a.asph_left, cz - a.dx * a.asph_left }, { cx - a.dz * a.asph_right, cz + a.dx * a.asph_right } };
		for (size_t j = 0; j < n && !bad[i]; j++) {
			if (j == i || j == (i + 1) % n || (j + 1) % n == i) continue;
			for (const auto &q : pts)
				if (in_forward_slab(tn, q[0], q[1], arms[j])) bad[i] = 1;
		}
	}
}

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

double bulb_retreat(const Tuning &tn, const Arm &arm, double len) {
	const double half = std::max(arm.asph_left, arm.asph_right);
	if (half > tn.junction_bulb_half_max || len < tn.junction_bulb_len_min) return 0;
	BulbLine line;
	if (!bulb_line(tn, arm.asph_left, arm.asph_right, 0, 1, 1, line)) return 0;
	// A road shorter than the circle and its returns has no end to round off.
	return len < line.mouth + tn.junction_bulb_tail_m ? 0 : line.mouth;
}

double arm_gap(const Arm &a, const Arm &b) {
	double g = arm_angle(b.dx, b.dz) - arm_angle(a.dx, a.dz);
	while (g <= 0) g += TAU_D;
	return g;
}

bool capped_corner(const Arm &a, const Arm &b) { return arm_gap(a, b) > PI_D + CAP_GAP_EPS; }

bool transition_pair(const Tuning &tn, const Arm &a, const Arm &b) {
	const double turn = turn_of(a, b);
	return turn < DEG_TO_RAD_D * tn.junction_transition_bend_deg && (turn > THROUGH_BEND_EPS || width_step(a, b) >= tn.junction_taper_min_step);
}

void junction_retreats(const Tuning &tn, const std::vector<Arm> &arms, const std::vector<double> &lens, std::vector<double> &out,
		std::vector<int> &order, bool bulb, bool seam) {
	size_t n = arms.size();
	out.assign(n, 0.0);
	order.resize(n);
	std::iota(order.begin(), order.end(), 0);
	if (n < 2) {
		if (n == 1) out[0] = bulb ? bulb_retreat(tn, arms[0], lens[0]) : 0;
		return;
	}
	std::stable_sort(order.begin(), order.end(), [&](int a, int b) {
		return arm_angle(arms[a].dx, arms[a].dz) < arm_angle(arms[b].dx, arms[b].dz);
	});
	std::vector<Arm> sorted;
	sorted.reserve(n);
	for (int k : order) sorted.push_back(arms[size_t(k)]);
	std::vector<double> tmp(n, 0.0);
	if (n == 2 && (transition_pair(tn, sorted[0], sorted[1]) || seam)) {
		tmp[0] = tmp[1] = transition_run(tn, sorted[0], sorted[1], seam);
	} else {
		for (size_t k = 0; k < n; k++) {
			Corner f = corner_fillet(tn, sorted[k], sorted[(k + 1) % n]);
			tmp[k] = std::max(tmp[k], f.trim_a);
			tmp[(k + 1) % n] = std::max(tmp[(k + 1) % n], f.trim_b);
		}
		// A mouth whose corner lands in the carriageway of an arm not next to it grows, never shrinks (build.ts SLAB_BUMP_CAP).
		double widest = 0;
		for (const Arm &a : sorted) widest = std::max({ widest, a.asph_left, a.asph_right });
		const double cap = tn.junction_slab_bump_cap * widest;
		std::vector<char> bad;
		for (int attempt = 0; attempt < tn.junction_slab_attempts; attempt++) {
			slab_violations(tn, sorted, tmp, bad);
			bool changed = false;
			for (size_t k = 0; k < n; k++) {
				if (!bad[k]) continue;
				const double bumped = std::min({ tmp[k] * tn.junction_slab_bump_scale + tn.junction_slab_bump_add_m, cap, tn.junction_max_retreat });
				if (bumped > tmp[k]) {
					tmp[k] = bumped;
					changed = true;
				}
			}
			if (!changed) break; // a cap can make a violation unavoidable
		}
	}
	for (size_t k = 0; k < n; k++) out[size_t(order[k])] = std::max(0.0, std::min(tmp[k], tn.junction_max_retreat));
}

EndTrims clamp_segment_trims(const Tuning &tn, double want_a, double want_b, double len) {
	const double reserve = std::min(tn.junction_drivable_reserve_m, len * tn.junction_reserve_cap);
	const double available = len - std::max(tn.junction_least_ribbon_m, reserve), total = want_a + want_b;
	if (available <= 0) { // a pathologically short road: split what little there is
		const double half = std::max(0.0, (len - tn.junction_least_ribbon_m) / 2);
		return { std::min(want_a, half), std::min(want_b, half) };
	}
	if (total <= available) return { want_a, want_b };
	const double k = available / total;
	return { want_a * k, want_b * k };
}

Corner corner_fillet(const Tuning &tn, const Arm &a, const Arm &b) {
	const double taper_angle = DEG_TO_RAD_D * tn.junction_taper_angle_deg, acute = DEG_TO_RAD_D * tn.junction_acute_deg;
	Corner none;
	double aw = a.asph_left, bw = b.asph_right;
	double dot = std::max(-1.0, std::min(1.0, a.dx * b.dx + a.dz * b.dz));
	double psi = std::acos(dot);
	if (psi > PI_D - STRAIGHT_EPS) return none;
	double step = std::fabs(aw - bw);
	if (PI_D - psi < taper_angle) {
		if (step < tn.junction_taper_min_step) return none;
		double run = step / std::tan(taper_angle) / 2;
		return { CornerMode::TAPER, run, run, 0, 0, 0 };
	}
	double s = 0, t = 0;
	bool hit = line_line(a.dz * aw, -a.dx * aw, a.dx, a.dz, -b.dz * bw, b.dx * bw, b.dx, b.dz, s, t);
	const double xx = a.dz * aw + a.dx * s, xz = -a.dx * aw + a.dz * s;
	const double scaled = std::max(tn.junction_kerb_radius_min, std::min(KERB_RADIUS_MAX, tn.junction_fillet_share * std::min(a.left, b.right)));
	// A kerb return of radius r on the corner: the arc's centre on the bisector, each leg cut where the arc leaves it.
	const auto fillet = [&](double r) {
		const double run = r / std::tan(psi / 2), k = r / std::sin(psi / 2);
		double bx = a.dx + b.dx, bz = a.dz + b.dz, bl = std::hypot(bx, bz);
		if (bl == 0) bl = 1;
		return Corner{ CornerMode::FILLET, s + run, t + run, r, xx + bx / bl * k, xz + bz / bl * k };
	};
	const double setback = tn.junction_bevel_pad * (aw + bw) / (2 * std::sin(std::max(psi, STRAIGHT_EPS) / 2));
	if (!hit) return { CornerMode::BEVEL, setback, setback, 0, 0, 0 };
	if (psi >= acute && std::hypot(xx, xz) <= tn.junction_corner_miter * std::max(aw, bw)) return fillet(scaled);
	// A wedge too long to round at its own radius: rounded at the one the setback asks for, when that is a kerb's (corners.ts), else cut straight.
	const double need = (setback - std::min(s, t)) * std::tan(psi / 2);
	if (psi >= acute && need <= KERB_RADIUS_MAX) return fillet(std::max(scaled, need));
	return { CornerMode::BEVEL, setback, setback, 0, 0, 0 };
}

float node_gap(const RoadNetwork &net, int n) {
	const RoadNode &node = net.nodes[size_t(n)];
	if (!node.alive || node.segs.size() < 2) return TAU_F;
	std::vector<float> legs;
	legs.reserve(node.segs.size());
	for (int sg : node.segs) { // each leg leaves towards its curve's control point
		const RoadSegment &g = net.segs[size_t(sg)];
		legs.push_back(float(std::atan2(g.q.cz - node.z, g.q.cx - node.x)));
	}
	std::sort(legs.begin(), legs.end());
	float gap = legs.front() + TAU_F - legs.back();
	for (size_t i = 1; i < legs.size(); i++) gap = std::min(gap, legs[i] - legs[i - 1]);
	return gap;
}

std::vector<SharpNode> nodes_too_sharp(const RoadNetwork &net, float min_gap) {
	std::vector<SharpNode> out;
	for (int n = 0; n < int(net.nodes.size()); n++) {
		const float gap = node_gap(net, n);
		if (gap < min_gap) out.push_back({ n, gap });
	}
	return out;
}

} // namespace city
