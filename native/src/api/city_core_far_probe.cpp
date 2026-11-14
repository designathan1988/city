// `probes` area (`far_traffic`): the far traffic's flows and the window of detailed traffic as the tests read them
// (project/tests/core/flows_test.gd): the streets the flows hold and those the window holds, the vehicles on the far streets, in
// their queues, waiting at their origins and owed to the window, the streets holding more than their room, where the window
// stands and how far it reaches, and the flows' tallies since the city opened. `reach` (0 or more) sets the view as a camera
// would (the ground looked at, `at`, and how far the eye stands from it), so a test moves the window without one.
#include "api/city_probes.h"

#include "sim/state/city_state.h"

#include <mutex>
#include <shared_mutex>

using namespace godot;

namespace city {

namespace {
constexpr float ROOM_SLACK = 0.5f; // a street's count may stand this much over its room: half a vehicle of the model's rounding
} // namespace

Dictionary CityProbes::far_traffic(const Vector3 &at, float reach) {
	if (reach >= 0) {
		state_->camera_x.store(at.x);
		state_->camera_z.store(at.z);
		state_->camera_reach.store(reach);
	}
	std::shared_lock<std::shared_mutex> lk(state_->map_mutex);
	std::lock_guard<std::mutex> tr(state_->traffic_mutex);
	const CityState &s = *state_;
	const TrafficState &t = s.traffic;
	const StreetFlows &f = t.flows;
	int streets = 0, far = 0, over = 0, detailed = 0;
	double on_far = 0, queued = 0, origins = 0, owed = 0;
	for (int ds = 0; ds < int(f.live.size()); ds++) {
		if (!f.live[size_t(ds)]) continue;
		streets++;
		origins += double(f.origin[size_t(ds)]);
		if (f.inside[size_t(ds)]) {
			owed += double(f.owed[size_t(ds)]);
			continue;
		}
		far++;
		const float count = traffic::flow_count(f, ds);
		on_far += double(count);
		queued += double(traffic::flow_queued(s, ds));
		over += count > f.store[size_t(ds)] + ROOM_SLACK ? 1 : 0;
	}
	for (int id : t.micro_list) detailed += t.vehicles[size_t(id)].alive && !t.vehicles[size_t(id)].off ? 1 : 0;
	Dictionary d;
	d["streets"] = streets;
	d["far_streets"] = far;
	d["window_streets"] = int(t.window_ds.size());
	d["ways_out"] = int(t.window_out_ds.size());
	d["on_far"] = on_far;
	d["queued"] = queued;
	d["at_origins"] = origins;
	d["owed"] = owed;
	d["over_room"] = over;
	d["detailed"] = detailed;
	d["window"] = Array::make(t.win_x, t.win_z, t.win_r);
	d["steps"] = int64_t(f.steps);
	d["late"] = f.late;
	d["started"] = f.started;
	d["unmade"] = f.unmade;
	d["ended"] = f.ended;
	d["handed_out"] = f.handed_out;
	d["handed_in"] = f.handed_in;
	return d;
}

} // namespace city
