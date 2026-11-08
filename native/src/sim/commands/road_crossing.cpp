// A crossing on a road between junctions (Roadcraft editor/streetObjects.ts commitPedestrianCrossing): a node where the walkers cross, a zebra the cars give way at or
// a signal with a stage of its own. Room is kept on both sides for the cars that stop there.
#include "sim/commands/commands.h"
#include "sim/commands/edit_journal.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"

#include <algorithm>

namespace city {

EditResult add_crossing(CityState &s, double x, double z, int kind) {
	EditResult r;
	RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	double t, d;
	const int seg = net.find_segment_near(x, z, tn.road_pick_m, t, d);
	if (seg < 0 || (kind != CROSSING_ZEBRA && kind != CROSSING_SIGNAL)) {
		r.error = "hint.split";
		return r;
	}
	const RoadSegment &g = net.segs[size_t(seg)];
	const Profile &p = net.profile_of(seg);
	if (!p.walk() || p.rail || p.ring || structure_of(g.flags) == Structure::TUNNEL) {
		r.error = "refuse.crossing.noWalk";
		return r;
	}
	RoadBox box;
	box.add(g);
	const CrossingDef &c = s.catalog.furniture.crossing;
	const double room = double(tn.road_min_len_m) + c.zebra_to_m + c.stop_to_m, at = t * g.len;
	int node = net.find_node_near(x, z, double(tn.edit_split_min_m));
	if (node >= 0 && net.nodes[size_t(node)].segs.size() != JOINT_ARMS) node = -1;
	if (node < 0) {
		if (at < double(g.retreat_a) + room || g.len - at < double(g.retreat_b) + room) {
			r.error = "refuse.crossing.room";
			return r;
		}
		node = split_segment(s, seg, t);
	}
	j_set_crossing(s, node, uint8_t(kind));
	for (int arm : net.nodes[size_t(node)].segs) net.mark_segment(arm);
	r.ok = true;
	return r;
}

EditResult add_uturn(CityState &s, double x, double z, bool toward_b) {
	EditResult r;
	RoadNetwork &net = s.roads;
	const Tuning &tn = s.catalog.tune;
	double t, d;
	const int seg = net.find_segment_near(x, z, tn.road_pick_m, t, d);
	if (seg < 0) {
		r.error = "hint.split";
		return r;
	}
	const RoadSegment g = net.segs[size_t(seg)];
	const Profile &p = net.profile_of(seg);
	float median = 0, lane = 0;
	for (size_t i = 0; i < p.strips.size(); i++) {
		const Strip &st = p.strips[i];
		if (st.kind == StripKind::LANE) lane = st.width;
		if (st.kind != StripKind::MEDIAN) continue;
		median = st.width; // with the kerbs that close it, what stands between the two inner lanes
		if (i > 0 && p.strips[i - 1].kind == StripKind::CURB) median += p.strips[i - 1].width;
		if (i + 1 < p.strips.size() && p.strips[i + 1].kind == StripKind::CURB) median += p.strips[i + 1].width;
	}
	if (median <= 0 || p.one_way || structure_of(g.flags) == Structure::TUNNEL) {
		r.error = "hint.uturn.noMedian";
		return r;
	}
	if ((median + lane) / 2 < tn.road_uturn_radius_m) { // from the inner lane to the inner lane, the half circle a car drives
		r.error = "hint.uturn.narrow";
		return r;
	}
	const double room = double(tn.road_min_len_m) + tn.road_uturn_opening_m, at = t * g.len;
	if (at < double(g.retreat_a) + room || g.len - at < double(g.retreat_b) + room) {
		r.error = "hint.uturn.tooShort";
		return r;
	}
	RoadBox box;
	box.add(g);
	const int node = split_segment(s, seg, t);
	const std::vector<int> arms = net.nodes[size_t(node)].segs;
	if (arms.size() != JOINT_ARMS) {
		r.error = "hint.split";
		return r;
	}
	const auto from_a = [&](int arm) { return net.segs[size_t(arm)].a == g.a || net.segs[size_t(arm)].b == g.a; };
	const int arrives = (from_a(arms[0]) == toward_b) ? arms[0] : arms[1];
	j_set_uturn(s, node, arrives == std::min(arms[0], arms[1]) ? UTURN_FROM_LOW : UTURN_FROM_HIGH);
	for (int arm : arms) net.mark_segment(arm);
	r.ok = true;
	return r;
}

EditResult remove_crossing(CityState &s, int node) {
	EditResult r;
	RoadNetwork &net = s.roads;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[size_t(node)].alive || net.nodes[size_t(node)].crossing == CROSSING_NONE) {
		r.error = "hint.move";
		return r;
	}
	for (int arm : net.nodes[size_t(node)].segs) net.mark_segment(arm);
	j_set_crossing(s, node, CROSSING_NONE);
	r.ok = true;
	return r;
}

} // namespace city
