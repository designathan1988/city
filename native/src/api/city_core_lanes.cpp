// The lane connector editor's API (Roadcraft ui/roads/connectorPanel.ts): a junction's lanes and their ends as the traffic last built them, and
// a connection toggled by hand, as an edit.
#include "api/city_core.h"

#include "api/edit_result.h"
#include "sim/commands/commands.h"
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

std::vector<LaneLink> built_links(const CityState &s, int node) {
	std::vector<LaneLink> out;
	const LaneGraph &g = s.traffic.g;
	if (node < 0 || node >= int(g.node_conn.size())) return out;
	for (int c = g.node_conn[size_t(node)]; c >= 0; c = g.conns[size_t(c)].next_at_node) {
		const Conn &k = g.conns[size_t(c)];
		out.push_back({ g.ds[size_t(k.from_ds)].seg, k.from_lane, g.ds[size_t(k.to_ds)].seg, k.to_lane });
	}
	return out;
}
} // namespace

// {arriving, leaving: [{seg, lane, x, z}], built: [[from, from_lane, to, to_lane]]}.
Dictionary CityCore::node_lanes(int node) {
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
		for (const LaneLink &k : built_links(s, node)) {
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

Dictionary CityCore::toggle_lane_link(int node, int from, int from_lane, int to, int to_lane) {
	std::vector<LaneLink> built;
	{
		std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
		std::lock_guard<std::mutex> tr(state_->traffic_mutex);
		built = built_links(*state_, node);
	}
	return run_edit(*state_, "cmd.lane.link", [&](CityState &s) { return city::toggle_lane_link(s, node, { from, from_lane, to, to_lane }, built); });
}

} // namespace city
