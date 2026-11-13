// The lane connector editor's API (Roadcraft ui/roads/connectorPanel.ts): a junction's lanes and their ends as the traffic last built them, and
// a connection toggled by hand, as an edit.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/commands/commands.h"
#include "sim/commands/street_commands.h"
#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <mutex>

using namespace godot;

namespace city {

namespace {
Dictionary lane_end(const CityState &s, int seg, int lane, int ds, float at) {
	Pose p;
	traffic::lane_pose(s, ds, lane, at, p);
	Dictionary d;
	d["seg"] = seg;
	d["lane"] = lane;
	d["x"] = p.x;
	d["z"] = p.z;
	return d;
}
} // namespace

// {arriving, leaving: [{seg, lane, x, z}], built: [[from, from_lane, to, to_lane]]}.
Dictionary CityCore::junction_lane_map(int node) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	Dictionary out;
	Array arriving, leaving, built;
	if (node >= 0 && node < int(s.roads.nodes.size()) && s.roads.nodes[size_t(node)].alive) {
		const LaneGraph &g = s.traffic.g;
		for (int seg : s.roads.nodes[size_t(node)].segs) {
			const int din = traffic::in_ds(s.roads, seg, node), dout = traffic::out_ds(s.roads, seg, node);
			if (din >= int(g.ds.size()) || dout >= int(g.ds.size())) continue;
			if (g.ds[size_t(din)].ok)
				for (int l = 0; l < g.ds[size_t(din)].lanes; l++) arriving.push_back(lane_end(s, seg, l, din, g.ds[size_t(din)].len));
			if (g.ds[size_t(dout)].ok)
				for (int l = 0; l < g.ds[size_t(dout)].lanes; l++) leaving.push_back(lane_end(s, seg, l, dout, 0));
		}
		std::vector<LaneLink> links;
		traffic::links_built(s, node, links);
		for (const LaneLink &k : links) {
			Array one;
			one.push_back(k.from);
			one.push_back(k.from_lane);
			one.push_back(k.to);
			one.push_back(k.to_lane);
			built.push_back(one);
		}
	}
	out["arriving"] = arriving;
	out["leaving"] = leaving;
	out["built"] = built;
	return out;
}

// The roads of a junction: {seg, rule, dx, dz}, the way each leaves it and the plate it carries there.
Array CityCore::node_legs(int node) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	const RoadNetwork &net = state_->roads;
	Array out;
	if (node < 0 || node >= int(net.nodes.size()) || !net.nodes[size_t(node)].alive) return out;
	const RoadNode &n = net.nodes[size_t(node)];
	for (int seg : n.segs) {
		const RoadSegment &g = net.segs[size_t(seg)];
		const RoadNode &there = net.nodes[size_t(g.a == node ? g.b : g.a)];
		const int end = g.a == node ? 0 : 1;
		Dictionary d;
		d["seg"] = seg;
		d["rule"] = (g.flags & seg_rule_mask(end)) >> seg_rule_shift(end);
		d["dx"] = there.x - n.x;
		d["dz"] = there.z - n.z;
		Array banned;
		for (int to : n.segs)
			if (net.movement_banned(node, seg, to)) banned.push_back(to);
		d["banned"] = banned;
		out.push_back(d);
	}
	return out;
}

Dictionary CityCore::set_movement_banned(int node, int from, int to, bool banned) {
	return run_edit(*state_, "cmd.lane.rule", [&](CityState &s) { return city::set_movement_banned(s, node, from, to, banned); });
}

Dictionary CityCore::remove_node(int node) {
	return run_edit(*state_, "cmd.demolish.road", [&](CityState &s) { return city::remove_node(s, node); });
}

Dictionary CityCore::set_approach_rule(int node, int seg, int rule) {
	return run_edit(*state_, "cmd.lane.rule", [&](CityState &s) { return city::set_approach_rule(s, node, seg, rule); });
}

Dictionary CityCore::flip_lane_link(int node, int from, int from_lane, int to, int to_lane) {
	return run_edit(*state_, "cmd.lane.link", [&](CityState &s) { return city::flip_lane_link(s, node, { from, from_lane, to, to_lane }); });
}

} // namespace city
