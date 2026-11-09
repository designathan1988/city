// The shapes the road jobs share (road_shapes.h): a node's ends round it by angle, each corner's kerb line (a kerb return on a
// fillet, sim/roads/junction.h corner_fillet, or a smooth curve; a transition's eased kerb lines along its cubic axis; the cap round
// the back of a node whose arms all leave on one side) and the outer line of its footway, squared to the point where two footways
// meet (junction.h corner_tip).
#include "render/roads/road_shapes.h"

#include "core/math.h"
#include "render/roads/road_mesh.h"
#include "sim/roads/junction.h"

#include <godot_cpp/variant/vector2.hpp>

#include <algorithm>
#include <cmath>

using namespace godot;

namespace city {

namespace {
using End = RoadEnd;
constexpr double PARALLEL = 1e-3, SAME = 1e-3; // two edges this near parallel do not cross; a step this short is no step
constexpr int MIN_ARC = 2;

std::vector<End> ends_of(const RoadEmit &e, int n) {
	std::vector<End> ends;
	for (int sid : e.s.roads.nodes[n].segs) {
		const RoadSegment &g = e.s.roads.segs[sid];
		End en;
		en.at_a = g.a == n;
		en.at = en.at_a ? g.retreat_a : g.retreat_b;
		SegFrame f = seg_frame(e.s, sid, en.at_a ? en.at : g.len - en.at);
		en.p = f.c;
		en.d = en.at_a ? f.fwd : -f.fwd;
		en.prof = &e.s.catalog.profile(g.profile);
		en.arm = arm_of(g.q, *en.prof, en.at_a ? 0 : 1);
		ends.push_back(en);
	}
	std::sort(ends.begin(), ends.end(), [](const End &a, const End &b) { return arm_angle(a.arm.dx, a.arm.dz) < arm_angle(b.arm.dx, b.arm.dz); });
	return ends;
}

// The kerb line from a's left side (u = ua) to b's right side (u = -ub).
std::vector<Vector3> corner(const Tuning &tn, const StreetLook &L, const Vector3 &node, const End &a, double ua, const End &b, double ub) {
	const NodeOutlineShape &K = L.shapes.road_shapes;
	std::vector<Vector3> out;
	Corner fc = corner_fillet(tn, a.arm, b.arm);
	Vector3 pa = side(a, ua), pb = side(b, -ub);
	if (fc.mode == CornerMode::FILLET && a.at >= fc.trim_a - FILLET_SLACK && b.at >= fc.trim_b - FILLET_SLACK) {
		double ccx = node.x + fc.cx, ccz = node.z + fc.cz;
		double tax = node.x + a.arm.dx * fc.trim_a + a.arm.dz * ua, taz = node.z + a.arm.dz * fc.trim_a - a.arm.dx * ua;
		double tbx = node.x + b.arm.dx * fc.trim_b - b.arm.dz * ub, tbz = node.z + b.arm.dz * fc.trim_b + b.arm.dx * ub;
		double a0 = std::atan2(taz - ccz, tax - ccx), sweep = std::atan2(tbz - ccz, tbx - ccx) - a0;
		while (sweep > PI_D) sweep -= TAU_D;
		while (sweep < -PI_D) sweep += TAU_D;
		double rho = std::max(FILLET_SLACK, std::hypot(tax - ccx, taz - ccz));
		int steps = std::max(MIN_ARC, int(std::ceil(std::fabs(sweep) / L.arc_step)));
		out.push_back(pa);
		for (int i = 0; i <= steps; i++) {
			double t = double(i) / steps, ang = a0 + sweep * t;
			out.push_back(Vector3(float(ccx + std::cos(ang) * rho), float(a.p.y + (b.p.y - a.p.y) * t), float(ccz + std::sin(ang) * rho)));
		}
		out.push_back(pb);
		return out;
	}
	double den = a.d.x * b.d.z - a.d.z * b.d.x, kx = (pa.x + pb.x) / 2, kz = (pa.z + pb.z) / 2;
	if (std::fabs(den) > PARALLEL) {
		double wx = pb.x - pa.x, wz = pb.z - pa.z;
		double sa = (wx * b.d.z - wz * b.d.x) / den, sb = (wx * a.d.z - wz * a.d.x) / den;
		if (sa < K.ctrl_max && sb < K.ctrl_max && sa > K.ctrl_min && sb > K.ctrl_min) { kx = pa.x + a.d.x * sa; kz = pa.z + a.d.z * sa; }
	}
	for (int i = 0; i <= L.bevel_steps; i++) {
		double t = double(i) / L.bevel_steps, s = 1 - t;
		out.push_back(Vector3(float(s * s * pa.x + 2 * s * t * kx + t * t * pb.x), float(a.p.y + (b.p.y - a.p.y) * t),
				float(s * s * pa.z + 2 * s * t * kz + t * t * pb.z)));
	}
	return out;
}

// A transition's line from a's left side (ua off its centre) to b's right (ub): the axis's right at the width eased from one to
// the other, the height from a's mouth to b's.
std::vector<Vector3> eased(const Tuning &tn, const End &a, double ua, const End &b, double ub) {
	const TransitionAxis axis = transition_axis(XZ{ a.p.x, a.p.z }, XZ{ a.d.x, a.d.z }, XZ{ b.p.x, b.p.z }, XZ{ b.d.x, b.d.z });
	const int count = axis.samples(tn);
	std::vector<Vector3> out;
	for (int j = 0; j <= count; j++) {
		const double u = double(j) / count, half = ua + (ub - ua) * taper_ease(u);
		const XZ p = axis.point(u) - axis.normal(u) * half;
		out.push_back(Vector3(float(p.x), a.p.y + (b.p.y - a.p.y) * float(u), float(p.z)));
	}
	return out;
}

// The kerb line round the open back of a node whose arms all leave on one side (polygon.ts nodeCap): from a's left mouth back along
// its edge to the cap, a circle of the widest carriageway round the node, round behind it, and out along b's right edge to its mouth.
std::vector<Vector3> capped(const Tuning &tn, const Vector3 &node, const End &a, const End &b, double radius) {
	const double ua = std::min(double(a.arm.asph_left), radius), ub = std::min(double(b.arm.asph_right), radius);
	const Vector3 la(float(a.arm.dz), 0, float(-a.arm.dx)), lb(float(b.arm.dz), 0, float(-b.arm.dx));
	const Vector3 da(float(a.arm.dx), 0, float(a.arm.dz)), db(float(b.arm.dx), 0, float(b.arm.dz));
	const Vector3 ea = node + da * float(std::sqrt(radius * radius - ua * ua)) + la * float(ua);
	const Vector3 eb = node + db * float(std::sqrt(radius * radius - ub * ub)) - lb * float(ub);
	const double a0 = std::atan2(ea.z - node.z, ea.x - node.x), a1 = std::atan2(eb.z - node.z, eb.x - node.x);
	double sweep = a1 - a0; // the arms' order runs against atan2's sense of z over x: round the back is a turn the other way
	while (sweep >= 0) sweep -= TAU_D;
	std::vector<Vector3> out{ side(a, ua), ea };
	const int steps = tn.junction_cap_steps;
	for (int i = 1; i < steps; i++) {
		const double ang = a0 + sweep * double(i) / steps;
		out.push_back(Vector3(float(node.x + std::cos(ang) * radius), node.y, float(node.z + std::sin(ang) * radius)));
	}
	out.push_back(eb);
	out.push_back(side(b, -ub));
	for (size_t i = 0; i < out.size(); i++) out[i].y = a.p.y + (b.p.y - a.p.y) * float(i) / float(out.size() - 1);
	return out;
}

} // namespace

// The line wa..wb metres outside the kerb line: square to the arms at the mouths, square to the line in between.
std::vector<Vector3> outside(const std::vector<Vector3> &line, const End &a, const End &b, float wa, float wb) {
	size_t n = line.size();
	float hand = 1;
	for (size_t k = 1; k < n; k++) {
		Vector3 t = line[k] - line[0];
		if (Vector2(t.x, t.z).length() > SAME) { hand = t.x * -a.d.x - t.z * a.d.z >= 0 ? 1.f : -1.f; break; }
	}
	std::vector<Vector3> out;
	for (size_t k = 0; k < n; k++) {
		Vector3 nn;
		if (k == 0) nn = Vector3(a.d.z, 0, -a.d.x);
		else if (k == n - 1) nn = Vector3(-b.d.z, 0, b.d.x);
		else {
			Vector3 t = line[k + 1] - line[k - 1];
			t.y = 0;
			t = t.length() > 0 ? t.normalized() : Vector3(1, 0, 0);
			nn = Vector3(-t.z, 0, t.x) * hand;
		}
		float w = wa + (wb - wa) * float(k) / float(std::max<size_t>(1, n - 1));
		out.push_back(line[k] + nn * w);
	}
	return out;
}

namespace {
// The lot's line round a corner whose footway's back is squared (junction.h corner_tip, the ground's outline the same): the two arms'
// edges run on until they meet, so the footway fills the corner and the lot ends in a point, however round the kerb. Gives the index
// of the point, 0 where the corner has none.
size_t sharpen(const Tuning &tn, std::vector<Vector3> &out, const End &a, const End &b) {
	const size_t n = out.size(), tip = n / 2;
	if (tip == 0) return 0;
	const Vector3 from = out.front(), to = out.back();
	XZ meet;
	if (!corner_tip(tn, XZ{ from.x, from.z }, XZ{ a.d.x, a.d.z }, XZ{ to.x, to.z }, XZ{ b.d.x, b.d.z }, meet)) return 0;
	// A mouth may stand past the point, its arm's footway overrunning it: the line goes back along that edge, over paving already there.
	Vector3 point(float(meet.x), (from.y + to.y) / 2, float(meet.z));
	for (size_t k = 0; k < n; k++) out[k] = k <= tip ? from.lerp(point, float(k) / float(tip)) : point.lerp(to, float(k - tip) / float(n - 1 - tip));
	return tip;
}

} // namespace

// Whether a node of several arms is drawn as a plate: one of its mouths stands back from it.
bool is_plate(const NodeOutlineShape &K, const std::vector<End> &ends) {
	if (ends.size() < 2) return false;
	bool any = false;
	for (const End &en : ends) any = any || en.at > K.plate_min_retreat;
	return any;
}

// The node's shape for the city's revision (road_shapes.h): its ends, and for a plate each corner's kerb and outer lines.
std::shared_ptr<const NodeShape> node_shape(const RoadEmit &e, int n) {
	return e.shapes.nodes.of(e.s.revision, n, [&]() {
		auto shape = std::make_shared<NodeShape>();
		shape->ends = ends_of(e, n);
		const std::vector<End> &ends = shape->ends;
		if (is_plate(e.L.shapes.road_shapes, ends)) {
			const Tuning &tn = e.s.catalog.tune;
			const RoadNode &nd = e.s.roads.nodes[n];
			const Vector3 node(float(nd.x), float(nd.y), float(nd.z));
			const bool transition = ends.size() == 2 && transition_pair(tn, ends[0].arm, ends[1].arm);
			double widest = 0;
			for (const End &en : ends) widest = std::max({ widest, en.arm.asph_left, en.arm.asph_right });
			for (size_t i = 0; i < ends.size(); i++) {
				const End &a = ends[i], &b = ends[(i + 1) % ends.size()];
				NodeShape::Corner c;
				if (transition) { // the kerb lines of one road carrying on: offsets of its cubic axis, the width eased (transition.ts)
					c.in = eased(tn, a, a.arm.asph_left, b, b.arm.asph_right);
					c.out = eased(tn, a, a.arm.left, b, b.arm.right);
				} else {
					c.in = capped_corner(a.arm, b.arm) ? capped(tn, node, a, b, widest) : corner(tn, e.L, node, a, a.arm.asph_left, b, b.arm.asph_right);
					c.out = outside(c.in, a, b, float(a.arm.left - a.arm.asph_left), float(b.arm.right - b.arm.asph_right));
					c.tip = sharpen(tn, c.out, a, b);
				}
				shape->corners.push_back(std::move(c));
			}
		}
		return std::shared_ptr<const NodeShape>(std::move(shape));
	}, [](const NodeShape &) { return size_t(1); });
}

} // namespace city
