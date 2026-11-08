// The shapes the road jobs share (road_shapes.h): a node's ends round it by angle, each corner's kerb line (a kerb return on a
// fillet, sim/roads/junction.h corner_fillet, or a smooth curve) and the outer line of its footway, sharpened to the point where two
// footways meet.
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
constexpr double PLATE_MIN_RETREAT = 0.05, CTRL_MAX = 0.5, CTRL_MIN = -60;
constexpr double PARALLEL = 1e-3, SAME = 1e-3;
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
std::vector<Vector3> corner(const StreetLook &L, const Vector3 &node, const End &a, double ua, const End &b, double ub) {
	std::vector<Vector3> out;
	Corner fc = corner_fillet(a.arm, b.arm);
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
		if (sa < CTRL_MAX && sb < CTRL_MAX && sa > CTRL_MIN && sb > CTRL_MIN) { kx = pa.x + a.d.x * sa; kz = pa.z + a.d.z * sa; }
	}
	for (int i = 0; i <= L.bevel_steps; i++) {
		double t = double(i) / L.bevel_steps, s = 1 - t;
		out.push_back(Vector3(float(s * s * pa.x + 2 * s * t * kx + t * t * pb.x), float(a.p.y + (b.p.y - a.p.y) * t),
				float(s * s * pa.z + 2 * s * t * kz + t * t * pb.z)));
	}
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
// The lot's line round a kerb return: the two arms' edges run on until they meet, so the footway fills the corner and the lot ends
// in a point, however round the kerb. Gives the index of the point, 0 where the corner has none.
size_t sharpen(std::vector<Vector3> &out, const End &a, const End &b) {
	const size_t n = out.size(), tip = n / 2;
	const double den = a.d.x * b.d.z - a.d.z * b.d.x;
	if (corner_fillet(a.arm, b.arm).mode != CornerMode::FILLET || std::fabs(den) < PARALLEL || tip == 0) return 0;
	const Vector3 from = out.front(), to = out.back(), w = to - from;
	// A mouth may stand past the point, its arm's footway overrunning it: the line goes back along that edge, over paving already there.
	Vector3 point = from + a.d * float((w.x * b.d.z - w.z * b.d.x) / den);
	point.y = (from.y + to.y) / 2;
	for (size_t k = 0; k < n; k++) out[k] = k <= tip ? from.lerp(point, float(k) / float(tip)) : point.lerp(to, float(k - tip) / float(n - 1 - tip));
	return tip;
}

} // namespace

// Whether a node of several arms is drawn as a plate: one of its mouths stands back from it.
bool is_plate(const std::vector<End> &ends) {
	if (ends.size() < 2) return false;
	bool any = false;
	for (const End &en : ends) any = any || en.at > PLATE_MIN_RETREAT;
	return any;
}

// The node's shape for the city's revision (road_shapes.h): its ends, and for a plate each corner's kerb and outer lines.
std::shared_ptr<const NodeShape> node_shape(const RoadEmit &e, int n) {
	return e.shapes.nodes.of(e.s.revision, n, [&]() {
		auto shape = std::make_shared<NodeShape>();
		shape->ends = ends_of(e, n);
		const std::vector<End> &ends = shape->ends;
		if (is_plate(ends)) {
			const RoadNode &nd = e.s.roads.nodes[n];
			const Vector3 node(float(nd.x), float(nd.y), float(nd.z));
			for (size_t i = 0; i < ends.size(); i++) {
				const End &a = ends[i], &b = ends[(i + 1) % ends.size()];
				NodeShape::Corner c;
				c.in = corner(e.L, node, a, a.arm.asph_left, b, b.arm.asph_right);
				c.out = outside(c.in, a, b, float(a.arm.left - a.arm.asph_left), float(b.arm.right - b.arm.asph_right));
				c.tip = sharpen(c.out, a, b);
				shape->corners.push_back(std::move(c));
			}
		}
		return std::shared_ptr<const NodeShape>(std::move(shape));
	}, [](const NodeShape &) { return size_t(1); });
}

} // namespace city
