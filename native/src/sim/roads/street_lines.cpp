// The streets' lines as one network (street_lines.h): a line's face, a junction's corner poles, how a segment's line meets each
// of its nodes and the spans each node strings.
#include "sim/roads/street_lines.h"

#include "sim/roads/centre_line.h"
#include "sim/roads/junction.h"
#include "sim/roads/node_outline.h"
#include "sim/roads/street_furniture_internal.h"

#include <algorithm>
#include <cmath>

namespace city {

using namespace furniture;

namespace {

constexpr float HALF_SHARE = 0.5f; // the middle of a share, and half of a footprint

// The arm's left looking out from its node is the segment's left (face 1) at its end a, its right (face 0) at its end b.
int arm_left_face(bool at_a) { return at_a ? 1 : 0; }

// The station of `pts` nearest the middle of its `share` (0 at its start, 1 at its end).
V2 middle_of(const std::vector<V2> &pts, const std::vector<float> &share) {
	size_t best = 0;
	for (size_t i = 1; i < pts.size() && i < share.size(); i++)
		if (std::fabs(share[i] - HALF_SHARE) < std::fabs(share[best] - HALF_SHARE)) best = i;
	return pts[best];
}

// A world point's place on segment `sid` near arc `guess`: its arc along the centre line and its offset to the segment's left.
void place_on(const RoadNetwork &net, int sid, double x, double z, double guess, double &v, float &u) {
	double cx, cz, fx, fz;
	centre_at(net, sid, guess, cx, cz, fx, fz);
	v = std::clamp(guess + (x - cx) * fx + (z - cz) * fz, 0.0, net.segs[size_t(sid)].len);
	centre_at(net, sid, v, cx, cz, fx, fz);
	u = float((x - cx) * fz - (z - cz) * fx); // the left of (fx, fz) is (fz, -fx)
}

// Segment `sid`'s end at node `n`: 0 at a, 1 at b.
int end_of(const RoadNetwork &net, int sid, int n) { return net.segs[size_t(sid)].a == n ? 0 : 1; }

// The arc of segment `seg`'s own end pole at its end `end`: past its mouth by line_end_setback.
double own_end_v(const RoadNetwork &net, int seg, int end) {
	const RoadSegment &g = net.segs[size_t(seg)];
	const double past = line_end_setback(net);
	return end == 0 ? g.retreat_a + past : g.len - g.retreat_b - past;
}

// Where a pole of segment `seg`'s line stands at arc `v`: on its line's face, kerbside_reach in from the kerb, as lamp_row stands
// its poles. False when the segment lays no line.
bool pole_point(const RoadNetwork &net, int seg, double v, double &x, double &z) {
	const int face = line_face(net, seg);
	if (face < 0) return false;
	const float u = kerb_in(net.profile_of(seg), face, kerbside_reach(net, FurnitureKind::LAMP));
	double cx, cz, fx, fz;
	centre_at(net, seg, v, cx, cz, fx, fz);
	x = cx + fz * u; // the left of (fx, fz) is (fz, -fx)
	z = cz - fx * u;
	return true;
}

// Whether arms `p` and `q` of node `n` go straight on into each other (each the other's way on): the kerb between them runs
// straight on either face, no corner there.
bool straight_pair(const RoadNetwork &net, int n, int p, int q) { return p != q && line_straight_on(net, n, p) == q && line_straight_on(net, n, q) == p; }

// The arm the line on `sid` runs on into at node `n` through one pole, with no corner between: the other arm of a joint, or at a
// junction the arm it goes straight on into when that arm's line reaches the same corner (the straight side of a T: the footway runs
// straight, the lots' fronts and gates along it); -1 when the line meets a corner of the node or ends there.
int line_runs_on(const RoadNetwork &net, int n, int sid) {
	const int joint = net.going_on(n, sid);
	if (joint >= 0) return line_face(net, joint) >= 0 ? joint : -1;
	const int on = line_straight_on(net, n, sid);
	if (on < 0 || !straight_pair(net, n, sid, on) || line_face(net, on) < 0) return -1;
	const int k = line_corner_at(net, n, sid);
	return k >= 0 && k == line_corner_at(net, n, on) ? on : -1;
}

// The pole a segment's line ends on at its end `end`, as a span names it: the corner pole it ends on, the pole of a joint the other
// arm owns (that arm's station nearest the node), else its own station nearest that end, and past a row with no pole of its own the
// joint's pole beyond. Invalid (seg -1) when none stands.
LinePole terminal(const RoadNetwork &net, int sid, int end, const FirstStation &first_station) {
	const LineEndAt e = line_end_at(net, sid, end);
	const int n = end == 0 ? net.segs[size_t(sid)].a : net.segs[size_t(sid)].b;
	if (e.corner >= 0) return LinePole{ corner_pole(net, n, e.corner).seg, -1, n, e.corner };
	const int own = e.joint && !e.own ? -1 : first_station(sid, end);
	if (own >= 0) return LinePole{ sid, own, -1, -1 };
	if (e.joint) {
		const int other = line_runs_on(net, n, sid);
		const int st = other >= 0 ? first_station(other, end_of(net, other, n)) : -1;
		if (st >= 0) return LinePole{ other, st, -1, -1 };
	}
	return LinePole{};
}

} // namespace

int line_face(const RoadNetwork &net, int sid) {
	return furniture_level(net.segs[size_t(sid)]) == FURNITURE_LEVEL_NONE ? -1 : street_face(net, sid);
}

int street_face(const RoadNetwork &net, int sid) {
	const RoadSegment &g = net.segs[size_t(sid)];
	if (!g.alive || g.a == g.b) return -1;
	const Profile &p = net.profile_of(sid);
	if (p.edges.empty() || p.rail || p.ring) return -1;
	const bool right = walk_on(p, 0), left = walk_on(p, 1);
	if (!right || !left) return right ? 0 : (left ? 1 : -1);
	const RoadNode &a = net.nodes[size_t(g.a)], &b = net.nodes[size_t(g.b)];
	return (b.x - a.x) - (b.z - a.z) >= 0 ? 0 : 1; // the right face looks to (-dz, dx): east is +x, south +z
}

void line_reads(const RoadNetwork &net, int n, std::vector<int> &out) {
	for (int sid : net.nodes[size_t(n)].segs) {
		out.push_back(sid);
		const RoadSegment &g = net.segs[size_t(sid)];
		for (int arm : net.nodes[size_t(g.a == n ? g.b : g.a)].segs) out.push_back(arm);
	}
}

int line_straight_on(const RoadNetwork &net, int n, int seg) {
	const RoadSegment &gs = net.segs[size_t(seg)];
	const Arm in = arm_of(gs.q, net.profile_of(seg), gs.a == n ? 0 : 1);
	int best = -1;
	double best_dot = net.cat->traffic.straight_dot;
	for (int sid : net.nodes[size_t(n)].segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		if (sid == seg || !g.alive || g.a == g.b) continue;
		const Arm out = arm_of(g.q, net.profile_of(sid), g.a == n ? 0 : 1);
		const double dot = -(in.dx * out.dx + in.dz * out.dz); // both arms point out of the node
		if (dot > best_dot) {
			best_dot = dot;
			best = sid;
		}
	}
	return best;
}

int line_corner_at(const RoadNetwork &net, int n, int sid) {
	const int face = line_face(net, sid);
	if (face < 0 || int(net.nodes[size_t(n)].segs.size()) < CROSSWALK_ARMS) return -1;
	const std::vector<NodeEnd> ends = node_ends(net, n);
	const int count = int(ends.size());
	for (int e = 0; e < count; e++) {
		if (ends[size_t(e)].seg != sid) continue;
		return face == arm_left_face(ends[size_t(e)].at_a) ? e : (e + count - 1) % count;
	}
	return -1;
}

CornerPole corner_pole(const RoadNetwork &net, int n, int k) {
	CornerPole c;
	const std::vector<NodeEnd> ends = node_ends(net, n);
	const int count = int(ends.size());
	if (count < CROSSWALK_ARMS || !node_plated(ends) || k < 0 || k >= count) return c;
	const NodeEnd &ea = ends[size_t(k)], &eb = ends[size_t((k + 1) % count)];
	if (straight_pair(net, n, ea.seg, eb.seg)) return c; // between two arms in line the kerb runs straight: a line runs on through it
	const bool reach_a = line_face(net, ea.seg) == arm_left_face(ea.at_a), reach_b = line_face(net, eb.seg) == 1 - arm_left_face(eb.at_a);
	if (!reach_a && !reach_b) return c;
	const RoadNode &nd = net.nodes[size_t(n)];
	const CornerOutline co = node_corner(net, nd.x, nd.z, ea, eb);
	if (co.kerb.empty() || co.back.empty() || !co.simple) return c;
	const V2 kerb = middle_of(co.kerb, co.share), back = middle_of(co.back, co.back_share);
	const V2 out(back.x - kerb.x, back.y - kerb.y);
	const float room = std::hypot(out.x, out.y);
	const StreetFurnitureDef &d = defs(net);
	const float inset = d.poles.kerb_inset_m;
	if (room < inset + d.footprint[int(FurnitureKind::LAMP)].x_m * HALF_SHARE) return c; // a corner too narrow to hold a pole
	c.stands = true;
	c.seg = std::min(ea.seg, eb.seg);
	c.x = nd.x + kerb.x + out.x * inset / room;
	c.z = nd.z + kerb.y + out.y * inset / room;
	const NodeEnd &own = ea.seg == c.seg ? ea : eb;
	const RoadSegment &g = net.segs[size_t(c.seg)];
	place_on(net, c.seg, c.x, c.z, own.at_a ? own.at : g.len - own.at, c.v, c.u);
	double cx, cz, fx, fz;
	centre_at(net, c.seg, c.v, cx, cz, fx, fz);
	const double tx = -out.x / room, tz = -out.y / room; // toward the kerb: the junction it lights
	c.face_fwd = float(tx * fx + tz * fz);
	c.face_left = float(tx * fz - tz * fx);
	return c;
}

void list_corner_poles(const RoadNetwork &net, int sid, std::vector<FurnitureSpot> &out) {
	const RoadSegment &g = net.segs[size_t(sid)];
	const Profile &p = net.profile_of(sid);
	for (const int n : { g.a, g.b }) {
		const int corners = int(net.nodes[size_t(n)].segs.size());
		for (int k = 0; k < corners; k++) {
			const CornerPole pole = corner_pole(net, n, k);
			if (!pole.stands || pole.seg != sid) continue;
			FurnitureSpot s = spot(net, FurnitureKind::LAMP, sid, p, pole.v, pole.u, pole.face_left, pole.face_fwd);
			s.side = pole.u > 0.f ? 1 : 0;
			s.node = n;
			s.corner = k;
			out.push_back(s);
		}
	}
}

double line_end_setback(const RoadNetwork &net) {
	const StreetFurnitureDef &d = defs(net);
	return d.crossing.zebra_to_m + d.footprint[int(FurnitureKind::SIGNAL)].z_m + d.items.gap_m + d.footprint[int(FurnitureKind::LAMP)].x_m * HALF_SHARE;
}

LineEndAt line_end_at(const RoadNetwork &net, int sid, int end) {
	LineEndAt e;
	const RoadSegment &g = net.segs[size_t(sid)];
	const int n = end == 0 ? g.a : g.b;
	const double mouth = end == 0 ? g.retreat_a : g.len - g.retreat_b;
	const int other = line_runs_on(net, n, sid);
	if (other >= 0) { // a joint, or the straight side of a T: the line goes on through one pole, the lower segment's own end pole
		e.joint = true;
		e.own = sid < other;
		const double here = own_end_v(net, sid, end);
		if (e.own) {
			e.v = here;
			return e;
		}
		// the other's row is spaced from that pole as it stands: carried past this end by its straight distance from where this row's
		// own end pole would stand (round a bend the poles on its inner side stand nearer than the arc between them)
		double jx, jz, px, pz;
		const bool both = pole_point(net, other, own_end_v(net, other, end_of(net, other, n)), jx, jz) && pole_point(net, sid, here, px, pz);
		const double gap = both ? std::hypot(px - jx, pz - jz) : 0.0;
		e.v = here + (end == 0 ? -gap : gap);
		return e;
	}
	const int k = line_corner_at(net, n, sid);
	const CornerPole c = k >= 0 ? corner_pole(net, n, k) : CornerPole{};
	if (!c.stands) return e;
	e.corner = k;
	e.own = false;
	float u;
	place_on(net, sid, c.x, c.z, mouth, e.v, u);
	return e;
}

bool line_end_point(const RoadNetwork &net, int sid, int end, double &x, double &z) {
	const LineEndAt e = line_end_at(net, sid, end);
	const RoadSegment &g = net.segs[size_t(sid)];
	const int n = end == 0 ? g.a : g.b;
	if (e.corner >= 0) {
		const CornerPole c = corner_pole(net, n, e.corner);
		x = c.x;
		z = c.z;
		return c.stands;
	}
	if (e.joint && !e.own) { // the other arm's own end pole
		const int other = line_runs_on(net, n, sid);
		return other >= 0 && pole_point(net, other, own_end_v(net, other, end_of(net, other, n)), x, z);
	}
	return pole_point(net, sid, own_end_v(net, sid, end), x, z); // its own end pole, a joint's or past its mouth
}

void node_line_spans(const RoadNetwork &net, int n, const FirstStation &first_station, std::vector<LineSpan> &out) {
	const RoadNode &nd = net.nodes[size_t(n)];
	if (!nd.alive) return;
	for (int sid : nd.segs) { // each line ends on a pole of this node, or runs along a block with no pole of its own
		if (line_face(net, sid) < 0) continue;
		const int end = end_of(net, sid, n);
		const LineEndAt e = line_end_at(net, sid, end);
		if (e.own && !e.joint) continue; // ends on its own end pole: its row strings it
		const LinePole here = e.corner >= 0 ? LinePole{ corner_pole(net, n, e.corner).seg, -1, n, e.corner } : terminal(net, sid, end, first_station);
		if (e.joint && e.own) continue; // the joint's own pole: the other segment strings to it
		LinePole there{ sid, first_station(sid, end), -1, -1 };
		if (there.station < 0) { // no pole of its own: along the block to its other end's pole, strung once from its end a
			if (end != 0) continue;
			there = terminal(net, sid, 1, first_station);
		}
		if (here.seg >= 0 && there.seg >= 0 && !(here == there)) out.push_back(LineSpan{ here, there, false });
	}
	if (int(nd.segs.size()) < CROSSWALK_ARMS) return;
	const float most = defs(net).poles.max_spacing_m;
	for (int sid : nd.segs) { // across the junction: a corner pole to the corner pole of the line going straight on
		const int on = line_straight_on(net, n, sid);
		if (on <= sid || line_straight_on(net, n, on) != sid) continue; // once a pair, each the other's way on
		const int ka = line_corner_at(net, n, sid), kb = line_corner_at(net, n, on);
		if (ka < 0 || kb < 0 || ka == kb) continue;
		const CornerPole pa = corner_pole(net, n, ka), pb = corner_pole(net, n, kb);
		if (!pa.stands || !pb.stands || std::hypot(pa.x - pb.x, pa.z - pb.z) > most) continue;
		out.push_back(LineSpan{ LinePole{ pa.seg, -1, n, ka }, LinePole{ pb.seg, -1, n, kb }, true });
	}
}

} // namespace city
