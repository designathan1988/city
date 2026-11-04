// Somebody getting out of a car at the kerb or getting in, and a truck's mate away with a delivery. A vehicle with an errand
// plans a stop on the kerb lane of a street, clear of its ends and of any queue; it pulls up, the door opens when the swing is
// clear, the person moves between the door and the footway (a walker under the stop's hand, PedMode::KERB) and the door shuts.
// Set down, they walk on as any walker; picked up, the walker is gone and the car carries one more (Vehicle::aboard).
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
constexpr float NEAR_STOP_M = 1.f, PAST_STOP_M = 2.f; // pulled up at it; gone past it
constexpr int RIGHT_SIDE = 0;
constexpr float MOVED_MIN_M = 0.05f; // shorter than this a move has no direction

bool is_car(const Vehicle &v) { return v.type <= VehicleType::PICKUP; }

bool held(const TrafficState &t, int ped) { return t.peds[size_t(ped)].alive && t.peds[size_t(ped)].mode == PedMode::KERB; }

// The walker a stop holds: called to a door within `reach`; set where it is, facing its way; let go to walk on, or gone aboard.
bool hail_walker(CityState &s, int ped, const V2 &door, float reach) {
	TrafficState &t = s.traffic;
	if (ped < 0 || size_t(ped) >= t.peds.size()) return false;
	Pedestrian &w = t.peds[size_t(ped)];
	if (!w.alive || w.mode != PedMode::SIDEWALK || (V2(w.cur.x, w.cur.z) - door).len() > reach) return false;
	w.mode = PedMode::KERB;
	return true;
}

void place_at(CityState &s, int ped, const V2 &at, const V2 &facing) {
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	const V2 f = facing.len() > MOVED_MIN_M ? facing.norm() : V2(w.cur.fx, w.cur.fz);
	w.walked += (V2(w.cur.x, w.cur.z) - at).len();
	w.cur.x = at.x;
	w.cur.z = at.y;
	w.cur.fx = f.x;
	w.cur.fz = f.y;
}

void let_go(CityState &s, int ped, bool on_foot) {
	Pedestrian &w = s.traffic.peds[size_t(ped)];
	if (!w.alive || w.mode != PedMode::KERB) return;
	if (on_foot && w.route.len > 0) w.mode = PedMode::SIDEWALK;
	else ped_gone(s, ped);
}

// A walker on the footway beside `foot`, on the kerb's sidewalk of `ds`.
int walker_out(CityState &s, int ds, const V2 &foot) {
	const DirSeg &d = s.traffic.g.ds[size_t(ds)];
	const RoadSegment &g = s.roads.segs[size_t(d.seg)];
	double at, dist;
	closest_t(g.q, foot.x, foot.y, at, dist);
	return ped_on_footway(s, d.seg, d.sign > 0 ? 0 : 1, float(arc_of_t(g, at)));
}

// The arc along its street of metre `at` of a directed segment, and which sidewalk is on its kerb side.
float arc_of(const DirSeg &d, float at) { return d.at0 + d.sign * at; }
int kerb_side(const DirSeg &d) { return d.sign > 0 ? RIGHT_SIDE : 1; }

bool has_footway(const CityState &s, const DirSeg &d) {
	const Profile &p = s.roads.profile_of(d.seg);
	return kerb_side(d) == RIGHT_SIDE ? p.has_walk_right : p.has_walk_left;
}

// Where a kerb-side door opens, and the footway beside it.
struct DoorPlaces {
	V2 door, foot;
};

DoorPlaces door_places(const CityState &s, const Vehicle &v) {
	const TrafficParams &p = s.catalog.traffic;
	const VehicleClass &c = p.classes[int(v.type)];
	const DirSeg &d = s.traffic.g.ds[size_t(v.ds)];
	const float at = std::clamp(v.s - c.length * p.kerb_door_along, 0.f, d.len);
	Pose lane, foot;
	lane_pose(s, v.ds, v.lane, at, lane);
	sidewalk_pose(s, d.seg, kerb_side(d), arc_of(d, at), 0, foot);
	const V2 right(lane.fz, -lane.fx);
	return { V2(lane.x, lane.z) + right * (c.half_width + p.kerb_door_out_m), V2(foot.x, foot.z) };
}

// Nobody in the door's swing or on the footway spot, and no vehicle along its side.
bool room_to_open(CityState &s, const Vehicle &v, const KerbStop &k) {
	const TrafficParams &p = s.catalog.traffic;
	const DoorPlaces at = door_places(s, v);
	if (k.errand != Errand::PICK && (walker_within(s, at.door.x, at.door.y, p.kerb_door_clear_m) || walker_within(s, at.foot.x, at.foot.y, p.kerb_foot_clear_m))) return false;
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

// The first walker alone on the kerb's footway between two arcs, or -1.
int hailable(const CityState &s, const DirSeg &d, float a0, float a1) {
	const TrafficState &t = s.traffic;
	const size_t key = size_t(d.seg) * 2 + size_t(kerb_side(d));
	if (key >= t.crowd.size()) return -1;
	const float lo = std::min(a0, a1), hi = std::max(a0, a1);
	for (const CrowdEntry &e : t.crowd[key])
		if (e.s >= lo && e.s <= hi && t.peds[size_t(e.id)].alive && t.peds[size_t(e.id)].mode == PedMode::SIDEWALK) return e.id;
	return -1;
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
		ped = hailable(s, d, arc_of(d, earliest), arc_of(d, std::min(latest, earliest + p.kerb_find_reach_m)));
		if (ped < 0) return;
		const float along = (t.peds[size_t(ped)].s - d.at0) * d.sign + p.classes[int(v.type)].length * p.kerb_door_along;
		at = std::clamp(along, earliest, latest);
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
	if (k.ped >= 0) let_go(s, k.ped, on_foot);
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

void enter(KerbStop &k, KerbPhase phase) {
	k.phase = phase;
	k.t = 0;
}

// The person moving between the door and the footway, by the share of the way from the door.
void place_walker(CityState &s, const KerbStop &k, const DoorPlaces &at, float share) {
	if (k.ped >= 0) place_at(s, k.ped, at.door + (at.foot - at.door) * share, at.foot - at.door);
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
float transfer_time(const CityState &s, const KerbStop &k, const DoorPlaces &at) {
	const TrafficParams &p = s.catalog.traffic;
	return p.kerb_seat_s + (k.errand == Errand::PICK ? 0.f : (at.foot - at.door).len() / p.kerb_pace_ms);
}

void halted(CityState &s, int id, KerbStop &k) {
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = s.traffic.vehicles[size_t(id)];
	if (k.t <= p.kerb_halt_s) return;
	if (k.errand == Errand::PICK) {
		const DoorPlaces at = door_places(s, v);
		if (!hail_walker(s, k.ped, at.door, p.kerb_pick_fetch_m)) {
			k.ped = -1;
			end_stop(s, id);
			return;
		}
		k.fetch = (V2(s.traffic.peds[size_t(k.ped)].cur.x, s.traffic.peds[size_t(k.ped)].cur.z) - at.door).len() / p.kerb_pace_ms;
		k.from = V2(s.traffic.peds[size_t(k.ped)].cur.x, s.traffic.peds[size_t(k.ped)].cur.z);
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
	if (k.ped >= 0 && k.phase > KerbPhase::HALT && !held(t, k.ped)) { // a walker of a street that went
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
	switch (k.phase) {
		case KerbPhase::APPROACH:
			if (v.v < STOPPED_MS && k.at - v.s < NEAR_STOP_M) {
				k.at = v.s;
				enter(k, KerbPhase::HALT);
			} else if (v.s > k.at + PAST_STOP_M) {
				end_stop(s, id);
			}
			break;
		case KerbPhase::HALT: halted(s, id, k); break;
		case KerbPhase::FETCH: {
			k.walked += dt;
			const float share = std::min(1.f, k.walked / std::max(p.kerb_seat_s, k.fetch));
			const DoorPlaces at = door_places(s, v);
			place_at(s, k.ped, k.from + (at.door - k.from) * share, at.door - k.from);
			if (share >= 1) enter(k, KerbPhase::OPEN);
			break;
		}
		case KerbPhase::OPEN:
			if (k.t < p.kerb_door_open_s) break;
			k.transfer = transfer_time(s, k, door_places(s, v));
			if (k.ped < 0 && k.errand != Errand::PICK) {
				const DoorPlaces at = door_places(s, v);
				k.ped = walker_out(s, v.ds, at.foot);
				if (k.ped < 0) {
					end_stop(s, id); // no room on the footway: they stay aboard
					return;
				}
			}
			enter(k, KerbPhase::TRANSFER);
			break;
		case KerbPhase::TRANSFER: {
			const float share = std::min(1.f, k.t / std::max(k.transfer, dt)), out = k.back ? 1 - share : share;
			if (k.errand != Errand::PICK) place_walker(s, k, door_places(s, v), out);
			if (k.t >= k.transfer) transfer_done(s, v, k);
			break;
		}
		case KerbPhase::AWAY:
			if (k.t >= p.kerb_delivery_s) {
				k.back = true;
				k.transfer = (door_places(s, v).foot - door_places(s, v).door).len() / p.kerb_pace_ms + p.kerb_seat_s;
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
