// Who the lots send for and send out: the trips follow the hours of each kind of place (traffic.json people_working,
// people_purchasing), the people going in and coming out of it; a bay is drawn by those shares, then the nearest passing car.
#include "sim/traffic/traffic_system.h"
#include "sim/traffic/day_table.h"
#include "core/generation.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

// People going in at and coming out of a kind of place at an hour, from how many are there and how long they stay.
void flows(const TrafficParams &p, PlaceKind kind, float hour, float &in, float &out) {
	if (kind == PlaceKind::HOME) {
		float wi, wo, si, so;
		flows(p, PlaceKind::WORK, hour, wi, wo);
		flows(p, PlaceKind::SHOP, hour, si, so);
		in = wo + so;
		out = wi + si;
		return;
	}
	const float *table = kind == PlaceKind::WORK ? p.people_working : p.people_purchasing;
	const float stay = kind == PlaceKind::WORK ? p.stay_work_h : p.stay_errand_h;
	const float there = at_hour(table, hour), rise = at_hour(table, hour + 0.5f) - at_hour(table, hour - 0.5f);
	in = std::max(0.f, rise) + there / stay;
	out = std::max(0.f, -rise) + there / stay;
}

// A car standing in a gated bay drives out: a Vehicle again from where it stood, on its way to the lane, the bay left empty.
void take_row(CityState &s, int b) {
	TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	Bay &bay = t.park.bays[size_t(b)];
	const int id = new_vehicle(s, VehicleType(bay.row_type), bay.row_paint);
	Vehicle &v = t.vehicles[size_t(id)];
	v.off = true;
	t.vehicle_count--; // off the lanes it is not of the fleet until it joins one
	set_off_pose(s, v, { bay.at.x, bay.at.y, std::atan2(-bay.open.y, -bay.open.x) });
	LotCar car;
	car.id = id;
	car.bay = b;
	car.phase = CarPhase::LEAVING;
	car.path = bay_departure(bay.at, bay.open, bay.via, bay.lane.p, bay.lane.t, p.man_lead_m, p.man_across_m, p.man_least_m, p.man_tangent);
	v.car = int(t.park.cars.size());
	t.park.cars.push_back(std::move(car));
	t.park.moving++;
	remove_row(t.park, b);
	bay.vacated = true;
	bay.holder = id;
	refile_bay(s, b, false);
}

// The nearest passing car a bay at `at` can take, -1 for none.
int passing_car(const CityState &s, const Bay &bay) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	int best = -1;
	float best_d = p.parker_call_reach_m;
	for (int id : t.vehicle_index.near(bay.lane.p.x, bay.lane.p.y, p.parker_call_reach_m)) {
		const Vehicle &v = t.vehicles[size_t(id)];
		if (v.micro_idx < 0) continue;
		// a commitment to a connector holds the junction for it: a new route would leave that holder behind for good
		if (v.type >= VehicleType::MOTO || v.car >= 0 || v.assignment_route_request || v.trip_route_request || v.ds < 0 || v.conn >= 0 || v.pass >= 0) continue;
		if (p.classes[int(v.type)].length * 0.5f > bay.room) continue;
		if (v.ds == bay.lane.ds && v.s > bay.lane.entry - p.parker_past_gate_m) continue; // past the gate: round the block
		const float d = (V2(v.cur.x, v.cur.z) - bay.lane.p).len();
		if (d < best_d || (best >= 0 && d == best_d && v.micro_idx < t.vehicles[size_t(best)].micro_idx)) { best_d = d; best = id; }
	}
	return best;
}

void submit_lot_route(CityState &s, int id, int bay, uint64_t generation, std::shared_ptr<const ParkingRouteTicket> ticket);
class LotRouteRecipient final : public RouteRecipient {
public:
	LotRouteRecipient(const Vehicle &v, int id, int bay, uint64_t generation, std::shared_ptr<const ParkingRouteTicket> ticket) :
		id_(id), bay_(bay), from_(v.ds), ri_(v.ri), generation_(v.generation), bay_generation_(generation), route_(v.route.version), ticket_(std::move(ticket)) {}
	RecipientState state(const CityState &s) const override {
		const Vehicle *v = actor(s); if (!v || !bay_current(s) || v->off || !v->micro || v->car >= 0 || v->lorry >= 0) return RecipientState::OBSOLETE;
		if (!s.traffic_levels.vehicles_on || v->conn >= 0) return RecipientState::WAIT_SOURCE;
		return eligible(s, *v) && v->ds == from_ && v->ri == ri_ && v->route.version == route_ ? RecipientState::READY : RecipientState::OBSOLETE;
	}
	bool complete(CityState &s, SearchStatus status, RouteDraft &prepared) override {
		const Vehicle *pv = actor(s);
		if (!pv) { finish(s); return true; }
		Vehicle &v = s.traffic.vehicles[size_t(id_)];
		const bool current = state(s) == RecipientState::READY;
		if (status != SearchStatus::TARGET_RETIRED && (!current || status == SearchStatus::INVALIDATED) && bay_current(s) && eligible(s, v)) {
			submit_lot_route(s, id_, bay_, bay_generation_, ticket_); return true; // same selected bay/car; no demand/RNG redraw on wait
		}
		if (!current || status != SearchStatus::FOUND || prepared.empty()) { finish(s); return true; } // failed call keeps its owed arrival
		TrafficState &t = s.traffic; ParkingState &pk = t.park; Bay &bay = pk.bays[size_t(bay_)];
		t.route_pool.adopt(v.route, prepared); v.ri = 0;
		v.stop_at = bay.lane.entry + s.catalog.traffic.classes[int(v.type)].length * 0.5f;
		v.stop_lane = bay.lane.lane; v.goal_s = BIG;
		bay.holder = id_; refile_bay(s, bay_, false);
		LotCar car; car.id = id_; car.bay = bay_; v.car = int(pk.cars.size()); pk.cars.push_back(std::move(car));
		++pk.coming; ++pk.called; pk.owed = std::max(0.f, pk.owed - 1.f);
		finish(s);
		if (pk.owed >= 1 && float(pk.coming) < pk.lot_call_limit) lot_call(s); // resume the original successful-call chain under later queue credits
		return true;
	}
private:
	const Vehicle *actor(const CityState &s) const {
		if (id_ < 0 || size_t(id_) >= s.traffic.vehicles.size()) return nullptr;
		const Vehicle &v = s.traffic.vehicles[size_t(id_)];
		return v.alive && v.generation == generation_ && v.assignment_route_request == ticket_->identity ? &v : nullptr;
	}
	bool bay_current(const CityState &s) const {
		const auto &bays = s.traffic.park.bays;
		if (bay_ < 0 || size_t(bay_) >= bays.size()) return false;
		const Bay &b = bays[size_t(bay_)];
		return b.alive && b.generation == bay_generation_ && !b.has_row && b.holder < 0 && b.way_read && b.reached && b.lane.ds >= 0 && size_t(b.lane.ds) < s.traffic.g.ds.size() && s.traffic.g.ds[size_t(b.lane.ds)].ok;
	}
	bool eligible(const CityState &s, const Vehicle &v) const {
		if (v.off || !v.micro || v.car >= 0 || v.lorry >= 0 || v.conn >= 0 || v.pass >= 0 || v.ds < 0 || size_t(v.ds) >= s.traffic.g.ds.size() || !s.traffic.g.ds[size_t(v.ds)].ok) return false;
		const Bay &b = s.traffic.park.bays[size_t(bay_)];
		return v.ds != b.lane.ds || v.s <= b.lane.entry - s.catalog.traffic.parker_past_gate_m;
	}
	void finish(CityState &s) const {
		if (actor(s)) s.traffic.vehicles[size_t(id_)].assignment_route_request = 0;
		auto &pending = s.traffic.park.pending_lot_route; if (pending.lock() == ticket_) pending.reset();
	}
	int id_, bay_, from_, ri_; uint64_t generation_, bay_generation_, route_;
	std::shared_ptr<const ParkingRouteTicket> ticket_;
};
void submit_lot_route(CityState &s, int id, int bay, uint64_t generation, std::shared_ptr<const ParkingRouteTicket> ticket) {
	Vehicle &v = s.traffic.vehicles[size_t(id)]; v.assignment_route_request = ticket->identity;
	s.traffic.route_admission.submit(s, {RoutePurpose::LOT, id}, {v.ds, s.traffic.park.bays[size_t(bay)].lane.ds}, std::make_unique<LotRouteRecipient>(v, id, bay, generation, std::move(ticket)));
}

} // namespace

// Sends cars out of the bays while fewer drive than the demand asks: those whose stay is over and the scenery's, weighed
// by how many come out of each kind of place at this hour.
void lot_depart(CityState &s, double target) {
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	const int short_of = std::min(int(p.parker_depart_per_look), int(target) - t.vehicle_count - pk.moving);
	const float hour = float(s.clock.hour());
	float w[PLACE_KINDS], total = 0;
	for (int k = 0; k < PLACE_KINDS; k++) {
		float in;
		flows(p, PlaceKind(k), hour, in, w[k]);
		w[k] *= float(pk.row_of[k].size());
		total += w[k];
	}
	for (int n = 0; n < short_of && total > 0; n++) {
		float roll = t.rng.f() * total;
		int kind = 0;
		while (kind + 1 < PLACE_KINDS && roll >= w[kind]) roll -= w[kind++];
		if (pk.row_of[kind].empty()) continue;
		const int b = pk.row_of[kind][size_t(t.rng.next() % uint32_t(pk.row_of[kind].size()))];
		// a car that has not stood its stay is left there: the draw goes to another next look
		if (t.time >= pk.bays[size_t(b)].stay_until && read_bay_way(s, b)) take_row(s, b);
	}
}

// A trip ending in a lot: a free bay drawn by how many go in at each kind of place, the nearest passing car sent to it.
LotRequest lot_call(CityState &s) {
	if (!s.traffic.park.pending_lot_route.expired()) return LotRequest::PENDING;
	TrafficState &t = s.traffic;
	ParkingState &pk = t.park;
	const TrafficParams &p = s.catalog.traffic;
	const float hour = float(s.clock.hour());
	float w[PLACE_KINDS], total = 0;
	for (int k = 0; k < PLACE_KINDS; k++) {
		float out;
		flows(p, PlaceKind(k), hour, w[k], out);
		w[k] *= float(pk.free_of[k].size());
		total += w[k];
	}
	if (total <= 0) return LotRequest::FAILED;
	float roll = t.rng.f() * total;
	int kind = 0;
	while (kind + 1 < PLACE_KINDS && roll >= w[kind]) roll -= w[kind++];
	if (pk.free_of[kind].empty()) return LotRequest::FAILED;
	const int b = pk.free_of[kind][size_t(t.rng.next() % uint32_t(pk.free_of[kind].size()))];
	if (!read_bay_way(s, b)) return LotRequest::FAILED;
	const int id = passing_car(s, pk.bays[size_t(b)]);
	if (id < 0) return LotRequest::FAILED;
	auto ticket = std::make_shared<const ParkingRouteTicket>(ParkingRouteTicket{next_generation()});
	pk.pending_lot_route = ticket;
	submit_lot_route(s, id, b, pk.bays[size_t(b)].generation, std::move(ticket));
	return LotRequest::PENDING;
}

} // namespace traffic
} // namespace city
