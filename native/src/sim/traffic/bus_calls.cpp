// A bus at its stop (the lab's kerbStops.ts planBusStop, the dwell of TCQSM: the doors, then each passenger in turn): one or two of
// its riders off by the middle door, stepping down to the footway and walking on from there; whoever walks alone along that side of
// the block called up to the front door while there are free seats, stepping in through the doorway and aboard. The bus goes when
// that is over. Its riders (Vehicle::riders) are who got on less who got off. And how far any kerb door stands open.
#include "sim/traffic/kerb_stops.h"

#include "core/random.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr int LEAVING_FEWEST = 1, LEAVING_SPREAD = 2; // one or two off at a stop (the lab's planBusStop: 1 + (id + age) % 2)
static_assert(LEAVING_FEWEST + LEAVING_SPREAD - 1 <= BUS_CALL_WALKERS, "a leaver with no place in the call");
// the riders roll's stream of the bus's hash (2^32 over the golden ratio), and the 24 bits a float holds exactly, for a share 0..1
constexpr uint32_t RIDERS_SALT = 0x9E3779B9u, RIDERS_SPAN = 0xFFFFFFu;

void begin(CityState &s, int id, BusCall &c) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	const Vehicle &v = t.vehicles[size_t(id)];
	c.begun = true;
	c.leaving = std::min(int(v.riders), LEAVING_FEWEST + (id + int(t.time)) % LEAVING_SPREAD);
	c.next_off = p.kerb_door_open_s;
	const KerbDoors front = kerb_door_at(s, v, p.bus_front_door_m);
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const float mid = kerb_arc_of(d, std::max(0.f, v.s - p.bus_front_door_m)), reach = p.bus_fetch_m * p.bus_fetch_share;
	const int room = int(p.bus_riders_most) - int(v.riders) + c.leaving; // the seats free once those leaving are off
	for (int k = 0; k < std::min(room, BUS_CALL_WALKERS); k++) {
		BusCall::Walker &b = c.boarders[k];
		const int ped = kerb_hailable(s, d, mid - reach, mid + reach);
		if (ped < 0 || !kerb_hail(s, ped, front.door, p.bus_fetch_m)) break;
		const Pedestrian &w = t.peds[size_t(ped)];
		b.ped = ped;
		b.from = V2(w.cur.x, w.cur.z);
		b.to = front.door;
		b.in = front.in;
		b.y = w.cur.y;
		b.step_y = front.floor + p.kerb_step_up_m;
		b.takes = (b.from - b.to).len() / p.kerb_pace_ms;
		b.walked = 0;
	}
}

// One of the riders off by the middle door: out on the footway's walker, set at the door, to walk to the footway; none when the
// footway has no room (they stay aboard).
void step_off(CityState &s, int id, BusCall &c) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	const KerbDoors at = kerb_door_at(s, v, p.classes[int(v.type)].length * p.kerb_door_along);
	BusCall::Walker *slot = nullptr;
	for (BusCall::Walker &w : c.leavers)
		if (w.ped < 0) slot = &w;
	if (!slot || v.riders == 0) return;
	const int ped = kerb_walker_out(s, v.ds, at.foot);
	if (ped < 0) return;
	v.riders--;
	t.bus_alighted++;
	*slot = BusCall::Walker{ ped, at.door, at.foot, at.door, 0.f, (at.foot - at.door).len() / p.kerb_pace_ms };
	kerb_place(s, ped, at.door, at.foot - at.door);
}

// A walker of the call a step on: false once it is through (or of a street that went).
bool walk(CityState &s, BusCall::Walker &w, float share) {
	if (!kerb_holds(s.traffic, w.ped)) {
		w.ped = -1;
		return false;
	}
	kerb_place(s, w.ped, w.from + (w.to - w.from) * share, w.to - w.from);
	return share < 1;
}
} // namespace

// A city bus is never empty: some of its seats taken at its birth (the lab's kerbStops.ts assignOccupancy).
void bus_riders_born(const CityState &s, Vehicle &v, int id) {
	const TrafficParams &p = s.catalog.traffic;
	const float share = float(hash_u32(uint32_t(id) ^ RIDERS_SALT) & RIDERS_SPAN) / float(RIDERS_SPAN);
	const float spread = p.bus_riders_most - p.bus_riders_least + 1;
	v.riders = uint8_t(std::min(p.bus_riders_most, p.bus_riders_least + std::floor(share * spread)));
}

bool bus_call(CityState &s, int id, BusCall &c, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (!c.begun) begin(s, id, c);
	Vehicle &v = t.vehicles[size_t(id)];
	c.t += dt;
	if (c.leaving > 0 && c.t >= c.next_off) {
		step_off(s, id, c);
		c.leaving--;
		c.next_off += p.bus_passenger_s;
	}
	bool walking = false;
	for (BusCall::Walker &w : c.leavers) {
		if (w.ped < 0) continue;
		w.walked += dt;
		if (walk(s, w, std::min(1.f, w.walked / std::max(w.takes, dt)))) {
			walking = true;
		} else if (w.ped >= 0) { // on the footway: walks on from there
			kerb_let_go(s, w.ped, true);
			w.ped = -1;
		}
	}
	for (BusCall::Walker &b : c.boarders) {
		if (b.ped < 0) continue;
		b.walked += dt;
		const float step = b.walked - b.takes; // seconds of the step in through the doorway, past the walk to the door
		if (step < 0) {
			walk(s, b, std::min(1.f, b.walked / std::max(b.takes, dt)));
			walking = walking || b.ped >= 0;
			continue;
		}
		if (!kerb_holds(t, b.ped)) {
			b.ped = -1;
			continue;
		}
		const float share = std::min(1.f, step / std::max(p.bus_step_aboard_s, dt));
		kerb_place(s, b.ped, b.to + (b.in - b.to) * share, b.in - b.to);
		t.peds[size_t(b.ped)].cur.y = b.y + (b.step_y - b.y) * share; // up the step (the lab's placeKerbPerson)
		if (share < 1) {
			walking = true;
			continue;
		}
		ped_gone(s, b.ped);
		t.bus_boarded++;
		v.riders = uint8_t(std::min(float(v.riders) + 1, p.bus_riders_most));
		b.ped = -1;
	}
	return c.leaving == 0 && !walking;
}

void bus_call_over(CityState &s, BusCall &c) {
	for (BusCall::Walker *list : { c.boarders, c.leavers })
		for (int k = 0; k < BUS_CALL_WALKERS; k++) {
			BusCall::Walker &w = list[k];
			if (w.ped >= 0) kerb_let_go(s, w.ped, true);
			w.ped = -1;
		}
	c = BusCall();
}

uint32_t riders_level(const Vehicle &v) { return v.type == VehicleType::BUS ? std::min(uint32_t(v.riders) + 1, LOOK_RIDERS_MASK) : 0; }

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
