// Somebody getting out of a car at the kerb or getting in, and a truck's mate away with a delivery. A vehicle with an errand
// plans a stop on the kerb lane of a street, clear of its ends and of any queue; it pulls up, the door opens when the swing is
// clear, the person moves between the door and the footway (a walker under the stop's hand, PedMode::KERB: kerb_walkers.cpp)
// and the door shuts. Set down, they walk on as any walker; picked up, the walker is gone and the car carries one more
// (Vehicle::aboard).
#include "sim/traffic/kerb_stops.h"

#include "core/random.h"
#include "sim/roads/parking_layout.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {
constexpr uint32_t ERRAND_SALT = 0x2545F491u, ERRAND_SPAN = 0xFFFFFFu;
constexpr float STOPPED_MS = 0.1f; // standing still, for a stop
constexpr float PULLED_UP_M = 1.f, GONE_BY_M = 2.f; // pulled up at its stop; gone past it
constexpr int RIGHT_SIDE = 0;

bool is_car(const Vehicle &v) { return v.type <= VehicleType::PICKUP; }

bool has_footway(const CityState &s, const DirSeg &d) {
	const Profile &p = s.roads.profile_of(d.seg);
	return kerb_side(d) == RIGHT_SIDE ? p.has_walk_right : p.has_walk_left;
}

} // namespace

// The arc along its street of metre `at` of a directed segment.
float kerb_arc_of(const DirSeg &d, float at) { return d.at0 + d.sign * at; }

// Where a kerb-side door opens, and the footway beside it.

namespace {
// The door of a vehicle of class `c` on lane `lane` of `ds` whose door stands at metre `at`, and the footway across from it.
KerbDoors doors_at(const CityState &s, int ds, int lane_i, const VehicleClass &c, float at) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	at = std::clamp(at, 0.f, d.len);
	Pose lane, foot;
	const float arc = kerb_arc_of(d, at);
	lane_pose(s, ds, lane_i, at, lane);
	sidewalk_pose(s, d.seg, kerb_side(d), arc, 0, foot);
	const V2 right(-lane.fz, lane.fx); // the world's z runs down the map: the side the kerb is on, -X of the vehicle
	return { V2(lane.x, lane.z) + right * (c.half_width + s.catalog.traffic.kerb_door_out_m), V2(foot.x, foot.z), arc };
}
} // namespace

KerbDoors kerb_door_at(const CityState &s, const Vehicle &v, float back_m) {
	return doors_at(s, v.ds, v.lane, s.catalog.traffic.classes[int(v.type)], v.s - back_m);
}

namespace {
KerbDoors door_places(const CityState &s, const Vehicle &v) {
	return kerb_door_at(s, v, s.catalog.traffic.classes[int(v.type)].length * s.catalog.traffic.kerb_door_along);
}

// Nobody in the door's swing or on the footway spot, and no vehicle along its side.
bool room_to_open(CityState &s, const Vehicle &v, const KerbStop &k) {
	const TrafficParams &p = s.catalog.traffic;
	const V2 door = door_places(s, v).door;
	if (k.errand != Errand::PICK && (walker_within(s, door.x, door.y, p.kerb_door_clear_m) || walker_within(s, k.foot.x, k.foot.y, p.kerb_foot_clear_m))) return false;
	for (int id : s.traffic.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane]) {
		const Vehicle &o = s.traffic.vehicles[size_t(id)];
		if (&o != &v && std::fabs(o.s - v.s) < p.kerb_door_clear_m + p.classes[int(o.type)].length) return false;
	}
	return true;
}

// Anybody standing or queued ahead of the stop would make it a queue, not a stop.
bool queued_ahead(const CityState &s, const Vehicle &v, float at) {
	const TrafficParams &p = s.catalog.traffic;
	for (int id : s.traffic.lane_vehicles[size_t(v.ds) * MAX_LANES + v.lane]) {
		const Vehicle &o = s.traffic.vehicles[size_t(id)];
		if (&o != &v && o.s > v.s && o.s - p.classes[int(o.type)].length < at + p.kerb_queue_m) return true;
	}
	return false;
}

int new_stop(TrafficState &t) {
	if (!t.free_kerb_stops.empty()) {
		const int k = t.free_kerb_stops.back();
		t.free_kerb_stops.pop_back();
		return k;
	}
	t.kerb_stops.emplace_back();
	return int(t.kerb_stops.size()) - 1;
}

// Chooses where to stop on the lane it is on, or waits for a later one.
void plan(CityState &s, int id) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.conn >= 0 || v.off || v.ds < 0 || v.lane != 0 || v.lc_off != 0 || v.stop_at >= 0) return;
	const DirSeg &d = t.g.ds[size_t(v.ds)];
	if (!has_footway(s, d) || v.ri + 1 >= v.route.len) return;
	const float brake = v.v * v.v / (2 * p.classes[int(v.type)].b);
	const float earliest = std::max(v.s + brake + p.kerb_plan_margin_m, p.kerb_end_clear_m), latest = d.len - p.kerb_end_clear_m;
	if (latest - earliest < p.kerb_stretch_m) return;
	float at = earliest + (latest - earliest) * p.kerb_stop_share;
	int ped = -1;
	if (v.errand == Errand::PICK) {
		ped = kerb_hailable(s, d, kerb_arc_of(d, earliest), kerb_arc_of(d, std::min(latest, earliest + p.kerb_find_reach_m)));
		if (ped < 0) return;
		const float along = (t.peds[size_t(ped)].s - d.at0) * d.sign + p.classes[int(v.type)].length * p.kerb_door_along;
		at = std::clamp(along, earliest, latest);
	} else {
		// set down or delivering: where the way between its door and the footway across from it passes no street piece (one called
		// finds its own way in clear of them, kerb_way_in; where the vehicle halts the foot is found again, kerb_foot)
		const VehicleClass &c = p.classes[int(v.type)];
		const float back = c.length * p.kerb_door_along;
		const auto clear = [&](float stop) {
			const KerbDoors there = doors_at(s, v.ds, v.lane, c, stop - back);
			return way_clear(s, d.seg, kerb_side(d), there.door, there.foot, p.ped_body_m);
		};
		while (at <= latest && !clear(at)) at += p.ped_body_m;
		if (at > latest) return;
	}
	if (queued_ahead(s, v, at)) return;
	v.kerb = new_stop(t);
	t.kerb_planned++;
	KerbStop &k = t.kerb_stops[size_t(v.kerb)];
	k = KerbStop();
	k.vehicle = id;
	k.errand = v.errand;
	k.ds = v.ds;
	k.lane = v.lane;
	k.at = at;
	k.ped = ped;
	v.errand = Errand::NONE;
}

void release_walker(CityState &s, KerbStop &k, bool on_foot) {
	if (k.ped >= 0) kerb_let_go(s, k.ped, on_foot);
	k.ped = -1;
}

void end_stop(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	release_walker(s, t.kerb_stops[size_t(v.kerb)], true);
	t.free_kerb_stops.push_back(v.kerb);
	t.kerb_stops[size_t(v.kerb)].vehicle = -1;
	v.kerb = -1;
}

// The stop moves to `phase` by its one setter (kerb_phase.cpp), the phase's clock from nought.
void enter(KerbStop &k, KerbPhase phase) {
	if (enter_kerb_phase(k, phase)) k.t = 0;
}

// The person moving between the door and its foot on the footway, by the share of the way: out from where its walk started (k.from:
// the door, or where it stood when the foot moved), back from the foot to the door; heading the way it goes.
void place_walker(CityState &s, const KerbStop &k, const V2 &door, float share) {
	const V2 start = k.back ? door : k.from;
	if (k.ped >= 0) kerb_place(s, k.ped, start + (k.foot - start) * share, k.back ? door - k.foot : k.foot - start);
}

// The street's furniture was placed again since its foot was found (a lot nearby changed: TrafficState::furniture_serial): the foot is found
// again from the door, clear of the pieces as they stand now; a person walking out goes on from where it stands to it (its walk started
// again there, no jump), with no clear place left it goes on to the old one.
void refresh_foot(CityState &s, const Vehicle &v, KerbStop &k) {
	const TrafficState &t = s.traffic;
	if (k.errand == Errand::PICK || k.back || k.foot_serial == t.furniture_serial) return;
	k.foot_serial = t.furniture_serial;
	V2 foot;
	if (!kerb_foot(s, v.ds, door_places(s, v), foot) || (foot - k.foot).len() == 0.f) return;
	k.foot = foot;
	if (k.phase != KerbPhase::TRANSFER || k.ped < 0) return;
	const Pedestrian &w = t.peds[size_t(k.ped)];
	k.from = V2(w.cur.x, w.cur.z);
	k.t = s.catalog.traffic.kerb_seat_s; // up from the seat already: it walks from where it stands
	k.transfer = k.t + (k.foot - k.from).len() / s.catalog.traffic.kerb_pace_ms;
}

// How far from the door toward the footway the person is in a transfer: out, it rises from the seat first (kerb_seat_s at
// the door) and then walks; back (a truck's mate), it walks first and then sits.
float share_out(const TrafficParams &p, const KerbStop &k, float dt) {
	const float walk = std::max(k.transfer - p.kerb_seat_s, dt);
	if (k.back) return 1 - std::min(1.f, k.t / walk);
	return std::clamp((k.t - p.kerb_seat_s) / walk, 0.f, 1.f);
}

void transfer_done(CityState &s, Vehicle &v, KerbStop &k) {
	TrafficState &t = s.traffic;
	if (k.errand == Errand::DROP) {
		v.aboard = ABOARD_SET_DOWN;
		t.kerb_drops++;
		release_walker(s, k, true);
		enter(k, KerbPhase::CLOSE);
	} else if (k.errand == Errand::PICK) {
		v.aboard = ABOARD_PICKED_UP;
		t.kerb_picks++;
		release_walker(s, k, false);
		enter(k, KerbPhase::CLOSE);
	} else if (k.back) { // the truck's mate is in again
		t.kerb_deliveries++;
		release_walker(s, k, false);
		enter(k, KerbPhase::CLOSE);
	} else {
		enter(k, KerbPhase::AWAY);
	}
}

// How long the move between the seat and the footway takes, walking at the kerb's pace.
float transfer_time(const CityState &s, const KerbStop &k, const V2 &door) {
	const TrafficParams &p = s.catalog.traffic;
	return p.kerb_seat_s + (k.errand == Errand::PICK ? 0.f : (k.foot - door).len() / p.kerb_pace_ms);
}

void halted(CityState &s, int id, KerbStop &k) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	if (k.t <= p.kerb_halt_s) return;
	if (k.errand == Errand::PICK) { // the one it stopped for sees it and walks up along its sidewalk (kerb_call)
		const KerbDoors at = door_places(s, v);
		if (!kerb_call(s, k.ped, at.door, at.arc, p.kerb_pick_fetch_m)) {
			k.ped = -1;
			end_stop(s, id);
			return;
		}
		k.came = false;
		enter(k, KerbPhase::FETCH);
	} else if (room_to_open(s, v, k)) {
		enter(k, KerbPhase::OPEN);
	}
}

void step_stop(CityState &s, int id, KerbStop &k, float dt) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	k.t += dt;
	k.elapsed += dt;
	if (k.ped >= 0 && k.phase > KerbPhase::HALT && kerb_hold(t, k.ped) == KerbHold::LOST) { // a walker of a street that went, or frightened off
		k.ped = -1;
		end_stop(s, id);
		return;
	}
	if (v.ds != k.ds || v.lane != k.lane || v.conn >= 0) { // it left the lane it meant to stop on
		end_stop(s, id);
		return;
	}
	if (k.phase != KerbPhase::APPROACH && k.phase != KerbPhase::TRANSFER && k.phase != KerbPhase::CLOSE && k.t > p.kerb_give_up_s + (k.phase == KerbPhase::AWAY ? p.kerb_delivery_s : 0.f)) {
		t.kerb_abandoned++;
		end_stop(s, id);
		return;
	}
	if (k.phase == KerbPhase::HALT || k.phase == KerbPhase::OPEN || k.phase == KerbPhase::TRANSFER) refresh_foot(s, v, k);
	switch (k.phase) {
		case KerbPhase::APPROACH:
			if (v.v < STOPPED_MS && k.at - v.s < PULLED_UP_M) {
				k.at = v.s;
				// the foot from the door where it halted, not where it planned to (with no clear place there they stay aboard)
				if (k.errand != Errand::PICK && !kerb_foot(s, v.ds, door_places(s, v), k.foot)) {
					end_stop(s, id);
					return;
				}
				k.foot_serial = t.furniture_serial;
				enter(k, KerbPhase::HALT);
			} else if (v.s > k.at + GONE_BY_M) {
				end_stop(s, id);
			}
			break;
		case KerbPhase::HALT: halted(s, id, k); break;
		case KerbPhase::FETCH: { // along its sidewalk to the door's arc (the give-up above holds the wait), then in by its way clear of the pieces
			const V2 door = door_places(s, v).door;
			if (!k.came) {
				if (kerb_hold(t, k.ped) == KerbHold::COMING) break;
				if (!kerb_way_in(s, k.ped, door, k.foot)) { // no clear way in from where it came to: it walks on, the car drives on
					end_stop(s, id);
					return;
				}
				const Pedestrian &w = t.peds[size_t(k.ped)];
				k.came = true;
				k.from = V2(w.cur.x, w.cur.z);
				k.walked = 0;
				k.fetch = kerb_way_len(k.from, k.foot, door) / p.kerb_pace_ms;
			}
			k.walked += dt;
			V2 heading;
			kerb_place(s, k.ped, kerb_way_at(k.from, k.foot, door, k.walked * p.kerb_pace_ms, heading), heading);
			if (k.walked >= k.fetch) enter(k, KerbPhase::OPEN);
			break;
		}
		case KerbPhase::OPEN:
			if (k.t < p.kerb_door_open_s) break;
			k.transfer = transfer_time(s, k, door_places(s, v).door);
			if (k.ped < 0 && k.errand != Errand::PICK) {
				const V2 door = door_places(s, v).door;
				k.ped = kerb_walker_out(s, v.ds, k.foot);
				if (k.ped < 0) {
					end_stop(s, id); // no room on the footway: they stay aboard
					return;
				}
				k.from = door; // the walk out starts at the door
				kerb_show(s, k.ped, door, k.foot - door);
			}
			enter(k, KerbPhase::TRANSFER);
			break;
		case KerbPhase::TRANSFER: {
			if (k.errand != Errand::PICK) place_walker(s, k, door_places(s, v).door, share_out(p, k, dt));
			if (k.t >= k.transfer) transfer_done(s, v, k);
			break;
		}
		case KerbPhase::AWAY:
			if (k.t >= p.kerb_delivery_s) {
				k.back = true;
				k.transfer = (k.foot - door_places(s, v).door).len() / p.kerb_pace_ms + p.kerb_seat_s;
				enter(k, KerbPhase::TRANSFER);
			}
			break;
		case KerbPhase::CLOSE:
			if (k.t >= p.kerb_door_shut_s) end_stop(s, id);
			break;
	}
}
} // namespace

void assign_errand(const CityState &s, Vehicle &v, int id) {
	const TrafficParams &p = s.catalog.traffic;
	const float roll = float(hash_u32(uint32_t(id) ^ ERRAND_SALT) & ERRAND_SPAN) / float(ERRAND_SPAN);
	if (is_car(v)) v.errand = roll < p.kerb_drop_share ? Errand::DROP : (roll > 1 - p.kerb_pick_share ? Errand::PICK : Errand::NONE);
	else if (v.type == VehicleType::TRUCK && roll < p.kerb_delivery_share) v.errand = Errand::DELIVER;
	if (v.errand == Errand::DROP) v.aboard = ABOARD_TO_SET_DOWN;
}

float kerb_stop_gap(const TrafficState &t, const Vehicle &v) {
	return std::max(0.f, t.kerb_stops[size_t(v.kerb)].at - v.s);
}

void kerb_forget(CityState &s, int id) {
	if (s.traffic.vehicles[size_t(id)].kerb >= 0) end_stop(s, id);
}

void step_kerb_stops(CityState &s, float dt) {
	TrafficState &t = s.traffic;
	for (int id : t.micro_list)
		if (t.vehicles[size_t(id)].errand != Errand::NONE && t.vehicles[size_t(id)].kerb < 0) plan(s, id);
	for (size_t i = 0; i < t.kerb_stops.size(); i++) {
		KerbStop &k = t.kerb_stops[i];
		if (k.vehicle >= 0) step_stop(s, k.vehicle, k, dt);
	}
}

} // namespace traffic
} // namespace city
