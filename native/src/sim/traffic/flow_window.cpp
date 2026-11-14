// The window's side of the far traffic (street_flows.h): a street coming into the window hands its vehicles to the detailed
// simulation, which makes them a few a tick (demand.cpp make_vehicle); a street leaving it takes the detailed vehicles on it back;
// whoever drives out of the window onto a street of the flows is counted in at its start. A bus is its timetable's: it goes to
// its lane's far queue (meso.cpp), never into the flows.
#include "sim/traffic/flow_parts.h"

namespace city {
namespace traffic {

namespace {

// A detailed vehicle on a street the window does not hold is the far traffic's from here on.
void hand_back(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.bus >= 0) {
		meso_enter(s, id, v.ds * MAX_LANES + v.lane, true); // leaves its lane list and micro_list there
		return;
	}
	flow_take(t.flows, v.ds);
	despawn_vehicle(s, id, false, false);
}

} // namespace

void flow_enters(CityState &s, int ds) {
	StreetFlows &f = s.traffic.flows;
	const size_t k = size_t(ds);
	if (k >= f.live.size() || !f.live[k] || f.inside[k]) return;
	// the far cars drawn on it a moment ago become vehicles, made anywhere along it; as whole ones, a street's fraction one more
	// with that chance: most streets carry under one vehicle, and their sum must come in with them
	const float n = flow_whole(s.traffic, flow_count(f, ds));
	f.inside[k] = 1;
	f.detailed[k] = 0;
	f.owed[k] = f.along[k] = n;
	f.handed_out += double(n);
	flow_owes(f, ds);
}

void flow_leaves(CityState &s, int ds) {
	TrafficState &t = s.traffic;
	StreetFlows &f = t.flows;
	const size_t k = size_t(ds);
	if (k >= f.live.size() || !f.inside[k]) return;
	const float owed = f.owed[k];
	f.inside[k] = 0;
	f.owed[k] = f.along[k] = 0;
	f.detailed[k] = 0;
	if (!f.live[k]) return;
	// the vehicles on its lanes are the flows' again, with those the window had not made yet; laid along it as in free flow
	int taken = 0;
	for (int lane = 0; lane < t.g.ds[k].lanes; lane++) {
		std::vector<int> &list = t.lane_vehicles[k * MAX_LANES + size_t(lane)];
		while (!list.empty()) {
			const int id = list.back();
			const bool bus = t.vehicles[size_t(id)].bus >= 0;
			hand_back(s, id); // takes it off the list
			if (!list.empty() && list.back() == id) list.pop_back(); // never the same one twice
			taken += bus ? 0 : 1;
		}
	}
	// hand_back counted each one in at the street's start: they are laid along it instead, with the rest
	lay_flow(s, ds, std::min(f.store[k], float(taken) + owed));
}

void flow_begin(CityState &s) {
	TrafficState &t = s.traffic;
	StreetFlows &f = t.flows;
	for (int i = int(t.micro_list.size()) - 1; i >= 0; i--) {
		const int id = t.micro_list[size_t(i)];
		const Vehicle &v = t.vehicles[size_t(id)];
		// on a street the window does not hold, on its lanes or off them (a lot's car, a lorry: its lot is let go with it)
		if (!v.alive || v.conn >= 0 || v.ds < 0 || size_t(v.ds) >= f.live.size() || f.inside[size_t(v.ds)]) continue;
		hand_back(s, id);
	}
	// the detailed vehicles on each street of the window, for the room it has left
	for (int ds : f.counted) f.detailed[size_t(ds)] = 0;
	f.counted.clear();
	for (int id : t.micro_list) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (!v.alive || v.off) continue;
		const int ds = v.conn >= 0 ? t.g.conns[size_t(v.conn)].to_ds : v.ds; // on a connector: it takes room on the street it enters
		if (ds < 0 || size_t(ds) >= f.live.size() || !f.inside[size_t(ds)]) continue;
		if (f.detailed[size_t(ds)]++ == 0) f.counted.push_back(ds);
	}
}

void flow_admit(CityState &s) {
	StreetFlows &f = s.traffic.flows;
	int tries = int(s.catalog.traffic.flow_admit_per_tick);
	// from the list's end, a street that could not take its vehicle now going to the front: every street has its turn
	for (int looked = int(f.owing.size()); looked > 0 && tries > 0 && !f.owing.empty(); looked--) {
		const int ds = f.owing.back();
		const size_t k = size_t(ds);
		if (!f.inside[k] || f.owed[k] < 1) {
			f.in_owing[k] = 0;
			f.owing.pop_back();
			continue;
		}
		if (!may_request_search(s)) return; // a new trip's search waits for the next tick past the tick's searches
		tries--;
		const bool along = f.along[k] >= 1;
		if (make_vehicle(s, ds, along) >= 0) {
			f.owed[k] -= 1;
			if (along) f.along[k] -= 1;
		} else {
			std::rotate(f.owing.begin(), f.owing.end() - 1, f.owing.end());
		}
	}
}

} // namespace traffic
} // namespace city
