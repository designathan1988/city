// A crossing on a road between junctions (Roadcraft editor/streetObjects.ts commitPedestrianCrossing): a node where the walkers cross, a zebra the cars give way at or
// a signal with a stage of its own. Room is kept on both sides for the cars that stop there.
#include "sim/commands/commands.h"
#include "sim/commands/road_edit.h"

#include "sim/state/city_state.h"

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
	finish_edit(s, box.minx, box.minz, box.maxx, box.maxz);
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
	RoadBox box;
	for (int arm : net.nodes[size_t(node)].segs) {
		box.add(net.segs[size_t(arm)]);
		net.mark_segment(arm);
	}
	j_set_crossing(s, node, CROSSING_NONE);
	finish_edit(s, box.minx, box.minz, box.maxx, box.maxz);
	r.ok = true;
	return r;
}

} // namespace city
