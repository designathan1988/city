// The traffic's published view (see traffic_view.h): filled in place in the back buffer, then swapped.
#include "sim/traffic/traffic_view.h"

#include "sim/state/city_state.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
void add(TrafficTally &t, const Vehicle &v, float standing) {
	t.vehicles++;
	if (!v.micro) return;
	t.seen++;
	t.speed += v.v;
	t.queued += v.v < standing ? 1 : 0;
}

// What only the graph decides: run again when the graph changed, never per tick.
void graph_part(const CityState &s, TrafficView &view) {
	const LaneGraph &g = s.traffic.g;
	view.seg.assign(s.roads.segs.size(), TrafficTally());
	view.node.assign(s.roads.nodes.size(), TrafficTally());
	for (size_t ds = 0; ds < g.ds.size(); ds++) {
		const DirSeg &d = g.ds[ds];
		if (!d.ok) continue;
		const float cap = float(d.lanes) * std::max(1.f, d.len / float(s.catalog.tune.inspect_lane_vehicle_m));
		if (ds / 2 < view.seg.size()) view.seg[ds / 2].capacity += cap;
		if (size_t(d.to) < view.node.size()) view.node[size_t(d.to)].capacity += cap;
	}
	const size_t nodes = std::min(s.roads.nodes.size(), g.junctions.size());
	view.junction.assign(s.roads.nodes.size(), JunctionView());
	view.signals.clear();
	for (size_t n = 0; n < nodes; n++) {
		const Junction &j = g.junctions[n];
		JunctionView &jv = view.junction[n];
		jv.known = true;
		jv.control = int(j.ctl);
		if (n < g.node_conn.size())
			for (int c = g.node_conn[n]; c >= 0; c = g.conns[size_t(c)].next_at_node) jv.movements += g.conns[size_t(c)].alive ? 1 : 0;
		if (j.ctl != Control::SIGNAL) continue;
		jv.signal = true;
		view.signals.push_back(int(n));
	}
	view.graph_serial = s.lane_graph_serial;
}

// Each signalled node's cycle, stage, lamps and minimum greens.
void signals(const CityState &s, TrafficView &view) {
	const LaneGraph &g = s.traffic.g;
	view.lamps.clear();
	view.least.clear();
	for (int n : view.signals) {
		const Junction &j = g.junctions[size_t(n)];
		JunctionView &jv = view.junction[size_t(n)];
		jv.cycle = j.cycle;
		jv.stage = j.run;
		jv.stages = int(j.green.size());
		jv.lamp_from = int(view.lamps.size());
		jv.lamp_n = int(j.approaches.size());
		for (const std::vector<int> &approaches : j.approaches) view.lamps.push_back(approaches.empty() ? int(Light::RED) : int(light_for(s, approaches[0])));
		jv.least_from = int(view.least.size());
		view.least.insert(view.least.end(), j.least.begin(), j.least.end());
	}
}
} // namespace

void publish_view(CityState &s) {
	const TrafficState &t = s.traffic;
	const LaneGraph &g = t.g;
	TrafficView &view = s.traffic_views.back();
	view.vehicles = t.vehicle_count + t.park.moving;
	view.people = t.ped_count;
	view.arrived = t.arrived;
	view.lost = t.lost;
	if (view.graph_serial != s.lane_graph_serial) {
		graph_part(s, view);
	} else {
		for (TrafficTally &tally : view.seg) tally.clear_fleet();
		for (TrafficTally &tally : view.node) tally.clear_fleet();
	}
	const float standing = s.catalog.traffic.tail_moving_ms;
	double sum = 0;
	view.near = view.queued = 0;
	for (const Vehicle &v : t.vehicles) {
		if (!v.alive) continue;
		if (v.micro) {
			view.near++;
			sum += v.v;
			view.queued += v.v < standing ? 1 : 0;
		}
		if (v.ds < 0) continue;
		const size_t ds = size_t(v.ds);
		if (ds / 2 < view.seg.size()) add(view.seg[ds / 2], v, standing);
		if (ds < g.ds.size() && g.ds[ds].ok && size_t(g.ds[ds].to) < view.node.size()) add(view.node[size_t(g.ds[ds].to)], v, standing);
	}
	view.mean_speed_ms = view.near > 0 ? sum / view.near : 0.0;
	view.volume.assign(t.seg_volume.begin(), t.seg_volume.end());
	signals(s, view);
	s.traffic_views.publish();
}

} // namespace traffic
} // namespace city
