// Demand and spawning (Roadcraft traffic.ts:954-1047): the fleet target follows lane-metres x density x the day
// profile; trips start and end on directed segments weighted by the zones painted beside them (home -> work in the
// morning, work -> home in the evening), 200-7000 m apart with an exponential preference for short trips.
#include "sim/traffic/kerb_stops.h"

#include "core/units.h"
#include "core/generation.h"

#include <cmath>
#include <cstdlib>
#include <limits>

namespace city {
namespace traffic {

struct VehicleBirthIntent {
	enum class Phase { TYPE, ORIGIN, DESTINATION, ROOM, QUERY, FAILED };
	struct Pick { enum class Phase { DRAW, CONNECTION, WEIGHT }; Phase phase = Phase::DRAW; int tried = 0, last = -1, candidate = -1, choice = -1; RouteConnCursor cursor; };
	struct Ticket {};
	uint64_t city = 0, batch = 0, graph = 0, from_generation = 0, goal_generation = 0;
	int domain = 0, from = -1, goal = -1, lane = 0, destination_tries = 0;
	float hour = 0, at = 0; V2 origin; VehicleType type = VehicleType::SEDAN; bool bike = false, truck = false;
	Phase phase = Phase::TYPE; Pick pick;
	std::weak_ptr<const Ticket> admission;
	std::shared_ptr<const BirthReservations::Lease> reservation;
};

namespace {

constexpr int PICK_TRIES = 24, DEST_TRIES = 16, MAX_FAILURES = 8, FLEET_MAX = 65536;
constexpr float SIDE_REACH_M = 120.f; // a segment counts fully once it is this long
constexpr float VF_BASE = 0.92f, VF_SPAN = 0.16f;
constexpr float PLACE_LO = 0.1f, PLACE_HI = 0.9f, GOAL_LO = 0.2f, GOAL_HI = 0.8f;
constexpr float START_SPEED_SHARE = 0.6f;

const float *weights_now(const TrafficParams &p, float hour, bool origin) {
	if (hour >= p.morning_from_h && hour < p.morning_to_h) return origin ? p.zone_home : p.zone_work;
	if (hour >= p.evening_from_h && hour < p.evening_to_h) return origin ? p.zone_work : p.zone_home;
	return p.zone_mid;
}

float seg_weight(const CityState &s, int seg, const float *w) {
	const RoadSegment &g = s.roads.segs[seg];
	float best = std::max(w[s.catalog.zone(g.zl).base % BASE_ZONES], w[s.catalog.zone(g.zr).base % BASE_ZONES]);
	return best * std::min(1.f, float(g.len) / SIDE_REACH_M);
}

int pick_ds(CityState &s, const float *w) {
	TrafficState &t = s.traffic;
	const int n = int(t.g.ds.size());
	int last = -1;
	for (int k = 0; k < PICK_TRIES && n > 0; k++) {
		int ds = int(t.rng.next() % uint32_t(n));
		if (!t.g.ds[ds].ok) ds ^= 1;
		if (!t.g.ds[ds].ok) continue;
		bool leaves = false;
		each_conn_from(t.g, ds, [&](int) { leaves = true; });
		if (!leaves) continue;
		last = ds;
		if (t.rng.f() < seg_weight(s, ds >> 1, w)) return ds;
	}
	return last;
}

V2 mid_of(const CityState &s, int ds) {
	Pose p;
	lane_pose(s, ds, 0, s.traffic.g.ds[ds].len * 0.5f, p);
	return V2(p.x, p.z);
}

VehicleType draw_type(CityState &s) {
	const TrafficParams &p = s.catalog.traffic;
	if (s.traffic.rng.f() < p.bike_share) return VehicleType::BICYCLE;
	float r = s.traffic.rng.f();
	if (r >= p.car_share) return r < p.car_share + p.bus_share ? VehicleType::BUS : VehicleType::TRUCK;
	float x = s.traffic.rng.f(), acc = 0;
	for (int k = 0; k < CAR_STYLES; k++) {
		acc += p.style_share[k];
		if (x < acc) return VehicleType(k);
	}
	return VehicleType::SEDAN;
}

// True when a vehicle of length `len` fits on the lane at `at` without touching anyone.
bool lane_free(const CityState &s, int key, float at, float len, bool reservations = true) {
	const TrafficState &t = s.traffic;
	auto overlaps = [&](float position, int type) {
		const VehicleClass &oc = s.catalog.traffic.classes[type];
		return at > position - oc.length - oc.min_gap && at - len - oc.min_gap < position;
	};
	for (int id : t.lane_vehicles[size_t(key)]) { const Vehicle &o = t.vehicles[size_t(id)]; if (overlaps(o.s, int(o.type))) return false; }
	return !reservations || !t.birth_reservations.any(key, s.roads.segment_generation((key / MAX_LANES) / 2), [&](const BirthReservations::Claim &claim) { return overlaps(claim.at, claim.type); });
}

// One draw/adjacency/weight transition per shared credit; a cancelled queue entry resumes this same scalar recipe.
bool advance_birth_pick(CityState &s, VehicleBirthIntent &intent, const float *weights, WorkBudget &work) {
	auto &pick = intent.pick; auto &t = s.traffic; const auto &g = t.g;
	using Phase = VehicleBirthIntent::Pick::Phase;
	while (work.take()) {
		if (pick.phase == Phase::DRAW) {
			if (pick.tried >= PICK_TRIES || intent.domain <= 0) { pick.choice = pick.last; return true; }
			++pick.tried; int ds = int(t.rng.next() % uint32_t(intent.domain));
			if (!g.ds[size_t(ds)].ok) ds ^= 1;
			if (size_t(ds) >= g.ds.size() || !g.ds[size_t(ds)].ok) continue;
			pick.candidate = ds; pick.cursor = {}; pick.phase = Phase::CONNECTION;
		} else if (pick.phase == Phase::CONNECTION) {
			bool leaves = false;
			if (size_t(pick.candidate) < g.route_ready.size() && size_t(pick.candidate) < g.route_conns.size() && g.route_ready[size_t(pick.candidate)]) leaves = !g.route_conns[size_t(pick.candidate)].empty();
			else {
				int ci = -1; const ConnRead read = next_physical_connection(g, pick.candidate, pick.cursor, ci);
				if (read == ConnRead::SKIP) continue;
				leaves = read == ConnRead::ENTRY;
			}
			if (!leaves) { pick.phase = Phase::DRAW; continue; }
			pick.last = pick.candidate; pick.phase = Phase::WEIGHT;
		} else {
			if (t.rng.f() < seg_weight(s, pick.candidate >> 1, weights)) { pick.choice = pick.candidate; return true; }
			pick.phase = Phase::DRAW;
		}
	}
	return false;
}
void queue_vehicle_birth(CityState &s);
class VehicleBirthRecipient final : public RouteRecipient {
public:
	VehicleBirthRecipient(std::shared_ptr<VehicleBirthIntent> intent, std::shared_ptr<const VehicleBirthIntent::Ticket> ticket) : intent_(std::move(intent)), ticket_(std::move(ticket)) {}
	RecipientState state(const CityState &s) const override {
		const auto &b = s.traffic.vehicle_birth; const auto &i = *intent_;
		if (!b.active || b.generation != i.batch || b.current != intent_ || s.generation != i.city) return RecipientState::OBSOLETE;
		if (!s.traffic_levels.vehicles_on) return RecipientState::WAIT_SOURCE;
		if (i.phase != VehicleBirthIntent::Phase::QUERY) return s.traffic.g.routing_revision == i.graph ? RecipientState::READY : RecipientState::OBSOLETE;
		if (i.from < 0 || i.goal < 0 || size_t(i.from) >= s.traffic.g.ds.size() || size_t(i.goal) >= s.traffic.g.ds.size() || s.roads.segment_generation(i.from / 2) != i.from_generation || s.roads.segment_generation(i.goal / 2) != i.goal_generation) return RecipientState::OBSOLETE;
		const auto &d = s.traffic.g.ds[size_t(i.from)]; const auto &p = s.catalog.traffic; const auto &body = p.classes[int(i.type)];
		return d.ok && i.lane >= 0 && i.lane < d.lanes && (!i.bike || d.bikes) && i.at >= body.length + p.mouth_clear_m && i.at <= d.len - d.stop_back - p.stop_margin_m ? RecipientState::READY : RecipientState::OBSOLETE;
	}
	RequestPreparation prepare_request(CityState &s, RoadRequest &request, WorkBudget &work) override {
		auto &i = *intent_; const auto &p = s.catalog.traffic; auto &t = s.traffic;
		using Phase = VehicleBirthIntent::Phase;
		while (work.left > 0) {
			if (i.phase == Phase::TYPE) {
				work.take(); i.type = draw_type(s); i.bike = i.type == VehicleType::BICYCLE; i.truck = i.type == VehicleType::TRUCK; i.phase = Phase::ORIGIN;
			} else if (i.phase == Phase::ORIGIN) {
				if (!advance_birth_pick(s, i, i.truck ? p.zone_truck : weights_now(p, i.hour, true), work)) return RequestPreparation::PENDING;
				i.from = i.pick.choice;
				if (i.from < 0 || (i.bike && !t.g.ds[size_t(i.from)].bikes)) return fail(i);
				i.from_generation = s.roads.segment_generation(i.from / 2); i.origin = mid_of(s, i.from); i.pick = {}; i.phase = Phase::DESTINATION;
			} else if (i.phase == Phase::DESTINATION) {
				if (i.destination_tries >= DEST_TRIES) return fail(i);
				if (!advance_birth_pick(s, i, i.truck ? p.zone_truck : weights_now(p, i.hour, false), work)) return RequestPreparation::PENDING;
				const int candidate = i.pick.choice; ++i.destination_tries; i.pick = {};
				if (candidate < 0) continue;
				const float distance = (mid_of(s, candidate) - i.origin).len();
				if (distance > p.trip_min_m && distance < p.trip_max_m && t.rng.f() < std::exp(-distance / p.trip_decay_m)) {
					i.goal = candidate; if (i.goal == i.from) return fail(i);
					i.goal_generation = s.roads.segment_generation(i.goal / 2); i.phase = Phase::ROOM;
				}
			} else if (i.phase == Phase::ROOM) {
				work.take(); const DirSeg &d = t.g.ds[size_t(i.from)]; const VehicleClass &body = p.classes[int(i.type)];
				if (d.lanes <= 0) return fail(i);
				i.lane = i.bike ? 0 : int(t.rng.next() % uint32_t(d.lanes)); i.at = t.rng.range(PLACE_LO, PLACE_HI) * d.len;
				const int key = i.from * MAX_LANES + i.lane;
				if (i.at < body.length + p.mouth_clear_m || i.at > d.len - d.stop_back - p.stop_margin_m || !lane_free(s, key, i.at, body.length)) return fail(i);
				if (float(uint64_t(lane_load(t, key)) + t.birth_reservations.count(key, i.from_generation) + 1) > d.len / p.slot_m) return fail(i);
				i.reservation = t.birth_reservations.reserve(key, i.from_generation, i.at, int(i.type)); i.phase = Phase::QUERY;
			} else if (i.phase == Phase::QUERY) {
				request = {i.from, i.goal, -1, -1, i.bike}; return RequestPreparation::READY;
			} else return RequestPreparation::SKIPPED;
		}
		return RequestPreparation::PENDING;
	}
	bool complete(CityState &s, SearchStatus status, RouteDraft &prepared) override {
		auto &b = s.traffic.vehicle_birth; auto &i = *intent_;
		if (!b.active || b.generation != i.batch || b.current != intent_) return true;
		const bool found = state(s) == RecipientState::READY && i.phase == VehicleBirthIntent::Phase::QUERY && status == SearchStatus::FOUND && prepared.size() >= 2;
		if (found) {
			auto &t = s.traffic; const auto &p = s.catalog.traffic; const DirSeg &d = t.g.ds[size_t(i.from)]; const auto &body = p.classes[int(i.type)];
			const int key = i.from * MAX_LANES + i.lane;
			if (!lane_free(s, key, i.at, body.length, false) || float(uint64_t(lane_load(t, key)) + t.birth_reservations.count(key, i.from_generation)) > d.len / p.slot_m) return false;
			const int id = new_vehicle(s, i.type, uint8_t(t.rng.next() % PAINTS)); Vehicle &v = t.vehicles[size_t(id)];
			assign_errand(s, v, id); v.ds = i.from; v.lane = i.lane; v.s = i.at;
			v.v = std::min(d.speed * v.vf * START_SPEED_SHARE, std::sqrt(2 * body.b * (d.len - d.stop_back - p.stop_margin_m - i.at)));
			t.route_pool.adopt(v.route, prepared); v.goal_s = t.rng.range(GOAL_LO, GOAL_HI) * t.g.ds[size_t(i.goal)].len;
			const V2 mid = mid_of(s, i.from); const float dx = mid.x - s.camera_x.load(), dz = mid.y - s.camera_z.load();
			if (dx * dx + dz * dz < p.micro_spawn_m * p.micro_spawn_m) { micro_add(t, id); lane_add(s, key, id); }
			else { meso_enter(s, id, key, true); if (in_window(s, i.from)) materialize(s, key); }
			if (v.micro) refresh_vehicle_pose(s, id);
		} else ++b.failures;
		i.reservation.reset(); b.current.reset();
		if (b.remaining <= 0 || b.failures >= MAX_FAILURES) b.active = false; else queue_vehicle_birth(s);
		return true;
	}
private:
	static RequestPreparation fail(VehicleBirthIntent &i) { i.phase = VehicleBirthIntent::Phase::FAILED; return RequestPreparation::SKIPPED; }
	std::shared_ptr<VehicleBirthIntent> intent_; std::shared_ptr<const VehicleBirthIntent::Ticket> ticket_;
};
void queue_vehicle_birth(CityState &s) {
	auto &b = s.traffic.vehicle_birth; if (!b.active) return;
	if (!b.current) {
		if (b.remaining <= 0 || b.failures >= MAX_FAILURES) { b.active = false; return; }
		if (s.traffic.g.ds.size() > size_t(std::numeric_limits<int>::max())) std::abort();
		--b.remaining; b.current = std::make_shared<VehicleBirthIntent>();
		auto &i = *b.current; i.city = s.generation; i.batch = b.generation; i.graph = s.traffic.g.routing_revision;
		i.domain = int(s.traffic.g.ds.size()); i.hour = float(s.clock.hour());
	}
	if (!b.current->admission.expired()) return;
	auto ticket = std::make_shared<const VehicleBirthIntent::Ticket>(); b.current->admission = ticket;
	s.traffic.route_admission.submit(s, RouteKey::demand(RoutePurpose::VEHICLE_BIRTH), {}, std::make_unique<VehicleBirthRecipient>(b.current, std::move(ticket)));
}

void queue_trip(CityState &s, int id, int goal, TripFailure failure);
class TripRouteRecipient final : public RouteRecipient {
public:
	TripRouteRecipient(const Vehicle &v, int id, int goal, TripFailure failure) : id_(id), from_(v.ds), ri_(v.ri), goal_(goal),
		generation_(v.generation), request_(v.trip_route_request), route_(v.route.version), failure_(failure) {}
	RecipientState state(const CityState &s) const override {
		const Vehicle *v = actor(s); if (!v || v->off || v->car >= 0 || v->lorry >= 0) return RecipientState::OBSOLETE;
		if (!s.traffic_levels.vehicles_on || v->conn >= 0) return RecipientState::WAIT_SOURCE; // finish the physical handover before capturing its current source
		return v->ds == from_ && v->ri == ri_ && v->route.version == route_ ? RecipientState::READY : RecipientState::OBSOLETE;
	}
	bool complete(CityState &s, SearchStatus status, RouteDraft &prepared) override {
		if (!actor(s)) return true;
		Vehicle &v = s.traffic.vehicles[size_t(id_)];
		const bool current = state(s) == RecipientState::READY;
		v.trip_route_request = 0;
		if (v.off || v.car >= 0 || v.lorry >= 0) return true; // a new physical owner superseded this desired trip
		const auto &ds = s.traffic.g.ds;
		const bool endpoints = v.ds >= 0 && goal_ >= 0 && size_t(v.ds) < ds.size() && size_t(goal_) < ds.size() && ds[size_t(v.ds)].ok && ds[size_t(goal_)].ok;
		if (current && status == SearchStatus::SOURCE_RETIRED) { despawn_vehicle(s, id_, false); return true; }
		if (status != SearchStatus::TARGET_RETIRED && (!current || status == SearchStatus::INVALIDATED) && endpoints) { queue_trip(s, id_, goal_, failure_); return true; }
		if (current && status == SearchStatus::FOUND && prepared.size() >= 2) {
			s.traffic.route_pool.adopt(v.route, prepared); v.ri = 0;
			v.goal_s = s.traffic.rng.range(GOAL_LO, GOAL_HI) * ds[size_t(goal_)].len;
			return true;
		}
		if (failure_ == TripFailure::ARRIVED) despawn_vehicle(s, id_, true); else v.goal_s = 0;
		return true;
	}
private:
	const Vehicle *actor(const CityState &s) const {
		if (id_ < 0 || size_t(id_) >= s.traffic.vehicles.size()) return nullptr;
		const Vehicle &v = s.traffic.vehicles[size_t(id_)];
		return v.alive && v.generation == generation_ && v.trip_route_request == request_ ? &v : nullptr;
	}
	int id_, from_, ri_, goal_; uint64_t generation_, request_, route_; TripFailure failure_;
};
void queue_trip(CityState &s, int id, int goal, TripFailure failure) {
	Vehicle &v = s.traffic.vehicles[size_t(id)]; v.trip_route_request = next_generation();
	s.traffic.route_admission.submit(s, {RoutePurpose::TRIP, id}, {v.ds, goal, -1, -1, v.type == VehicleType::BICYCLE}, std::make_unique<TripRouteRecipient>(v, id, goal, failure));
}

} // namespace

int new_vehicle(CityState &s, VehicleType type, uint8_t paint) {
	TrafficState &t = s.traffic;
	int id;
	if (!t.free_vehicles.empty()) {
		id = t.free_vehicles.back();
		t.free_vehicles.pop_back();
	} else {
		t.vehicles.emplace_back();
		id = int(t.vehicles.size()) - 1;
	}
	Vehicle &v = t.vehicles[size_t(id)];
	v = Vehicle();
	v.generation = next_generation();
	v.alive = true;
	t.live_vehicles.insert(id);
	v.micro = false; // micro_add registers it in micro_list
	v.type = type;
	v.paint = paint;
	v.vf = s.catalog.traffic.classes[int(type)].speed_share * (VF_BASE + VF_SPAN * t.rng.f());
	v.drift = t.rng.range(0.f, TAU_F);
	index_vehicle(s, id);
	t.vehicle_count++;
	return id;
}

void free_vehicle(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	t.route_admission.cancel_actor(s, id); t.birth_reservations.erase(id);
	t.route_pool.release(v.route);
	t.vehicle_index.erase(id);
	t.live_vehicles.erase(id);
	v = Vehicle();
	t.free_vehicles.push_back(id);
}

TripRequest new_trip(CityState &s, int id, int from_ds, TripFailure failure) {
	TrafficState &t = s.traffic;
	if (t.vehicles[size_t(id)].trip_route_request) return TripRequest::PENDING;
	if (from_ds < 0 || size_t(from_ds) >= t.g.ds.size() || !t.g.ds[size_t(from_ds)].ok) return TripRequest::NO_DESTINATION;
	const TrafficParams &p = s.catalog.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	const float *w = v.type == VehicleType::TRUCK ? p.zone_truck : weights_now(p, float(s.clock.hour()), false);
	const V2 a = mid_of(s, from_ds);
	for (int k = 0; k < DEST_TRIES; k++) {
		const int c = pick_ds(s, w);
		if (c < 0 || c == from_ds) continue;
		const float d = (mid_of(s, c) - a).len();
		if (d <= p.trip_min_m || d >= p.trip_max_m || t.rng.f() >= std::exp(-d / p.trip_decay_m)) continue;
		queue_trip(s, id, c, failure);
		return TripRequest::PENDING;
	}
	return TripRequest::NO_DESTINATION;
}

double fleet_target(const CityState &s) {
	const TrafficState &t = s.traffic;
	const TrafficParams &p = s.catalog.traffic;
	return std::min(double(FLEET_MAX), t.g.lane_metres * p.density_per_lane_m * day_factor(p, float(s.clock.hour())) * p.traffic_share * s.traffic_levels.traffic * s.traffic_levels.population * s.traffic_levels.demand * t.gov);
}

float day_factor(const TrafficParams &p, float h) {
	auto g = [h](float c, float w) { float d = h - c; return std::exp(-d * d / (2 * w * w)); };
	float f = p.day_base + p.day_morning_amp * g(p.day_morning_h, p.day_morning_w) + p.day_evening_amp * g(p.day_evening_h, p.day_evening_w) +
			p.day_midday_amp * g(p.day_midday_h, p.day_midday_w);
	if (h < p.day_night_to_h || h > p.day_night_from_h) f -= p.day_night_cut;
	return std::max(0.f, f);
}

void spawn_vehicles(CityState &s, float dt) {
	TrafficState &t = s.traffic; const auto &p = s.catalog.traffic; auto &birth = t.vehicle_birth;
	const double target = fleet_target(s);
	// A captured uncreated birth already claims one desired fleet slot; on-lane/off-bay counts retain their original policy.
	const double driving = double(t.vehicle_count) + double(t.park.moving) + (birth.current ? 1.0 : 0.0);
	const bool filling = driving < p.fill_share * target;
	if (!filling) t.spawn_credit += float(std::max(0.0, target - driving)) * dt / p.fleet_tau_s;
	if (birth.active) { queue_vehicle_birth(s); return; } // credits accrue while its scalar attempt debt/choices survive
	const int attempts = filling ? WorkBudget::from_count(p.spawn_fill_attempts).left : std::min(WorkBudget::from_count(t.spawn_credit).left, WorkBudget::from_count(p.spawn_attempts_per_step).left);
	if (!filling) t.spawn_credit -= float(attempts);
	if (attempts <= 0) return;
	birth.generation = next_generation(); birth.remaining = attempts; birth.failures = 0; birth.active = true;
	queue_vehicle_birth(s);
}

} // namespace traffic
} // namespace city
