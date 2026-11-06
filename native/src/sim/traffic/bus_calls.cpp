// A bus at its stop (kerbStops.ts planBusStop, the dwell of TCQSM: the doors, then each passenger in turn): one or two off by the middle door, on to the footway, and
// whoever walks alone along that side of the block called up to the front door and aboard. The bus goes when that is over. And how far any kerb door stands open.
#include "sim/traffic/kerb_stops.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
constexpr int LEAVING_FEWEST = 1, LEAVING_SPREAD = 2;

void begin(CityState &s, int id, BusCall &c) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	c.begun = true;
	c.leaving = LEAVING_FEWEST + (id + int(t.time)) % LEAVING_SPREAD;
	c.next_off = p.kerb_door_open_s;
	const V2 door = kerb_door_at(s, v, p.bus_front_door_m).door;
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const float mid = kerb_arc_of(d, std::max(0.f, v.s - p.bus_front_door_m)), reach = p.bus_fetch_m * p.bus_fetch_share;
	for (BusCall::Boarder &b : c.boarders) {
		const int ped = kerb_hailable(s, d, mid - reach, mid + reach);
		if (ped < 0 || !kerb_hail(s, ped, door, p.bus_fetch_m)) break;
		const Pedestrian &w = t.peds[size_t(ped)];
		b.ped = ped;
		b.from = V2(w.cur.x, w.cur.z);
		b.takes = (b.from - door).len() / p.kerb_pace_ms + p.bus_step_aboard_s;
		b.walked = 0;
	}
}
} // namespace

bool bus_call(CityState &s, int id, BusCall &c, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (!c.begun) begin(s, id, c);
	const Vehicle &v = t.vehicles[size_t(id)];
	c.t += dt;
	if (c.leaving > 0 && c.t >= c.next_off) {
		const KerbDoors at = kerb_door_at(s, v, p.classes[int(v.type)].length * p.kerb_door_along);
		if (kerb_walker_out(s, v.ds, at.foot) >= 0) t.bus_alighted++;
		c.leaving--;
		c.next_off += p.bus_passenger_s;
	}
	const V2 door = kerb_door_at(s, v, p.bus_front_door_m).door;
	bool walking = false;
	for (BusCall::Boarder &b : c.boarders) {
		if (b.ped < 0) continue;
		if (!kerb_holds(t, b.ped)) { // of a street that went
			b.ped = -1;
			continue;
		}
		b.walked += dt;
		const float share = std::min(1.f, b.walked / std::max(b.takes, dt));
		kerb_place(s, b.ped, b.from + (door - b.from) * share, door - b.from);
		if (share >= 1) {
			ped_gone(s, b.ped);
			t.bus_boarded++;
			b.ped = -1;
		} else {
			walking = true;
		}
	}
	return c.leaving == 0 && !walking;
}

void bus_call_over(CityState &s, BusCall &c) {
	for (BusCall::Boarder &b : c.boarders) {
		if (b.ped >= 0) kerb_let_go(s, b.ped, true);
		b.ped = -1;
	}
	c = BusCall();
}

uint32_t kerb_door_level(const CityState &s, const Vehicle &v) {
	if (v.bus >= 0) return uint32_t(v.bus_doors * float(LOOK_DOOR_MASK) + 0.5f);
	if (v.kerb < 0) return 0;
	const KerbStop &k = s.traffic.kerb_stops[size_t(v.kerb)];
	const TrafficParams &p = s.catalog.traffic;
	float open = 0;
	if (k.phase == KerbPhase::OPEN) open = k.t / p.kerb_door_open_s;
	else if (k.phase == KerbPhase::TRANSFER || k.phase == KerbPhase::AWAY) open = 1;
	else if (k.phase == KerbPhase::CLOSE) open = 1 - k.t / p.kerb_door_shut_s;
	return uint32_t(std::clamp(open, 0.f, 1.f) * float(LOOK_DOOR_MASK) + 0.5f);
}

} // namespace traffic
} // namespace city
