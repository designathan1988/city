// The Move tab (Roadcraft editor/nodeMover.ts, commit.ts moveNodeChecked): a node dropped elsewhere. Its roads are taken up
// and laid again through the same steps as a new road, so a crossing makes a junction, an overlap is refused and the
// whole edit rolls back as one undo step.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"

#include <algorithm>
#include <cmath>
#include <vector>

namespace city {

Quad moved_quad(const Quad &q, bool at_a, double x, double z) {
	Quad out = q;
	(at_a ? out.x0 : out.x1) = x;
	(at_a ? out.z0 : out.z1) = z;
	// the similarity that takes the old ends to the new ones takes the control point along
	const double ox = q.x1 - q.x0, oz = q.z1 - q.z0, nx = out.x1 - out.x0, nz = out.z1 - out.z0, len2 = ox * ox + oz * oz;
	if (len2 <= 0) return out;
	const double ar = (nx * ox + nz * oz) / len2, ai = (nz * ox - nx * oz) / len2, dx = q.cx - q.x0, dz = q.cz - q.z0;
	out.cx = out.x0 + ar * dx - ai * dz;
	out.cz = out.z0 + ai * dx + ar * dz;
	return out;
}

EditResult move_node(CityState &s, int node, double x, double z) {
	EditResult r;
	RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[size_t(node)].alive || net.nodes[size_t(node)].segs.empty()) {
		r.error = "hint.move";
		return r;
	}
	const RoadNode was = net.nodes[size_t(node)];
	std::vector<RoadSegment> roads;
	double value = 0;
	RoadBox box;
	box.add(was.x, was.z);
	box.add(x, z);
	for (int sid : was.segs) {
		const RoadSegment &g = net.segs[size_t(sid)];
		const RoadNode &far = net.nodes[size_t(g.a == node ? g.b : g.a)];
		if (std::hypot(far.x - x, far.z - z) < tn.road_min_len_m) {
			r.error = "hint.move.tooShort";
			return r;
		}
		roads.push_back(g);
		value += road_value(s, g.profile, g.len);
		box.add(g);
	}
	const std::map<int, double> before = squeezes_in(s, box);
	for (int sid : was.segs) j_remove_segment(s, sid);
	j_money(s, value);
	Anchor to = snap_point(s, x, z, tn.road_merge_m);
	if (to.node == node) {
		to = Anchor{};
		to.x = x;
		to.z = z;
	}
	Anchor end;
	end.node = anchor_node(s, to);
	end.x = net.nodes[size_t(end.node)].x;
	end.z = net.nodes[size_t(end.node)].z;
	double paid = 0;
	for (const RoadSegment &g : roads) {
		const bool at_a = g.a == node;
		const Quad q = moved_quad(g.q, at_a, end.x, end.z);
		Anchor far;
		far.node = at_a ? g.b : g.a;
		far.x = at_a ? g.q.x1 : g.q.x0;
		far.z = at_a ? g.q.z1 : g.q.z0;
		const RoadKeeps keeps{ g.zl, g.zr, g.flags };
		EditResult laid = lay_road(s, at_a ? end : far, true, q.cx, q.cz, at_a ? far : end, g.profile, keeps);
		if (!laid.ok) {
			r.error = laid.error;
			return r;
		}
		paid += laid.money;
		r.segments += laid.segments;
		r.buildings_removed += laid.buildings_removed;
	}
	if (end.node != node) {
		if (!was.plain) j_set_plain(s, end.node, false);
		j_remove_node_if_orphan(s, node);
	}
	finish_edit(s, box.minx, box.minz, box.maxx, box.maxz);
	if (squeezed_worse(s, before, box)) {
		r.error = "hint.rule.squeezed";
		return r;
	}
	r.ok = true;
	r.money = paid - value;
	return r;
}

} // namespace city
