// Reading a point against the segments near it, see footway.h.
#include "sim/street/footway.h"

#include "sim/roads/centre_line.h"
#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>
#include <limits>

namespace city {

namespace {
constexpr int ARMS_WITH_CROSSWALK = 3;
constexpr float BESIDE_M = 0.05f; // the closest-point search's own error: under this a point is beside the segment

bool strip_at(const Profile &p, float u, StripKind kind, float &centre) {
	for (const StripEdge &e : p.edges)
		if (e.strip.kind == kind && u >= e.u0 && u <= e.u1) {
			centre = (e.u0 + e.u1) * 0.5f;
			return true;
		}
	return false;
}

// Whether (x, z) lies along a segment at an offset `band` accepts.
// The farthest a point beside a segment (`past` under BESIDE_M) and on its profile may stand from the centre line.
double band_reach(const RoadNetwork &net) { return std::hypot(double(net.cat->widest_half()), double(BESIDE_M)); }

template <typename F>
bool on_band(const RoadNetwork &net, double x, double z, F &&band) {
	bool on = false;
	const double r = band_reach(net);
	net.for_segments_near(x - r, z - r, x + r, z + r, [&](int sid) {
		const Profile &p = net.profile_of(sid);
		if (on || p.edges.empty()) return;
		double v;
		float u, past;
		segment_place(net, sid, x, z, v, u, past);
		on = past < BESIDE_M && band(p, u);
	});
	return on;
}

// Whether (px, pz), from a node, stands in the footway block of corner `c` between arm a and the next arm anticlockwise b:
// beyond a's left kerb and b's right kerb, short of both mouths, and not on the asphalt the kerb return (a fillet) or the cut
// (a bevel) gives the junction there.
// Beyond an arm's left kerb or its right kerb, from the node: the left of (dx, dz) is (dz, -dx), the kerbs' lines as
// corner_fillet takes them.
bool beyond_left(const Arm &a, double px, double pz) { return px * a.dz - pz * a.dx > a.asph_left; }
bool beyond_right(const Arm &b, double px, double pz) { return -px * b.dz + pz * b.dx > b.asph_right; }
// Beyond a's left kerb and b's right kerb: the footway of the corner between them, where that corner is under half a turn.
bool beyond_kerbs(const Arm &a, const Arm &b, double px, double pz) { return beyond_left(a, px, pz) && beyond_right(b, px, pz); }

bool in_corner_block(const Arm &a, const Arm &b, const Corner &c, double mouth_a, double mouth_b, double px, double pz) {
	if (c.mode != CornerMode::FILLET && c.mode != CornerMode::BEVEL) return false;
	const double along_a = px * a.dx + pz * a.dz, along_b = px * b.dx + pz * b.dz;
	if (!beyond_kerbs(a, b, px, pz) || along_a > mouth_a || along_b > mouth_b) return false;
	if (c.mode == CornerMode::FILLET) return along_a >= c.trim_a || along_b >= c.trim_b || std::hypot(px - c.cx, pz - c.cz) <= c.r;
	// a bevel: the straight cut from a's kerb at its trim to b's kerb at its trim; the node's side of it is asphalt
	const double ax = a.dx * c.trim_a + a.dz * a.asph_left, az = a.dz * c.trim_a - a.dx * a.asph_left;
	const double bx = b.dx * c.trim_b - b.dz * b.asph_right, bz = b.dz * c.trim_b + b.dx * b.asph_right;
	const auto side = [&](double x, double z) { return (bx - ax) * (z - az) - (bz - az) * (x - ax); };
	return side(px, pz) * side(0, 0) < 0;
}
} // namespace

void segment_frame(const RoadNetwork &net, int seg, double v, double &fx, double &fz, double &lx, double &lz) {
	double x, z;
	centre_at(net, seg, v, x, z, fx, fz);
	lx = fz;
	lz = -fx;
}

void footway_point(const RoadNetwork &net, int seg, double v, float u, double &x, double &z, double &y) {
	double fx, fz;
	centre_at(net, seg, v, x, z, fx, fz);
	x += fz * u; // the left of the tangent (fx, fz) is (fz, -fx)
	z -= fx * u;
	y = net.seg_y(seg, v);
}

void segment_place(const RoadNetwork &net, int seg, double x, double z, double &v, float &u, float &past) {
	const RoadSegment &g = net.segs[size_t(seg)];
	double t, d, cx, cz, fx, fz, lx, lz;
	closest_t(g.q, x, z, t, d);
	v = arc_at(g.table, t); // the one conversion from the curve's parameter to its length (sim/roads/geometry.h)
	g.q.at(t, cx, cz);
	segment_frame(net, seg, v, fx, fz, lx, lz);
	u = float((x - cx) * lx + (z - cz) * lz);
	past = float(std::fabs((x - cx) * fx + (z - cz) * fz));
}

double footway_reach(const RoadNetwork &net, double reach) { return std::hypot(double(net.cat->widest_half()) + reach, reach); }

FootwayHit footway_at(const RoadNetwork &net, double x, double z, double reach) {
	FootwayHit best;
	best.away = std::numeric_limits<float>::max();
	const double r = footway_reach(net, reach);
	net.for_segments_near(x - r, z - r, x + r, z + r, [&](int sid) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (p.edges.empty() || g.a == g.b) return;
		double v;
		float u, past;
		segment_place(net, sid, x, z, v, u, past);
		if (past > reach || v < g.retreat_a - reach || v > g.len - g.retreat_b + reach) return;
		for (int side = 0; side < 2; side++) {
			const StripEdge &e = side == 0 ? p.edges.front() : p.edges.back();
			if (e.strip.kind != StripKind::SIDEWALK) continue;
			const float away = std::max({ e.u0 - u, u - e.u1, past });
			if (away > reach || away >= best.away) continue;
			double fx, fz, lx, lz;
			segment_frame(net, sid, v, fx, fz, lx, lz);
			const float toward = side == 0 ? 1.f : -1.f; // +u runs from the right sidewalk to the road
			best.ok = true;
			best.seg = sid;
			best.side = side;
			best.v = std::clamp(v, g.retreat_a, g.len - g.retreat_b);
			best.u = std::clamp(u, e.u0, e.u1);
			best.away = away;
			best.kx = float(lx) * toward;
			best.kz = float(lz) * toward;
		}
	});
	return best;
}

FootwayHit median_at(const RoadNetwork &net, double x, double z, double reach) {
	FootwayHit hit;
	const double r = band_reach(net);
	net.for_segments_near(x - r, z - r, x + r, z + r, [&](int sid) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		double v;
		float u, centre, past;
		segment_place(net, sid, x, z, v, u, past);
		if (hit.ok || past > BESIDE_M || v < g.retreat_a || v > g.len - g.retreat_b || !strip_at(p, u, StripKind::MEDIAN, centre)) return;
		if (std::fabs(u - centre) > reach) return;
		hit.ok = true;
		hit.seg = sid;
		hit.v = v;
		hit.u = centre;
	});
	return hit;
}

bool on_road(const RoadNetwork &net, double x, double z) {
	return on_band(net, x, z, [](const Profile &p, float u) { return u >= p.edges.front().u0 && u <= p.edges.back().u1; });
}

bool on_carriageway(const RoadNetwork &net, double x, double z, float keep) {
	return on_band(net, x, z, [keep](const Profile &p, float u) {
		const StripEdge &r = p.edges.front(), &l = p.edges.back();
		const float lo = r.strip.kind == StripKind::SIDEWALK ? r.u1 : r.u0, hi = l.strip.kind == StripKind::SIDEWALK ? l.u0 : l.u1;
		return u > lo - keep && u < hi + keep;
	});
}

bool on_footway(const RoadNetwork &net, double x, double z) {
	bool on = false;
	const double beside = band_reach(net);
	// beside a segment between its mouths, across one of its sidewalks
	net.for_segments_near(x - beside, z - beside, x + beside, z + beside, [&](int sid) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const Profile &p = net.profile_of(sid);
		if (on || p.edges.empty() || g.a == g.b) return;
		double v;
		float u, past;
		segment_place(net, sid, x, z, v, u, past);
		if (past >= BESIDE_M || v < g.retreat_a || v > g.len - g.retreat_b) return;
		for (const StripEdge *e : { &p.edges.front(), &p.edges.back() })
			on = on || (e->strip.kind == StripKind::SIDEWALK && u >= e->u0 && u <= e->u1);
	});
	if (on) return true;
	const double r = MAX_RETREAT + double(net.cat->widest_half());
	net.node_hash.query(float(x - r), float(z - r), float(x + r), float(z + r), [&](int n) {
		if (!on && net.nodes[size_t(n)].alive && net.nodes[size_t(n)].segs.size() >= 2) on = JunctionKerbs(net, n).on_footway(x, z);
	});
	return on;
}

JunctionKerbs::JunctionKerbs(const RoadNetwork &net, int node) {
	const RoadNode &nd = net.nodes[size_t(node)];
	x = nd.x;
	z = nd.z;
	struct Leg {
		Arm arm;
		double mouth;
	};
	std::vector<Leg> legs;
	for (int sid : nd.segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const int end = g.a == node ? 0 : 1;
		legs.push_back({ arm_of(g.q, net.profile_of(sid), end), end == 0 ? g.retreat_a : g.retreat_b });
	}
	std::sort(legs.begin(), legs.end(), [](const Leg &a, const Leg &b) { return arm_angle(a.arm.dx, a.arm.dz) < arm_angle(b.arm.dx, b.arm.dz); });
	for (const Leg &l : legs) {
		arms.push_back(l.arm);
		mouths.push_back(l.mouth);
	}
	for (size_t k = 0; k < arms.size(); k++) corners.push_back(corner_fillet(arms[k], arms[(k + 1) % arms.size()]));
}

bool JunctionKerbs::on_footway(double px, double pz) const {
	for (size_t k = 0; arms.size() >= 2 && k < arms.size(); k++) {
		const size_t j = (k + 1) % arms.size();
		if (in_corner_block(arms[k], arms[j], corners[k], mouths[k], mouths[j], px - x, pz - z)) return true;
	}
	return false;
}

bool JunctionKerbs::kerb_return(double px, double pz, double qx, double qz, double &cx, double &cz) const {
	for (size_t k = 0; arms.size() >= 2 && k < arms.size(); k++) {
		const size_t j = (k + 1) % arms.size();
		// one point on the footway of a's side facing the corner, the other on b's: round a corner wider than half a turn (the
		// outside of a bend) the footway lies beyond either kerb, never both
		const Arm &a = arms[k], &b = arms[j];
		const bool p_a = beyond_left(a, px - x, pz - z), p_b = beyond_right(b, px - x, pz - z);
		const bool q_a = beyond_left(a, qx - x, qz - z), q_b = beyond_right(b, qx - x, qz - z);
		if (!fillet_drawn(corners[k], mouths[k], mouths[j]) || !((p_a && q_b) || (q_a && p_b))) continue;
		cx = x + corners[k].cx;
		cz = z + corners[k].cz;
		return true;
	}
	return false;
}

bool on_landing(const RoadNetwork &net, const FootwayHit &hit, float radius, float clear) {
	if (!hit.ok || hit.side < 0) return false;
	const RoadSegment &g = net.segs[size_t(hit.seg)];
	const CrossingDef &c = net.cat->furniture.crossing;
	const float pad = radius + clear;
	const bool at_a = int(net.nodes[size_t(g.a)].segs.size()) >= ARMS_WITH_CROSSWALK, at_b = int(net.nodes[size_t(g.b)].segs.size()) >= ARMS_WITH_CROSSWALK;
	const double from_a = hit.v - g.retreat_a, from_b = g.len - g.retreat_b - hit.v;
	return (at_a && from_a > c.zebra_from_m - pad && from_a < c.zebra_to_m + pad) || (at_b && from_b > c.zebra_from_m - pad && from_b < c.zebra_to_m + pad);
}

} // namespace city
