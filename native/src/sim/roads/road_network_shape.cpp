// The nodes' shapes (RoadNetwork::refresh_node, settle_shapes, refresh_all): each arm's setback from its node, a dead end's turning
// bulb where no road stands too close, a joint where two arms go on in line; every mouth that moves is told to the grade, the edits,
// the change log and the lane graph.
#include "sim/roads/road_network.h"

#include "sim/roads/junction.h"

#include <algorithm>
#include <cmath>

namespace city {

namespace {
constexpr double STRAIGHT_ANGLE_COS = -0.9986; // two arms this close to opposite are one street going on
constexpr double BULB_REACH_PAD = 3, BULB_SEARCH_PAD = 30;
constexpr double BULB_REACH_M = BULB_R + BULB_WALK + BULB_REACH_PAD; // a road closer than this to a dead end leaves no room for a bulb
constexpr double BULB_LOOK_M = BULB_REACH_M + BULB_SEARCH_PAD; // the roads a dead end looks at for its bulb lie within this

// A float bound that never lies inside the double box it stands for (the hash finds its cells from floats).
float below(double v) { return std::nextafter(float(v), -INFINITY); }
float above(double v) { return std::nextafter(float(v), INFINITY); }
} // namespace

bool RoadNetwork::bulb_clear(int n) const {
	const RoadNode &nd = nodes[n];
	int s = nd.segs[0];
	const RoadSegment &g = segs[s];
	if (cat->profile(g.profile).rail) return false;
	if (bulb_retreat(arm_of(g.q, cat->profile(g.profile), g.a == n ? 0 : 1), g.len) <= 0) return false;
	const double reach = BULB_REACH_M, pad = BULB_LOOK_M;
	bool clear = true;
	for_segments_near(nd.x - pad, nd.z - pad, nd.x + pad, nd.z + pad, [&](int o) {
		if (!clear || o == s) return;
		double t, d;
		closest_t(segs[o].q, nd.x, nd.z, t, d);
		if (d - cat->profile(segs[o].profile).right < reach) clear = false;
	});
	return clear;
}

bool RoadNetwork::refresh_node(int n, bool arms_changed) {
	RoadNode &nd = nodes[n];
	if (!nd.alive) return false;
	size_t k = nd.segs.size();
	const bool was_joint = nd.joint, was_bulb = nd.bulb;
	nd.bulb = k == 1 && !nd.plain && bulb_clear(n);
	nd.joint = false;
	std::vector<Arm> arms;
	std::vector<double> lens, out;
	std::vector<int> order;
	for (int s : nd.segs) {
		arms.push_back(arm_of(segs[s].q, profile_of(s), segs[s].a == n ? 0 : 1));
		lens.push_back(segs[s].len);
	}
	if (k == 2) nd.joint = arms[0].dx * arms[1].dx + arms[0].dz * arms[1].dz < STRAIGHT_ANGLE_COS;
	if (nd.joint != was_joint) grade.node_changed(n); // a plate there, or none
	// the node itself is drawn and driven otherwise: a bulb or a plate, or arms come or gone (crosswalks, the junction's
	// outline and connectors read every arm)
	const bool whole = arms_changed || nd.joint != was_joint || nd.bulb != was_bulb;
	if (arms_changed || nd.joint != was_joint) run_dirty_.insert(run_dirty_.end(), nd.segs.begin(), nd.segs.end()); // the street goes on here, or no more
	junction_retreats(arms, lens, out, order, nd.bulb);
	bool changed = whole;
	for (size_t i = 0; i < k; i++) {
		RoadSegment &g = segs[nd.segs[i]];
		double &mouth = g.a == n ? g.retreat_a : g.retreat_b;
		const bool moved = mouth != out[i];
		if (moved) grade.seg_changed(nd.segs[i]); // its plate there moved
		mouth = out[i];
		if (!moved && !whole) continue;
		changed = true;
		edited_segs_.push_back(nd.segs[i]);
		mark_segment(nd.segs[i]);
		// the mouths moved: the lane graph rebuilds this arm and its node on the next traffic step
		if (log) {
			log->net_seg(nd.segs[i]);
			log->net_node(n);
		}
	}
	return changed;
}

void RoadNetwork::settle_shapes() {
	std::vector<int> own, near;
	own.swap(shape_nodes_);
	const double pad = BULB_LOOK_M;
	for (const ChangedBox &b : shape_boxes_)
		node_hash.query(below(b.minx - pad), below(b.minz - pad), above(b.maxx + pad), above(b.maxz + pad), [&](int n) { near.push_back(n); });
	shape_boxes_.clear();
	ChangeLog::dedup(own);
	ChangeLog::dedup(near);
	for (int n : own)
		if (n >= 0 && n < int(nodes.size())) refresh_node(n, true);
	// the nodes near a changed road whose own arms stand as they were: only a dead end's bulb (and its mouth) can change
	for (int n : near)
		if (!std::binary_search(own.begin(), own.end(), n)) refresh_node(n, false);
	settle_runs();
}

void RoadNetwork::refresh_all() {
	shape_nodes_.clear();
	shape_boxes_.clear();
	for (int n = 0; n < int(nodes.size()); n++) refresh_node(n, true);
	settle_runs();
}

} // namespace city
