// A bus at its stop (the dwell of TCQSM: the doors, then each passenger in turn): one or two of those aboard step down from the middle
// door and walk out on to the footway, then walk on as any walker; whoever walks alone along that side of the block is called up to the
// front door, steps up and is aboard. The bus counts who rides in it (Bus::riders): off ones less, on ones more, never past its room.
// The bus goes when that is over. And how far any kerb door stands open, and how many ride in a bus, for the look.
#include "sim/traffic/kerb_stops.h"

#include "sim/state/city_state.h"

#include <algorithm>

namespace city {
namespace traffic {

namespace {
constexpr int LEAVING_FEWEST = 1, LEAVING_SPREAD = 2;
static_assert(LEAVING_FEWEST + LEAVING_SPREAD - 1 <= BUS_CALL_WALKERS, "everybody leaving at a stop has a mover");

V2 middle_door(const CityState &s, const Vehicle &v) {
	const TrafficParams &p = s.catalog.traffic;
	return kerb_door_at(s, v, p.classes[int(v.type)].length * p.kerb_door_along).door;
}

V2 front_door(const CityState &s, const Vehicle &v) { return kerb_door_at(s, v, s.catalog.traffic.bus_front_door_m).door; }

void begin(CityState &s, int id, BusCall &c, int riders) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	c.begun = true;
	c.leaving = std::min(riders, LEAVING_FEWEST + (id + int(t.time)) % LEAVING_SPREAD);
	c.next_off = p.kerb_door_open_s;
	const V2 door = front_door(s, v);
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const float mid = kerb_arc_of(d, std::max(0.f, v.s - p.bus_front_door_m)), reach = p.bus_fetch_m * p.bus_fetch_share;
	int room = int(p.bus_riders_max) - riders; // those getting off may find no room on the footway and stay aboard
	for (BusCall::Mover &b : c.boarders) {
		if (room <= 0) break;
		const int ped = kerb_hailable(s, d, mid - reach, mid + reach);
		if (ped < 0 || !kerb_hail(s, ped, door, p.bus_fetch_m)) break;
		const Pedestrian &w = t.peds[size_t(ped)];
		b.ped = ped;
		b.foot = V2(w.cur.x, w.cur.z);
		b.takes = (b.foot - door).len() / p.kerb_pace_ms + p.bus_step_aboard_s;
		b.walked = 0;
		room--;
	}
}

// Somebody out of the middle door: a walker born on the footway beside it, set at the door to step down and walk out.
bool step_off(CityState &s, const Vehicle &v, BusCall &c) {
	const TrafficParams &p = s.catalog.traffic;
	BusCall::Mover *free = nullptr;
	for (BusCall::Mover &a : c.alighters)
		if (a.ped < 0) free = &a;
	if (!free) return false;
	const KerbDoors at = kerb_door_at(s, v, p.classes[int(v.type)].length * p.kerb_door_along);
	const int ped = kerb_walker_out(s, v.ds, at.foot);
	if (ped < 0) return false; // no room on the footway: they stay aboard
	const Pedestrian &w = s.traffic.peds[size_t(ped)];
	free->ped = ped;
	free->foot = V2(w.cur.x, w.cur.z); // its own place on the footway: let go there, it walks on from where it stands
	free->walked = 0;
	free->takes = p.bus_step_down_s + (free->foot - at.door).len() / p.kerb_pace_ms;
	kerb_place(s, ped, at.door, free->foot - at.door);
	return true;
}

// Each mover a step further: true while any is still on the way.
bool move_alighters(CityState &s, const Vehicle &v, BusCall &c, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	const V2 door = middle_door(s, v);
	bool moving = false;
	for (BusCall::Mover &a : c.alighters) {
		if (a.ped < 0) continue;
		if (!kerb_holds(s.traffic, a.ped)) { // of a street that went
			a.ped = -1;
			continue;
		}
		a.walked += dt;
		const float walk = std::max(a.takes - p.bus_step_down_s, dt);
		const float share = std::clamp((a.walked - p.bus_step_down_s) / walk, 0.f, 1.f); // standing in the door first, stepping down
		kerb_place(s, a.ped, door + (a.foot - door) * share, a.foot - door);
		if (a.walked >= a.takes) {
			kerb_let_go(s, a.ped, true);
			s.traffic.bus_set_down++;
			a.ped = -1;
		} else {
			moving = true;
		}
	}
	return moving;
}

bool move_boarders(CityState &s, const Vehicle &v, BusCall &c, int &riders, float dt) {
	TrafficState &t = s.traffic;
	const V2 door = front_door(s, v);
	bool moving = false;
	for (BusCall::Mover &b : c.boarders) {
		if (b.ped < 0) continue;
		if (!kerb_holds(t, b.ped)) { // of a street that went
			b.ped = -1;
			continue;
		}
		b.walked += dt;
		const float share = std::min(1.f, b.walked / std::max(b.takes, dt));
		kerb_place(s, b.ped, b.foot + (door - b.foot) * share, door - b.foot);
		if (share >= 1) {
			ped_gone(s, b.ped);
			t.bus_boarded++;
			riders++;
			b.ped = -1;
		} else {
			moving = true;
		}
	}
	return moving;
}
} // namespace

bool bus_call(CityState &s, int id, BusCall &c, int &riders, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (!c.begun) begin(s, id, c, riders);
	const Vehicle &v = t.vehicles[size_t(id)];
	c.t += dt;
	if (c.leaving > 0 && c.t >= c.next_off) {
		if (step_off(s, v, c)) {
			t.bus_alighted++;
			riders--;
		}
		c.leaving--;
		c.next_off += p.bus_passenger_s;
	}
	const bool off = move_alighters(s, v, c, dt), on = move_boarders(s, v, c, riders, dt);
	return c.leaving == 0 && !off && !on;
}

void bus_call_over(CityState &s, BusCall &c) {
	for (BusCall::Mover &a : c.alighters)
		if (a.ped >= 0) kerb_let_go(s, a.ped, true); // out of the door already: on the footway
	for (BusCall::Mover &b : c.boarders)
		if (b.ped >= 0) kerb_let_go(s, b.ped, true);
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

uint32_t bus_riders_level(const CityState &s, const Vehicle &v) {
	if (v.bus < 0 || size_t(v.bus) >= s.transit_run.buses.size()) return LOOK_RIDERS_AS_DRAWN;
	return uint32_t(std::clamp(s.transit_run.buses[size_t(v.bus)].riders, 0, int(LOOK_RIDERS_MASK) - 1)) + 1;
}

} // namespace traffic
} // namespace city
