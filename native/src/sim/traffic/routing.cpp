// Car routing (Roadcraft src/sim/routing.ts, traffic/micro.ts detour/reconsider): A* over directed segments, so lane
// connectors and one-way streets are respected. Edge time = (length / limit + a fixed extra) x (1 + gain r^2) with
// r = vehicles / slots on the segment (BPR); junction cost by turn (right, left, U) and signals; heuristic =
// straight distance at GUESS_SPEED (astar.h). Detours and
// reconsiderations are short searches that rejoin the old route a few segments on.
#include "sim/traffic/traffic_system.h"

#include "core/units.h"
#include "core/generation.h"
#include "sim/traffic/route_search.h"
#include "sim/traffic/route_join.h"

#include <algorithm>
#include <cmath>

namespace city {
namespace traffic {

namespace {

// BPR congestion factor of a directed segment (routing.ts: cap = max(1, len * lanes / slot)).
float load_factor(const CityState &s, int ds, RouteCounts::Reader *counts, bool micro) {
	const TrafficState &t = s.traffic;
	const TrafficParams &tp = s.catalog.traffic;
	const DirSeg &d = t.g.ds[ds];
	if (!micro) return 1.f;
	int n = 0;
	for (int l = 0; l < d.lanes; l++) n += counts ? (*counts)[size_t(ds) * MAX_LANES + l] : int(t.lane_vehicles[size_t(ds) * MAX_LANES + l].size());
	float r = float(n) / std::max(1.f, d.len * d.lanes / tp.slot_m);
	return 1.f + tp.bpr_gain * r * r;
}

void end_point(const CityState &s, int ds, V2 &p) {
	const DirSeg &d = s.traffic.g.ds[ds];
	p = V2(d.ex, d.ez);
}

class RepairRouteRecipient final : public RouteRecipient {
public:
	RepairRouteRecipient(const Vehicle &v, int id) : id_(id), from_(v.ds), lane_(v.lane), ri_(v.ri), generation_(v.generation), request_(v.repair_route_request), route_(v.route.version) {}
	RecipientState state(const CityState &s) const override {
		const Vehicle *v = actor(s);
		if (!v || v->off || !v->micro || v->conn >= 0 || destination_pending(*v) || v->ds != from_ || v->lane != lane_ || v->ri != ri_ || v->route.version != route_) return RecipientState::OBSOLETE;
		return s.traffic_levels.vehicles_on ? RecipientState::READY : RecipientState::WAIT_SOURCE;
	}
	bool complete(CityState &s, SearchStatus status, RouteDraft &prepared) override {
		if (!actor(s)) return true;
		Vehicle &v = s.traffic.vehicles[size_t(id_)];
		const bool current = state(s) == RecipientState::READY; v.repair_route_request = 0;
		if (!current) return true; // a new route/lane or physical handover owns its next decision
		if (status != SearchStatus::FOUND || prepared.empty()) { despawn_vehicle(s, id_, false); return true; }
		s.traffic.route_pool.adopt(v.route, prepared); v.ri = 0; ++s.traffic.reroutes;
		return true;
	}
private:
	const Vehicle *actor(const CityState &s) const {
		if (id_ < 0 || size_t(id_) >= s.traffic.vehicles.size()) return nullptr;
		const Vehicle &v = s.traffic.vehicles[size_t(id_)];
		return v.alive && v.generation == generation_ && v.repair_route_request == request_ ? &v : nullptr;
	}
	int id_, from_, lane_, ri_; uint64_t generation_, request_, route_;
};

class ReconsiderRouteRecipient final : public RouteRecipient {
public:
	ReconsiderRouteRecipient(const Vehicle &v, int id, int join) : id_(id), from_(v.ds), lane_(v.lane), ri_(v.ri), join_(join), generation_(v.generation), request_(v.reconsider_route_request), route_(v.route.version) {}
	RecipientState state(const CityState &s) const override {
		const Vehicle *v = actor(s);
		if (!v || !vehicle_route_goal_current(s, *v) || v->off || !v->micro || v->conn >= 0 || v->pass >= 0 || v->repair_route_request || destination_pending(*v) || v->detour_route_request || v->ds != from_ || v->lane != lane_ || v->ri != ri_ || v->route.version != route_) return RecipientState::OBSOLETE;
		return s.traffic_levels.vehicles_on ? RecipientState::READY : RecipientState::WAIT_SOURCE;
	}
	bool prepare_result(CityState &s, SearchStatus status, const RouteDraft &path, WorkBudget &work) override {
		if (status != SearchStatus::FOUND || path.size() < 2) return true;
		Vehicle &v = s.traffic.vehicles[size_t(id_)]; RoutePool &pool = s.traffic.route_pool;
		if (!checked_) {
			accepted_ = lane_reaches(s, from_, lane_, pool.at(path, 1)); checked_ = true;
			same_ = path.size() == join_ - ri_ + 1;
		}
		if (!accepted_) return true;
		while (same_ && compared_ < path.size() && work.take()) { same_ = pool.at(v.route, ri_ + compared_) == pool.at(path, compared_); ++compared_; }
		if (same_ && compared_ < path.size()) return false;
		if (same_) { accepted_ = false; return true; }
		return output_.prepare(pool, v.route, from_, path, 0, false, join_ + 1, work);
	}
	void retire_result(CityState &s) override { output_.retire(s.traffic.route_pool); checked_ = accepted_ = same_ = false; compared_ = 0; }
	bool complete(CityState &s, SearchStatus status, RouteDraft &) override {
		if (!actor(s)) return true;
		Vehicle &v = s.traffic.vehicles[size_t(id_)]; const bool current = state(s) == RecipientState::READY;
		v.reconsider_route_request = 0;
		if (current && status == SearchStatus::FOUND && accepted_) { output_.adopt(s.traffic.route_pool, v.route); v.ri = 0; ++s.traffic.reroutes; }
		return true;
	}
private:
	const Vehicle *actor(const CityState &s) const {
		if (id_ < 0 || size_t(id_) >= s.traffic.vehicles.size()) return nullptr;
		const Vehicle &v = s.traffic.vehicles[size_t(id_)];
		return v.alive && v.generation == generation_ && v.reconsider_route_request == request_ ? &v : nullptr;
	}
	int id_, from_, lane_, ri_, join_; uint64_t generation_, request_, route_;
	RouteJoin output_; bool checked_ = false, accepted_ = false, same_ = false; int compared_ = 0;
};

struct DetourIntent {
	enum class Stage { SELECT, LOCAL, FULL };
	int id = -1, from = -1, lane = 0, ri = 0, join = 0, local_goal = -1, goal = -1, blocked = -1, connector = -1;
	uint64_t generation = 0, request = 0, route = 0, graph = 0;
	bool need_room = false; Stage stage = Stage::SELECT; RouteConnCursor cursor;
};
void submit_detour(CityState &s, std::shared_ptr<DetourIntent> intent);
class DetourRouteRecipient final : public RouteRecipient {
public:
	explicit DetourRouteRecipient(std::shared_ptr<DetourIntent> intent) : intent_(std::move(intent)) {}
	RecipientState state(const CityState &s) const override {
		const Vehicle *v = actor(s); const auto &d = *intent_;
		if (!v || !vehicle_route_goal_current(s, *v) || s.traffic.g.routing_revision != d.graph || v->off || !v->micro || v->conn >= 0 || v->pass >= 0 || v->repair_route_request || destination_pending(*v) || v->ds != d.from || v->lane != d.lane || v->ri != d.ri || v->route.version != d.route) return RecipientState::OBSOLETE;
		return s.traffic_levels.vehicles_on ? RecipientState::READY : RecipientState::WAIT_SOURCE;
	}
	RequestPreparation prepare_request(CityState &s, RoadRequest &request, WorkBudget &work) override {
		auto &d = *intent_; const auto &g = s.traffic.g;
		while (d.stage == DetourIntent::Stage::SELECT && work.take()) {
			int ci = -1; const ConnRead read = next_physical_connection(g, d.from, d.cursor, ci);
			if (read == ConnRead::END) { finished_ = true; return RequestPreparation::SKIPPED; }
			if (read == ConnRead::SKIP) continue;
			const Conn &c = g.conns[size_t(ci)];
			if (c.from_lane != d.lane || ci == d.blocked || c.turn == Turn::UTURN || (d.need_room && !has_exit(s, c, d.id))) continue;
			d.connector = ci;
			d.stage = d.join > d.ri + 1 && d.local_goal != c.to_ds ? DetourIntent::Stage::LOCAL : DetourIntent::Stage::FULL;
		}
		if (d.stage == DetourIntent::Stage::SELECT) return RequestPreparation::PENDING;
		const bool local = d.stage == DetourIntent::Stage::LOCAL;
		request = {g.conns[size_t(d.connector)].to_ds, local ? d.local_goal : d.goal, local ? int(s.catalog.traffic.detour_max_expanded) : -1, -1, s.traffic.vehicles[size_t(d.id)].type == VehicleType::BICYCLE};
		return RequestPreparation::READY;
	}
	bool prepare_result(CityState &s, SearchStatus status, const RouteDraft &path, WorkBudget &work) override {
		if (status != SearchStatus::FOUND || finished_) return true;
		const auto &d = *intent_; const Vehicle &v = s.traffic.vehicles[size_t(d.id)];
		return output_.prepare(s.traffic.route_pool, v.route, d.from, path, 0, true, d.stage == DetourIntent::Stage::LOCAL ? d.join + 1 : v.route.len, work);
	}
	void retire_result(CityState &s) override { output_.retire(s.traffic.route_pool); }
	bool complete(CityState &s, SearchStatus status, RouteDraft &) override {
		if (!actor(s)) return true;
		auto &d = *intent_; Vehicle &v = s.traffic.vehicles[size_t(d.id)];
		if (state(s) != RecipientState::READY || finished_ || status == SearchStatus::SOURCE_RETIRED || status == SearchStatus::TARGET_RETIRED) { v.detour_route_request = 0; return true; }
		if (status == SearchStatus::FOUND) {
			if (d.need_room && !has_exit(s, s.traffic.g.conns[size_t(d.connector)], d.id)) return false;
			output_.adopt(s.traffic.route_pool, v.route); v.ri = 0; v.detour_route_request = 0; ++s.traffic.reroutes; return true;
		}
		d.stage = d.stage == DetourIntent::Stage::LOCAL ? DetourIntent::Stage::FULL : DetourIntent::Stage::SELECT;
		submit_detour(s, intent_); return true; // full fallback, then the next original connector; no search/quota loop outside admission
	}
private:
	const Vehicle *actor(const CityState &s) const {
		const auto &d = *intent_; if (d.id < 0 || size_t(d.id) >= s.traffic.vehicles.size()) return nullptr;
		const Vehicle &v = s.traffic.vehicles[size_t(d.id)];
		return v.alive && v.generation == d.generation && v.detour_route_request == d.request ? &v : nullptr;
	}
	std::shared_ptr<DetourIntent> intent_; RouteJoin output_; bool finished_ = false;
};
void submit_detour(CityState &s, std::shared_ptr<DetourIntent> intent) {
	const int id = intent->id;
	s.traffic.route_admission.submit(s, {RoutePurpose::DETOUR, id}, {}, std::make_unique<DetourRouteRecipient>(std::move(intent)));
}

} // namespace

bool vehicle_route_goal_current(const CityState &s, const Vehicle &v) {
	if (v.route.len <= 0) return true;
	const int goal = route_at(s.traffic.route_pool, v.route, v.route.len - 1);
	return goal >= 0 && s.roads.segment_generation(goal / 2) == v.route.goal_generation;
}
bool walker_route_goal_current(const CityState &s, const Pedestrian &w) {
	if (w.route.len <= 0) return true;
	const int goal = route_at(s.traffic.ped_pool, w.route, w.route.len - 1);
	return goal >= 0 && s.roads.node_generation(goal) == w.route.goal_generation;
}

float junction_cost(const CityState &s, const Conn &c) {
	const TrafficParams &tp = s.catalog.traffic;
	float turn = 0;
	switch (c.turn) {
		case Turn::RIGHT: turn = tp.turn_cost_right_s; break;
		case Turn::LEFT: turn = tp.turn_cost_left_s; break;
		case Turn::UTURN: turn = tp.turn_cost_uturn_s; break;
		default: break;
	}
	return turn + (s.traffic.g.junctions[c.node].ctl == Control::SIGNAL ? tp.signal_cost_s : 0.f);
}

void RoadRouteSearch::begin(const CityState &s, int from, int goal, int max_expanded, int banned, bool bike) {
	const LaneGraph &g = s.traffic.g;
	counts_ = {}; walking_ = false; search_.begin(0, -1, -1, 0); result_ = SearchStatus::INVALIDATED;
	city_ = s.generation; graph_ = g.routing_revision; roads_ = uint64_t(s.roads.version);
	if (from < 0 || goal < 0 || size_t(from) >= g.ds.size() || size_t(goal) >= g.ds.size() || !g.ds[size_t(from)].ok || !g.ds[size_t(goal)].ok) return;
	goal_ = goal; banned_ = banned; bike_ = bike;
	micro_ = s.traffic.lane_vehicles.size() >= g.ds.size() * MAX_LANES;
	counts_ = s.traffic.route_counts.capture(s.traffic.lane_vehicles.size());
	const int limit = max_expanded < 0 ? int(s.catalog.traffic.route_max_expanded) : max_expanded;
	search_.begin(g.ds.size(), from, goal, limit); result_ = SearchStatus::PENDING;
}
void RoadRouteSearch::discard() {
	counts_ = {}; search_.begin(0, -1, -1, 0); result_ = SearchStatus::INVALIDATED; city_ = graph_ = roads_ = 0; walking_ = false;
}
bool RoadRouteSearch::source_current(const CityState &s) const { return city_ == s.generation && graph_ == s.traffic.g.routing_revision && (!walking_ || roads_ == uint64_t(s.roads.version)); }

template<class Take> SearchStatus RoadRouteSearch::step(const CityState &s, Take &&take) {
	if (!source_current(s)) { counts_ = {}; result_ = SearchStatus::INVALIDATED; return result_; }
	if (result_ != SearchStatus::PENDING) return result_;
	const LaneGraph &g = s.traffic.g;
	const TrafficParams &tp = s.catalog.traffic; // catalogue is immutable after boot, belongs to the guarded city
	{
		auto count_values = counts_.values.reader(); // the retained immutable input outlives this local cursor
		V2 goal; end_point(s, goal_, goal);
		const float guess = tp.guess_speed_kmh / KMH_PER_MS;
		auto h = [&](int ds) { V2 p; end_point(s, ds, p); return (p - goal).len() / guess; };
		auto next = [&](int ds, RouteConnCursor &cursor, int &to, float &cost) {
			int ci = -1;
			const ConnRead read = next_route_connection(g, ds, cursor, ci);
			if (read == ConnRead::END) return EdgeStep::END;
			if (read == ConnRead::SKIP) return EdgeStep::SKIP;
			const Conn &c = g.conns[size_t(ci)];
			if (c.to_ds == banned_) return EdgeStep::SKIP;
			const DirSeg &d = g.ds[size_t(c.to_ds)];
			if (bike_ && !d.bikes) return EdgeStep::SKIP;
			to = c.to_ds;
			cost = (d.len / std::max(d.speed, 1.f) + tp.edge_extra_s) * load_factor(s, c.to_ds, counts_.ready ? &count_values : nullptr, micro_) + junction_cost(s, c);
			return EdgeStep::EDGE;
		};
		result_ = search_.advance(std::forward<Take>(take), h, next);
	}
	if (result_ != SearchStatus::PENDING) counts_ = {}; // a completed admitted query does not pin old congestion forever
	return result_;
}
SearchStatus RoadRouteSearch::advance(const CityState &s, WorkBudget &work) {
	if (walking_) return step_walk(s, work);
	// A resumable query requires the prepared immutable graph/count input; no unbudgeted fallback.
	if (source_current(s) && result_ == SearchStatus::PENDING && !resumable()) return SearchStatus::UNPREPARED;
	return step(s, [&work] { return work.take(); });
}
RouteChoice detour(CityState &s, int id, int blocked, bool need_room) {
	TrafficState &t = s.traffic; Vehicle &v = t.vehicles[size_t(id)];
	if (v.detour_route_request) return RouteChoice::PENDING;
	if (!vehicle_route_goal_current(s, v)) return RouteChoice::UNCHANGED;
	if (v.ds < 0 || size_t(v.ds) >= t.g.ds.size() || !t.g.ds[size_t(v.ds)].ok || v.conn >= 0 || v.pass >= 0 || v.route.len == 0 || v.reconsider_route_request || v.repair_route_request || destination_pending(v)) return RouteChoice::UNCHANGED;
	auto d = std::make_shared<DetourIntent>();
	d->id = id; d->from = v.ds; d->lane = v.lane; d->ri = v.ri; d->blocked = blocked; d->need_room = need_room;
	d->join = std::min(v.route.len - 1, v.ri + 1 + int(s.catalog.traffic.rejoin_ds));
	d->local_goal = route_at(t.route_pool, v.route, d->join); d->goal = route_at(t.route_pool, v.route, v.route.len - 1);
	d->generation = v.generation; d->request = v.detour_route_request = next_generation(); d->route = v.route.version; d->graph = t.g.routing_revision;
	submit_detour(s, std::move(d)); return RouteChoice::PENDING;
}

RouteChoice reconsider(CityState &s, int id) {
	TrafficState &t = s.traffic; Vehicle &v = t.vehicles[size_t(id)];
	if (v.reconsider_route_request) return RouteChoice::PENDING;
	if (!vehicle_route_goal_current(s, v)) return RouteChoice::UNCHANGED;
	if (v.ri + 1 >= v.route.len || v.ds < 0 || v.conn >= 0 || v.pass >= 0 || v.detour_route_request || v.repair_route_request || destination_pending(v)) return RouteChoice::UNCHANGED;
	const int j = std::min(v.route.len - 1, v.ri + int(s.catalog.traffic.rejoin_ds));
	v.reconsider_route_request = next_generation();
	t.route_admission.submit(s, {RoutePurpose::RECONSIDER, id}, {v.ds, route_at(t.route_pool, v.route, j), int(s.catalog.traffic.detour_max_expanded), -1, v.type == VehicleType::BICYCLE}, std::make_unique<ReconsiderRouteRecipient>(v, id, j));
	return RouteChoice::PENDING;
}

// The route says a turn the vehicle's lane cannot make (a connector was removed or the lanes were rearranged): try
// another lane immediately, or capture a repair request. A terminal failed query retires the actor at delivery.
RouteRepair retry_route(CityState &s, int id) {
	TrafficState &t = s.traffic;
	Vehicle &v = t.vehicles[size_t(id)];
	if (v.ds < 0 || v.ds >= int(t.g.ds.size()) || v.ri + 1 >= v.route.len) return RouteRepair::FAILED;
	if (!vehicle_route_goal_current(s, v)) return destination_pending(v) ? RouteRepair::PENDING : RouteRepair::FAILED;
	const int next = route_at(t.route_pool, v.route, v.ri + 1);
	int alt = -1, bd = MAX_LANES + 1;
	for (int lane = 0; lane < t.g.ds[size_t(v.ds)].lanes; lane++) {
		if (lane == v.lane || !lane_reaches(s, v.ds, lane, next)) continue;
		const int d = std::abs(lane - v.lane);
		if (d < bd) { bd = d; alt = lane; }
	}
	if (alt >= 0) {
		move_vehicle_lane(s, id, alt);
		return RouteRepair::CHANGED;
	}
	if (destination_pending(v)) return RouteRepair::PENDING; // a newer desired destination owns the repair
	const int goal = route_at(t.route_pool, v.route, v.route.len - 1);
	if (goal < 0) return RouteRepair::FAILED;
	if (!v.repair_route_request) {
		v.repair_route_request = next_generation();
		t.route_admission.submit(s, {RoutePurpose::REPAIR, id}, {v.ds, goal}, std::make_unique<RepairRouteRecipient>(v, id));
	}
	return RouteRepair::PENDING;
}

} // namespace traffic
} // namespace city
