// `probes` area (`junction_facts`, `kerb_call`): a junction as the traffic rules it now, for the junctions' test
// (project/tests/core/junctions_test.gd): its control, whether it is a mini-roundabout's circle, each arriving arm's rank, stage,
// turn stage, light and crossing lamp; a signal's stages (how many, the turn stages, the walkers' own, the stage running, whether
// every turn stage serves an approach of an ordinary stage and every crossing walks somewhere); the gap an ordinary driver asks by
// turn, fresh and after a long wait. `kerb_call` says a walker waits at a crossing's kerb, as a held walker does.
#include "api/city_probes.h"

#include "sim/simulation/simulation.h"
#include "sim/state/city_state.h"
#include "sim/traffic/motion_law.h"
#include "sim/traffic/signal_plan.h"
#include "sim/traffic/traffic_system.h"

#include <algorithm>
#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float LONG_WAIT_S = 1000.f; // a wait past every impatience: the gap at its floor

// The gaps an ordinary driver asks: [right, straight on, left, left off the road that has the way], waiting `waited` seconds.
Array gaps_of(const TrafficParams &tp, float waited) {
	Vehicle ordinary;
	ordinary.wait = waited;
	return Array::make(traffic::turn_gap(tp, ordinary, Turn::RIGHT, false), traffic::turn_gap(tp, ordinary, Turn::STRAIGHT, false),
			traffic::turn_gap(tp, ordinary, Turn::LEFT, false), traffic::turn_gap(tp, ordinary, Turn::LEFT, true));
}

// A signal's plan in numbers: its stages, those of turns, the walkers' own, and whether the plan holds together.
void put_plan(const CityState &s, int node, const Junction &j, Dictionary &d) {
	int turn_stages = 0, walk_only = 0;
	bool turns_served = true, walked_all = true;
	for (size_t k = 0; k < j.approaches.size(); k++) {
		const int of = j.turn_of[k];
		turn_stages += of >= 0 ? 1 : 0;
		walk_only += j.approaches[k].empty() ? 1 : 0;
		turns_served = turns_served && (of < 0 || (s.traffic.g.ds[size_t(of)].stage >= 0 && j.turn_of[size_t(s.traffic.g.ds[size_t(of)].stage)] < 0));
	}
	for (int seg : s.roads.nodes[size_t(node)].segs) {
		if (!s.roads.crosswalk_on(node, seg)) continue;
		bool walked = false;
		for (const std::vector<int> &walks : j.walks) walked = walked || std::find(walks.begin(), walks.end(), seg) != walks.end();
		walked_all = walked_all && walked;
	}
	d["stages"] = int(j.green.size());
	d["turn_stages"] = turn_stages;
	d["walk_only"] = walk_only;
	d["turns_served"] = turns_served;
	d["walked_all"] = walked_all;
	d["run"] = j.run;
	d["green_running"] = j.run_green < 0;
}
} // namespace

Dictionary CityProbes::junction_facts(int node) {
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const LaneGraph &g = s.traffic.g;
	Dictionary d;
	if (node < 0 || size_t(node) >= g.junctions.size() || size_t(node) >= s.roads.nodes.size() || !s.roads.nodes[size_t(node)].alive) return d;
	const Junction &j = g.junctions[size_t(node)];
	d["control"] = int(j.ctl);
	d["circle"] = j.circle;
	Array arms;
	for (int seg : s.roads.nodes[size_t(node)].segs) {
		const int dir = s.roads.segs[size_t(seg)].b == node ? 0 : 1, ds = seg * 2 + dir;
		if (size_t(ds) >= g.ds.size() || !g.ds[size_t(ds)].ok) continue;
		Dictionary a;
		a["seg"] = seg;
		a["dir"] = dir;
		a["rank"] = g.ds[size_t(ds)].rank;
		a["stage"] = g.ds[size_t(ds)].stage;
		a["turn_stage"] = g.ds[size_t(ds)].turn_stage;
		a["light"] = int(traffic::light_for(s, ds));
		a["lamp"] = j.ctl == Control::SIGNAL ? int(traffic::walk_lamp(s, node, seg)) : int(traffic::WalkLamp::WALK);
		a["crossing"] = s.roads.crosswalk_on(node, seg);
		arms.push_back(a);
	}
	d["arms"] = arms;
	if (j.ctl == Control::SIGNAL && !j.green.empty()) put_plan(s, node, j, d);
	d["gaps"] = gaps_of(s.catalog.traffic, 0.f);
	d["gaps_waited"] = gaps_of(s.catalog.traffic, LONG_WAIT_S);
	return d;
}

void CityProbes::kerb_call(int node, int seg, float waited) {
	if (!sim_->hold_still()) return;
	CityState &s = *state_;
	map_hold::WriterHold map(s.map_mutex, map_hold::Writer::PROBE);
	std::lock_guard<std::mutex> tr(s.traffic_mutex);
	if (node < 0 || size_t(node) >= s.roads.nodes.size() || !s.roads.nodes[size_t(node)].alive) return;
	traffic::kerb_word(s, node, seg, waited);
}

} // namespace city
