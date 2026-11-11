// A bus at its stop (the dwell of TCQSM: the doors, then each passenger in turn): one or two off by the middle door, on to the footway, and
// whoever walks alone along that side of the block called up to the front door and aboard: along its sidewalk as any walker to the door's arc,
// then across to its foot and in, clear of the street's pieces (kerb_walkers.cpp). The bus goes when that is over. And how far any kerb door
// stands open.
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
	const KerbDoors front = kerb_door_at(s, v, p.bus_front_door_m);
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	const float reach = p.bus_fetch_m * p.bus_fetch_share;
	int there = 0;
	kerb_hailable(s, d, front.arc - reach, front.arc + reach, &there);
	t.bus_in_reach += there;
	for (BusCall::Boarder &b : c.boarders) {
		const int ped = kerb_hailable(s, d, front.arc - reach, front.arc + reach); // each a different one: one called is not again
		if (ped < 0 || !kerb_call(s, ped, front.door, front.arc, p.bus_fetch_m)) break;
		b = BusCall::Boarder();
		b.ped = ped;
		t.bus_called++;
	}
}

// Somebody called on its way: still walking to the front door's arc (within kerb_give_up_s: one held up longer is let go), there taken on its
// way in, or lost to the call. False when it is no longer the call's.
bool boarder_comes(CityState &s, BusCall::Boarder &b, const V2 &door) {
	const TrafficParams &p = s.catalog.traffic;
	const KerbHold hold = kerb_hold(s.traffic, b.ped);
	if (hold == KerbHold::COMING && b.walked <= p.kerb_give_up_s) return true;
	s.traffic.bus_came += hold == KerbHold::HELD ? 1 : 0;
	if (hold != KerbHold::HELD || !kerb_way_in(s, b.ped, door, b.foot)) { // gone, frightened off, held up too long, or no clear way in
		if (hold != KerbHold::LOST) kerb_let_go(s, b.ped, true);
		return false;
	}
	const Pedestrian &w = s.traffic.peds[size_t(b.ped)];
	b.came = true;
	b.from = V2(w.cur.x, w.cur.z);
	b.walked = 0;
	b.takes = kerb_way_len(b.from, b.foot, door) / p.kerb_pace_ms + p.bus_step_aboard_s;
	return true;
}

// The one getting off: a step down at the door, then across to the footway at the kerb's pace, and a walker from there.
void step_off(CityState &s, BusCall &c, float dt) {
	const TrafficParams &p = s.catalog.traffic;
	BusCall::Boarder &b = c.leaver;
	if (!kerb_holds(s.traffic, b.ped)) { // of a street that went
		b.ped = -1;
		return;
	}
	b.walked += dt;
	const float share = std::clamp((b.walked - p.bus_step_aboard_s) / std::max(b.takes - p.bus_step_aboard_s, dt), 0.f, 1.f);
	kerb_place(s, b.ped, b.from + (c.leaver_to - b.from) * share, c.leaver_to - b.from);
	if (share < 1) return;
	kerb_let_go(s, b.ped, true);
	b.ped = -1;
}
} // namespace

bool bus_call(CityState &s, int id, BusCall &c, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	if (!c.begun) begin(s, id, c);
	const Vehicle &v = t.vehicles[size_t(id)];
	c.t += dt;
	if (c.leaving > 0 && c.leaver.ped < 0 && c.t >= c.next_off) { // the next one off, once the one before is on the footway
		const KerbDoors at = kerb_door_at(s, v, p.classes[int(v.type)].length * p.kerb_door_along);
		V2 foot;
		const int ped = kerb_foot(s, v.ds, at, foot) ? kerb_walker_out(s, v.ds, foot) : -1;
		if (ped >= 0) { // it steps out at the middle door and walks to its foot on the footway, clear of the street's pieces (with no room it stays aboard)
			kerb_show(s, ped, at.door, foot - at.door);
			c.leaver.ped = ped;
			c.leaver.from = at.door;
			c.leaver.walked = 0;
			c.leaver.takes = p.bus_step_aboard_s + (foot - at.door).len() / p.kerb_pace_ms;
			c.leaver_to = foot;
			t.bus_alighted++;
		}
		c.leaving--;
		c.next_off += p.bus_passenger_s;
	}
	if (c.leaver.ped >= 0) step_off(s, c, dt);
	const V2 door = kerb_door_at(s, v, p.bus_front_door_m).door;
	bool walking = false;
	for (BusCall::Boarder &b : c.boarders) {
		if (b.ped < 0) continue;
		b.walked += dt;
		if (b.came ? !kerb_holds(t, b.ped) : !boarder_comes(s, b, door)) { // of a street that went, or no longer coming
			b.ped = -1;
			continue;
		}
		if (!b.came) { // still on its way along its sidewalk
			walking = true;
			continue;
		}
		V2 heading;
		const V2 at = kerb_way_at(b.from, b.foot, door, b.walked * p.kerb_pace_ms, heading);
		kerb_place(s, b.ped, at, heading);
		if (b.walked >= b.takes) { // at the door, and the step aboard
			ped_gone(s, b.ped);
			t.bus_boarded++;
			b.ped = -1;
		} else {
			walking = true;
		}
	}
	return c.leaving == 0 && !walking && c.leaver.ped < 0;
}

void bus_call_over(CityState &s, BusCall &c) {
	for (BusCall::Boarder &b : c.boarders) {
		if (b.ped >= 0) kerb_let_go(s, b.ped, true);
		b.ped = -1;
	}
	if (c.leaver.ped >= 0) kerb_let_go(s, c.leaver.ped, true);
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
