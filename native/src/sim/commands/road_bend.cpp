// What the Inspector does to a road's shape (Roadcraft ui/inspector.ts, main.ts setNodeHeightMetres, world/doc.ts setSegmentCurve, joinSegments):
// the height the player gives a node, within the steepest grade a road may be drawn at; the whole road bent, taken up and laid again
// through the steps of a new road; and two roads in line made one.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"
#include "sim/commands/road_refs.h"
#include "sim/commands/street_commands.h"

#include "sim/economy/road_economy.h"
#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <vector>

namespace city {

namespace {

constexpr double BEZIER_MID = 2; // a quadratic's middle stands half way to its control point
constexpr uint16_t RUN_MARKS = SEG_RUNS_ON_A | SEG_RUNS_ON_B;

} // namespace

Quad road_line(const RoadNetwork &net, int seg) {
	const std::vector<int> pieces = net.pieces_of(seg);
	Quad line = net.segs[size_t(pieces.front())].q;
	for (size_t k = 1; k < pieces.size(); k++) {
		const Quad &next = net.segs[size_t(pieces[k])].q;
		Quad joined;
		line = joined_quad(line, next, joined) ? joined : Quad::straight(line.x0, line.z0, next.x1, next.z1);
	}
	return line;
}

RoadBend road_bend_of(const Quad &line) {
	RoadBend b;
	const double dx = line.x1 - line.x0, dz = line.z1 - line.z0, len2 = dx * dx + dz * dz;
	if (len2 <= 0) return b;
	const double ox = line.cx - line.x0, oz = line.cz - line.z0;
	b.at = (ox * dx + oz * dz) / len2;
	b.height = (oz * dx - ox * dz) / std::sqrt(len2) / BEZIER_MID;
	return b;
}

EditResult set_node_lift(CityState &s, int node, float metres) {
	EditResult r;
	RoadNetwork &net = s.roads;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[size_t(node)].alive) {
		r.error = "hint.move";
		return r;
	}
	const RoadNode &n = net.nodes[size_t(node)];
	float lower = -std::numeric_limits<float>::max(), upper = std::numeric_limits<float>::max();
	for (int sid : n.segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const float other = net.nodes[size_t(g.a == node ? g.b : g.a)].lift, rise = g.len * s.catalog.tune.grade_authored;
		lower = std::max(lower, other - rise);
		upper = std::min(upper, other + rise);
	}
	const float height = lower <= upper ? std::clamp(metres, lower, upper) : n.lift;
	if (height == n.lift) {
		r.error = "refuse.road.steep";
		return r;
	}
	j_set_lift(s, node, height);
	r.ok = true;
	return r;
}

EditResult set_road_bend(CityState &s, int seg, double height, double at, double &mid_x, double &mid_z) {
	EditResult r;
	RoadNetwork &net = s.roads;
	if (seg < 0 || seg >= int(net.segs.size()) || !net.segs[size_t(seg)].alive) {
		r.error = "tools.hint.upgradePick";
		return r;
	}
	const std::vector<int> pieces = net.pieces_of(seg);
	const Quad line = road_line(net, seg);
	const double dx = line.x1 - line.x0, dz = line.z1 - line.z0, len = std::hypot(dx, dz);
	if (len <= 0) {
		r.error = "tools.hint.upgradePick";
		return r;
	}
	std::vector<RoadSegment> old;
	std::vector<std::vector<CarriedPiece>> kits; // what the player put on it stays on it (Roadcraft carryStreetKit)
	RoadBox box;
	for (int p : pieces) {
		old.push_back(net.segs[size_t(p)]);
		kits.push_back(street_kit_of(s, p));
		box.add(net.segs[size_t(p)]);
	}
	const double cx = line.x0 + dx * at - dz / len * height * BEZIER_MID, cz = line.z0 + dz * at + dx / len * height * BEZIER_MID;
	box.add(cx, cz);
	Quad{ line.x0, line.z0, height == 0 ? (line.x0 + line.x1) / 2 : cx, height == 0 ? (line.z0 + line.z1) / 2 : cz, line.x1, line.z1 }.at(0.5, mid_x, mid_z);
	const std::map<int, double> before = squeezes_in(s, box);
	const RoadSegment &first = old.front(), &last = old.back();
	const bool forward = old.size() == 1 || first.b == last.a || first.b == last.b || first.a != last.a;
	Anchor a, b;
	a.node = forward ? first.a : first.b;
	b.node = forward ? last.b : last.a;
	a.x = line.x0;
	a.z = line.z0;
	b.x = line.x1;
	b.z = line.z1;
	std::vector<int> joints;
	for (const RoadSegment &g : old)
		for (int n : { g.a, g.b })
			if (n != a.node && n != b.node) joints.push_back(n);
	RoadRefs refs = refs_of(s, pieces); // the bans, links and stops of the road, carried to the road laid again
	for (int n : joints) forget_refs_at(refs, n); // its inner joints go: what they held goes with them
	for (int p : pieces) j_remove_segment(s, p);
	for (int n : joints) j_remove_node_if_orphan(s, n);
	const RoadKeeps keeps{ first.zl, first.zr, uint16_t(first.flags & ~RUN_MARKS) };
	std::vector<int> laid;
	EditResult done = height == 0 ? lay_road(s, a, false, 0, 0, b, first.profile, keeps, &laid) : lay_road(s, a, true, cx, cz, b, first.profile, keeps, &laid);
	if (!done.ok) {
		r.error = done.error;
		return r;
	}
	for (size_t i = 0; i < old.size(); i++) carry_kit_along(s, kits[i], old[i].len, old[i].q, laid);
	for (int p : pieces) carry_refs(s, refs, p, laid); // at its two end nodes, to the piece meeting each; a stop to the nearest
	r.segments += done.segments;
	r.buildings_removed += done.buildings_removed;
	for (int p : laid) box.add(net.segs[size_t(p)]);
	settle_network(s); // the mouths squeezed_worse reads
	if (squeezed_worse(s, before, box)) {
		r.error = "hint.rule.squeezed";
		return r;
	}
	if (!settle_road_edit(s, r)) return r; // the caller rolls the journal back
	r.ok = true;
	return r;
}

EditResult join_roads(CityState &s, int node) {
	EditResult r;
	RoadNetwork &net = s.roads;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[size_t(node)].alive || net.nodes[size_t(node)].segs.size() != JOINT_ARMS) {
		r.error = "hint.road.join";
		return r;
	}
	RoadBox box;
	std::vector<int> made;
	if (!merge_joint(s, node, -1, box, made)) {
		r.error = "hint.road.join";
		return r;
	}
	r.ok = true;
	return r;
}

} // namespace city
